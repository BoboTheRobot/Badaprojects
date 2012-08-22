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
 * Name        : VLC_remote
 * Version     : 
 * Vendor      : Bostjan Mrak
 * Description : 
 */


#include "VLC_remote.h"
#include "FMainForm.h"
#include "FClientsForm.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

VLC_remote::VLC_remote()
{
}

VLC_remote::~VLC_remote()
{
}

Application*
VLC_remote::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new VLC_remote();
}

bool
VLC_remote::OnAppInitializing(AppRegistry& appRegistry)
{
	FormMgr *pFormMgr = new FormMgr();
	pFormMgr->Initialize();
	GetAppFrame()->GetFrame()->AddControl(*pFormMgr);

	pFormMgr->SendUserEvent(FormMgr::REQUEST_CLIENTSFORMSTART, null);

	return true;
}

bool
VLC_remote::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	// TODO:
	// Deallocate resources allocated by this application for termination.
	// The application's permanent data and context can be saved via appRegistry.
	return true;
}

void
VLC_remote::OnForeground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetControl("FormMgr") != null) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->publicfunc_->StartListening();
	}
	if (pFrame->GetCurrentForm()->GetName() == L"FClientsForm") {
		FClientsForm * pClientsForm = static_cast<FClientsForm*>(pFrame->GetCurrentForm());
		pClientsForm->CheckingClients();
	} else if ((pFrame->GetControl("FMainForm") != null) && ((pFrame->GetCurrentForm()->GetName() == L"FMainForm") || (pFrame->GetCurrentForm()->GetName() == L"FPlaylistForm") || (pFrame->GetCurrentForm()->GetName() == L"FBrowseForm") || (pFrame->GetCurrentForm()->GetName() == L"FHelpForm"))) {
		FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
		pMainForm->Foreground();
	}
}

void
VLC_remote::OnBackground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetControl("FormMgr") != null) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->publicfunc_->StopListening();
	}
	if (pFrame->GetCurrentForm()->GetName() == L"FClientsForm") {
		FClientsForm * pClientsForm = static_cast<FClientsForm*>(pFrame->GetCurrentForm());
		pClientsForm->CheckingClientsStop();
	} else if ((pFrame->GetControl("FMainForm") != null) && ((pFrame->GetCurrentForm()->GetName() == L"FMainForm") || (pFrame->GetCurrentForm()->GetName() == L"FPlaylistForm") || (pFrame->GetCurrentForm()->GetName() == L"FBrowseForm") || (pFrame->GetCurrentForm()->GetName() == L"FHelpForm"))) {
		FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
		pMainForm->Background();
	}
}

void
VLC_remote::OnLowMemory(void)
{
	// TODO:
	// Free unused resources or close the application.
}

void
VLC_remote::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO:
	// Handle any changes in battery level here.
	// Stop using multimedia features(camera, mp3 etc.) if the battery level is CRITICAL.
}

void
VLC_remote::OnScreenOn (void)
{
	// TODO:
	// Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
}

void
VLC_remote::OnScreenOff (void)
{
	// TODO:
	//  Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode to save the battery.
	// Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a callback before the device enters the sleep mode.
	// Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
}
