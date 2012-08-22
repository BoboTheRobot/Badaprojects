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
#include "FTripAddEdit.h"
#include "CarConClass.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Locales;
using namespace Osp::Graphics;

FTripAddEdit::FTripAddEdit(int addeditid, bool detailform)
{
	this->itemid = addeditid;
	this->detailform = detailform;
	this->detailmode = detailform;
	isfirstdraw = true;
}

FTripAddEdit::~FTripAddEdit(void)
{
}

bool
FTripAddEdit::Initialize()
{
	Form::Construct(L"IDF_FTRIPADDEDIT");

	return true;
}

result
FTripAddEdit::OnInitializing(void)
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
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(11);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	pDatePicker_ = new DatePicker();
    pDatePicker_->Construct(L"Date");
    pDatePicker_->AddDateChangeEventListener(*this);
    pDatePicker_->SetCurrentDate();
    pDatePicker_->SetYearRange(pDatePicker_->GetYear()-10, pDatePicker_->GetYear());

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	SettingsData settingsdata_;
	carconclass_->GetSettingsData(settingsdata_);

	controlhandler_->Add(L"IDPC_startdate", ControlHandler::DATETEXT, true, L"Select departure date!", L"Select departure date");
	controlhandler_->Add(L"IDPC_startodometer", ControlHandler::NUMBER, true, L"Input departure odometer value!", L"Input departure odometer value", settingsdata_.distanceunitstr);
	controlhandler_->Add(L"IDPC_startlocation", ControlHandler::TEXT, false, L"", L"Input departure location");
	controlhandler_->Add(L"IDPC_passingers", ControlHandler::NUMBER, true, L"Input number of passengers!", L"Input number of passengers");
	controlhandler_->Add(L"IDPC_priceperl", ControlHandler::CURRENCY, true, L"Input price per " + settingsdata_.volumeunitstr, L"Input price per " + settingsdata_.volumeunitstr);
	controlhandler_->Add(L"IDPC_enddate", ControlHandler::DATETEXT, false, L"", L"Select arrival date");
	controlhandler_->Add(L"IDPC_endodometer", ControlHandler::NUMBER, false, L"", L"Input arrival odometer value", settingsdata_.distanceunitstr);
	controlhandler_->Add(L"IDPC_endlocation", ControlHandler::TEXT, false, L"", L"Input arrival location");
	controlhandler_->Add(L"IDPC_realexpenses", ControlHandler::CURRENCY, false, L"", L"Input expenses");
	controlhandler_->Add(L"IDPC_remark", ControlHandler::TEXTAREA, false, L"", L"Input remark");

	controlhandler_->InitControls(this,ID_BUTTON_EDITFIELD_DONE,ID_BUTTON_EDITFIELD_CLOSE,static_cast<Osp::Ui::IActionEventListener *>(this),static_cast<Osp::Ui::ITouchEventListener *>(this),static_cast<Osp::Ui::ITextEventListener *>(this),static_cast<Osp::Ui::IFocusEventListener *>(this),static_cast<Osp::Ui::IScrollPanelEventListener *>(this));

	SelectLocationComboTimer_ = new Osp::Base::Runtime::Timer;
	SelectLocationComboTimer_->Construct(*this);

	SelectLocationStartCombo_ = static_cast<CustomList *>(GetControl(L"IDPC_LOCATIONSTART", true));
	SelectLocationStartCombo_->AddFocusEventListener(*this);
	SelectLocationStartCombo_->AddCustomItemEventListener(*this);

	SelectLocationEndCombo_ = static_cast<CustomList *>(GetControl(L"IDPC_LOCATIONEND", true));
	SelectLocationEndCombo_->AddFocusEventListener(*this);
	SelectLocationEndCombo_->AddCustomItemEventListener(*this);

	SelectLocationComboItemFormat_ = new CustomListItemFormat();
	SelectLocationComboItemFormat_->Construct();
	SelectLocationComboItemFormat_->AddElement(SelectLocationItemBG, Rectangle(0, 0, SelectLocationStartCombo_->GetBounds().width, 80));
	SelectLocationComboItemFormat_->SetElementEventEnabled(SelectLocationItemBG,true);

	ReloadLocationCombo(1);
	ReloadLocationCombo(2);

	Osp::Ui::Controls::EditField * selectlocationbomboef_;
	selectlocationbomboef_ = static_cast<EditField *>(GetControl(L"IDPC_startlocation", true));
	selectlocationbomboef_->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_PREDICTIVE, false);
	selectlocationbomboef_->AddTextEventListener(*this);
	selectlocationbomboef_ = static_cast<EditField *>(GetControl(L"IDPC_endlocation", true));
	selectlocationbomboef_->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_PREDICTIVE, false);
	selectlocationbomboef_->AddTextEventListener(*this);

	CarConData lastcarcondata_;
	carconclass_->GetCarConGetLastData(carconclass_->SelectedCar.ID, 0, lastcarcondata_, true);
	AvgCon = lastcarcondata_.AvgCon;
	ConType = settingsdata_.contype;

	if (this->itemid > 0) {
		ReloadSavedData();
		DetailEditMode();
	} else {
		curdata_.CarID = carconclass_->GetLastSelectedID();
		controlhandler_->SetValueDate(this, L"IDPC_startdate", L"TODAY");
		controlhandler_->SetValueDate(this, L"IDPC_enddate", L"TODAY");
		controlhandler_->SetValue(this, L"IDPC_passingers", 1);
		controlhandler_->SetValue(this,L"IDPC_startodometer",lastcarcondata_.StKm);
		controlhandler_->SetValueCurrency(this,L"IDPC_priceperl",lastcarcondata_.Price);
		OnTextValueChanged(*GetControl(L"IDPC_startodometer", true));
		OnFocusLost(*GetControl(L"IDPC_startodometer", true));
		this->SetTitleText(L"Trip add");
	}

	return r;
}

