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
 * Name        : TimeLapse
 * Version     : 
 * Vendor      : Bostjan Mrak
 * Description : 
 */


#include "TimeLapse.h"
#include "FormMgr.h"
#include "FPlayer.h"
#include "FCameraForm.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

TimeLapse::TimeLapse()
{
}

TimeLapse::~TimeLapse()
{
}

Application*
TimeLapse::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new TimeLapse();
}

bool
TimeLapse::OnAppInitializing(AppRegistry& appRegistry)
{
	FormMgr *pFormMgr = new FormMgr();
	pFormMgr->Initialize();
	GetAppFrame()->GetFrame()->AddControl(*pFormMgr);

	pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	
	return true;
}

bool
TimeLapse::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	// TODO:
	// Deallocate resources allocated by this application for termination.
	// The application's permanent data and context can be saved via appRegistry.
	return true;
}

void
TimeLapse::OnForeground(void)
{
	// TODO:
	// Start or resume drawing when the application is moved to the foreground.
}

void
TimeLapse::OnBackground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FPlayer") {
		FPlayer * pPlayerForm = static_cast<FPlayer*>(pFrame->GetCurrentForm());
		pPlayerForm->Background();
	}
	if (pFrame->GetCurrentForm()->GetName() == L"FCameraForm") {
		FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
		pCameraForm->Interupt(pCameraForm->INTERUPTED_BACKGROUND);
	}
}

void
TimeLapse::OnLowMemory(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCameraForm") {
		FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
		pCameraForm->Interupt(pCameraForm->INTERUPTED_MEMORY);
	}
}

void
TimeLapse::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	if ((batteryLevel == BATTERY_CRITICAL) || (batteryLevel == BATTERY_EMPTY)) {
		Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		if (pFrame->GetCurrentForm()->GetName() == L"FCameraForm") {
			FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
			pCameraForm->Interupt(pCameraForm->INTERUPTED_BATTERY);
		}
	}
}

void
TimeLapse::OnScreenOn (void)
{
	// TODO:
	// Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
}

void
TimeLapse::OnScreenOff (void)
{
	// TODO:
	//  Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode to save the battery.
	// Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a callback before the device enters the sleep mode.
	// Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
}
