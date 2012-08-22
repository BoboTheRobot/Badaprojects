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
#include "FSettings.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Locales;

FSettings::FSettings(int formtype)
{
	this->formtype = formtype;
	this->firstdraw = true;
}

FSettings::~FSettings(void)
{
}

bool
FSettings::Initialize()
{
	Form::Construct(L"IDF_FSETTINGS");

	return true;
}

result
FSettings::OnInitializing(void)
{
	result r = E_SUCCESS;
	pSettingsThread_ = null;
	pPopupProgress_ = null;

	SetSoftkeyActionId(SOFTKEY_0, ID_SAVEBTN);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	SetSoftkeyActionId(SOFTKEY_1, ID_BACKBTN);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(10);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	if (this->formtype == 2) {
	SetSoftkeyText(SOFTKEY_0, L"Next");
	SetSoftkeyText(SOFTKEY_1, L"Back");
	}

	carconclass_ = pFormMgr->carconclass_;
	carconclass_->GetSettingsData(settingsdata_);

	String tmps;
	String tmpssmall;
	int itemindex;
	Osp::Ui::Controls::Button *pPopupSelectListAddButton_;
	Osp::Ui::Controls::Button *pPopupSelectListCancelButton_;

	pPopupSelectVolumeUnit_ = new Popup();
	pPopupSelectVolumeUnit_->Construct(L"IDP_POPUP2");
	pPopupSelectVolumeUnit_->SetTitleText(L"Select volume unit");
	pPopupSelectVolumeUnitList_ = static_cast<List *>(pPopupSelectVolumeUnit_->GetControl(L"IDC_LIST1"));

	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_VOLUME, SETTINGSVOLUMEUNIT_GALON, tmpssmall, tmps);
	tmps = tmps + " [" + tmpssmall + "]";
	pPopupSelectVolumeUnitList_->AddItem(&tmps, null, null, null, SETTINGSVOLUMEUNIT_GALON);
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_VOLUME, SETTINGSVOLUMEUNIT_LITER, tmpssmall, tmps);
	tmps = tmps + " [" + tmpssmall + "]";
	pPopupSelectVolumeUnitList_->AddItem(&tmps, null, null, null, SETTINGSVOLUMEUNIT_LITER);

	pPopupSelectListAddButton_ = static_cast<Button *>(pPopupSelectVolumeUnit_->GetControl(L"IDC_BUTTON2"));
	pPopupSelectListAddButton_->SetShowState(false);
	pPopupSelectListCancelButton_ = static_cast<Button *>(pPopupSelectVolumeUnit_->GetControl(L"IDC_BUTTON1"));
	pPopupSelectListCancelButton_->SetActionId(ID_POPUPCANCEL);
	pPopupSelectListCancelButton_->AddActionEventListener(*this);
	pPopupSelectVolumeUnitList_->AddItemEventListener(*this);
	itemindex = pPopupSelectVolumeUnitList_->GetItemIndexFromItemId(settingsdata_.volumeunit);
	if (itemindex > -1) {
		pPopupSelectVolumeUnitList_->SetItemChecked(itemindex, true);
		PopupSelect(SELECTTYPE_VOLUME, settingsdata_.volumeunit);
	}

	pPopupSelectDistanceUnit_ = new Popup();
	pPopupSelectDistanceUnit_->Construct(L"IDP_POPUP2");
	pPopupSelectDistanceUnit_->SetTitleText(L"Select distance unit");
	pPopupSelectDistanceUnitList_ = static_cast<List *>(pPopupSelectDistanceUnit_->GetControl(L"IDC_LIST1"));

	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_DISTANCE, SETTINGSDISTANCEUNIT_MILE, tmpssmall, tmps);
	tmps = tmps + " [" + tmpssmall + "]";
	pPopupSelectDistanceUnitList_->AddItem(&tmps, null, null, null, SETTINGSDISTANCEUNIT_MILE);
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_DISTANCE, SETTINGSDISTANCEUNIT_KM, tmpssmall, tmps);
	tmps = tmps + " [" + tmpssmall + "]";
	pPopupSelectDistanceUnitList_->AddItem(&tmps, null, null, null, SETTINGSDISTANCEUNIT_KM);

	pPopupSelectListAddButton_ = static_cast<Button *>(pPopupSelectDistanceUnit_->GetControl(L"IDC_BUTTON2"));
	pPopupSelectListAddButton_->SetShowState(false);
	pPopupSelectListCancelButton_ = static_cast<Button *>(pPopupSelectDistanceUnit_->GetControl(L"IDC_BUTTON1"));
	pPopupSelectListCancelButton_->SetActionId(ID_POPUPCANCEL);
	pPopupSelectListCancelButton_->AddActionEventListener(*this);
	pPopupSelectDistanceUnitList_->AddItemEventListener(*this);
	itemindex = pPopupSelectDistanceUnitList_->GetItemIndexFromItemId(settingsdata_.distanceunit);
	if (itemindex > -1) {
		pPopupSelectDistanceUnitList_->SetItemChecked(itemindex, true);
		PopupSelect(SELECTTYPE_DISTANCE, settingsdata_.distanceunit);
	}

	pPopupSelectConUnit_ = new Popup();
	pPopupSelectConUnit_->Construct(L"IDP_POPUP2");
	pPopupSelectConUnit_->SetTitleText(L"Select consumption unit");
	pPopupSelectConUnitList_ = static_cast<List *>(pPopupSelectConUnit_->GetControl(L"IDC_LIST1"));

	fillupselectconunit();

	pPopupSelectListAddButton_ = static_cast<Button *>(pPopupSelectConUnit_->GetControl(L"IDC_BUTTON2"));
	pPopupSelectListAddButton_->SetShowState(false);
	pPopupSelectListCancelButton_ = static_cast<Button *>(pPopupSelectConUnit_->GetControl(L"IDC_BUTTON1"));
	pPopupSelectListCancelButton_->SetActionId(ID_POPUPCANCEL);
	pPopupSelectListCancelButton_->AddActionEventListener(*this);
	pPopupSelectConUnitList_->AddItemEventListener(*this);
	itemindex = pPopupSelectConUnitList_->GetItemIndexFromItemId(settingsdata_.contype);
	if (itemindex > -1) {
		pPopupSelectConUnitList_->SetItemChecked(itemindex, true);
		PopupSelect(SELECTTYPE_CON, settingsdata_.contype);
	} else {
		int defcarconid = carconclass_->GetSettingsDataGetDefaultCon(settingsdata_.volumeunit, settingsdata_.distanceunit);
		itemindex = pPopupSelectConUnitList_->GetItemIndexFromItemId(defcarconid);
		if (itemindex > -1) {
			pPopupSelectConUnitList_->SetItemChecked(itemindex, true);
			PopupSelect(SELECTTYPE_CON, settingsdata_.contype);
		}
	}

	pPopupSelectCurrency_ = new Popup();
	pPopupSelectCurrency_->Construct(L"IDP_POPUP2");
	pPopupSelectCurrency_->SetTitleText(L"Select currency unit");
	pPopupSelectCurrencyList_ = static_cast<List *>(pPopupSelectCurrency_->GetControl(L"IDC_LIST1"));

	pPopupSelectListAddButton_ = static_cast<Button *>(pPopupSelectCurrency_->GetControl(L"IDC_BUTTON2"));
	pPopupSelectListAddButton_->SetShowState(false);
	pPopupSelectListCancelButton_ = static_cast<Button *>(pPopupSelectCurrency_->GetControl(L"IDC_BUTTON1"));
	pPopupSelectListCancelButton_->SetActionId(ID_POPUPCANCEL);
	pPopupSelectListCancelButton_->AddActionEventListener(*this);
	pPopupSelectCurrencyList_->AddItemEventListener(*this);
	currencylist_ = new Osp::Base::Collection::ArrayList;

	Osp::Ui::Controls::CheckButton *formelcb_ = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON1", true));
	formelcb_->SetSelected((settingsdata_.avgcalctype == 2));

	Osp::Ui::Controls::EditField *formel_;
	formel_ = static_cast<EditField *>(GetControl(L"IDPC_EDITFIELD1", true));
	formel_->SetKeypadEnabled(false);
	formel_->AddTouchEventListener(*this);
	formel_ = static_cast<EditField *>(GetControl(L"IDPC_EDITFIELD2", true));
	formel_->SetKeypadEnabled(false);
	formel_->AddTouchEventListener(*this);
	formel_ = static_cast<EditField *>(GetControl(L"IDPC_EDITFIELD3", true));
	formel_->SetKeypadEnabled(false);
	formel_->AddTouchEventListener(*this);
	formel_ = static_cast<EditField *>(GetControl(L"IDPC_EDITFIELD4", true));
	formel_->SetKeypadEnabled(false);
	formel_->AddTouchEventListener(*this);

	Osp::Ui::Controls::Button * buttonel;
	buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON1", true));
	buttonel->SetActionId(ID_FUELTYPESBTN);
	buttonel->AddActionEventListener(*this);

	buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON2", true));
	buttonel->SetActionId(ID_LOCATIONSBTN);
	buttonel->AddActionEventListener(*this);

	buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON3", true));
	buttonel->SetActionId(ABACKUP);
	buttonel->AddActionEventListener(*this);

	buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON4", true));
	buttonel->SetActionId(ARESTORE);
	buttonel->AddActionEventListener(*this);

	buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON5", true));
	buttonel->SetActionId(ACLEARCOUNTRYLANGCACHE);
	buttonel->AddActionEventListener(*this);


	BackupExportpopup_ = new Popup();
	BackupExportpopup_->Construct(L"IDP_POPUP7");
	buttonel = static_cast<Button *>(BackupExportpopup_->GetControl(L"IDC_BUTTON1"));
	buttonel->SetActionId(ABACKUP_PHONE);
	buttonel->AddActionEventListener(*this);
	buttonel = static_cast<Button *>(BackupExportpopup_->GetControl(L"IDC_BUTTON2"));
	buttonel->SetActionId(ABACKUP_SD);
	buttonel->AddActionEventListener(*this);
	buttonel = static_cast<Button *>(BackupExportpopup_->GetControl(L"IDC_BUTTON3"));
	buttonel->SetActionId(ABACKUP_CANCEL);
	buttonel->AddActionEventListener(*this);

	Restorepopup_ = new Popup();
	Restorepopup_->Construct(L"IDP_POPUP8");
	buttonel = static_cast<Button *>(Restorepopup_->GetControl(L"IDC_BUTTON2"));
	buttonel->SetActionId(ARESTORE_SEL);
	buttonel->AddActionEventListener(*this);
	buttonel = static_cast<Button *>(Restorepopup_->GetControl(L"IDC_BUTTON1"));
	buttonel->SetActionId(ARESTORE_CANCEL);
	buttonel->AddActionEventListener(*this);

	RestorepopupFilelist_ = static_cast<List *>(Restorepopup_->GetControl(L"IDC_LIST1"));
	RestorepopupFilelist_->AddItemEventListener(*this);
	restorepopupselecteditemid_ = -1;

	RestorepopupFilelistList_ = new Osp::Base::Collection::ArrayList;

	pPopupProgress_ = null;

	if (carconclass_->GetCountryLangCodeStart()) {
		carconclass_->GetCountryLangCodeEnd();
		LoadOnShow();
	} else {
		carconclass_->GetCountryLangCodeEnd();
	}

	return r;
}

