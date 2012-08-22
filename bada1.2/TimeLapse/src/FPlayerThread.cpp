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
 * FPlayerThread.cpp
 *
 *  Created on: 14.6.2011
 *      Author: mrak
 */

#include "FPlayerThread.h"
#include "FPlayer.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Io;
using namespace Osp::Media;
using namespace Osp::Base::Utility;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Base::Runtime;

FPlayerThread::FPlayerThread() {


}

FPlayerThread::~FPlayerThread() {

}

result
FPlayerThread::Construct()
{
	result r = E_SUCCESS;
	r = Thread::Construct(THREAD_TYPE_EVENT_DRIVEN);

	return r;
}

bool FPlayerThread::SkipFrame() {
	if (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		curframefilepos = curfilepos;
		curframesize = frameheader.size;
		curframe = frameheader.frameno;
		if (curframe > frameindexlist->GetCount()) {
			frameindexlist->Add(*(new LongLong(curfilepos)));
		}
		curframedatetime.SetValue(frameheader.year + 2000,frameheader.month,frameheader.day,frameheader.hour,frameheader.min,frameheader.sec);

		file.Seek(FILESEEKPOSITION_CURRENT, frameheader.size);

		curfilepos += sizeof(frameheader);
		curfilepos += frameheader.size;

		curposproc = (double)(curframe-1) * curposprocfactor;
		if (curframe == fileinfo_.noofframes) {
			file.Seek(FILESEEKPOSITION_BEGIN, sizeof(fileformatdata));
			curfilepos = sizeof(fileformatdata);
		}
		return true;
	} else {
		return false;
	}
}

bool FPlayerThread::GetNextFrame() {
	//long long benchmarktimestart, benchmarktimeend;

	if (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		//Osp::System::SystemTime::GetTicks(benchmarktimestart);

		curframefilepos = curfilepos;
		curframesize = frameheader.size;
		curframe = frameheader.frameno;
		if (curframe > frameindexlist->GetCount()) {
			frameindexlist->Add(*(new LongLong(curfilepos)));
		}

		curframedatetime.SetValue(frameheader.year + 2000,frameheader.month,frameheader.day,frameheader.hour,frameheader.min,frameheader.sec);

		tmpimgbuffer->SetLimit(frameheader.size);
		tmpimgbuffer->Rewind();
		file.Read(*tmpimgbuffer);

		if (framebmp != null) {
			delete framebmp;
		}

		framebmp = new Osp::Graphics::Bitmap();
		framebmp = pimagedecoder_->DecodeN(*tmpimgbuffer, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_ARGB8888, vresolution.width, vresolution.height);
		framecanvas->DrawBitmap(Point(0,0),*framebmp);

		curfilepos += sizeof(frameheader);
		curfilepos += frameheader.size;

		curposproc = (double)(curframe-1) * curposprocfactor;
		if (curframe == fileinfo_.noofframes) {
			file.Seek(FILESEEKPOSITION_BEGIN, sizeof(fileformatdata));
			curfilepos = sizeof(fileformatdata);
		}

		/*if (tmpframebuffer != null) {
			delete tmpframebuffer;
		}
		int imgw, imgh;

		tmpframebuffer = pimagedecoder_->DecodeToBufferN(*tmpimgbuffer, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_ARGB8888, imgw, imgh);

		curfilepos += sizeof(frameheader);
		curfilepos += frameheader.size;

		curposproc = (double)(curframe-1) * curposprocfactor;

		if (curframe == fileinfo_.noofframes) {
			file.Seek(FILESEEKPOSITION_BEGIN, sizeof(fileformatdata));
			curfilepos = sizeof(fileformatdata);
		}

		BufferInfo * _pBuffer = new BufferInfo();
		framecanvas->Lock(*_pBuffer);
		int * colorout;
		tmpframebuffer->Rewind();
		for (int y=0;y<_pBuffer->height;y++) {
			for (int x=0;x<_pBuffer->width;x++) {
				colorout = ((int *)((byte *) _pBuffer->pPixels + y * _pBuffer->pitch + x *4));
				tmpframebuffer->GetInt(*colorout);
			}
		}
		framecanvas->Unlock();
		delete _pBuffer;*/

		/*Osp::System::SystemTime::GetTicks(benchmarktimeend);
		AppLog("benchmark %d", (int)(benchmarktimeend-benchmarktimestart));*/

		Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FPlayer* pPlayerForm = static_cast<FPlayer*>(pFrame->GetControl(L"FPlayer"));
		pPlayerForm->SendUserEvent(FPlayer::THREADCALLBACK_REDRAW, null);

		return true;
	} else {
		return false;
	}


}

void
FPlayerThread::OnTimerExpired(Timer& timer) {
	timerhasstarted = false;
	GetNextFrame();
}

