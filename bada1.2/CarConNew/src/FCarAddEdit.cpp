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
#include "FCarAddEdit.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Base::Utility;

FCarAddEdit::FCarAddEdit(int addeditid, bool detailform, int formtype)
{
	this->itemid = addeditid;
	this->detailform = detailform;
	this->detailmode = detailform;
	this->formtype = formtype;
}

FCarAddEdit::~FCarAddEdit(void)
{

}

bool
FCarAddEdit::Initialize()
{
	Form::Construct(L"IDF_FCARADDEDIT");

	return true;
}

result
FCarAddEdit::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_1, ID_CANCEL);
	AddSoftkeyActionListener(SOFTKEY_1, *this);
	SetSoftkeyActionId(SOFTKEY_0, ID_SAVE);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	if (this->formtype == 2) {
		SetSoftkeyText(SOFTKEY_0, L"Next");
		SetSoftkeyText(SOFTKEY_1, L"Back");
	}

	pTimer_ = new Osp::Base::Runtime::Timer;
	pTimer_->Construct(*this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(1);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	carconclass_ = pFormMgr->carconclass_;
	FuelTypeData fueltypedata_;

	pPopup_ = new Popup();
	pPopup_->Construct(L"IDP_POPUP1");
	pPopup_->SetTitleText(L"Select fuel types");
	pListSelectFuelTypes_ = static_cast<List *>(pPopup_->GetControl(L"IDC_LIST1"));
	if (carconclass_->GetFuelTypeDataListStart()) {
		while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
			pListSelectFuelTypes_->AddItem(&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
		}
		carconclass_->GetFuelTypeDataListEnd();
	}
	Osp::Ui::Controls::Button *pListSelectFuelTypesOKButton_ = static_cast<Button *>(pPopup_->GetControl(L"IDC_BUTTON1"));
	pListSelectFuelTypesOKButton_->SetActionId(ID_SELECTFUELTYPEPOPUPOK);
	pListSelectFuelTypesOKButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button *pListSelectFuelTypesAddButton_ = static_cast<Button *>(pPopup_->GetControl(L"IDC_BUTTON2"));
	pListSelectFuelTypesAddButton_->SetActionId(ID_SELECTFUELTYPEPOPUPADD);
	pListSelectFuelTypesAddButton_->SetText(L"Add new");
	pListSelectFuelTypesAddButton_->AddActionEventListener(*this);
	fueltypesselectedarray_.Construct(0);

	pAddnewfueltypepopup_ = new Popup();
	pAddnewfueltypepopup_->Construct(L"IDP_POPUP3");
	pAddnewfueltypepopup_->SetTitleText(L"New fuel type");
	Osp::Ui::Controls::EditField *pAddnewfueltypepopupEditField_ = static_cast<EditField *>(pAddnewfueltypepopup_->GetControl(L"IDC_EDITFIELD1"));
	pAddnewfueltypepopupEditField_->SetTitleText(L"Fuel type caption");
	pAddnewfueltypepopupEditField_->SetGuideText(L"Input caption");
	Osp::Ui::Controls::Button *pAddnewfueltypepopupOKButton_ = static_cast<Button *>(pAddnewfueltypepopup_->GetControl(L"IDC_BUTTON2"));
	pAddnewfueltypepopupOKButton_->SetActionId(ID_ADDNEWFUELTYPEPOPUPSAVE);
	pAddnewfueltypepopupOKButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button *pAddnewfueltypepopupCancelButton_ = static_cast<Button *>(pAddnewfueltypepopup_->GetControl(L"IDC_BUTTON1"));
	pAddnewfueltypepopupCancelButton_->SetActionId(ID_ADDNEWFUELTYPEPOPUPCANCEL);
	pAddnewfueltypepopupCancelButton_->AddActionEventListener(*this);

	SettingsData settingsdata_;
	carconclass_->GetSettingsData(settingsdata_);

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	controlhandler_->Add(L"EF_1_brand", ControlHandler::TEXT, true, L"Input car make/manufacturer!", L"Input make/manufacturer");
	controlhandler_->Add(L"EF_2_model", ControlHandler::TEXT, true, L"Input car model", L"Input car model");
	controlhandler_->Add(L"EF_year", ControlHandler::NUMBER, true, L"Input car year", L"Input year of car");
	controlhandler_->Add(L"EFN_3_startkm", ControlHandler::NUMBER, true, L"Input odometer value", L"Input initial odometer value", settingsdata_.distanceunitstr);
	controlhandler_->Add(L"EF_licenceplate", ControlHandler::TEXT, false, L"", L"Input license plate number");
	controlhandler_->Add(L"EF_vin", ControlHandler::TEXT, false, L"", L"Input VIN number");
	controlhandler_->Add(L"EF_isurance", ControlHandler::TEXT, false, L"", L"Input insurance policy num.");
	controlhandler_->Add(L"CB_4_fueltypestring", ControlHandler::COMBOBOX, true, L"Select fueltype/s", L"Select fuel type/s");

	controlhandler_->InitControls(this,ID_BUTTON_EDITFIELD_DONE,ID_BUTTON_EDITFIELD_CLOSE,static_cast<Osp::Ui::IActionEventListener *>(this),static_cast<Osp::Ui::ITouchEventListener *>(this),static_cast<Osp::Ui::ITextEventListener *>(this),static_cast<Osp::Ui::IFocusEventListener *>(this),static_cast<Osp::Ui::IScrollPanelEventListener *>(this));

	if (this->itemid > 0) {
		ReloadSavedData();
		DetailEditMode();
	} else {
		this->SetTitleText(L"Car add");
	}

	return r;
}

