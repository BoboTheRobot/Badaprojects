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
#include "FStroskiAddEdit.h"
#include "CarConClass.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Locales;
using namespace Osp::Graphics;

FStroskiAddEdit::FStroskiAddEdit(int addeditid, bool detailform)
{
	this->itemid = addeditid;
	this->detailform = detailform;
	this->detailmode = detailform;
	this->initformelhideondraw = true;
}

FStroskiAddEdit::~FStroskiAddEdit(void)
{
}

bool
FStroskiAddEdit::Initialize()
{
	Form::Construct(L"IDF_FSTROSKIADDEDIT");

	return true;
}

result
FStroskiAddEdit::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_1, ID_CANCEL);
	AddSoftkeyActionListener(SOFTKEY_1, *this);
	SetSoftkeyActionId(SOFTKEY_0, ID_SAVE);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	pTimer_ = new Osp::Base::Runtime::Timer;
	pTimer_->Construct(*this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	carconclass_ = pFormMgr->carconclass_;

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(5);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	CarExpenseKatData expensekatdata_;
	pPopupSelectKat_ = new Popup();
	pPopupSelectKat_->Construct(L"IDP_POPUP2");
	pPopupSelectKat_->SetTitleText(L"Select category");
	pListSelectKat_ = static_cast<List *>(pPopupSelectKat_->GetControl(L"IDC_LIST1"));
	if (carconclass_->GetExpenseKatDataListStart()) {
		while (carconclass_->GetExpenseKatDataListGetData(expensekatdata_)) {
			pListSelectKat_->AddItem(&expensekatdata_.Caption, null, null, null, expensekatdata_.ID);
		}
		carconclass_->GetExpenseKatDataListEnd();
	}
	Osp::Ui::Controls::Button *pListSelectKatAddButton_ = static_cast<Button *>(pPopupSelectKat_->GetControl(L"IDC_BUTTON2"));
	pListSelectKatAddButton_->SetActionId(ID_SELECTKATPOPUPADD);
	pListSelectKatAddButton_->SetText(L"Add new category");
	pListSelectKatAddButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button *pListSelectKatCancelButton_ = static_cast<Button *>(pPopupSelectKat_->GetControl(L"IDC_BUTTON1"));
	pListSelectKatCancelButton_->SetActionId(ID_SELECTKATPOPUPCANCEL);
	pListSelectKatCancelButton_->AddActionEventListener(*this);
	pAddnewkatpopup_ = new Popup();
	pAddnewkatpopup_->Construct(L"IDP_POPUP3");
	pAddnewkatpopup_->SetTitleText(L"New category");
	Osp::Ui::Controls::EditField *pAddnewkatpopupEditField_ = static_cast<EditField *>(pAddnewkatpopup_->GetControl(L"IDC_EDITFIELD1"));
	pAddnewkatpopupEditField_->SetTitleText(L"Category caption");
	pAddnewkatpopupEditField_->SetGuideText(L"Input caption");
	Osp::Ui::Controls::Button *pAddnewkatpopupOKButton_ = static_cast<Button *>(pAddnewkatpopup_->GetControl(L"IDC_BUTTON2"));
	pAddnewkatpopupOKButton_->SetActionId(ID_ADDNEWKATPOPUPSAVE);
	pAddnewkatpopupOKButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button *pAddnewkatpopupCancelButton_ = static_cast<Button *>(pAddnewkatpopup_->GetControl(L"IDC_BUTTON1"));
	pAddnewkatpopupCancelButton_->SetActionId(ID_ADDNEWKATPOPUPCANCEL);
	pAddnewkatpopupCancelButton_->AddActionEventListener(*this);

	pListSelectKat_->AddItemEventListener(*this);

	pDatePicker_ = new DatePicker();
    pDatePicker_->Construct(L"Date");
    pDatePicker_->AddDateChangeEventListener(*this);
    pDatePicker_->SetCurrentDate();
    pDatePicker_->SetYearRange(pDatePicker_->GetYear()-10, pDatePicker_->GetYear());

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	SettingsData settingsdata_;
	carconclass_->GetSettingsData(settingsdata_);

	controlhandler_->Add(L"IDPC_date", ControlHandler::DATETEXT, true, L"Select date!", L"Select date");
	controlhandler_->Add(L"IDPC_kat", ControlHandler::COMBOBOX, true, L"Select kategory!", L"Select kategory");
	controlhandler_->Add(L"IDPC_caption", ControlHandler::TEXT, true, L"Input caption!", L"Input caption");
	controlhandler_->Add(L"IDPC_price", ControlHandler::CURRENCY, true, L"Input price!", L"Input price");
	controlhandler_->Add(L"IDPC_odometer", ControlHandler::NUMBER, false, L"", L"Input odometer value", settingsdata_.distanceunitstr);
	controlhandler_->Add(L"IDPC_remark", ControlHandler::TEXTAREA, false, L"", L"Input remark");

	controlhandler_->InitControls(this,ID_BUTTON_EDITFIELD_DONE,ID_BUTTON_EDITFIELD_CLOSE,static_cast<Osp::Ui::IActionEventListener *>(this),static_cast<Osp::Ui::ITouchEventListener *>(this),static_cast<Osp::Ui::ITextEventListener *>(this),static_cast<Osp::Ui::IFocusEventListener *>(this),static_cast<Osp::Ui::IScrollPanelEventListener *>(this));

	Osp::Ui::Controls::Button *setcurrodometerbtn = static_cast<Button *>(GetControl(L"IDPC_BUTTON1", true));
	Bitmap * button1nbmp = drawingclass_->FDrawTextToBmpN(Rectangle(0,0,120,107),L"Set to current odometer", 26, Color(0,0,0));
	Bitmap * button1hbmp = drawingclass_->FDrawTextToBmpN(Rectangle(0,0,120,107),L"Set to current odometer", 26, Color(255,255,255));
	setcurrodometerbtn->SetNormalBitmap(Point(10,0), *button1nbmp);
	setcurrodometerbtn->SetPressedBitmap(Point(10,0), *button1hbmp);
	setcurrodometerbtn->SetText(L"");
	delete button1nbmp;
	delete button1hbmp;
	setcurrodometerbtn->SetActionId(ID_SETCURODOMETER);
	setcurrodometerbtn->AddActionEventListener(*this);

	if (this->itemid > 0) {
		ReloadSavedData();
		DetailEditMode();
	} else {
		curdata_.CarID = carconclass_->GetLastSelectedID();
		controlhandler_->SetValueDate(this, L"IDPC_date", L"TODAY");
		this->SetTitleText(L"Expense add");
	}

	return r;
}