void FTripAddEdit::ReloadSavedData() {
	if (this->itemid > 0) {
		carconclass_->GetTripData(this->itemid, curdata_);
		controlhandler_->SetValueDate(this, L"IDPC_startdate", curdata_.StartTime);
		controlhandler_->SetValue(this, L"IDPC_startodometer", curdata_.StartOdometer);
		controlhandler_->SetValue(this, L"IDPC_startlocation", curdata_.StartLocationCaption);
		controlhandler_->SetValue(this, L"IDPC_passingers", curdata_.StOseb);
		controlhandler_->SetValueCurrency(this, L"IDPC_priceperl", curdata_.PricePerL);
		controlhandler_->SetValueDate(this, L"IDPC_enddate", curdata_.EndTime);
		controlhandler_->SetValue(this, L"IDPC_endodometer", curdata_.EndOdometer);
		controlhandler_->SetValue(this, L"IDPC_endlocation", curdata_.EndLocationCaption);
		controlhandler_->SetValueCurrency(this, L"IDPC_realexpenses", curdata_.RealExpenses);
		controlhandler_->SetValue(this, L"IDPC_remark", curdata_.Remark);
		OnTextValueChanged(*GetControl(L"IDPC_startodometer", true));
		OnFocusLost(*GetControl(L"IDPC_startodometer", true));
	}
}

