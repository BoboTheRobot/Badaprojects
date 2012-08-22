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
 * Name        : Camera3D
 * Version     : 
 * Vendor      : 
 * Description : 
 */


#include "Camera3D.h"
#include "FormMgr.h"
#include "FCameraForm.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

Camera3D::Camera3D()
{
}

Camera3D::~Camera3D()
{
}

Application*
Camera3D::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new Camera3D();
}

bool
Camera3D::OnAppInitializing(AppRegistry& appRegistry)
{
	// TODO:
	// Initialize UI resources and application specific data.
	// The application's permanent data and context can be obtained from the appRegistry.
	//
	// If this method is successful, return true; otherwise, return false.
	// If this method returns false, the application will be terminated.

	// Uncomment the following statement to listen to the screen on/off events.
	//PowerManager::SetScreenEventListener(*this);

	FormMgr *pFormMgr = new FormMgr();
	pFormMgr->Initialize();
	GetAppFrame()->GetFrame()->AddControl(*pFormMgr);

	pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);

	return true;
}

bool
Camera3D::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	return true;
}

void
Camera3D::OnForeground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"CameraForm") {
		AppLog("foreground");
		FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
		pCameraForm->ResumeCam();
	}
}

void
Camera3D::OnBackground(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"CameraForm") {
		AppLog("background");
		FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
		pCameraForm->PauseCam();
	}
}

void
Camera3D::OnLowMemory(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"CameraForm") {
		FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
		pCameraForm->TerminateCam();
	}
}

void
Camera3D::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{

	if (batteryLevel == BATTERY_CRITICAL) {
		Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		if (pFrame->GetCurrentForm()->GetName() == L"CameraForm") {
			FCameraForm * pCameraForm = static_cast<FCameraForm*>(pFrame->GetCurrentForm());
			pCameraForm->TerminateCam();
		}
	}

}

void
Camera3D::OnScreenOn (void)
{

}

void
Camera3D::OnScreenOff (void)
{

}
