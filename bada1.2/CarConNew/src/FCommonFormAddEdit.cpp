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
#include "FCommonFormAddEdit.h"
#include "CarConClass.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

FCommonFormAddEdit::FCommonFormAddEdit(int addeditid, int FormTypeID, bool detailform)
{
	this->itemid = addeditid;
	this->formtypeid = FormTypeID;
	this->detailform = detailform;
	this->detailmode = detailform;
}

FCommonFormAddEdit::~FCommonFormAddEdit(void)
{
}

bool
FCommonFormAddEdit::Initialize()
{
	Form::Construct(L"IDF_FCOMMONFORMADDEDIT");

	return true;
}

result
FCommonFormAddEdit::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_1, ID_CANCEL);
	AddSoftkeyActionListener(SOFTKEY_1, *this);
	SetSoftkeyActionId(SOFTKEY_0, ID_SAVE);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	pTimer_ = new Osp::Base::Runtime::Timer;
	pTimer_->Construct(*this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	controlhandler_->Add(L"IDPC_caption", ControlHandler::TEXT, true, L"Input caption!", L"Input caption");
	controlhandler_->InitControls(this,ID_BUTTON_EDITFIELD_DONE,ID_BUTTON_EDITFIELD_CLOSE,static_cast<Osp::Ui::IActionEventListener *>(this),static_cast<Osp::Ui::ITouchEventListener *>(this),static_cast<Osp::Ui::ITextEventListener *>(this),static_cast<Osp::Ui::IFocusEventListener *>(this),static_cast<Osp::Ui::IScrollPanelEventListener *>(this));

	carconclass_ = pFormMgr->carconclass_;

	if (this->itemid > 0) {
		ReloadSavedData();
		DetailEditMode();
	} else {
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
		this->SetTitleText(L"Fuel type add");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
		this->SetTitleText(L"Location add");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
		this->SetTitleText(L"Expense category add");
		}
	}

	return r;
}

void FCommonFormAddEdit::ReloadSavedData() {
	if (this->itemid > 0) {
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
			carconclass_->GetFuelTypeData(this->itemid, fueltypedatacurdata_);
			controlhandler_->SetValue(this, L"IDPC_caption", fueltypedatacurdata_.Caption);
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
			carconclass_->GetLocationData(this->itemid, locationdatacurdata_);
			controlhandler_->SetValue(this, L"IDPC_caption", locationdatacurdata_.Caption);
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
			carconclass_->GetExpenseKatData(this->itemid, expensekatdata_);
			controlhandler_->SetValue(this, L"IDPC_caption", expensekatdata_.Caption);
		}
	}
}

void FCommonFormAddEdit::DetailEditMode() {
	controlhandler_->ControlReadOnly(this,L"IDPC_caption",this->detailmode);
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
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
		this->SetTitleText(L"Fuel type detail");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
		this->SetTitleText(L"Location detail");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
		this->SetTitleText(L"Expense category detail");
		}
	} else {
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
		this->SetTitleText(L"Fuel type edit");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
		this->SetTitleText(L"Location edit");
		}
		if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
		this->SetTitleText(L"Expense category edit");
		}
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

void FCommonFormAddEdit::OnFocusGained(const Osp::Ui::Control& source) {
	if (this->detailmode) {
		this->SetFocus();
		if ((timerstep > 0) && (timerstep < 6)) {
			pTimer_->Cancel();
		}
		timerstep = 1;
		this->RequestRedraw();
		pTimer_->Start(200);
	} else {

	}
}
void FCommonFormAddEdit::OnFocusLost(const Osp::Ui::Control& source) {

}

void FCommonFormAddEdit::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (timerstep < 6) {
		timer.Start(200);
		timerstep++;
		this->RequestRedraw();
	}
}