void FTripAddEdit::DetailEditMode() {
	controlhandler_->ControlReadOnly(this,L"IDPC_startdate",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_startodometer",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_startlocation",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_passingers",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_priceperl",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_enddate",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_endodometer",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_endlocation",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_realexpenses",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"IDPC_remark",this->detailmode);
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
		SetTitleText(L"Trip detail");
	} else {
		SetTitleText(L"Trip edit");
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

class CustomListElementLocationTripCombo : public ICustomListElement
 {
public:
 String col1;
 String col1b, col1s, col1a;
 bool notestyle;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     if ((itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) || (notestyle)) {
    	 pCanvas->FillRectangle(Color(4,58,99), rect);
     } else {
    	 pCanvas->FillRectangle(Color(216,228,254), rect);
     }
     pCanvas->SetLineWidth(1);

     Font fontcol1;
     if (notestyle) {
    	fontcol1.Construct(Osp::Graphics::FONT_STYLE_ITALIC, 24);
     } else {
    	fontcol1.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 36);
     }

     EnrichedText texteelcol1;
     texteelcol1.Construct(Dimension(rect.width-20, rect.height));
     if (notestyle) {
    	 texteelcol1.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_CENTER);
     } else {
    	 texteelcol1.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     }
     texteelcol1.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
     texteelcol1.SetTextAbbreviationEnabled(true);
     if (col1s.GetLength() > 0) {
    	 TextElement * textelcol1b = new TextElement();
    	 textelcol1b->Construct(L" ");
    	 if (col1b.GetLength() > 0) {
    		 textelcol1b->SetText(col1b);
    		 if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    			 textelcol1b->SetTextColor(Color::COLOR_WHITE);
    		 } else {
    			 textelcol1b->SetTextColor(Color::COLOR_BLACK);
    		 }
    		 textelcol1b->SetFont(fontcol1);
    		 texteelcol1.Add(*textelcol1b);
    	 }
    	 TextElement * textelcol1s = new TextElement();
    	 textelcol1s->Construct(col1s);
    	 if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    		 textelcol1s->SetTextColor(Color::COLOR_WHITE);
    	 } else {
    		 textelcol1s->SetTextColor(Color::COLOR_BLUE);
    	 }
    	 textelcol1s->SetFont(fontcol1);
    	 texteelcol1.Add(*textelcol1s);
    	 TextElement * textelcol1a = new TextElement();
    	 textelcol1a->Construct(L" ");
    	 if (col1a.GetLength() > 0) {
    		 textelcol1a->SetText(col1a);
    		 if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    			 textelcol1a->SetTextColor(Color::COLOR_WHITE);
    		 } else {
    			 textelcol1a->SetTextColor(Color::COLOR_BLACK);
    		 }
    		 textelcol1a->SetFont(fontcol1);
    		 texteelcol1.Add(*textelcol1a);
    	 }
    	 pCanvas->DrawText(Point(10,0), texteelcol1);
		 delete textelcol1b;
		 delete textelcol1s;
		 delete textelcol1a;
     } else {
    	 TextElement textelcol1;
		 if (col1.GetLength() > 0) {
			 textelcol1.Construct(col1);
			 if ((itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) || (notestyle)) {
				 textelcol1.SetTextColor(Color::COLOR_WHITE);
			 } else {
				 textelcol1.SetTextColor(Color::COLOR_BLACK);
			 }
			 textelcol1.SetFont(fontcol1);
			 texteelcol1.Add(textelcol1);
		 }
		 pCanvas->DrawText(Point(10,0), texteelcol1);
     }
     pCanvas->SetForegroundColor(Color(4,58,99));
     pCanvas->DrawLine(Point(rect.x, rect.height - 1), Point(rect.width, rect.height - 1));

     return r;
 }
};

void
FTripAddEdit::SelectLocationComboItemAdd(int type, Osp::Base::String col1, int itemId, int searchql, int searchposcol1)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementLocationTripCombo * custom_element = new CustomListElementLocationTripCombo();

 newItem->Construct(SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height);
 newItem->SetItemFormat(*SelectLocationComboItemFormat_);
 custom_element->col1 = col1;
 if (searchql > 0) {
 if (searchposcol1 > -1) {
 if (searchposcol1 > 10) {
 custom_element->col1.SubString(searchposcol1-3,custom_element->col1);
 custom_element->col1.Insert("...",0);
 searchposcol1 = 6;
 }
 custom_element->col1.SubString(0,searchposcol1,custom_element->col1b);
 custom_element->col1.SubString(searchposcol1,searchql,custom_element->col1s);
 if (searchposcol1+searchql < custom_element->col1.GetLength())
 custom_element->col1.SubString(searchposcol1+searchql, custom_element->col1a);
 }
 }
 custom_element->notestyle = (itemId == 0);
 newItem->SetElement(SelectLocationItemBG, *(static_cast<ICustomListElement *>(custom_element)));
 if (type == 1) {
	 SelectLocationStartCombo_->AddItem(*newItem, itemId);
 } else if (type == 2) {
	 SelectLocationEndCombo_->AddItem(*newItem, itemId);
 }
}

