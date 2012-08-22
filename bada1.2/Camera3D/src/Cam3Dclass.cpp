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
 * Cam3Dclass.cpp
 *
 *  Created on: 29.12.2010
 *      Author: mrak
 */

#include "Cam3Dclass.h"

using namespace Osp::Base;
using namespace Osp::Io;
using namespace Osp::Base::Utility;
using namespace Osp::Base::Collection;
using namespace Osp::Media;
using namespace Osp::Graphics;

Cam3Dclass::Cam3Dclass() {


}

Cam3Dclass::~Cam3Dclass() {

}

Osp::Graphics::Rectangle Cam3Dclass::MakeAnaglyphRC(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp) {
	BufferInfo leftbmpbuff;
	BufferInfo rightbmpbuff;
	BufferInfo outbmpbuff;
	leftbmp->Lock(leftbmpbuff);
	Osp::Graphics::Rectangle posrect;
	Osp::Graphics::Rectangle croprect;
	Osp::Graphics::Rectangle outsize;
	outsize = Rectangle(0,0,outbitmap->GetWidth(),outbitmap->GetHeight());
	posrect = outsize;
	if (aligmentrp.x < 0) {
		posrect.x = Osp::Base::Utility::Math::Abs(aligmentrp.x);
		posrect.width = posrect.width - posrect.x;
		aligmentrp.x = 0;
	} else if (aligmentrp.x > 0) {
		posrect.width = posrect.width - aligmentrp.x;
	}
	if (aligmentrp.y < 0) {
		posrect.y = Osp::Base::Utility::Math::Abs(aligmentrp.y);
		posrect.height = posrect.height - posrect.y;
		aligmentrp.y = 0;
	} else if (aligmentrp.y > 0) {
		posrect.height = posrect.height - aligmentrp.y;
	}
	croprect = Rectangle(aligmentrp.x, aligmentrp.y, posrect.width, posrect.height);
	outbitmap->Merge(aligmentrp,*rightbmp,posrect);
	outbitmap->Lock(outbmpbuff);
	int width = leftbmpbuff.width;
	int height = leftbmpbuff.height;
	int bitsPerPixel = leftbmpbuff.bitsPerPixel;
	AppLog("bits per pixel %d", bitsPerPixel);
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	} else if (bitsPerPixel == 16) {
		bytesPerPixel = 2;
	}
	#define RGB(R, G, B) ((R << (5+6)) + (G << 5) + B );
	for(int x=0;x<width;x++) {
		for (int y=0;y<height;y++) {
			unsigned short * rgbSrc = ((unsigned short *)((byte *) outbmpbuff.pPixels + y * outbmpbuff.pitch + x *bytesPerPixel));
			unsigned short * rgbSrcL = ((unsigned short *)((byte *) leftbmpbuff.pPixels + y * leftbmpbuff.pitch + x *bytesPerPixel));

			//byte red = (*rgbSrc >> 11) & 31;
			byte green = (*rgbSrc >> 5 ) & 63;
			byte blue = (*rgbSrc & 31);
			byte redL = (*rgbSrcL >> 11) & 31;
			//byte greenL = (*rgbSrcL >> 5 ) & 63;
			//byte blueL = (*rgbSrcL & 31);
			if ((x >= croprect.width) || (y >= croprect.height) || (x < croprect.x) || (y < croprect.y)) {
				*rgbSrc = RGB(0,0,0);
			} else {
				*rgbSrc = RGB(redL,green,blue);
			}

	   }
	}
	leftbmp->Unlock();
	outbitmap->Unlock();
	if (croprect.x > 0) {
		croprect.width = croprect.width - croprect.x;
	}
	if (croprect.y > 0) {
		croprect.height = croprect.height - croprect.y;
	}
	double hratio = (croprect.height / ((double)outsize.height));
	double wratio = (croprect.width / ((double)outsize.width));
	if (wratio > hratio) {
		croprect.width = outsize.width * hratio;
		croprect.x = croprect.x + ((outsize.width / 2) - (croprect.width / 2));
	} else if (wratio < hratio) {
		croprect.height = outsize.height * wratio;
		croprect.y = croprect.y + ((outsize.height / 2) - (croprect.height / 2));
	}
	return croprect;
}