bool FSettings::LoadOnShow() {
	if (currencylist_->GetCount() <= 0) {
	if (carconclass_->GetCountryLangCodeStart()) {
		Osp::Base::String langcountrycode;
		Osp::Base::String desc;
		int itemid = 1;
		int selectedcurrencyid = -1;
		while (carconclass_->GetCountryLangCodeGetData(langcountrycode, desc)) {
			pPopupSelectCurrencyList_->AddItem(&desc, null, null, null, itemid);
			currencylist_->Add(*(new CurrencyListItem(desc, langcountrycode, itemid)));
			if (settingsdata_.currencycountrycode == langcountrycode) {
				selectedcurrencyid = itemid;
			}
			itemid++;
		}
		carconclass_->GetCountryLangCodeEnd();
		int itemindex = pPopupSelectCurrencyList_->GetItemIndexFromItemId(selectedcurrencyid);
    	if (itemindex > -1) {
    		pPopupSelectCurrencyList_->SetItemChecked(itemindex, true);
    		PopupSelect(SELECTTYPE_CURRENCY, selectedcurrencyid);
    	}
    	this->RequestRedraw();
	} else {
		if (pPopupProgress_ == null) {
			pPopupProgress_ = new Popup();
			pPopupProgress_->Construct(L"IDP_POPUP6");
		}
		Osp::Ui::Controls::Progress * progressbar = static_cast<Progress *>(pPopupProgress_->GetControl(L"IDC_PROGRESS1"));
		progressbar->SetValue(0);
		pPopupProgress_->SetShowState(true);
		pPopupProgress_->Show();
		pSettingsThread_ = new FSettingsThread();
		pSettingsThread_->Construct();
		pSettingsThread_->selectedcc = settingsdata_.currencycountrycode;
		pSettingsThread_->Start();
	}
	carconclass_->GetCountryLangCodeEnd();
	}
    return true;
}

