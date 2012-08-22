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
 * FormMgr.cpp
 *
 *  Created on: 16.8.2010
 *      Author: mrak
 */
#include "FormMgr.h"
#include "FMainForm.h"
#include "FSettings.h"
#include "FAbout.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::System;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Net;

FormMgr::FormMgr(void) :
__pPreviousForm(null)
{
}

FormMgr::~FormMgr(void)
{
}

bool
FormMgr::Initialize(void)
{
	result r = E_SUCCESS;
	r = Form::Construct(FORM_STYLE_NORMAL);
	SetName(L"FormMgr");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	__pPreviousForm = null;

	return true;
}

result
FormMgr::OnInitializing(void)
{
	result r = E_SUCCESS;
	drawingclass_ = new DrawingClass(L"/Res/sprite.png");

	prevform = null;
	curform = null;

	resetcshutdown_ = null;

	anistep = 0;
	anistepc = 0;

	anitimer_ = new Osp::Base::Runtime::Timer;
	anitimer_->Construct(*this);

	TransitionDirection = TRANSITIONRIGHT;

	return r;
}

result
FormMgr::OnTerminating(void)
{
	result r = E_SUCCESS;
	
	anitimer_->Cancel();
	delete anitimer_;

	if (resetcshutdown_ != null) {
		resetcshutdown_->Cancel();
	}
	delete resetcshutdown_;

	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (__pPreviousForm != null)
		pFrame->RemoveControl(*__pPreviousForm);
	__pPreviousForm = null;

	delete drawingclass_;

	delete prevform;
	delete curform;

	return r;
}

result
FormMgr::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		if ((prevform != null) && (curform != null)) {
			if (TransitionDirection == TRANSITIONLEFT) {
				int pf = (480.0f * (anistepc));
				if (pf <= 480) {
					pCanvas_->DrawBitmap(Rectangle(0,0,pf,800), *curform, Rectangle(480-pf,0,pf,800));
				}
				int cf = (480.0f * (1-anistepc));
				if (cf > 0) {
					pCanvas_->DrawBitmap(Rectangle(480-cf,0,cf,800), *prevform, Rectangle(0,0,cf,800));
				}
			} else {
				int pf = (480.0f * (1-anistepc));
				if (pf <= 480) {
					pCanvas_->DrawBitmap(Rectangle(0,0,pf,800), *prevform, Rectangle(480-pf,0,pf,800));
				}
				int cf = (480.0f * anistepc);
				if (cf > 0) {
					pCanvas_->DrawBitmap(Rectangle(480-cf,0,cf,800), *curform, Rectangle(0,0,cf,800));
				}
			}
		} else if (prevform != null) {
			pCanvas_->DrawBitmap(Rectangle(0,0,480,800), *prevform, Rectangle(0,0,480,800));
		} else {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,80,440,200), L"Restarting counter...please wait.\n\nApplication will shutdown in 5 sec!", 30, Osp::Graphics::FONT_STYLE_BOLD, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_TOP);
			int pw = (440.0f * (anistepc));
			pCanvas_->SetForegroundColor(Color(0,255,0));
			pCanvas_->DrawRectangle(Rectangle(20,280,440,50));
			if (pw > 0) {
				pCanvas_->FillRectangle(Color(0,255,0), Rectangle(20,280,pw,50));
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void FormMgr::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (anitimer_->Equals(timer)) {
		Osp::System::SystemTime::GetTicks(anicurtime);
		int timepassed = anicurtime - anilasttime;
		double anistepproc = (double)timepassed / 500.0f;
		anilasttime = anicurtime;
		anistep += anistepproc;
		if (anistep > 1) anistep = 1;
		double anipowf = 1.5f;

		anistepc = Osp::Base::Utility::Math::Pow(anistep, anipowf*2); //easeout

		RequestRedraw();
		if (anistep == 1) {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			pFrame->SetCurrentForm(*__pPreviousForm);
			__pPreviousForm->Draw();
			__pPreviousForm->Show();
			__pPreviousForm->SendUserEvent(FormMgr::CALLBACKDONE, null);
		} else {
			anitimer_->Start(20);
		}
	} else {
		Osp::System::SystemTime::GetTicks(anicurtime);
		int timepassed = anicurtime - anilasttime;
		double anistepproc = (double)timepassed / 5000.0f;
		anilasttime = anicurtime;
		anistep += anistepproc;
		if (anistep > 1) anistep = 1;
		anistepc = anistep;
		RequestRedraw();
		if (anistep == 1) {
			Application::GetInstance()->Terminate();
		} else {
			resetcshutdown_->Start(100);
		}
	}
}

