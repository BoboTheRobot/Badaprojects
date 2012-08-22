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
 * Name        : PD_monitor
 * Version     : 
 * Vendor      : Bostjan Mrak
 * Description : Control your packet data download rate
 */


#include "PD_monitor.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

PD_monitor::PD_monitor()
{
}

PD_monitor::~PD_monitor()
{
}

Application*
PD_monitor::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new PD_monitor();
}

bool
PD_monitor::OnAppInitializing(AppRegistry& appRegistry)
{
	bool isautostart = false;
	Osp::Base::Collection::IList * startarglist = new Osp::Base::Collection::ArrayList();
	Application * thisapp = Application::GetInstance();
	startarglist = thisapp->GetAppArgumentListN();
	String * arg1 = static_cast<String*>(startarglist->GetAt(1));
	isautostart = (arg1->Equals(L"osp.appcontrol.CONDMGR", false));
	startarglist->RemoveAll(true);
	delete startarglist;

	FormMgr *pFormMgr = new FormMgr();
	pFormMgr->Initialize();
	GetAppFrame()->GetFrame()->AddControl(*pFormMgr);

	if (isautostart == true) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_RESETC, null);
	} else {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}

	return true;
}

bool
PD_monitor::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	DateTime today, nextrundate;

	Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, today);
	int dayinm = 1;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"day", dayinm) != E_SUCCESS) {
		dayinm = 1;
	}
	nextrundate.SetValue(today.GetYear(),today.GetMonth(),dayinm,0,0,0);
	if (today.GetDay() >= dayinm) {
		nextrundate.AddMonths(1);
	}

	AppManager * appmgr = Osp::App::AppManager::GetInstance();
	if (appmgr->IsAppLaunchRegistered() == true) {
		appmgr->UnregisterAppLaunch();
	}
	appmgr->RegisterAppLaunch(L"DateTime='" + nextrundate.ToString() + "'", null, AppManager::LAUNCH_OPTION_DEFAULT);

	AppLog("set next run %S", nextrundate.ToString().GetPointer());

	return true;
}

void
PD_monitor::OnForeground(void)
{
	// TODO:
	// Start or resume drawing when the application is moved to the foreground.
}

void
PD_monitor::OnBackground(void)
{
	// TODO:
	// Stop drawing when the application is moved to the background.
}

void
PD_monitor::OnLowMemory(void)
{
	// TODO:
	// Free unused resources or close the application.
}

void
PD_monitor::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO:
	// Handle any changes in battery level here.
	// Stop using multimedia features(camera, mp3 etc.) if the battery level is CRITICAL.
}

void
PD_monitor::OnScreenOn (void)
{
	// TODO:
	// Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
}

void
PD_monitor::OnScreenOff (void)
{
	// TODO:
	//  Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode to save the battery.
	// Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a callback before the device enters the sleep mode.
	// Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
}

void PD_monitor::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs) {
	if (requestId == -65535) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_RESETCNOW, null);
	}
}