void FStroskiAddEdit::ReloadSavedData() {
	if (this->itemid > 0) {
		carconclass_->GetExpenseData(this->itemid, curdata_);
		controlhandler_->SetValueDate(this, L"IDPC_date", curdata_.timejustdate);
		controlhandler_->SetValue(this, L"IDPC_caption", curdata_.Caption);
		controlhandler_->SetValueCurrency(this, L"IDPC_price", curdata_.Price);
		controlhandler_->SetValue(this, L"IDPC_odometer", curdata_.Odometer);
		controlhandler_->SetValue(this, L"IDPC_remark", curdata_.Remark);
		controlhandler_->SetValue(this, L"IDPC_kat", curdata_.katcaption);
		PopupSelectKatSelect(curdata_.katid);
		int selecteditemindex = pListSelectKat_->GetItemIndexFromItemId(curdata_.katid);
		if (selecteditemindex > -1) {
			pListSelectKat_->SetItemChecked(selecteditemindex, true);
		}
		pListSelectKat_->ScrollToTop(selecteditemindex);
	}
}

void FStroskiAddEdit::DetailEditMode() {
	controlhandler_->ControlReadOnly(this,L"IDPC_date",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_caption",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_price",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_odometer",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_remark",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_kat",this->detailmode);
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
		SetTitleText(L"Expense detail");
	} else {
		SetTitleText(L"Expense edit");
	}
	Osp::Ui::Controls::Button *setcurrodometerbtn = static_cast<Button *>(GetControl(L"IDPC_BUTTON1", true));
	setcurrodometerbtn->SetShowState(!this->detailmode);
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

void FStroskiAddEdit::OnDateChanged(const Control& source, int year, int month, int day) {
	controlhandler_->SetValueDate(this, L"IDPC_date", pDatePicker_->GetDate());
	SetFocus();
	if (popupmovetonextctrl) {
	Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"IDPC_date", true));
	OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
	}
}

void FStroskiAddEdit::OnDateChangeCanceled(const Control& source) {
   	SetFocus();
}