void FCarAddEdit::ReloadSavedData() {
	if (this->itemid > 0) {
		carconclass_->GetCarData(this->itemid, curcardata_, true);
		controlhandler_->SetValue(this,L"EF_1_brand", curcardata_.Brand);
		controlhandler_->SetValue(this,L"EF_2_model", curcardata_.Model);
		controlhandler_->SetValue(this,L"EFN_3_startkm", curcardata_.startkm);
		controlhandler_->SetValue(this,L"CB_4_fueltypestring", curcardata_.fueltypesstring);
		controlhandler_->SetValue(this,L"EF_year", curcardata_.year);
		controlhandler_->SetValue(this,L"EF_licenceplate", curcardata_.licence);
		controlhandler_->SetValue(this,L"EF_vin", curcardata_.vin);
		controlhandler_->SetValue(this,L"EF_isurance", curcardata_.isurance);
		fueltypesselectedarray_.RemoveAll();
		for (int i=0; i < pListSelectFuelTypes_->GetItemCount(); i++) {
			pListSelectFuelTypes_->SetItemChecked(i, false);
		}
		IEnumeratorT<int>* enumint = curcardata_.fueltypesarray.GetEnumeratorN();
		if (enumint){
		    int fueltypeid;
		    while (enumint->MoveNext() == E_SUCCESS){
		    	enumint->GetCurrent(fueltypeid);
		    	int selecteditemindex = pListSelectFuelTypes_->GetItemIndexFromItemId(fueltypeid);
		    	if (selecteditemindex > -1) {
		    		pListSelectFuelTypes_->SetItemChecked(selecteditemindex, true);
		    		fueltypesselectedarray_.Add(fueltypeid);
		    	}
		    }
		}
		delete enumint;
	}
}

void FCarAddEdit::DetailEditMode() {
	controlhandler_->ControlReadOnly(this,L"EF_1_brand",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_2_model",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EFN_3_startkm",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"CB_4_fueltypestring",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_year",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_licenceplate",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_vin",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_isurance",this->detailmode);
	controlhandler_->ControlDisable(this,L"EFN_3_startkm",!this->detailmode);
	if (this->detailmode) {
		SetSoftkeyText(SOFTKEY_0, L"Edit");
		SetSoftkeyActionId(SOFTKEY_0, ID_EDIT);
		SetSoftkeyText(SOFTKEY_1, L"Back");
	} else {
		SetSoftkeyText(SOFTKEY_0, L"Save");
		SetSoftkeyActionId(SOFTKEY_0, ID_SAVE);
		SetSoftkeyText(SOFTKEY_1, L"Cancel");
	}
	if (this->detailmode) {
		SetTitleText(L"Car detail");
	} else {
		SetTitleText(L"Car edit");
	}
	Osp::Ui::Controls::Label *requredfieldslabel = static_cast<Label *>(GetControl(L"IDPC_LABEL1", true));
	Osp::Ui::Controls::ScrollPanel *scrollpanel = static_cast<ScrollPanel *>(GetControl(L"IDC_SCROLLPANEL1", true));
	if (this->detailmode) {
		requredfieldslabel->SetText(L"");
		scrollpanel->SetBounds(Rectangle(0,74,480,712-74));
		timerstep = 0;
	} else {
		requredfieldslabel->SetText(L"Fields marked with (*) are mandatory.");
		scrollpanel->SetBounds(Rectangle(0,0,480,712));
	}
}


void FCarAddEdit::OnFocusGained(const Osp::Ui::Control& source) {
	if (this->detailmode) {
		this->SetFocus();
		if ((timerstep > 0) && (timerstep < 6)) {
			pTimer_->Cancel();
		}
		timerstep = 1;
		this->RequestRedraw();
		pTimer_->Start(200);
	} else {
		if ((source.GetName() == L"EFN_3_startkm") || (source.GetName() == L"EF_year")) {
			controlhandler_->ControlUnformatValue(this, source);
		}
	}
}
void FCarAddEdit::OnFocusLost(const Osp::Ui::Control& source) {
	if ((source.GetName() == L"EFN_3_startkm") || (source.GetName() == L"EF_year")) {
		controlhandler_->ControlFormatValue(this, source);
	}
}