Osp::Graphics::Rectangle Cam3Dclass::MakeAnaglyphRCGray(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp) {
	BufferInfo leftbmpbuff;
	BufferInfo rightbmpbuff;
	BufferInfo outbmpbuff;
	leftbmp->Lock(leftbmpbuff);
	Osp::Graphics::Rectangle posrect;
	Osp::Graphics::Rectangle croprect;
	Osp::Graphics::Rectangle outsize;
	outsize = Rectangle(0,0,outbitmap->GetWidth(),outbitmap->GetHeight());
	posrect = outsize;
	if (aligmentrp.x < 0) {
		posrect.x = Osp::Base::Utility::Math::Abs(aligmentrp.x);
		posrect.width = posrect.width - posrect.x;
		aligmentrp.x = 0;
	} else if (aligmentrp.x > 0) {
		posrect.width = posrect.width - aligmentrp.x;
	}
	if (aligmentrp.y < 0) {
		posrect.y = Osp::Base::Utility::Math::Abs(aligmentrp.y);
		posrect.height = posrect.height - posrect.y;
		aligmentrp.y = 0;
	} else if (aligmentrp.y > 0) {
		posrect.height = posrect.height - aligmentrp.y;
	}
	croprect = Rectangle(aligmentrp.x, aligmentrp.y, posrect.width, posrect.height);
	outbitmap->Merge(aligmentrp,*rightbmp,posrect);
	outbitmap->Lock(outbmpbuff);
	int width = leftbmpbuff.width;
	int height = leftbmpbuff.height;
	int bitsPerPixel = leftbmpbuff.bitsPerPixel;
	AppLog("bits per pixel %d", bitsPerPixel);
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}
	for(int x=0;x<width;x++) {
		for (int y=0;y<height;y++) {
			int* colorr = ((int *)((byte *) outbmpbuff.pPixels + y * outbmpbuff.pitch + x *bytesPerPixel));
			int* colorl = ((int *)((byte *) leftbmpbuff.pPixels + y * leftbmpbuff.pitch + x *bytesPerPixel));
			byte* bluer = (byte*) colorr;
			byte* greenr = bluer + 1;
			byte* redr = greenr + 1;
			byte* bluel = (byte*) colorl;
			byte* greenl = bluel + 1;
			byte* redl = greenl + 1;
			if ((x >= croprect.width) || (y >= croprect.height) || (x < croprect.x) || (y < croprect.y)) {
				*redr = 0;
				*greenr = 0;
				*bluer = 0;
			} else {
				byte grayr = ((*bluer) * 0.11) + ((*greenr) * 0.59) + ((*redr) *0.3);
				byte grayl = ((*bluel) * 0.11) + ((*greenl) * 0.59) + ((*redl) *0.3);
				*redr = grayl;
				*greenr = grayr;
				*bluer = grayr;
			}
	   }
	}
	leftbmp->Unlock();
	outbitmap->Unlock();
	if (croprect.x > 0) {
		croprect.width = croprect.width - croprect.x;
	}
	if (croprect.y > 0) {
		croprect.height = croprect.height - croprect.y;
	}
	double hratio = (croprect.height / ((double)outsize.height));
	double wratio = (croprect.width / ((double)outsize.width));
	if (wratio > hratio) {
		croprect.width = outsize.width * hratio;
		croprect.x = croprect.x + ((outsize.width / 2) - (croprect.width / 2));
	} else if (wratio < hratio) {
		croprect.height = outsize.height * wratio;
		croprect.y = croprect.y + ((outsize.height / 2) - (croprect.height / 2));
	}
	return croprect;
}

Osp::Graphics::Rectangle Cam3Dclass::MakeAnaglyphParallel(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp) {
	//najprej naredi crop leve slike na width/2 in pozicija glede na align
	//potem naredi crop desne slike na width/2 in pozicija glede na align
	//potem narisi levo sliko na levo stran in desno na desno stran
	//vrni croprect
	Osp::Graphics::Rectangle croprect;
	croprect = Rectangle(0, 0, outbitmap->GetWidth(),outbitmap->GetHeight());
	return croprect;
}