void
FTripAddEdit::ReloadLocationCombo(int type) {
	String searchq;
	if (type == 1) {
		searchq = controlhandler_->GetValue(this, L"IDPC_startlocation");
		SelectLocationStartCombo_->RemoveAllItems();
	} else if (type == 2) {
		searchq = controlhandler_->GetValue(this, L"IDPC_endlocation");
		SelectLocationEndCombo_->RemoveAllItems();
	}
	int itemscount(0);
	if ((searchq.GetLength()) > 1) {
		int searchwfoundi1(-1);
		Osp::Base::String lowercasedsearchq, lowercased1;
		searchq.ToLower(lowercasedsearchq);
		LocationData locationdata_;
		if (carconclass_->GetLocationDataListStart(searchq, 10)) {
			while (carconclass_->GetLocationDataListGetData(locationdata_)) {
				lowercased1 = L"";
				locationdata_.Caption.ToLower(lowercased1);
				if (lowercased1 == L"") lowercased1.Append(L" ");
				if (lowercased1.IndexOf(lowercasedsearchq,0,searchwfoundi1) == E_SUCCESS) {
					SelectLocationComboItemAdd(type, locationdata_.Caption, locationdata_.ID, searchq.GetLength(), searchwfoundi1);
					itemscount++;
				}
				if (itemscount > 10) {
					break;
				}
			}
			carconclass_->GetLocationDataListEnd();
		}
		if (itemscount == 0) {
			SelectLocationComboItemAdd(type, L"New location", 0);
			itemscount++;
		}
	} else {
		SelectLocationComboItemAdd(type, L"Input at least two characters", 0);
		itemscount++;
	}
	if (itemscount >= 2) {
		if (type == 1) {
			SelectLocationStartCombo_->SetSize(SelectLocationStartCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*2);
			SelectLocationStartCombo_->ScrollToTop();
		} else if (type == 2) {
			SelectLocationEndCombo_->SetSize(SelectLocationEndCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*2);
			SelectLocationEndCombo_->ScrollToTop();
		}
	} else {
		if (type == 1) {
			SelectLocationStartCombo_->SetSize(SelectLocationStartCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*itemscount);
			SelectLocationStartCombo_->ScrollToTop();
		} else if (type == 2) {
			SelectLocationEndCombo_->SetSize(SelectLocationEndCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*itemscount);
			SelectLocationEndCombo_->ScrollToTop();
		}
	}
}

void FTripAddEdit::OnDateChanged(const Control& source, int year, int month, int day) {
	controlhandler_->SetValueDate(this, datefieldcurrentid, pDatePicker_->GetDate());
	SetFocus();
	if (popupmovetonextctrl) {
	Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(datefieldcurrentid, true));
	OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
	}
}

void FTripAddEdit::OnDateChangeCanceled(const Control& source) {
   	SetFocus();
}

void FTripAddEdit::OnFocusGained(const Osp::Ui::Control& source) {
	if (this->detailmode) {
		this->SetFocus();
		if ((timerstep > 0) && (timerstep < 6)) {
			pTimer_->Cancel();
		}
		timerstep = 1;
		this->RequestRedraw();
		pTimer_->Start(200);
	} else {
		if ((source.GetName() == L"IDPC_startodometer") || (source.GetName() == L"IDPC_endodometer") || (source.GetName() == L"IDPC_priceperl") || (source.GetName() == L"IDPC_realexpenses")) {
			controlhandler_->ControlUnformatValue(this, source);
		}
		if ((source.GetName() == L"IDPC_startlocation") || (source.GetName() == L"IDPC_endlocation")) {
			SelectLocationComboTimer_->Cancel();
			if (!SelectLocationComboDonthide) {
			SelectLocationComboDonthide = true;
			if (source.GetName() == L"IDPC_startlocation") {
				ReloadLocationCombo(1);
				SelectLocationStartCombo_->SetShowState(true);
				SelectLocationStartCombo_->SetFocus();
				SelectLocationStartCombo_->RequestRedraw(true);
			} else if (source.GetName() == L"IDPC_endlocation") {
				ReloadLocationCombo(2);
				SelectLocationEndCombo_->SetShowState(true);
				SelectLocationEndCombo_->SetFocus();
				SelectLocationEndCombo_->RequestRedraw(true);
			}
			EditField *control = (EditField*) &source;
			control->SetFocus();
			SelectLocationComboDonthide = false;
			RequestRedraw(true);
			}
		} else if ((source.GetName() == L"IDPC_LOCATIONSTART") || (source.GetName() == L"IDPC_LOCATIONEND")) {
			SelectLocationComboTimer_->Cancel();
		} else {
			SelectLocationComboTimer_->Cancel();
			OnTimerExpired(*SelectLocationComboTimer_);
		}
	}
}
void FTripAddEdit::OnFocusLost(const Osp::Ui::Control& source) {
	if ((source.GetName() == L"IDPC_startodometer") || (source.GetName() == L"IDPC_endodometer")) {
		if (!this->detailmode) {
			controlhandler_->ControlFormatValue(this, source);
		}
	} else if ((source.GetName() == L"IDPC_priceperl") || (source.GetName() == L"IDPC_realexpenses")) {
		controlhandler_->ControlFormatValue(this, source);
	}
	if ((source.GetName() == L"IDPC_startlocation") || (source.GetName() == L"IDPC_LOCATIONSTART") || (source.GetName() == L"IDPC_endlocation") || (source.GetName() == L"IDPC_LOCATIONEND")) {
		if (!SelectLocationComboDonthide) {
			SelectLocationComboTimer_->Cancel();
			SelectLocationComboTimer_->Start(200);
		}
	}
}

