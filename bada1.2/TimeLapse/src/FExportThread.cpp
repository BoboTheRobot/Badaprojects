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
 * FExportThread.cpp
 *
 *  Created on: 18.6.2011
 *      Author: mrak
 */

#include "FExportThread.h"
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

FExportThread::FExportThread() {


}

FExportThread::~FExportThread() {

}

result
FExportThread::Construct()
{
	result r = E_SUCCESS;
	r = Thread::Construct(THREAD_TYPE_EVENT_DRIVEN);

	return r;
}

bool
FExportThread::OnStart(void)
{

	TimeLapseClass_ = new TimeLapseClass();

	return true;
}

void
FExportThread::OnStop(void)
{

	delete TimeLapseClass_;
	TimeLapseClass_ = null;

}

void
FExportThread::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FPlayer* pPlayerForm = static_cast<FPlayer*>(pFrame->GetControl(L"FPlayer"));
	if (requestId == THREAD_EXPORTMJPEG) {
		TimeLapseClass_->ExportToMJPEGfile(this->filename, this->filenameout);
		pPlayerForm->SendUserEvent(FPlayer::THREADCALLBACK_EXPORTDONE, null);
	} else if (requestId == THREAD_EXPORTFRAME) {
		TimeLapseClass_->ExportToJPEGframefile(this->filename, this->framefilepos, this->filenameout);
		pPlayerForm->SendUserEvent(FPlayer::THREADCALLBACK_EXPORTDONE, null);
	}
}