Osp::Graphics::Rectangle Cam3Dclass::MakeAnaglyphCrossEyed(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp) {
	//najprej naredi crop leve slike na width/2 in pozicija glede na align
	//potem naredi crop desne slike na width/2 in pozicija glede na align
	//potem narisi levo sliko na desno stran in desno na levo stran
	//vrni croprect
	Osp::Graphics::Rectangle croprect;
	croprect = Rectangle(0, 0, outbitmap->GetWidth(),outbitmap->GetHeight());
	return croprect;
}

result Cam3Dclass::SaveCam3Dfile(Osp::Base::String leftimgfn, Osp::Base::String rightimgfn, Osp::Base::DateTime time, unsigned long authorid, int aligmentx, int aligmenty, Osp::Base::String filename) {
	String signature = L"CAM3D";
	FileAttributes attr;
	CAM3D_FILEFORMAT fileheader;

	Osp::Base::ByteBuffer * imgbufl = new Osp::Base::ByteBuffer();
	Osp::Base::ByteBuffer * imgbufr = new Osp::Base::ByteBuffer();

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
	fileheader.authorid = authorid;
	fileheader.aligmentx = aligmentx;
	fileheader.aligmenty = aligmenty;
	File::GetAttributes(leftimgfn, attr);
	fileheader.leftimgsize = attr.GetFileSize();
	File::GetAttributes(rightimgfn, attr);
	fileheader.rightimgsize = attr.GetFileSize();

	File file;
	File imgfilel, imgfiler;

	result r;
	r = file.Construct(filename, L"w");
	if (r != E_SUCCESS) {
		return r;
	}
	r = file.Write(&fileheader, sizeof(fileheader));
	if (r != E_SUCCESS) {
		return r;
	}

	r = imgfilel.Construct(leftimgfn, L"r");
	if (r != E_SUCCESS) {
		return r;
	}

	r = imgbufl->Construct(fileheader.leftimgsize);
	if (r != E_SUCCESS) {
		return r;
	}
	r = imgfilel.Read(*imgbufl);
	if (r != E_SUCCESS) {
		return r;
	}
	r = file.Write(*imgbufl);
	if (r != E_SUCCESS) {
		return r;
	}
	delete imgbufl;

	r = imgfiler.Construct(rightimgfn, L"r");
	if (r != E_SUCCESS) {
		return r;
	}

	r = imgbufr->Construct(fileheader.rightimgsize);
	if (r != E_SUCCESS) {
		return r;
	}
	r = imgfiler.Read(*imgbufr);
	if (r != E_SUCCESS) {
		return r;
	}
	r = file.Write(*imgbufr);
	if (r != E_SUCCESS) {
		return r;
	}
	delete imgbufr;

	r = file.Flush();
	if (r != E_SUCCESS) {
		return r;
	}

	return E_SUCCESS;
}

bool Cam3Dclass::ReadCam3Dfileinfo(Osp::Base::String filename, CAM3D_FILEINFO & data) {
	CAM3D_FILEFORMAT fileformatdata;
	File file;
	file.Construct(filename, L"r");
	file.Read(&fileformatdata, sizeof(fileformatdata));
	data.authorid = fileformatdata.authorid;
	data.leftimgsize = fileformatdata.leftimgsize;
	data.rightimgsize = fileformatdata.rightimgsize;
	data.time.SetValue(fileformatdata.year + 2000,fileformatdata.month,fileformatdata.day,fileformatdata.hour,fileformatdata.min,fileformatdata.sec);
	data.aligmentx = fileformatdata.aligmentx;
	data.aligmenty = fileformatdata.aligmenty;
	return true;
}

bool Cam3Dclass::ReadCam3Dfileimg(Osp::Base::String filename, int imgtype, Osp::Base::ByteBuffer & imagebuf) {
	CAM3D_FILEFORMAT fileformatdata;
	File file;
	file.Construct(filename, L"r");
	file.Read(&fileformatdata, sizeof(fileformatdata));
	if (imgtype == RIGHTIMG) {
		file.Seek(FILESEEKPOSITION_CURRENT, fileformatdata.leftimgsize);
		file.Read(imagebuf);
	} else {
		file.Read(imagebuf);
	}
	return true;
}