void FTripAddEdit::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (timer.Equals(*SelectLocationComboTimer_)) {
		SelectLocationStartCombo_->SetShowState(false);
		SelectLocationEndCombo_->SetShowState(false);
		RequestRedraw(true);
	}
	if (timer.Equals(*pTimer_)) {
	if (timerstep < 6) {
		timer.Start(200);
		timerstep++;
		this->RequestRedraw();
	}
	}
}

void FTripAddEdit::PopupSelectLocationSelect(int type, int itemid) {
	LocationData locationdata_;
	String ctrlid;
	if (type == 1) {
		ctrlid = L"IDPC_startlocation";
	} else if (type == 2) {
		ctrlid = L"IDPC_endlocation";
	}
	if (carconclass_->GetLocationData(itemid, locationdata_)) {
		controlhandler_->SetValue(this, ctrlid, locationdata_.Caption);
	} else {
		controlhandler_->SetValue(this, ctrlid, L"");
	}
	if (type == 1) {
		if (SelectLocationStartCombo_->IsVisible()) {
			SelectLocationComboTimer_->Cancel();
			SelectLocationStartCombo_->SetShowState(false);
			RequestRedraw(true);
		}
	} else if (type == 2) {
		if (SelectLocationEndCombo_->IsVisible()) {
			SelectLocationComboTimer_->Cancel();
			SelectLocationEndCombo_->SetShowState(false);
			RequestRedraw(true);
		}
	}
	if (popupmovetonextctrl) {
		Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(ctrlid, true));
		OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
	}
}

int FTripAddEdit::GetLocationId(Osp::Base::String caption) {
	caption.Trim();
	if (caption != L"") {
		return carconclass_->SelectAddLocationData(caption);
	} else {
		return 0;
	}
}