void FStroskiAddEdit::OnFocusGained(const Osp::Ui::Control& source) {
	if (this->detailmode) {
		this->SetFocus();
		if ((timerstep > 0) && (timerstep < 6)) {
			pTimer_->Cancel();
		}
		timerstep = 1;
		this->RequestRedraw();
		pTimer_->Start(200);
	} else {
		if ((source.GetName() == L"IDPC_odometer") || (source.GetName() == L"IDPC_price") || (source.GetName() == L"IDPC_date")) {
			controlhandler_->ControlUnformatValue(this, source);
		}
	}
}
void FStroskiAddEdit::OnFocusLost(const Osp::Ui::Control& source) {
	if (source.GetName() == L"IDPC_odometer") {
		if (!this->detailmode) {
			controlhandler_->ControlFormatValue(this, source);
		}
	} else if ((source.GetName() == L"IDPC_price") || (source.GetName() == L"IDPC_date")) {
		controlhandler_->ControlFormatValue(this, source);
	}
}

void FStroskiAddEdit::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (timerstep < 6) {
		timer.Start(200);
		timerstep++;
		this->RequestRedraw();
	}
}

result
FStroskiAddEdit::OnDraw() {
	if (this->initformelhideondraw == true) {
		Osp::Ui::Controls::Button *setcurrodometerbtn = static_cast<Button *>(GetControl(L"IDPC_BUTTON1", true));
		setcurrodometerbtn->SetShowState(!this->detailmode);
		this->initformelhideondraw = false;
	}
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
FStroskiAddEdit::OnTextValueChanged(const Osp::Ui::Control& source)
{
	if (!this->detailmode) {
		if ((source.GetName() == L"IDPC_odometer") || (source.GetName() == L"IDPC_price") || (source.GetName() == L"IDPC_date")) {
			controlhandler_->ControlFormatValueOnInput(this, source);
		}
	}
}

void
FStroskiAddEdit::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{

}


bool FStroskiAddEdit::GetInputData(CarExpenseData & data, bool validate) {
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
	data.CarID = curdata_.CarID;
	data.Odometer = controlhandler_->GetValueInt(this,L"IDPC_odometer");
	data.Price = controlhandler_->GetValueDouble(this,L"IDPC_price");
	data.Caption = controlhandler_->GetValue(this, L"IDPC_caption");
	data.time = controlhandler_->GetValueDate(this, L"IDPC_date");
	data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
	data.Remark = controlhandler_->GetValue(this, L"IDPC_remark");
	data.katid = this->selectedkat;
	return true;
}

bool FStroskiAddEdit::CheckIfDataChanged(void) {
	CarExpenseData data_;
	GetInputData(data_);
	if ((data_.Caption == curdata_.Caption) || (data_.Remark == curdata_.Remark) || (data_.Price == curdata_.Price) || (data_.Odometer == curdata_.Odometer) || (data_.timejustdate == curdata_.timejustdate) || (data_.katid == curdata_.katid)) {
		return false;
	} else {
		return true;
	}
	return false;
}

bool FStroskiAddEdit::Save(void) {
	CarExpenseData data_;
	if (GetInputData(data_, true)) {
		carconclass_->SaveExpenseData(data_);
		this->itemid = data_.ID;
		return true;
	} else {
		return false;
	}
	return false;
}

result
FStroskiAddEdit::OnTerminating(void)
{
	result r = E_SUCCESS;

	controlhandler_->Clear();
	delete pDatePicker_;
	delete pTimer_;
	delete pPopupSelectKat_;
	delete pAddnewkatpopup_;

	return r;
}


void
FStroskiAddEdit::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (actionId == ID_CANCEL){
		if (!this->detailmode) {
		if (CheckIfDataChanged()) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Confirm cancel", "Expense data changed. Save data?", MSGBOX_STYLE_YESNO, 10000);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				if (Save() == false) {
					return;
				}
			}
		}
		}
		if ((!this->detailform) || (this->detailmode)) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->itemid)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSESFORM, list);
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
	} else if (actionId == ID_SAVE) {
		if (Save() == false) {
			return;
		}
		if (!this->detailform) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->itemid)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSESFORM, list);
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_DONE) {
		popupmovetonextctrl = true;
		int actiondone = controlhandler_->ActionDone(this, source);
		if ((actiondone == ControlHandler::ACTIONDONERESULT_HIDEKEYPAD) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENDATEPICKER)) {
			ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
			vpScrollPanel->CloseOverlayWindow();
		}
		if (actiondone == ControlHandler::ACTIONDONERESULT_OPENDATEPICKER) {
			pDatePicker_->SetDate(controlhandler_->GetValueDate(this, L"IDPC_date"));
			pDatePicker_->Show();
		}
		if (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) {
			pPopupSelectKat_->SetShowState(true);
			pPopupSelectKat_->Show();
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_CLOSE) {
		popupmovetonextctrl = false;
		ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
		vpScrollPanel->CloseOverlayWindow();
	} else if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		popupmovetonextctrl = false;
		this->SetFocus();
		if (actionId == ID_DETAIL) {
			ReloadSavedData();
		}
		this->detailmode = (actionId == ID_DETAIL);
		DetailEditMode();
		RequestRedraw(true);
	} else if (actionId == ID_SETCURODOMETER) {
		CarData cardata_;
		carconclass_->GetCarData(curdata_.CarID, cardata_, false);
		controlhandler_->SetValue(this, L"IDPC_odometer", cardata_.curodometervalue);
	} else if (actionId == ID_ADDNEWKATPOPUPCANCEL) {
		pAddnewkatpopup_->SetShowState(false);
		RequestRedraw(true);
	} else if (actionId == ID_ADDNEWKATPOPUPSAVE) {
		Osp::Ui::Controls::EditField *pAddnewkatpopupEditField_ = static_cast<EditField *>(pAddnewkatpopup_->GetControl(L"IDC_EDITFIELD1"));
		String tmps = pAddnewkatpopupEditField_->GetText();
		tmps.Trim();
		if (tmps == "") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Empty value!", "Input category caption!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		} else {
			CarExpenseKatData expensekatdata_;
			expensekatdata_.Caption = tmps;
			expensekatdata_.ID = 0;
			carconclass_->SaveExpenseKatData(expensekatdata_);
			pListSelectKat_->AddItem(&expensekatdata_.Caption, null, null, null, expensekatdata_.ID);
			int selecteditemindex = pListSelectKat_->GetItemIndexFromItemId(expensekatdata_.ID);
			if (selecteditemindex > -1) {
				pListSelectKat_->SetItemChecked(selecteditemindex, true);
			}
			pListSelectKat_->ScrollToTop(selecteditemindex);
			pAddnewkatpopup_->SetShowState(false);
			RequestRedraw(true);
			PopupSelectKatSelect(expensekatdata_.ID);
		}
	} else if (actionId == ID_SELECTKATPOPUPADD) {
		Osp::Ui::Controls::EditField *pAddnewkatpopupEditField_ = static_cast<EditField *>(pAddnewkatpopup_->GetControl(L"IDC_EDITFIELD1"));
		pAddnewkatpopupEditField_->SetText(L"");
		pAddnewkatpopup_->SetShowState(true);
		pAddnewkatpopup_->Show();
	} else if (actionId == ID_SELECTKATPOPUPCANCEL) {
		popupmovetonextctrl = false;
		if (pPopupSelectKat_->IsVisible()) {
			pPopupSelectKat_->SetShowState(false);
			RequestRedraw(true);
		}
	}
}