CurrencyListItem::CurrencyListItem(Osp::Base::String name, Osp::Base::String value, int itemid) {
	this->name = name;
	this->value = value;
	this->itemid = itemid;
}

CurrencyListItem::~CurrencyListItem(void) {

}

RestoreFileListItem::RestoreFileListItem(Osp::Base::String filename, int itemid) {
	this->filename = filename;
	this->itemid = itemid;
}

RestoreFileListItem::~RestoreFileListItem(void) {

}

void FSettings::ThreadCallbackAddItem(Osp::Base::String col1, Osp::Base::String itemValue, int itemid) {
	carconclass_->AddCountryLangCodeCache(itemValue, col1);
	pPopupSelectCurrencyList_->AddItem(&col1, null, null, null, itemid);
	currencylist_->Add(*(new CurrencyListItem(col1, itemValue, itemid)));
}

void FSettings::ThreadCallbackEnd(int selectedcurrencyid, int systemdefaultlocaleid) {
	if (selectedcurrencyid > -1) {
    	int itemindex = pPopupSelectCurrencyList_->GetItemIndexFromItemId(selectedcurrencyid);
    	if (itemindex > -1) {
        pPopupSelectCurrencyList_->SetItemChecked(itemindex, true);
    	PopupSelect(SELECTTYPE_CURRENCY, selectedcurrencyid);
    	}
    } else {
    	int itemindex = pPopupSelectCurrencyList_->GetItemIndexFromItemId(systemdefaultlocaleid);
    	if (itemindex > -1) {
        pPopupSelectCurrencyList_->SetItemChecked(itemindex, true);
        PopupSelect(SELECTTYPE_CURRENCY, systemdefaultlocaleid);
    	}
    }
	pPopupProgress_->SetShowState(false);
    RequestRedraw(true);
}

