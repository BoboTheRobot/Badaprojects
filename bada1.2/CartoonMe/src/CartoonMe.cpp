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
/**
 * Name        : CartoonMe
 * Version     : 
 * Vendor      : Bostjan Mrak
 * Description : 
 */


#include "CartoonMe.h"
#include "FormMgr.h"
#include "FCamera.h"
#include "FCartoonMe.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

CartoonMe::CartoonMe()
{
}

CartoonMe::~CartoonMe()
{
}

Application*
CartoonMe::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new CartoonMe();
}

bool
CartoonMe::OnAppInitializing(AppRegistry& appRegistry)
{
	// Create a form
	FormMgr *pFormMgr = new FormMgr();
	pFormMgr->Initialize();
	GetAppFrame()->GetFrame()->AddControl(*pFormMgr);

	pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);

	return true;
}

bool
CartoonMe::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	AppLog("terminating");
	return true;
}

void
CartoonMe::OnForeground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCamera") {
		AppLog("foreground");
		FCamera * pCameraForm = static_cast<FCamera*>(pFrame->GetCurrentForm());
		pCameraForm->ResumeCam();
	}
	if (pFrame->GetCurrentForm()->GetName() == L"FCartoonMe") {
		FCartoonMe * pCartoonMeForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());
		pCartoonMeForm->BackgroundResume();
	}
}

void
CartoonMe::OnBackground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCamera") {
		AppLog("background");
		FCamera * pCameraForm = static_cast<FCamera*>(pFrame->GetCurrentForm());
		pCameraForm->PauseCam();
	}
	if (pFrame->GetCurrentForm()->GetName() == L"FCartoonMe") {
		FCartoonMe * pCartoonMeForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());
		pCartoonMeForm->BackgroundPause();
	}
}

void
CartoonMe::OnLowMemory(void)
{
	/*Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCamera") {
		FCamera * pCameraForm = static_cast<FCamera*>(pFrame->GetCurrentForm());
		pCameraForm->TerminateCam();
	}*/
}

void
CartoonMe::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	/*if (batteryLevel == BATTERY_CRITICAL) {
		Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		if (pFrame->GetCurrentForm()->GetName() == L"FCamera") {
			FCamera * pCameraForm = static_cast<FCamera*>(pFrame->GetCurrentForm());
			pCameraForm->TerminateCam();
		}
	}*/
}

void
CartoonMe::OnScreenOn (void)
{

}

void
CartoonMe::OnScreenOff (void)
{

}