void FormMgr::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	SwitchToForm(requestId, pArgs);
}

void
FormMgr::SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();

	if (requestId == REQUEST_MAINFORM) {
		if (prevform == null) {
			FMainForm* pExeForm = new FMainForm();
			pExeForm->Initialize();
			pFrame->AddControl(*pExeForm);
			pFrame->SetCurrentForm(*pExeForm);
			pExeForm->Draw();
			pExeForm->Show();
			pExeForm->SendUserEvent(FormMgr::CALLBACKDONE, null);
			if (__pPreviousForm != null)
				pFrame->RemoveControl(*__pPreviousForm);
			__pPreviousForm = pExeForm;
		} else {
			delete prevform;
			delete curform;
			prevform = __pPreviousForm->GetCapturedBitmapN();
			FMainForm* pExeForm = new FMainForm();
			pExeForm->Initialize();
			pFrame->AddControl(*pExeForm);
			pExeForm->Draw();
			curform = pExeForm->GetCapturedBitmapN();
			if (__pPreviousForm != null)
				pFrame->RemoveControl(*__pPreviousForm);
			__pPreviousForm = pExeForm;

			anistep = 0;
			anistepc = 0;
			this->Draw();
			pFrame->SetCurrentForm(*this);
			this->RequestRedraw();
			Osp::System::SystemTime::GetTicks(anilasttime);
			anitimer_->Start(20);
		}
	} else if (requestId == REQUEST_SETTINGS) {
		delete prevform;
		delete curform;
		prevform = __pPreviousForm->GetCapturedBitmapN();
		FSettings* pExeForm = new FSettings();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pExeForm->Draw();
		curform = pExeForm->GetCapturedBitmapN();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;

		anistep = 0;
		anistepc = 0;
		this->Draw();
		pFrame->SetCurrentForm(*this);
		this->RequestRedraw();
		Osp::System::SystemTime::GetTicks(anilasttime);
		anitimer_->Start(20);
	} else if (requestId == REQUEST_ABOUT) {
		delete prevform;
		delete curform;
		prevform = __pPreviousForm->GetCapturedBitmapN();
		FAbout* pExeForm = new FAbout();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pExeForm->Draw();
		curform = pExeForm->GetCapturedBitmapN();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;

		anistep = 0;
		anistepc = 0;
		this->Draw();
		pFrame->SetCurrentForm(*this);
		this->RequestRedraw();
		Osp::System::SystemTime::GetTicks(anilasttime);
		anitimer_->Start(20);
	} else if (requestId == REQUEST_RESETC) {
		resetcshutdown_ = new Osp::Base::Runtime::Timer;
		resetcshutdown_->Construct(*this);
		this->Draw();
		pFrame->SetCurrentForm(*this);
		this->RequestRedraw();
		NetStatistics * netstat = new NetStatistics();
		netstat->Construct();
		netstat->ResetAll(NET_BEARER_PS);
		delete netstat;
		Osp::System::SystemTime::GetTicks(anilasttime);
		resetcshutdown_->Start(100);
		NotificationManager* pNotiMgr = new NotificationManager();
        pNotiMgr->Construct();
        DateTime today;
        Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, today);
        pNotiMgr->Notify(L"Reset at " + today.ToString());
        delete pNotiMgr;
    } else if (requestId == REQUEST_RESETCNOW) {
		NetStatistics * netstat = new NetStatistics();
		netstat->Construct();
		netstat->ResetAll(NET_BEARER_PS);
		delete netstat;
		NotificationManager* pNotiMgr = new NotificationManager();
        pNotiMgr->Construct();
        DateTime today;
        Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, today);
        pNotiMgr->Notify(L"Reset at " + today.ToString());
        delete pNotiMgr;
	}
}