void FCarAddEdit::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (timerstep < 6) {
		timer.Start(200);
		timerstep++;
		this->RequestRedraw();
	}
}

result
FCarAddEdit::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
		if (this->detailmode) {
			pCanvas_->FillRectangle(Color(0,200,50,50), Rectangle(0,0,480,800));
			pCanvas_->FillRectangle(Color(0,0,0,50), Rectangle(0,0,480,160));
			pCanvas_->SetForegroundColor(Color(255,255,255));
			pCanvas_->SetLineWidth(1);
			pCanvas_->DrawLine(Point(0,160), Point(480, 160));
			Osp::Graphics::Color textcolor;
			textcolor = Color(255,255,255);
			if (timerstep % 2) {
				textcolor = Color(174,0,0);
			}
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(20,95,440,60), L"You can not edit fields in detail mode.\nPress edit button for edit mode.", 26, FONT_STYLE_PLAIN, true, textcolor,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void
FCarAddEdit::OnTextValueChanged(const Osp::Ui::Control& source)
{
	if (!this->detailmode) {
		if ((source.GetName() == L"EFN_3_startkm") || (source.GetName() == L"EF_year")) {
			controlhandler_->ControlFormatValueOnInput(this, source);
		}
	}

}

void
FCarAddEdit::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{
    //
}


bool FCarAddEdit::GetInputData(CarData & data, bool validate) {
	if (validate) {
		String validatemsg = controlhandler_->ValidateFields(this);
		if (validatemsg != L"") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Validate", validatemsg, MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return false;
		}
	}
	data.ID = this->itemid;
	data.Brand = controlhandler_->GetValue(this,L"EF_1_brand");
	data.Model = controlhandler_->GetValue(this,L"EF_2_model");
	data.startkm = controlhandler_->GetValueInt(this,L"EFN_3_startkm");
	data.fueltypesarray.Construct(fueltypesselectedarray_);
	data.fueltypesstring = GetFuelTypesString();
	data.year = controlhandler_->GetValueInt(this,L"EF_year");
	data.licence = controlhandler_->GetValue(this,L"EF_licenceplate");
	data.vin = controlhandler_->GetValue(this,L"EF_vin");
	data.isurance = controlhandler_->GetValue(this,L"EF_isurance");

	return true;
}

String FCarAddEdit::GetFuelTypesString() {
	String fueltypesstring = "";
	FuelTypeData fueltypedata_;
	if (carconclass_->GetFuelTypeDataListStart()) {
		while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
			if (fueltypesselectedarray_.Contains(fueltypedata_.ID)) {
				if (fueltypesstring != "") {
					fueltypesstring += ", ";
				}
				fueltypesstring += fueltypedata_.Caption;
			}
		}
		carconclass_->GetFuelTypeDataListEnd();
	}
	return fueltypesstring;
}


bool FCarAddEdit::CheckIfDataChanged() {
	CarData cardata_;
	GetInputData(cardata_);
	if ((cardata_.Brand == curcardata_.Brand) && (cardata_.Model == curcardata_.Model) && (cardata_.fueltypesstring == curcardata_.fueltypesstring) && (cardata_.year == curcardata_.year) && (cardata_.licence == curcardata_.licence) && (cardata_.vin == curcardata_.vin) && (cardata_.isurance == curcardata_.isurance)) {
		return false;
	} else {
		return true;
	}
}

bool FCarAddEdit::Save() {
	CarData cardata_;
	if (GetInputData(cardata_, true)) {
		carconclass_->SaveCarData(cardata_);
		this->itemid = cardata_.ID;
		return true;
	} else {
		return false;
	}
}

result
FCarAddEdit::OnTerminating(void)
{
	result r = E_SUCCESS;
	delete pPopup_;
	delete pAddnewfueltypepopup_;
	controlhandler_->Clear();
	delete pTimer_;
	return r;
}


