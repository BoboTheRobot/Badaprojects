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
 * Name        : CarConNew
 * Version     : 
 * Vendor      : 
 * Description : 
 */


#include "CarConNew.h"
#include "CarConClass.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

CarConNew::CarConNew()
{
}

CarConNew::~CarConNew()
{

}

Application*
CarConNew::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new CarConNew();
}

bool
CarConNew::OnAppInitializing(AppRegistry& appRegistry)
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

	if (pFormMgr->carconclass_->isfiststart) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_WELLCOMEFORM, null);
	} else if (pFormMgr->carconclass_->SelectedCar.ID == 0) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CARSELECTFORM, null);
	} else {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}

	return true;
}

bool
CarConNew::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	// TODO:
	// Deallocate resources allocated by this application for termination.
	// The application's permanent data and context can be saved via appRegistry.

	return true;
}

void
CarConNew::OnForeground(void)
{
	// TODO:
	// Start or resume drawing when the application is moved to the foreground.
}

void
CarConNew::OnBackground(void)
{
	// TODO:
	// Stop drawing when the application is moved to the background.
}

void
CarConNew::OnLowMemory(void)
{
	// TODO:
	// Free unused resources or close the application.
}

void
CarConNew::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO:
	// Handle any changes in battery level here.
	// Stop using multimedia features(camera, mp3 etc.) if the battery level is CRITICAL.
}

void
CarConNew::OnScreenOn (void)
{
	// TODO:
	// Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
}

void
CarConNew::OnScreenOff (void)
{
	// TODO:
	//  Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode to save the battery.
	// Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a callback before the device enters the sleep mode.
	// Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
}