void FSettings::ThreadCallbackSetB(int min, int max) {
	Osp::Ui::Controls::Progress * progressbar = static_cast<Progress *>(pPopupProgress_->GetControl(L"IDC_PROGRESS1"));
	progressbar->SetValue(0);
	progressbar->SetRange(min,max);
    progressbar->RequestRedraw(true);
}

void FSettings::ThreadCallbackProgress(int progress) {
	Osp::Ui::Controls::Progress * progressbar = static_cast<Progress *>(pPopupProgress_->GetControl(L"IDC_PROGRESS1"));
	progressbar->SetValue(progress);
	progressbar->RequestRedraw(true);
}

result
FSettings::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
	}
	if ((this->formtype == 2) && (this->firstdraw)) {
		Osp::Ui::Controls::Button * buttonelft = static_cast<Button *>(GetControl(L"IDPC_BUTTON1", true));
		buttonelft->SetShowState(false);
		Osp::Ui::Controls::Button * buttonell = static_cast<Button *>(GetControl(L"IDPC_BUTTON2", true));
		buttonell->SetShowState(false);
		Osp::Ui::Controls::Button * buttonel;
		buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON3", true));
		buttonel->SetPosition(buttonelft->GetPosition());
		buttonel = static_cast<Button *>(GetControl(L"IDPC_BUTTON4", true));
		buttonel->SetPosition(buttonell->GetPosition());
	}
	delete pCanvas_;
	return E_SUCCESS;
}