void
FStroskiAddEdit::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FStroskiAddEdit::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (!this->detailmode) {
		if (source.GetName() == L"IDPC_date") {
			popupmovetonextctrl = true;
			pDatePicker_->SetDate(controlhandler_->GetValueDate(this, L"IDPC_date"));
			pDatePicker_->Show();
		}
		if (source.GetName() == L"IDPC_kat") {
			popupmovetonextctrl = false;
			pPopupSelectKat_->SetShowState(true);
			pPopupSelectKat_->Show();
		}
	} else {
		OnFocusGained(source);
	}
}

void FStroskiAddEdit::OnOverlayControlCreated(const Osp::Ui::Control& source) {

}

void FStroskiAddEdit::OnOverlayControlOpened(const Osp::Ui::Control& source) {
	//
}

void FStroskiAddEdit::OnOverlayControlClosed(const Osp::Ui::Control& source) {
	SetFocus();
}

void FStroskiAddEdit::OnOtherControlSelected(const Osp::Ui::Control& source) {
	ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
	vpScrollPanel->CloseOverlayWindow();
}

void FStroskiAddEdit::PopupSelectKatSelect(int itemid) {
	CarExpenseKatData expensekatdata_;
	if (carconclass_->GetExpenseKatData(itemid, expensekatdata_)) {
		controlhandler_->SetValue(this, L"IDPC_kat", expensekatdata_.Caption);
		selectedkat = itemid;
	} else {
		controlhandler_->SetValue(this, L"IDPC_kat", L"");
		selectedkat = 0;
	}
	if (pPopupSelectKat_->IsVisible()) {
		pPopupSelectKat_->SetShowState(false);
		RequestRedraw(true);
	}
	if (popupmovetonextctrl) {
		Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"IDPC_kat", true));
		OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
	}
}

void
FStroskiAddEdit::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (source.GetName() == "IDC_LIST1") {
		if (pPopupSelectKat_->IsVisible()) {
			PopupSelectKatSelect(itemId);
		}
	}
}
