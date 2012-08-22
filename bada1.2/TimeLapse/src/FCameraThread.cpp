/*
 Copyright (C) 2012  Boštjan Mrak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * FCamereThread.cpp
 *
 *  Created on: 15.6.2011
 *      Author: mrak
 */

#include "FCameraThread.h"
#include "TimeLapseClass.h"

using namespace Osp::Base;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Io;
using namespace Osp::App;
using namespace Osp::Base::Runtime;

FCameraThread::FCameraThread() {


}

FCameraThread::~FCameraThread() {

}

result
FCameraThread::Construct()
{
	result r = E_SUCCESS;
	r = Thread::Construct(THREAD_TYPE_EVENT_DRIVEN);
	frameindex = 0;
	filesize = 0;
	frameindex = 0;
	framepreview = null;
	pimageencoder_ = null;
	TimeLapseClass_ = null;
	framebmp = null;

	isconstructed = false;
	issaving = false;

	return r;
}

void FCameraThread::SaveFrame() {

	issaving = true;

	frameindex++;


	ConvertYCbCr420p2RGB888(framepreview);
	delete framepreview;
	framepreview = null;


	Osp::Base::ByteBuffer * framejpg;
	framejpg = pimageencoder_->EncodeToBufferN(*framebmp, IMG_FORMAT_JPG);


	Osp::Base::DateTime time;
	Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, time);


	String frameheadersignature = L"TLFJ";
	for (int chari=0;chari < frameheadersignature.GetLength();chari++) {
		frameheader.signature[chari] = frameheadersignature[chari];
	}
	frameheader.size = framejpg->GetCapacity();
	frameheader.year = (byte)(time.GetYear() - 2000);
	frameheader.month = (byte)(time.GetMonth());
	frameheader.day = (byte)(time.GetDay());
	frameheader.hour = (byte)(time.GetHour());
	frameheader.min = (byte)(time.GetMinute());
	frameheader.sec = (byte)(time.GetSecond());
	frameheader.skip = false;
	frameheader.frameno = frameindex;
	file.Write(&frameheader, sizeof(frameheader));
	file.Write(*framejpg);
	file.Flush();


	delete framejpg;

	filesize += sizeof(frameheader);
	filesize += frameheader.size;

	issaving = false;

}

bool
FCameraThread::OnStart(void)
{

	TimeLapseClass_ = new TimeLapseClass();

	pimageencoder_ = new Image();
	pimageencoder_->Construct();

	framebmp = new Bitmap();
	framebmp->Construct(Dimension(resolution.width,resolution.height), BITMAP_PIXEL_FORMAT_ARGB8888);

	if (File::IsFileExist(L"/Home/lastcapture.tmp")) {
		File::Remove(L"/Home/lastcapture.tmp");
	}

	Osp::Base::DateTime time;
	Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, time);
	file.Construct(filename, L"w+", true);
	String signature = L"TIMELAPSE";
	for (int chari=0;chari < signature.GetLength();chari++) {
		fileheader.signature[chari] = signature[chari];
	}
	fileheader.ver = 1;
	fileheader.year = (byte)(time.GetYear() - 2000);
	fileheader.month = (byte)(time.GetMonth());
	fileheader.day = (byte)(time.GetDay());
	fileheader.hour = (byte)(time.GetHour());
	fileheader.min = (byte)(time.GetMinute());
	fileheader.sec = (byte)(time.GetSecond());
	fileheader.authorid = 0;
	fileheader.interval = interval;
	fileheader.playframerate = playframerate;
	fileheader.innerframecodecs = 1;
	fileheader.noofframes = 0;
	fileheader.width = resolution.width;
	fileheader.height = resolution.height;
	fileheader.yearend = 0;
	fileheader.monthend = 0;
	fileheader.dayend = 0;
	fileheader.hourend = 0;
	fileheader.minend = 0;
	fileheader.secend = 0;
	fileheader.flag = 0;
	fileheader.indexdatafilepos = 0;
	fileheader.reserved1 = 0;
	fileheader.reserved2 = 0;
	fileheader.reserved3 = 0;
	fileheader.reserved4 = 0;
	file.Write(&fileheader, sizeof(fileheader));
	file.Flush();

	filesize += sizeof(fileheader);

	isconstructed = true;

	issaving = false;

	return true;
}

void
FCameraThread::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == THREAD_SAVEFRAME) {
		if (isconstructed == true) {
			SaveFrame();
		}
	} else if (requestId == THREAD_FINISH) {
		//finnish saving

	}
}

void
FCameraThread::OnStop(void)
{
	delete pimageencoder_;
	delete framepreview;
	delete TimeLapseClass_;
	delete framebmp;
}