bool FSettings::Save() {
	if (((settingsdata_.volumeunit == SETTINGSVOLUMEUNIT_LITER) || (settingsdata_.volumeunit == SETTINGSVOLUMEUNIT_GALON)) && ((settingsdata_.distanceunit == SETTINGSDISTANCEUNIT_MILE) || (settingsdata_.distanceunit == SETTINGSDISTANCEUNIT_KM))) {
		if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit , settingsdata_.contype)) {
			//preveri ce se spremeni volume ali distance > opozorilo uporabniku in moznost prekalkulacije podatkov
			//1 liter = 0.264172051242 gal
			//1 km = 0.621371192237 miles
			if (settingsdata_.currencycountrycode != L"") {
				Osp::Ui::Controls::CheckButton *formelcb_ = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON1", true));
				if (formelcb_->IsSelected()) {
					settingsdata_.avgcalctype = 2;
				} else {
					settingsdata_.avgcalctype = 1;
				}
				carconclass_->SaveSettingsData(settingsdata_);
				FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
				pFormMgr->controlhandler_->Destroy();
				delete pFormMgr->controlhandler_;
				pFormMgr->controlhandler_ = new ControlHandler();
				pFormMgr->controlhandler_->Construct(settingsdata_.currencycountrycode);
				return true;
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Validate", "Select currency!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				return false;
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Invalid value", "Consumption unit is invalid!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return false;
		}
	} else {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Invalid value", "Volume or distance unit is invalid!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		return false;
	}
}

void FSettings::fillupselectconunit() {
	String tmps;
	String tmpssmall;
	pPopupSelectConUnitList_->RemoveAllItems();
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_MPG)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_MPG, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_MPG);
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_LKM)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_LKM, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_LKM);
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_GMI)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_GMI, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_GMI);
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_KML)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_KML, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_KML);
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_KMG)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_KMG, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_KMG);
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit ,SETTINGSCONTYPE_MIL)) {
	carconclass_->GetSettingsDataGetCaptions(SELECTTYPE_CON, SETTINGSCONTYPE_MIL, tmpssmall, tmps);
	pPopupSelectConUnitList_->AddItem(&tmps, &tmpssmall, null, null, SETTINGSCONTYPE_MIL);
	}
}

void FSettings::PopupSelect(int selecttype, int value) {
	Osp::Ui::Controls::ScrollPanel *panelel_;
	panelel_ = static_cast<ScrollPanel *>(GetControl(L"IDC_SCROLLPANEL1"));
	Osp::Ui::Controls::EditField *formel_ = null;
	String tmps;
	String tmpssmall;
	if (selecttype == SELECTTYPE_VOLUME)
		formel_ = static_cast<EditField *>(panelel_->GetControl(L"IDPC_EDITFIELD1"));
	if (selecttype == SELECTTYPE_DISTANCE)
		formel_ = static_cast<EditField *>(panelel_->GetControl(L"IDPC_EDITFIELD2"));
	if (selecttype == SELECTTYPE_CON)
		formel_ = static_cast<EditField *>(panelel_->GetControl(L"IDPC_EDITFIELD3"));
	if (selecttype == SELECTTYPE_CURRENCY)
		formel_ = static_cast<EditField *>(panelel_->GetControl(L"IDPC_EDITFIELD4"));
	carconclass_->GetSettingsDataGetCaptions(selecttype, value, tmpssmall, tmps);
	if (selecttype != SELECTTYPE_CURRENCY) {
	if (selecttype == SELECTTYPE_CON) {
		formel_->SetText(tmps);
	} else {
		formel_->SetText(tmps + " [" + tmpssmall + "]");
	}
	}
	if (selecttype == SELECTTYPE_VOLUME) {
	settingsdata_.volumeunit = value;
	if (pPopupSelectVolumeUnit_->IsVisible()) {
		pPopupSelectVolumeUnit_->SetShowState(false);
		RequestRedraw(true);
	}
	}
	if (selecttype == SELECTTYPE_DISTANCE) {
	settingsdata_.distanceunit = value;
	if (pPopupSelectDistanceUnit_->IsVisible()) {
		pPopupSelectDistanceUnit_->SetShowState(false);
		RequestRedraw(true);
	}
	}
	if (selecttype == SELECTTYPE_CON) {
	settingsdata_.contype = value;
	if (pPopupSelectConUnit_->IsVisible()) {
		pPopupSelectConUnit_->SetShowState(false);
		RequestRedraw(true);
	}
	}
	if (selecttype == SELECTTYPE_CURRENCY) {
		CurrencyListItem* currencyitem_;
		for (int i=0;i < currencylist_->GetCount();i++) {
			currencyitem_ = static_cast<CurrencyListItem *> (currencylist_->GetAt(i));
			if (currencyitem_->itemid == value) {
				settingsdata_.currencycountrycode = currencyitem_->value;
				formel_->SetText(currencyitem_->name);
				break;
			}
		}
		if (pPopupSelectCurrency_->IsVisible()) {
			pPopupSelectCurrency_->SetShowState(false);
			RequestRedraw(true);
		}
	}
	if (carconclass_->GetSettingsDataCheckConIfValid(settingsdata_.volumeunit, settingsdata_.distanceunit , settingsdata_.contype) == false) {
		formel_ = static_cast<EditField *>(panelel_->GetControl(L"IDPC_EDITFIELD3"));
		formel_->SetText(L"");
		if (formel_->IsVisible())
		formel_->RequestRedraw(true);
	}
}