void
FCarAddEdit::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (actionId == ID_CANCEL){
		bool issaved = false;
		if (!this->detailmode) {
		if (CheckIfDataChanged()) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Confirm cancel", "Car data changed. Save data?", MSGBOX_STYLE_YESNO, 10000);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				if (Save() == false) {
					return;
				} else {
					issaved = true;
				}
			}
		}
		}
		if (this->formtype == 2) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			if (issaved) {
				carconclass_->SetLastSelectedID(this->itemid);
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			} else {
				ArrayList * list = new ArrayList;
				list->Construct();
				list->Add(*(new Integer(2)));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_SETTINGSFORM, list);
			}
		} else {
		if ((!this->detailform) || (this->detailmode)) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->itemid)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_CARSELECTFORM, list);
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
		}
	} else if (actionId == ID_SAVE) {
		if (Save() == false) {
			return;
		}
		if (this->formtype == 2) {
		carconclass_->SetLastSelectedID(this->itemid);
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
		} else {
		if (!this->detailform) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->itemid)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_CARSELECTFORM, list);
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
		}
	} else if (actionId == ID_SELECTFUELTYPEPOPUPOK) {
		fueltypesselectedarray_.RemoveAll();
		int checkeditemindex;
		int fueltypeid;
		checkeditemindex = pListSelectFuelTypes_->GetFirstCheckedItemIndex();
		while (checkeditemindex > -1) {
			fueltypeid = pListSelectFuelTypes_->GetItemIdAt(checkeditemindex);
			fueltypesselectedarray_.Add(fueltypeid);
			checkeditemindex = pListSelectFuelTypes_->GetNextCheckedItemIndexAfter(checkeditemindex);
		}
		controlhandler_->SetValue(this,L"CB_4_fueltypestring", GetFuelTypesString());
		pPopup_->SetShowState(false);
		this->SetFocus();
		RequestRedraw(true);
	} else if (actionId == ID_ADDNEWFUELTYPEPOPUPCANCEL) {
		pAddnewfueltypepopup_->SetShowState(false);
		this->SetFocus();
		RequestRedraw(true);
	} else if (actionId == ID_ADDNEWFUELTYPEPOPUPSAVE) {
		Osp::Ui::Controls::EditField *pAddnewfueltypepopupEditField_ = static_cast<EditField *>(pAddnewfueltypepopup_->GetControl(L"IDC_EDITFIELD1"));
		String tmps = pAddnewfueltypepopupEditField_->GetText();
		tmps.Trim();
		if (tmps == "") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Empty value!", "Input fuel type caption!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		} else {
			FuelTypeData fueltypedata_;
			fueltypedata_.Caption = tmps;
			fueltypedata_.ID = 0;
			carconclass_->SaveFuelTypeData(fueltypedata_);
			pListSelectFuelTypes_->AddItem(&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
			int selecteditemindex = pListSelectFuelTypes_->GetItemIndexFromItemId(fueltypedata_.ID);
			if (selecteditemindex > -1) {
				pListSelectFuelTypes_->SetItemChecked(selecteditemindex, true);
			}
			pListSelectFuelTypes_->ScrollToBottom();
			pAddnewfueltypepopup_->SetShowState(false);
			RequestRedraw(true);
		}
	} else if (actionId == ID_SELECTFUELTYPEPOPUPADD) {
		Osp::Ui::Controls::EditField *pAddnewfueltypepopupEditField_ = static_cast<EditField *>(pAddnewfueltypepopup_->GetControl(L"IDC_EDITFIELD1"));
		pAddnewfueltypepopupEditField_->SetText(L"");
		pAddnewfueltypepopup_->SetShowState(true);
		pAddnewfueltypepopup_->Show();
	} else if (actionId == ID_BUTTON_EDITFIELD_DONE) {
		int actiondone = controlhandler_->ActionDone(this, source);
		if ((actiondone == ControlHandler::ACTIONDONERESULT_HIDEKEYPAD) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX)) {
			ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
			vpScrollPanel->CloseOverlayWindow();
		}
		if (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) {
			pPopup_->SetShowState(true);
			pPopup_->Show();
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_CLOSE) {
		ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
		vpScrollPanel->CloseOverlayWindow();
	} else if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		this->SetFocus();
		if (actionId == ID_DETAIL) {
			ReloadSavedData();
		}
		this->detailmode = (actionId == ID_DETAIL);
		DetailEditMode();
		RequestRedraw(true);
	}
}

void
FCarAddEdit::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarAddEdit::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (!this->detailmode) {
		if (source.GetName() == L"CB_4_fueltypestring") {
			pPopup_->SetShowState(true);
			pPopup_->Show();
		}
	} else {
		OnFocusGained(source);
		//this->SetFocus();
	}
}

void FCarAddEdit::OnOverlayControlCreated(const Osp::Ui::Control& source) {

}

void FCarAddEdit::OnOverlayControlOpened(const Osp::Ui::Control& source) {

}

void FCarAddEdit::OnOverlayControlClosed(const Osp::Ui::Control& source) {
	SetFocus();
}

void FCarAddEdit::OnOtherControlSelected(const Osp::Ui::Control& source) {
	ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
	vpScrollPanel->CloseOverlayWindow();
}