void FCameraThread::ConvertYCbCr420p2RGB565(Osp::Base::ByteBuffer * pB)
{
	BufferInfo bi;
	framebmp->Lock(bi);
	int __w = bi.width;
	int __h = bi.height;
	int cropw = bi.width;
	int cropl = 0;

	int hw = __w/2;
	int hh = __h/2;
	byte* pOriY= (byte *)pB->GetPointer();
	byte * pOriCb= pOriY + __w*__h;
	byte * pOriCr = pOriCb + hw*hh;

	for(int j=0;j<__h;j+=2)
	{
		byte * pY0,*pY1, *pCb, *pCr;
		pY0 = pOriY + j*__w;
		pY1 = pOriY + (j+1)*__w;
		pCb = pOriCb + hw*(j/2);
		pCr = pOriCr + hw*(j/2);

		unsigned char * buf = (unsigned char *)bi.pPixels + bi.pitch * j;
		unsigned short * pB0= (unsigned short *)buf;

		buf = (unsigned char *)bi.pPixels + bi.pitch * (j+1);
		unsigned short * pB1= (unsigned short *)buf;
		for( int i=cropl;i < (cropl+cropw);i+=2)
		{
			int Y[4],Cb,Cr;
			Cb = pCb[i/2] - 128;
			Cr = pCr[i/2] - 128;

			Y[0]=pY0[i];
			Y[1]=pY0[i+1];
			Y[2]=pY1[i];
			Y[3]=pY1[i+1];

			int A,B,C;
			A= Cr + (Cr>>2) + (Cr>>3) + (Cr>>5);
			B= (Cb>>2) + (Cb>>4) + (Cb>>5) + (Cr>>1) + (Cr>>3)
				+ (Cr>>4) + (Cr>>5);
			C= Cb + (Cb>>1) + (Cb>>2) + (Cb>>6);

			#define CL(A) 	((A>255)? 255:( (A<0)? 0:A ))
			#define RGBA(R, G, B, A) ( (A <<24) | (R<<16) | (G<<8) | (B) )//FOR BITMAP
			#define RGB(R, G, B) (((R >> 3) << (5+6)) + ((G >> 2) << 5) + ((B) >> 3) )//FOR BITMAP

			pB0[(cropl+cropw)-i-1]   = RGB(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C));
			pB0[(cropl+cropw)-i-2] = RGB(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C));
			pB1[(cropl+cropw)-i-1]   = RGB(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C));
			pB1[(cropl+cropw)-i-2] = RGB(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C));
		}
	}
	framebmp->Unlock();
}

void FCameraThread::ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB)
{
	BufferInfo bi;
	framebmp->Lock(bi);
	int __w = bi.width;
	int __h = bi.height;

	int hw = __w/2;
	int hh = __h/2;
	byte* pOriY= (byte *)pB->GetPointer();
	byte * pOriCb= pOriY + __w*__h;
	byte * pOriCr = pOriCb + hw*hh;
	bool flipoutput = false;
	int jdiv2;
	int idiv2;

	for(int j=0;j<__h;j+=2)
	{
		jdiv2 = (j/2);
		byte * pY0,*pY1, *pCb, *pCr;
		pY0 = pOriY + j*__w;
		pY1 = pOriY + (j+1)*__w;
		pCb = pOriCb + hw*jdiv2;
		pCr = pOriCr + hw*jdiv2;

		unsigned char * buf = (unsigned char *)bi.pPixels + bi.pitch * j;
		unsigned int * pB0= (unsigned int *)buf;

		buf = (unsigned char *)bi.pPixels + bi.pitch * (j+1);
		unsigned int * pB1= (unsigned int *)buf;
		for( int i=0;i <__w;i+=2)
		{
			idiv2 = i/2;
			int Y[4],Cb,Cr;
			Cb = pCb[idiv2] - 128;
			Cr = pCr[idiv2] - 128;

			Y[0]=pY0[i];
			Y[1]=pY0[i+1];
			Y[2]=pY1[i];
			Y[3]=pY1[i+1];

			int A,B,C;
			A= Cr + (Cr>>2) + (Cr>>3) + (Cr>>5);
			B= (Cb>>2) + (Cb>>4) + (Cb>>5) + (Cr>>1) + (Cr>>3)
				+ (Cr>>4) + (Cr>>5);
			C= Cb + (Cb>>1) + (Cb>>2) + (Cb>>6);

#define CL(A) 	((A>255)? 255:( (A<0)? 0:A ))
//#define RGBA(R, G, B, A) ( (A <<24) | (B<<16) | (G<<8) | (R) )//FOR OPENGLES
#define RGBA(R, G, B, A) ( (A <<24) | (R<<16) | (G<<8) | (B) )//FOR BITMAP
			if (flipoutput == true) {
				pB0[__w-i-1]   = RGBA(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C), 0XFF);
				pB0[__w-i-2] = RGBA(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C), 0XFF);
				pB1[__w-i-1]   = RGBA(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C), 0XFF);
				pB1[__w-i-2] = RGBA(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C), 0XFF);
			} else {
				pB0[i]   = RGBA(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C), 0XFF);
				pB0[i+1] = RGBA(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C), 0XFF);
				pB1[i]   = RGBA(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C), 0XFF);
				pB1[i+1] = RGBA(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C), 0XFF);
			}
		}
	}
	framebmp->Unlock();
}