result
FSettings::OnTerminating(void)
{
	result r = E_SUCCESS;

	if (pSettingsThread_ != null) {
	delete pSettingsThread_;
	}
	if (pPopupProgress_ != null) {
	delete pPopupProgress_;
	}
	delete pPopupSelectVolumeUnit_;
	delete pPopupSelectDistanceUnit_;
	delete pPopupSelectConUnit_;
	delete pPopupSelectCurrency_;
	currencylist_->RemoveAll(true);
	delete currencylist_;

	delete BackupExportpopup_;
	delete Restorepopup_;

	RestorepopupFilelistList_->RemoveAll(true);
	delete RestorepopupFilelistList_;

	return r;
}


void
FSettings::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		switch(actionId){
			case ID_BACKBTN: {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				if (pFormMgr != null) {
					if (this->formtype == 2) {
						pFormMgr->SendUserEvent(FormMgr::REQUEST_WELLCOMEFORM, null);
					} else {
						pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
					}
				}
				}
				break;
			case ID_SAVEBTN: {
				if (Save()) {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				if (pFormMgr != null) {
					if (this->formtype == 2) {
						ArrayList * list = new ArrayList;
						list->Construct();
						list->Add(*(new Integer(0)));
						list->Add(*(new Integer(2)));
						pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITCAR, list);
					} else {
						pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
					}
				}
				}
				}
				break;
			case ID_POPUPCANCEL: {
				if (pPopupSelectVolumeUnit_->IsVisible()) {
					pPopupSelectVolumeUnit_->SetShowState(false);
					RequestRedraw(true);
					break;
				}
				if (pPopupSelectDistanceUnit_->IsVisible()) {
					pPopupSelectDistanceUnit_->SetShowState(false);
					RequestRedraw(true);
					break;
				}
				if (pPopupSelectConUnit_->IsVisible()) {
					pPopupSelectConUnit_->SetShowState(false);
					RequestRedraw(true);
					break;
				}
				if (pPopupSelectCurrency_->IsVisible()) {
					pPopupSelectCurrency_->SetShowState(false);
					RequestRedraw(true);
					break;
				}
				break;
			}
			case ID_FUELTYPESBTN: {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_FUELTYPEFORM, null);
				break;
			}
			case ID_LOCATIONSBTN: {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_LOCATIONFORM, null);
				break;
			}
			case ABACKUP: {
				if (Osp::Io::Database::Exists(L"/Home/data.db")) {
				bool hasmediacard = false;
				hasmediacard = (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Others") == true);
				Osp::Ui::Controls::Button * saveaspopupSDCardButton_ = static_cast<Button *>(BackupExportpopup_->GetControl(L"IDC_BUTTON2"));
				saveaspopupSDCardButton_->SetEnabled(hasmediacard);
				BackupExportpopup_->SetShowState(true);
				BackupExportpopup_->Show();
				}
				break;
			}
			case ARESTORE: {
				restorepopupselecteditemid_ = -1;
				RestorepopupFilelist_->RemoveAllItems();
				RestorepopupFilelistList_->RemoveAll(true);
				//get file list
				Osp::Base::Collection::ArrayList * dirlist = new Osp::Base::Collection::ArrayList;
				dirlist->Construct();
				if (Osp::Io::File::IsFileExist(L"/Media/Others")) {
					dirlist->Add(*(new String(L"/Media/Others")));
				}
				if (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Others")) {
					dirlist->Add(*(new String(L"/Storagecard/Media/Others")));
				}
				int curdirindex = 0;
				int itemid = 1;
				String * curdirp;
				String curdir;
				while (curdirindex < dirlist->GetCount()) {
					curdirp = static_cast<String *> (dirlist->GetAt(curdirindex));
					curdir = L"";
					curdir.Append(*curdirp);
					Directory* pDir = new Directory;
					DirEnumerator* pDirEnum;
					pDir->Construct(curdir);
					pDirEnum = pDir->ReadN();
					while(pDirEnum->MoveNext() == E_SUCCESS) {
						DirEntry entry = pDirEnum->GetCurrentDirEntry();
						String filename = entry.GetName();
						if (entry.IsDirectory()) {
							//do nothing
						} else if (!entry.IsHidden()) {
							if ((Osp::Io::File::GetFileExtension(filename) == L"backup") && (filename.StartsWith(L"carcon_",0))) {
								RestoreFileListItem * RestoreFileListItem_ = new RestoreFileListItem(curdir + L"/" + filename, itemid);
								RestorepopupFilelistList_->Add(*RestoreFileListItem_);
								RestorepopupFilelist_->AddItem(&filename,null,null,null,itemid);
								itemid++;
							}
						}
					}
					delete pDir;
					delete pDirEnum;
					curdirindex++;
				}
				dirlist->RemoveAll(true);
				delete dirlist;
				Restorepopup_->SetShowState(true);
				Restorepopup_->Show();
				RestorepopupFilelist_->ScrollToTop();
				break;
			}
			case ABACKUP_CANCEL: {
				BackupExportpopup_->SetShowState(false);
				RequestRedraw(true);
				break;
			}
			case ARESTORE_CANCEL: {
				Restorepopup_->SetShowState(false);
				RequestRedraw(true);
				break;
			}
			case ACLEARCOUNTRYLANGCACHE: {
				carconclass_->ClearCountryLangCodeCache();
				pPopupSelectCurrencyList_->RemoveAllItems();
				currencylist_->RemoveAll(true);
				LoadOnShow();
			}
			default:
				break;
		};
	if (actionId == ARESTORE_SEL) {
		if (restorepopupselecteditemid_ > 0) {
			int selitemindex = RestorepopupFilelist_->GetItemIndexFromItemId(restorepopupselecteditemid_);
			if (selitemindex >= 0) {
				if (!RestorepopupFilelist_->IsItemChecked(selitemindex)) {
					restorepopupselecteditemid_ = 0;
				}
			} else {
				restorepopupselecteditemid_ = 0;
			}
		}
		if (restorepopupselecteditemid_ > 0) {
			String restorefromfilename = L"";
			Osp::Base::Collection::IEnumerator * pEnum = RestorepopupFilelistList_->GetEnumeratorN();
			RestoreFileListItem * RestoreFileListItem_;
			while (pEnum->MoveNext() == E_SUCCESS) {
				RestoreFileListItem_ = static_cast<RestoreFileListItem *> (pEnum->GetCurrent());
				if (RestoreFileListItem_->itemid == restorepopupselecteditemid_) {
					restorefromfilename = RestoreFileListItem_->filename;
					break;
				}
			}
			delete pEnum;
			if (restorefromfilename != L"") {
				bool restoredok = false;
				if (Osp::Io::File::IsFileExist(L"/Home/temprestore.db")) {
					Osp::Io::File::Remove(L"/Home/temprestore.db");
				}
				Osp::Io::File::Copy(restorefromfilename,L"/Home/temprestore.db", false);
				Osp::Io::Database * temprestoredb_ = new Database();
				if (temprestoredb_->Construct(L"/Home/temprestore.db", false) != E_SUCCESS) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Corrupted backup file!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
				} else {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Restore?", "All data in current database will be lost!\nConfirm restore from backup?", MSGBOX_STYLE_YESNO, 10000);
					msgbox.ShowAndWait(modalResult);
					if (modalResult == MSGBOX_RESULT_YES) {
						carconclass_->CloseDatabase();
						if (Osp::Io::File::Remove(L"/Home/data.db") == E_SUCCESS) {
							Osp::Io::File::Copy(L"/Home/temprestore.db",L"/Home/data.db", false);
							restoredok = true;
						} else {
							MessageBox msgbox;
							int modalResult = 0;
							msgbox.Construct("Error", "Error removing current database!", MSGBOX_STYLE_OK, 10000);
							msgbox.ShowAndWait(modalResult);
						}
					}
				}
				delete temprestoredb_;
				Osp::Io::File::Remove(L"/Home/temprestore.db");
				if (restoredok == true) {
					Restorepopup_->SetShowState(false);
					RequestRedraw(true);
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Restart", "Restore from backup complete.\nApplication needs restart and it will close now!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					Application::GetInstance()->Terminate();
				}
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Select", "Select backup file!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	}
	if ((actionId == ABACKUP_PHONE) or (actionId == ABACKUP_SD)) {
		carconclass_->CloseDatabase();
		bool hasmediacard = false;
		hasmediacard = (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Others") == true);
		DateTime today;
		String todaydate;
		Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, today);
		carconclass_->DateToSQLlite(today).SubString(0,10,todaydate);
		todaydate.Replace(L"-","");
		String filename = L"carcon_" + todaydate;
		String checkfilename = filename + ".backup";
		int filenumindex = 1;
		while (filenumindex < 100) {
			if (Osp::Io::File::IsFileExist(L"/Media/Others/" + checkfilename) == false) {
				if (hasmediacard) {
					if (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Others/" + checkfilename) == false) {
						break;
					}
				} else {
					break;
				}
			}
			checkfilename = filename + "_" + Osp::Base::Integer::ToString(filenumindex) + ".backup";
			filenumindex++;
		}
		filename = checkfilename;
		if (actionId == ABACKUP_SD) {
			filename = L"/Storagecard/Media/Others/" + filename;
		} else {
			filename = L"/Media/Others/" + filename;
		}

		Osp::Content::ContentId contentId;
		Osp::Content::ContentManager contentManager;
		contentManager.Construct();
		if (Osp::Content::ContentManagerUtil::CopyToMediaDirectory(L"/Home/data.db", filename) != E_SUCCESS) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Error saving file! Maybe out of disk space.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		Osp::Content::OtherContentInfo otherContentInfo;
		otherContentInfo.Construct(filename);
		contentId = contentManager.CreateContent(otherContentInfo);
		contentManager.UpdateContent(otherContentInfo);
		carconclass_->Create(carconclass_->GetLastSelectedID());
		BackupExportpopup_->SetShowState(false);
		RequestRedraw(true);
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Saved", L"Backup saved to:\n" + filename, MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
	}
}