result
FTripAddEdit::OnDraw() {
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
	if (isfirstdraw) {
		SelectLocationStartCombo_->SetShowState(false);
		SelectLocationEndCombo_->SetShowState(false);
		isfirstdraw = false;
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void
FTripAddEdit::OnTextValueChanged(const Osp::Ui::Control& source)
{
	if (!this->detailmode) {
		if ((source.GetName() == L"IDPC_startodometer") || (source.GetName() == L"IDPC_endodometer") || (source.GetName() == L"IDPC_priceperl") || (source.GetName() == L"IDPC_realexpenses")) {
			controlhandler_->ControlFormatValueOnInput(this, source);
		}
		if (source.GetName() == L"IDPC_startlocation") {
			ReloadLocationCombo(1);
			RequestRedraw();
		}
		if (source.GetName() == L"IDPC_endlocation") {
			ReloadLocationCombo(2);
			RequestRedraw();
		}
	}
	if ((source.GetName() == L"IDPC_startodometer") || (source.GetName() == L"IDPC_endodometer") || (source.GetName() == L"IDPC_priceperl") || (source.GetName() == L"IDPC_realexpenses") || (source.GetName() == L"IDPC_passingers")) {
			double priceperl = controlhandler_->GetValueDouble(this,L"IDPC_priceperl");
			double priceperkm(0);
			if ((ConType == SETTINGSCONTYPE_LKM) || (ConType == SETTINGSCONTYPE_GMI) || (ConType == SETTINGSCONTYPE_KMG) || (ConType == SETTINGSCONTYPE_MIL)) {
				priceperkm = (AvgCon * priceperl) / 100;
			} else {
				if (AvgCon > 0) {
					priceperkm = ((100 / AvgCon) * priceperl) / 100;
				} else {
					priceperkm = 0;
				}
			}
			int tripdistance = controlhandler_->GetValueInt(this,L"IDPC_endodometer") - controlhandler_->GetValueInt(this,L"IDPC_startodometer");
			if (tripdistance <= 0)
				tripdistance = 0;
			int stoseb = controlhandler_->GetValueInt(this,L"IDPC_passingers");
			Osp::Ui::Controls::Label *calcexpenseslabel = static_cast<Label *>(GetControl(L"IDPC_calcexpenses", true));
			calcexpenseslabel->SetText(controlhandler_->CurrencyFormater(priceperkm * tripdistance));
			Osp::Ui::Controls::Label *expensesperpersoncaptionlabel = static_cast<Label *>(GetControl(L"IDPC_LABEL7", true));
			Osp::Ui::Controls::Label *expensesperpersonlabel = static_cast<Label *>(GetControl(L"IDPC_expensesperperson", true));
			double realexpenses = controlhandler_->GetValueDouble(this,L"IDPC_realexpenses");
			double expensesperperson(0);
			if (realexpenses > 0) {
				if (stoseb > 0) {
					expensesperperson = realexpenses / stoseb;
				} else {
					expensesperperson = 0;
				}
				expensesperpersoncaptionlabel->SetText(L"Expenses per person:");
				expensesperpersonlabel->SetText(controlhandler_->CurrencyFormater(expensesperperson));
			} else {
				if (stoseb > 0) {
					expensesperperson = (priceperkm * tripdistance) / stoseb;
				} else {
					expensesperperson = 0;
				}
				expensesperpersoncaptionlabel->SetText(L"Calculated expenses per person:");
				expensesperpersonlabel->SetText(controlhandler_->CurrencyFormater(expensesperperson));
			}
	}
}

void
FTripAddEdit::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{

}


bool FTripAddEdit::GetInputData(TripData & data, bool validate) {
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
	if (controlhandler_->GetValueDate(this, L"IDPC_enddate") < controlhandler_->GetValueDate(this, L"IDPC_startdate")) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Validate", "Arrival time can not be less than departure time!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		return false;
	}
	if ((controlhandler_->GetValueInt(this,L"IDPC_endodometer") > 0) && (controlhandler_->GetValueInt(this,L"IDPC_endodometer") < controlhandler_->GetValueInt(this,L"IDPC_startodometer"))) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Validate", "Arrival odometer can not be less than departure odometer!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		return false;
	}
	double priceperl = controlhandler_->GetValueDouble(this,L"IDPC_priceperl");
	double priceperkm(0);
	if ((ConType == SETTINGSCONTYPE_LKM) || (ConType == SETTINGSCONTYPE_GMI) || (ConType == SETTINGSCONTYPE_KMG) || (ConType == SETTINGSCONTYPE_MIL)) {
		priceperkm = (AvgCon * priceperl) / 100;
	} else {
		if (AvgCon > 0) {
			priceperkm = ((100 / AvgCon) * priceperl) / 100;
		} else {
			priceperkm = 0;
		}
	}
	int tripdistance = controlhandler_->GetValueInt(this,L"IDPC_endodometer") - controlhandler_->GetValueInt(this,L"IDPC_startodometer");
	if (tripdistance <= 0)
		tripdistance = 0;
	data.ID = this->itemid;
	data.CarID = curdata_.CarID;
	data.StartTime = controlhandler_->GetValueDate(this, L"IDPC_startdate");
	data.StartOdometer = controlhandler_->GetValueInt(this,L"IDPC_startodometer");
	data.StOseb = controlhandler_->GetValueInt(this,L"IDPC_passingers");
	data.PricePerL = controlhandler_->GetValueDouble(this,L"IDPC_priceperl");
	data.EndTime = controlhandler_->GetValueDate(this, L"IDPC_enddate");
	data.EndOdometer = controlhandler_->GetValueInt(this,L"IDPC_endodometer");
	data.RealExpenses = controlhandler_->GetValueDouble(this,L"IDPC_realexpenses");
	data.Remark = controlhandler_->GetValue(this, L"IDPC_remark");
	data.CalcExpenses = priceperkm * tripdistance;
	if (validate) {
	data.StartLocation = GetLocationId(controlhandler_->GetValue(this, L"IDPC_startlocation"));
	data.EndLocation = GetLocationId(controlhandler_->GetValue(this, L"IDPC_endlocation"));
	}
	return true;
}