result
FCommonFormAddEdit::OnDraw() {
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
FCommonFormAddEdit::OnTextValueChanged(const Osp::Ui::Control& source)
{

}

void
FCommonFormAddEdit::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{

}


bool FCommonFormAddEdit::GetInputDataFuelType(FuelTypeData & data, bool validate) {
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
	data.Caption = controlhandler_->GetValue(this,L"IDPC_caption");
	return true;
}

bool FCommonFormAddEdit::GetInputDataLocation(LocationData & data, bool validate) {
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
	data.Caption = controlhandler_->GetValue(this,L"IDPC_caption");
	return true;
}

bool FCommonFormAddEdit::GetInputDataExpenseKat(CarExpenseKatData & data, bool validate) {
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
	data.Caption = controlhandler_->GetValue(this,L"IDPC_caption");
	return true;
}

bool FCommonFormAddEdit::CheckIfDataChanged(void) {
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
	FuelTypeData data_;
	GetInputDataFuelType(data_);
	if (data_.Caption == fueltypedatacurdata_.Caption) {
		return false;
	} else {
		return true;
	}
	}
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
	LocationData data_;
	GetInputDataLocation(data_);
	if (data_.Caption == locationdatacurdata_.Caption) {
		return false;
	} else {
		return true;
	}
	}
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
	CarExpenseKatData data_;
	GetInputDataExpenseKat(data_);
	if (data_.Caption == locationdatacurdata_.Caption) {
		return false;
	} else {
		return true;
	}
	}
	return false;
}

bool FCommonFormAddEdit::Save(void) {
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
	FuelTypeData data_;
	if (GetInputDataFuelType(data_, true)) {
		carconclass_->SaveFuelTypeData(data_);
		this->itemid = data_.ID;
		return true;
	} else {
		return false;
	}
	}
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
	LocationData data_;
	if (GetInputDataLocation(data_, true)) {
		carconclass_->SaveLocationData(data_);
		this->itemid = data_.ID;
		return true;
	} else {
		return false;
	}
	}
	if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
	CarExpenseKatData data_;
	if (GetInputDataExpenseKat(data_, true)) {
		carconclass_->SaveExpenseKatData(data_);
		this->itemid = data_.ID;
		return true;
	} else {
		return false;
	}
	}
	return false;
}

result
FCommonFormAddEdit::OnTerminating(void)
{
	result r = E_SUCCESS;

	controlhandler_->Clear();
	delete pTimer_;

	return r;
}


void
FCommonFormAddEdit::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (actionId == ID_CANCEL){
		if (!this->detailmode) {
		if (CheckIfDataChanged()) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Confirm cancel", "Data changed. Save data?", MSGBOX_STYLE_YESNO, 10000);
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
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_FUELTYPEFORM, list);
			}
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_LOCATIONFORM, list);
			}
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSEKATFORM, list);
			}
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
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITFUELTYPE) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_FUELTYPEFORM, list);
			}
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITLOCATION) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_LOCATIONFORM, list);
			}
			if (this->formtypeid == FormMgr::REQUEST_ADDEDITEXPENSEKAT) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSEKATFORM, list);
			}
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_DONE) {
		int actiondone = controlhandler_->ActionDone(this, source);
		if ((actiondone == ControlHandler::ACTIONDONERESULT_HIDEKEYPAD) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENDATEPICKER)) {
			ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
			vpScrollPanel->CloseOverlayWindow();
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
FCommonFormAddEdit::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormAddEdit::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (!this->detailmode) {

	} else {
		OnFocusGained(source);
	}
}

void FCommonFormAddEdit::OnOverlayControlCreated(const Osp::Ui::Control& source) {

}

void FCommonFormAddEdit::OnOverlayControlOpened(const Osp::Ui::Control& source) {
	//
}

void FCommonFormAddEdit::OnOverlayControlClosed(const Osp::Ui::Control& source) {
	SetFocus();
}

void FCommonFormAddEdit::OnOtherControlSelected(const Osp::Ui::Control& source) {
	ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
	vpScrollPanel->CloseOverlayWindow();
}