void
FSettings::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
//
}

void
FSettings::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (source.GetName() == L"IDPC_EDITFIELD1") {
		pPopupSelectVolumeUnit_->SetShowState(true);
		pPopupSelectVolumeUnit_->Show();
	}
	if (source.GetName() == L"IDPC_EDITFIELD2") {
		pPopupSelectDistanceUnit_->SetShowState(true);
		pPopupSelectDistanceUnit_->Show();
	}
	if (source.GetName() == L"IDPC_EDITFIELD3") {
		fillupselectconunit();
		pPopupSelectConUnit_->SetShowState(true);
		pPopupSelectConUnit_->Show();
	}
	if (source.GetName() == L"IDPC_EDITFIELD4") {
		pPopupSelectCurrency_->SetShowState(true);
		pPopupSelectCurrency_->Show();
	}
}

void
FSettings::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (source.GetName() == "IDC_LIST1") {
		if (pPopupSelectVolumeUnit_->IsVisible()) {
			PopupSelect(SELECTTYPE_VOLUME, itemId);
		}
		if (pPopupSelectDistanceUnit_->IsVisible()) {
			PopupSelect(SELECTTYPE_DISTANCE, itemId);
		}
		if (pPopupSelectConUnit_->IsVisible()) {
			PopupSelect(SELECTTYPE_CON, itemId);
		}
		if (pPopupSelectCurrency_->IsVisible()) {
			PopupSelect(SELECTTYPE_CURRENCY, itemId);
		}
		if (Restorepopup_->IsVisible()) {
			restorepopupselecteditemid_ = itemId;
		}
	}
}