bool FTripAddEdit::CheckIfDataChanged(void) {
	TripData data_;
	GetInputData(data_);
	if ((data_.StartTime == curdata_.StartTime) || (data_.StartOdometer == curdata_.StartOdometer) || (data_.StartLocationCaption == curdata_.StartLocationCaption) || (data_.StOseb == curdata_.StOseb) || (data_.EndTime == curdata_.EndTime) || (data_.EndOdometer == curdata_.EndOdometer) || (data_.EndLocationCaption == curdata_.EndLocationCaption) || (data_.PricePerL == curdata_.PricePerL) || (data_.Remark == curdata_.Remark) || (data_.RealExpenses == curdata_.RealExpenses)) {
		return false;
	} else {
		return true;
	}
	return false;
}

bool FTripAddEdit::Save(void) {
	TripData data_;
	if (GetInputData(data_, true)) {
		carconclass_->SaveTripData(data_);
		this->itemid = data_.ID;
		return true;
	} else {
		return false;
	}
	return false;
}

result
FTripAddEdit::OnTerminating(void)
{
	result r = E_SUCCESS;

	controlhandler_->Clear();
	delete pDatePicker_;
	delete pTimer_;
	delete SelectLocationComboItemFormat_;
	delete SelectLocationComboTimer_;

	return r;
}


void
FTripAddEdit::OnActionPerformed(const Control& source, int actionId)
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
			pFormMgr->SendUserEvent(FormMgr::REQUEST_TRIPFORM, list);
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
			pFormMgr->SendUserEvent(FormMgr::REQUEST_TRIPFORM, list);
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
			if (source.GetName() == L"IDPC_priceperl") {
				datefieldcurrentid = L"IDPC_enddate";
				pDatePicker_->SetDate(controlhandler_->GetValueDate(this, datefieldcurrentid));
				pDatePicker_->Show();
			}
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
	}
}

void
FTripAddEdit::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTripAddEdit::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (!this->detailmode) {
		if (source.GetName() == L"IDPC_startdate") {
			datefieldcurrentid = source.GetName();
			popupmovetonextctrl = true;
			pDatePicker_->SetDate(controlhandler_->GetValueDate(this, datefieldcurrentid));
			pDatePicker_->Show();
		}
		if (source.GetName() == L"IDPC_enddate") {
			datefieldcurrentid = source.GetName();
			pDatePicker_->SetDate(controlhandler_->GetValueDate(this, datefieldcurrentid));
			pDatePicker_->Show();
		}
	} else {
		OnFocusGained(source);
	}
}

void FTripAddEdit::OnOverlayControlCreated(const Osp::Ui::Control& source) {

}

void FTripAddEdit::OnOverlayControlOpened(const Osp::Ui::Control& source) {
	//
}

void FTripAddEdit::OnOverlayControlClosed(const Osp::Ui::Control& source) {
	SetFocus();
}

void FTripAddEdit::OnOtherControlSelected(const Osp::Ui::Control& source) {
	ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
	vpScrollPanel->CloseOverlayWindow();
}

void FTripAddEdit::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status) {
	if ((elementId == SelectLocationItemBG) && (itemId > 0)) {
		if (source.GetName() == "IDPC_LOCATIONSTART") {
			PopupSelectLocationSelect(1, itemId);
		} else if (source.GetName() == "IDPC_LOCATIONEND") {
			PopupSelectLocationSelect(2, itemId);
		}
	}
}

void FTripAddEdit::OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status) {
	//
}