bool
FPlayerThread::OnStart(void)
{

	timerhasstarted = false;

	pTimer_ = new Timer;

	pTimer_->Construct(*this);

	TimeLapseClass_ = new TimeLapseClass();

	pimagedecoder_ = new Image();
	pimagedecoder_->Construct();

	tmpimgbuffer = new Osp::Base::ByteBuffer();
	tmpimgbuffer->Construct(1048576);

	tmpframebuffer = null;

	framebmp = null;

	curfilepos = 0;

	TimeLapseClass_->ReadTLVfileinfo(filename, fileinfo_);
	framescount = fileinfo_.noofframes;
	startframetime = fileinfo_.datetime;
	endframetime = fileinfo_.datetimeend;
	curframesize = 0;
	resolution.SetSize(fileinfo_.resolution.width,fileinfo_.resolution.height);
	vresolution.SetSize(fileinfo_.resolution.width,fileinfo_.resolution.height);

	if (vresolution.width == 800) {
		vresolution.SetSize(vresolution.width/2,vresolution.height/2);
	}

	framecanvas = new Osp::Graphics::Canvas();
	framecanvas->Construct(Rectangle(0,0,fileinfo_.resolution.width,fileinfo_.resolution.height));
	framecanvas->Clear();

	FileAttributes attr;
	File::GetAttributes(filename, attr);
	filesize = attr.GetFileSize();

	file.Construct(filename, L"r+");
	file.Read(&fileformatdata, sizeof(fileformatdata));

	curfilepos += sizeof(fileformatdata);

	curposproc = 0;
	curposprocfactor = 1.0f / (double)(fileinfo_.noofframes-1);
	curframefilepos = 0;

	frameindexlist = new Osp::Base::Collection::ArrayList();
	frameindexlist->Construct(fileinfo_.noofframes);

	if (fileinfo_.playframerate > 20)  {
		skipframes = 2;
	} else if (fileinfo_.playframerate > 10)  {
		skipframes = 1;
	} else {
		skipframes = 0;
	}

	frameinterval = (1000 / fileinfo_.playframerate);

	curframe = 0;
	playing = false;
	GetNextFrame();

	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FPlayer* pPlayerForm = static_cast<FPlayer*>(pFrame->GetControl(L"FPlayer"));
	pPlayerForm->SendUserEvent(FPlayer::THREADCALLBACK_FILELOADED, null);

	return true;
}

bool FPlayerThread::SetFrameRate(int framerate) {

	fileinfo_.playframerate = framerate;
	if (fileinfo_.playframerate > 20)  {
		skipframes = 2;
	} else if (fileinfo_.playframerate > 10)  {
		skipframes = 1;
	} else {
		skipframes = 0;
	}
	frameinterval = (1000 / fileinfo_.playframerate);

	file.Seek(FILESEEKPOSITION_BEGIN, 0);

	fileformatdata.playframerate = framerate;
	file.Write(&fileformatdata, sizeof(fileformatdata));
	file.Flush();

	file.Seek(FILESEEKPOSITION_BEGIN, curfilepos);

	return true;
}

void
FPlayerThread::OnStop(void)
{
	if (timerhasstarted == true) {
		pTimer_->Cancel();
		timerhasstarted = false;
	}
	delete pTimer_;
	pTimer_ = null;

	delete tmpimgbuffer;
	tmpimgbuffer = null;
	delete tmpframebuffer;
	tmpframebuffer = null;
	delete framecanvas;
	framecanvas = null;
	delete framebmp;
	framebmp = null;
	delete pimagedecoder_;
	pimagedecoder_ = null;
	delete TimeLapseClass_;
	TimeLapseClass_ = null;

	frameindexlist->RemoveAll(true);
	delete frameindexlist;
}

void
FPlayerThread::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == THREAD_FRAMEDRAWED) {
		if (timerhasstarted == true) {
			pTimer_->Cancel();
			timerhasstarted = false;
		}
		if (playing == true) {
			if (skipframes > 0) {
				for (int skipi=0;skipi<skipframes;skipi++) {
					SkipFrame();
				}
			}
			long long fpscontrol_lasttimeold = fpscontrol_lasttime;
			Osp::System::SystemTime::GetTicks(fpscontrol_lasttime);
			int delay = (frameinterval*(skipframes+1))-(fpscontrol_lasttime-fpscontrol_lasttimeold);
			//AppLog("set delay %d", delay);
			if (delay < 0) delay = 0;
			if (delay > 0) {
				timerhasstarted = true;
				pTimer_->Start(delay);
			} else {
				delay = 10;
				timerhasstarted = true;
				pTimer_->Start(delay);
				//GetNextFrame();
			}
		}
	} else if (requestId == THREAD_PLAYPAUSE) {
		if (playing == true) {
			playing = false;
			if (timerhasstarted == true) {
				pTimer_->Cancel();
				timerhasstarted = false;
			}
		} else {
			Osp::System::SystemTime::GetTicks(fpscontrol_lasttime);
			playing = true;
			Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FPlayer* pPlayerForm = static_cast<FPlayer*>(pFrame->GetControl(L"FPlayer"));
			pPlayerForm->SendUserEvent(FPlayer::THREADCALLBACK_REDRAW, null);
		}
	} else if (requestId == THREAD_PREVFRAME) {
		if (curframe > 1) {
			//get prev frame position - loc file on that position
			LongLong * plnglng = static_cast<LongLong*> (frameindexlist->GetAt((curframe-2)));
			curfilepos = plnglng->ToLongLong();
			file.Seek(FILESEEKPOSITION_BEGIN, curfilepos);
			GetNextFrame();
		}
	} else if (requestId == THREAD_NEXTFRAME) {
		if (curframe < fileinfo_.noofframes) {
			GetNextFrame();
		}
	}
}
