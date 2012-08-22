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
#include "FVnosPorabe.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

FVnosPorabe::FVnosPorabe(int addeditid, bool detailform, bool fromhome)
{
	isfirstdraw = true;
	this->itemid = addeditid;
	this->detailform = detailform;
	this->detailmode = detailform;
	this->fromhome = fromhome;
	this->initformelhideondraw = true;
}

FVnosPorabe::~FVnosPorabe(void)
{
}

bool
FVnosPorabe::Initialize()
{
	Form::Construct(L"IDF_FVNOSPORABE");

	return true;
}

result
FVnosPorabe::OnInitializing(void)
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
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(2);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	carconclass_ = pFormMgr->carconclass_;
	carconclass_->GetSettingsData(settingsdata_);
	carconclass_->GetCarConData(this->itemid, curcarcondata_);

	pDatePicker_ = new DatePicker();
    pDatePicker_->Construct(L"Date");
    pDatePicker_->AddDateChangeEventListener(*this);
    pDatePicker_->SetCurrentDate();
    pDatePicker_->SetYearRange(pDatePicker_->GetYear()-10, pDatePicker_->GetYear());

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	controlhandler_->Add(L"EF_date", ControlHandler::DATETEXT, true, L"Select date!", L"Select date");
	controlhandler_->Add(L"EF_odometer", ControlHandler::NUMBER, true, L"Input odometer or trip value!", L"Input value", settingsdata_.distanceunitstr);
	controlhandler_->Add(L"EF_volume", ControlHandler::DOUBLE, true, L"Input fill up volume!", L"Input liters", settingsdata_.volumeunitstr);
	controlhandler_->Add(L"CB_fueltype", ControlHandler::COMBOBOX, true, L"Select fuel type!", L"Select");
	controlhandler_->Add(L"EF_price", ControlHandler::CURRENCY4, true, L"Input price!", L"Input price");
	controlhandler_->Add(L"CH_partial", ControlHandler::CHECKBOX);
	controlhandler_->Add(L"CH_missed", ControlHandler::CHECKBOX);
	controlhandler_->Add(L"CB_location", ControlHandler::TEXT, false, L"", L"Select location");
	controlhandler_->Add(L"EA_remark", ControlHandler::TEXTAREA, false, L"", "Input remark");

	controlhandler_->InitControls(this,ID_BUTTON_EDITFIELD_DONE,ID_BUTTON_EDITFIELD_CLOSE,static_cast<Osp::Ui::IActionEventListener *>(this),static_cast<Osp::Ui::ITouchEventListener *>(this),static_cast<Osp::Ui::ITextEventListener *>(this),static_cast<Osp::Ui::IFocusEventListener *>(this),static_cast<Osp::Ui::IScrollPanelEventListener *>(this));

	FuelTypeData fueltypedata_;
	pPopupSelectFuelType_ = new Popup();
	pPopupSelectFuelType_->Construct(L"IDP_POPUP2");
	pPopupSelectFuelType_->SetTitleText(L"Select fuel type");
	pListSelectFuelTypes_ = static_cast<List *>(pPopupSelectFuelType_->GetControl(L"IDC_LIST1"));
	if (carconclass_->GetFuelTypeDataListStart(carconclass_->SelectedCar.ID)) {
		while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
			pListSelectFuelTypes_->AddItem(&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
		}
		carconclass_->GetFuelTypeDataListEnd();
	}
	String showmoreitem(L" + Show more +");
	pListSelectFuelTypes_->AddItem(&showmoreitem, null, null, null, -1);
	Osp::Ui::Controls::Button *pListSelectFuelTypesAddButton_ = static_cast<Button *>(pPopupSelectFuelType_->GetControl(L"IDC_BUTTON2"));
	pListSelectFuelTypesAddButton_->SetActionId(ID_SELECTFUELTYPEPOPUPADD);
	pListSelectFuelTypesAddButton_->SetText(L"Add new fuel type");
	pListSelectFuelTypesAddButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button *pListSelectFuelTypesCancelButton_ = static_cast<Button *>(pPopupSelectFuelType_->GetControl(L"IDC_BUTTON1"));
	pListSelectFuelTypesCancelButton_->SetActionId(ID_SELECTFUELTYPEPOPUPCANCEL);
	pListSelectFuelTypesCancelButton_->AddActionEventListener(*this);
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

	pListSelectFuelTypes_->AddItemEventListener(*this);

	SelectLocationComboTimer_ = new Osp::Base::Runtime::Timer;
	SelectLocationComboTimer_->Construct(*this);

	SelectLocationCombo_ = static_cast<CustomList *>(GetControl(L"IDPC_CUSTOMLIST1", true));
	SelectLocationCombo_->AddFocusEventListener(*this);
	SelectLocationCombo_->AddCustomItemEventListener(*this);

	SelectLocationComboItemFormat_ = new CustomListItemFormat();
	SelectLocationComboItemFormat_->Construct();
	SelectLocationComboItemFormat_->AddElement(SelectLocationItemBG, Rectangle(0, 0, SelectLocationCombo_->GetBounds().width, 80));
	SelectLocationComboItemFormat_->SetElementEventEnabled(SelectLocationItemBG,true);


	selectedodometertrip_type = SELECTEDODOMETER;
	selectedpriceunitfull_type = SELECTEDUNITPRICE;

	buttontripodometerswitchbmpodo = drawingclass_->FDrawButtonSwitchToBmpN(false,L"Odometer",L"Trip");
	buttontripodometerswitchbmptrip = drawingclass_->FDrawButtonSwitchToBmpN(true,L"Odometer",L"Trip");
	buttontripodometerswitchbmppricepl = drawingclass_->FDrawButtonSwitchToBmpN(false,L"Price per " + settingsdata_.volumeunitstr,L"Full price");
	buttontripodometerswitchbmppricefull = drawingclass_->FDrawButtonSwitchToBmpN(true,L"Price per " + settingsdata_.volumeunitstr,L"Full price");

	Osp::Ui::Controls::Button *buttontripodometerswitch = static_cast<Button *>(GetControl(L"BTN_tripodometer", true));
	buttontripodometerswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmpodo);
	buttontripodometerswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmpodo);
	buttontripodometerswitch->SetText(L"");
	buttontripodometerswitch->SetActionId(ID_ODOMETERTRIPBTN);
	buttontripodometerswitch->AddActionEventListener(*this);

	Osp::Ui::Controls::Button *buttonpriceswitch = static_cast<Button *>(GetControl(L"BTN_price", true));
	buttonpriceswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmppricepl);
	buttonpriceswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmppricepl);
	buttonpriceswitch->SetText(L"");
	buttonpriceswitch->SetActionId(ID_PRICEUNITFULLBTN);
	buttonpriceswitch->AddActionEventListener(*this);

	Osp::Ui::Controls::EditField *efprice = static_cast<EditField *>(GetControl(L"EF_price", true));
	efprice->SetTitleText(L"Price per " + settingsdata_.volumeunitstr + L" *");
	Osp::Ui::Controls::EditField *efvolume = static_cast<EditField *>(GetControl(L"EF_volume", true));
	efvolume->SetTitleText(L"Fill up " + settingsdata_.volumeunitfullstr + L" *");

	ReloadLocationCombo();

	Osp::Ui::Controls::EditField *selectlocationbomboef_ = static_cast<EditField *>(GetControl(L"CB_location", true));
	selectlocationbomboef_->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_PREDICTIVE, false);
	selectlocationbomboef_->AddTextEventListener(*this);

	if (this->itemid > 0) {
		ReloadSavedData();
		DetailEditMode();
	} else {
		selectedfueltype = 0;
		if (pListSelectFuelTypes_->GetItemCount() == 2) {
			pListSelectFuelTypes_->SetItemChecked(0,true);
			selectedfueltype = pListSelectFuelTypes_->GetItemIdAt(0);
		}
		PopupSelectFuelTypeSelect(selectedfueltype);
		selectedlocation = 0;
		CarConData data_;
		carconclass_->GetCarConGetLastData(carconclass_->SelectedCar.ID, selectedfueltype, data_);
		lastpriceperl = data_.Price;
		minodometer = data_.StKm;
		maxodometer = 0;
		Osp::Ui::Controls::Label *labellastfillupel_ = static_cast<Label *>(GetControl(L"label_lastfillupdate", true));
		if (data_.time == data_.time.GetMinValue()) {
			labellastfillupel_->SetText(L"First fill up");
		} else {
			labellastfillupel_->SetText(L"Last fill up was: " + controlhandler_->DateFormater(data_.time));
		}
		curcarcondata_.PreviusType = data_.Type;
		odometervalue = minodometer;
		tripvalue = 0;
		priceperl = lastpriceperl;
		fullprice = 0;
		curcarcondata_.CarID = carconclass_->GetLastSelectedID();
		controlhandler_->SetValueDate(this, L"EF_date", L"TODAY");
		controlhandler_->SetValue(this,L"EF_odometer",odometervalue);
		controlhandler_->SetValueCurrency(this,L"EF_price",priceperl);
		curcarcondata_.FuelType = selectedfueltype;
		curcarcondata_.Location = selectedlocation;
		curcarcondata_.Price = priceperl;
		curcarcondata_.StKm = odometervalue;
		prevtime = data_.time;
		nexttime = nexttime.GetMaxValue();
		if (data_.time == data_.time.GetMinValue()) {
			controlhandler_->ControlDisable(this, L"CH_missed", true);
			Osp::Ui::Controls::CheckButton * missedcb = static_cast<CheckButton *>(GetControl(L"CH_missed", true));
			missedcb->SetSelected(true);
			missedcb->SetTitleText(L"First fill up record");
			missedcb->SetText(L"First fill up");
			curcarcondata_.Type = CARCONDATATYPEMISSED;
		} else {
			curcarcondata_.Type = 1;
			Osp::Ui::Controls::CheckButton * missedcb = static_cast<CheckButton *>(GetControl(L"CH_missed", true));
			missedcb->SetTitleText(L"I have missed last fill up");
			missedcb->SetText(L"Missed fill up");
		}
		curcarcondata_.time = controlhandler_->GetValueDate(this, L"EF_date");
		curcarcondata_.timejustdate.SetValue(curcarcondata_.time.GetYear(), curcarcondata_.time.GetMonth(), curcarcondata_.time.GetDay());
		OnTextValueChanged(*GetControl(L"EF_odometer", true));
		OnFocusLost(*GetControl(L"EF_odometer", true));
		OnTextValueChanged(*GetControl(L"EF_price", true));
		OnFocusLost(*GetControl(L"EF_price", true));
	}

	return r;
}

void FVnosPorabe::ReloadSavedData() {
	if (this->itemid > 0) {
		carconclass_->GetCarConData(this->itemid, curcarcondata_);
		selectedfueltype = 0;
		int fueltypeindex = pListSelectFuelTypes_->GetItemIndexFromItemId(curcarcondata_.FuelType);
		if (fueltypeindex > -1) {
			pListSelectFuelTypes_->SetItemChecked(fueltypeindex,true);
			selectedfueltype = pListSelectFuelTypes_->GetItemIdAt(fueltypeindex);
		}
		PopupSelectFuelTypeSelect(selectedfueltype);
		minodometer = curcarcondata_.PreviusStKm;
		maxodometer = curcarcondata_.NextStKm;
		prevtime = curcarcondata_.Previustime;
		nexttime = curcarcondata_.Nexttime;
		selectedlocation = curcarcondata_.Location;
		odometervalue = curcarcondata_.StKm;
		tripvalue = curcarcondata_.Trip;
		priceperl = curcarcondata_.Price;
		fullprice = curcarcondata_.Price * curcarcondata_.StL;
		controlhandler_->SetValueDate(this, L"EF_date", curcarcondata_.timejustdate);
		controlhandler_->SetValue(this, L"EF_odometer", curcarcondata_.StKm);
		controlhandler_->SetValue(this, L"EF_volume", curcarcondata_.StL);
		controlhandler_->SetValueCurrency(this, L"EF_price", curcarcondata_.Price);
		controlhandler_->SetValue(this, L"CB_location", curcarcondata_.LocationCaption);
		controlhandler_->SetValue(this, L"EA_remark", curcarcondata_.Remark);
		controlhandler_->SetValue(this,L"CH_missed", ((curcarcondata_.Type == CARCONDATATYPEMISSED) || (curcarcondata_.Type == CARCONDATATYPEMISSEDPARTIAL)));
		controlhandler_->SetValue(this,L"CH_partial", ((curcarcondata_.Type == CARCONDATATYPEPARTIAL) || (curcarcondata_.Type == CARCONDATATYPEMISSEDPARTIAL)));
		OnTextValueChanged(*GetControl(L"EF_odometer", true));
		OnFocusLost(*GetControl(L"EF_odometer", true));
		OnTextValueChanged(*GetControl(L"EF_price", true));
		OnFocusLost(*GetControl(L"EF_price", true));
	}
}

void FVnosPorabe::DetailEditMode() {
	controlhandler_->ControlReadOnly(this,L"EF_date",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_odometer",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_volume",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"CB_fueltype",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EF_price",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"CH_partial",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"CH_missed",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"CB_location",this->detailmode);
	controlhandler_->ControlReadOnly(this,L"EA_remark",this->detailmode);
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
		SetTitleText(L"Fill up detail");
	} else {
		SetTitleText(L"Fill up edit");
	}
	Osp::Ui::Controls::Label *requredfieldslabel = static_cast<Label *>(GetControl(L"label_required", true));
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

class CustomListElementLocationCombo : public ICustomListElement
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
FVnosPorabe::SelectLocationComboItemAdd(Osp::Base::String col1, int itemId, int searchql, int searchposcol1)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementLocationCombo * custom_element = new CustomListElementLocationCombo();

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
 SelectLocationCombo_->AddItem(*newItem, itemId);

}

void
FVnosPorabe::ReloadLocationCombo() {
	String searchq = controlhandler_->GetValue(this, L"CB_location");
	SelectLocationCombo_->RemoveAllItems();
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
					SelectLocationComboItemAdd(locationdata_.Caption, locationdata_.ID, searchq.GetLength(), searchwfoundi1);
					itemscount++;
				}
				if (itemscount > 10) {
					break;
				}
			}
			carconclass_->GetLocationDataListEnd();
		}
		if (itemscount == 0) {
			SelectLocationComboItemAdd(L"New location", 0);
			itemscount++;
		}
	} else {
		SelectLocationComboItemAdd(L"Input at least two characters", 0);
		itemscount++;
	}
	if (itemscount >= 2) {
		SelectLocationCombo_->SetSize(SelectLocationCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*2);
		SelectLocationCombo_->ScrollToTop();
	} else {
		SelectLocationCombo_->SetSize(SelectLocationCombo_->GetBounds().width, SelectLocationComboItemFormat_->GetElement(SelectLocationItemBG).height*itemscount);
		SelectLocationCombo_->ScrollToTop();
	}
}

result
FVnosPorabe::OnDraw() {
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
		SelectLocationCombo_->SetShowState(false);
		isfirstdraw = false;
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void FVnosPorabe::OnDateChanged(const Control& source, int year, int month, int day) {
	if ((pDatePicker_->GetDate() < prevtime) || (pDatePicker_->GetDate() > nexttime)) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Date error", "Date must be greater or same that previous and lower or same that next record date!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
	} else {
		controlhandler_->SetValueDate(this, L"EF_date", pDatePicker_->GetDate());
		SetFocus();
		if (popupmovetonextctrl) {
		Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"EF_date", true));
		OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
		}
	}
}

void FVnosPorabe::OnDateChangeCanceled(const Control& source) {
   	SetFocus();
}

void FVnosPorabe::OnFocusGained(const Osp::Ui::Control& source) {
	if (this->detailmode) {
		this->SetFocus();
		if ((timerstep > 0) && (timerstep < 6)) {
			pTimer_->Cancel();
		}
		timerstep = 1;
		this->RequestRedraw();
		pTimer_->Start(200);
	} else {
		if ((source.GetName() == L"EF_odometer") || (source.GetName() == L"EF_volume") || (source.GetName() == L"EF_price")) {
			controlhandler_->ControlUnformatValue(this, source);
		}
		if (source.GetName() == L"CB_location") {
			SelectLocationComboTimer_->Cancel();
			if (!SelectLocationComboDonthide) {
			SelectLocationComboDonthide = true;
			ReloadLocationCombo();
			SelectLocationCombo_->SetShowState(true);
			SelectLocationCombo_->SetFocus();
			SelectLocationCombo_->RequestRedraw(true);
			EditField *control = (EditField*) &source;
			control->SetFocus();
			SelectLocationComboDonthide = false;
			RequestRedraw(true);
			}
		} else if (source.GetName() == L"IDPC_CUSTOMLIST1") {
			SelectLocationComboTimer_->Cancel();
		} else {
			SelectLocationComboTimer_->Cancel();
			OnTimerExpired(*SelectLocationComboTimer_);
		}
	}
}
void FVnosPorabe::OnFocusLost(const Osp::Ui::Control& source) {
	if ((source.GetName() == L"EF_odometer") || (source.GetName() == L"EF_volume") || (source.GetName() == L"EF_price")) {
		controlhandler_->ControlFormatValue(this, source);
	}
	if (source.GetName() == L"EF_odometer") {
		if (selectedodometertrip_type == SELECTEDODOMETER) {
		if (odometervalue == 0) {
			odometervalue = minodometer;
			if (odometervalue != 0) {
				controlhandler_->SetValue(this, source.GetName(), odometervalue);
				OnTextValueChanged(source);
				controlhandler_->ControlFormatValue(this, source);
			}
		}
		}
	}
	if (source.GetName() == L"EF_price") {
		if (selectedpriceunitfull_type == SELECTEDUNITPRICE) {
		if (priceperl == 0) {
			priceperl = lastpriceperl;
			if (priceperl != 0) {
				controlhandler_->SetValueCurrency(this, source.GetName(), priceperl);
				OnTextValueChanged(source);
				controlhandler_->ControlFormatValue(this, source);
			}
		}
		}
	}
	if ((source.GetName() == L"CB_location") || (source.GetName() == L"IDPC_CUSTOMLIST1")) {
		if (!SelectLocationComboDonthide) {
			SelectLocationComboTimer_->Cancel();
			SelectLocationComboTimer_->Start(200);
		}
	}
}

void FVnosPorabe::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (timer.Equals(*SelectLocationComboTimer_)) {
		SelectLocationCombo_->SetShowState(false);
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

void FVnosPorabe::Odometertripchangeselection(int setselection) {
	Osp::Ui::Controls::Button *buttontripodometerswitch = static_cast<Button *>(GetControl(L"BTN_tripodometer", true));
	Osp::Ui::Controls::EditField *eftripodometer = static_cast<EditField *>(GetControl(L"EF_odometer", true));
	selectedodometertrip_type = setselection;
	if (setselection == SELECTEDODOMETER) {
		buttontripodometerswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmpodo);
		buttontripodometerswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmpodo);
		if (this->detailmode) {
			eftripodometer->SetTitleText(L"Odometer value");
		} else {
			eftripodometer->SetTitleText(L"Odometer value *");
		}
		controlhandler_->SetValue(this,L"EF_odometer",odometervalue);
	} else {
		buttontripodometerswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmptrip);
		buttontripodometerswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmptrip);
		if (this->detailmode) {
			eftripodometer->SetTitleText(L"Trip value");
		} else {
			eftripodometer->SetTitleText(L"Trip value *");
		}
		controlhandler_->SetValue(this,L"EF_odometer",tripvalue);
	}
	OnTextValueChanged(*GetControl(L"EF_odometer", true));
	OnFocusLost(*GetControl(L"EF_odometer", true));
	buttontripodometerswitch->RequestRedraw();
	eftripodometer->RequestRedraw();
}

void FVnosPorabe::Priceunitfullchangeselection(int setselection) {
	Osp::Ui::Controls::Button *buttonpriceswitch = static_cast<Button *>(GetControl(L"BTN_price", true));
	Osp::Ui::Controls::EditField *efprice = static_cast<EditField *>(GetControl(L"EF_price", true));
	selectedpriceunitfull_type = setselection;
	if (setselection == SELECTEDUNITPRICE) {
		buttonpriceswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmppricepl);
		buttonpriceswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmppricepl);
		if (this->detailmode) {
			efprice->SetTitleText(L"Price per " + settingsdata_.volumeunitstr);
		} else {
			efprice->SetTitleText(L"Price per " + settingsdata_.volumeunitstr + L" *");
		}
		controlhandler_->SetValueCurrency(this,L"EF_price",priceperl);
	} else {
		buttonpriceswitch->SetNormalBackgroundBitmap(*buttontripodometerswitchbmppricefull);
		buttonpriceswitch->SetPressedBackgroundBitmap(*buttontripodometerswitchbmppricefull);
		if (this->detailmode) {
			efprice->SetTitleText(L"Full price");
		} else {
			efprice->SetTitleText(L"Full price *");
		}
		controlhandler_->SetValueCurrency(this,L"EF_price",fullprice);
	}
	OnTextValueChanged(*GetControl(L"EF_price", true));
	OnFocusLost(*GetControl(L"EF_price", true));
	buttonpriceswitch->RequestRedraw();
	efprice->RequestRedraw();
}

void FVnosPorabe::PopupSelectFuelTypeSelect(int itemid) {
	FuelTypeData fueltypedata_;
	if (carconclass_->GetFuelTypeData(itemid, fueltypedata_)) {
		controlhandler_->SetValue(this, L"CB_fueltype", fueltypedata_.Caption);
		selectedfueltype = itemid;
	} else {
		controlhandler_->SetValue(this, L"CB_fueltype", L"");
		selectedfueltype = 0;
	}
	if (pPopupSelectFuelType_->IsVisible()) {
		pPopupSelectFuelType_->SetShowState(false);
		RequestRedraw(true);
		if (popupmovetonextctrl) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"CB_fueltype", true));
			OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
		}
	}
}

void FVnosPorabe::PopupSelectLocationSelect(int itemid) {
	LocationData locationdata_;
	if (carconclass_->GetLocationData(itemid, locationdata_)) {
		controlhandler_->SetValue(this, L"CB_location", locationdata_.Caption);
		selectedlocation = itemid;
	} else {
		controlhandler_->SetValue(this, L"CB_location", L"");
		selectedlocation = 0;
	}
	if (SelectLocationCombo_->IsVisible()) {
		SelectLocationComboTimer_->Cancel();
		SelectLocationCombo_->SetShowState(false);
		RequestRedraw(true);
	}
	if (popupmovetonextctrl) {
		Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"CB_location", true));
		OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
	}
}

int FVnosPorabe::GetLocationId(Osp::Base::String caption) {
	caption.Trim();
	if (caption != L"") {
		return carconclass_->SelectAddLocationData(caption);
	} else {
		return 0;
	}
}

void
FVnosPorabe::OnTextValueChanged(const Osp::Ui::Control& source)
{
	if (!this->detailmode) {
	if ((source.GetName() == L"EF_odometer") || (source.GetName() == L"EF_volume") || (source.GetName() == L"EF_price") || (source.GetName() == L"EF_date")) {
		controlhandler_->ControlFormatValueOnInput(this, source);
	}
	if (source.GetName() == L"CB_location") {
		ReloadLocationCombo();
		RequestRedraw();
	}
	}
	if (source.GetName() == L"EF_odometer") {
		Osp::Ui::Controls::Label *labeltripodomrem_ = static_cast<Label *>(GetControl(L"label_odometertrip", true));
		if (selectedodometertrip_type == SELECTEDODOMETER) {
			odometervalue = controlhandler_->GetValueInt(this, L"EF_odometer");
			tripvalue = odometervalue - minodometer;
			if (tripvalue < 0) tripvalue = 0;
			labeltripodomrem_->SetText(L"Trip: " + Integer::ToString(tripvalue) + L" " + settingsdata_.distanceunitstr);
		} else {
			tripvalue = controlhandler_->GetValueInt(this, L"EF_odometer");
			odometervalue = minodometer + tripvalue;
			labeltripodomrem_->SetText(L"Odometer: " + Integer::ToString(odometervalue) + L" " + settingsdata_.distanceunitstr);
		}
		if (labeltripodomrem_->IsVisible())
		labeltripodomrem_->RequestRedraw();
	}
	if ((source.GetName() == L"EF_price") || (source.GetName() == L"EF_volume")) {
		Osp::Ui::Controls::Label *labelpricerem_ = static_cast<Label *>(GetControl(L"label_price", true));
		double volume = controlhandler_->GetValueDouble(this, L"EF_volume");
		if (selectedpriceunitfull_type == SELECTEDUNITPRICE) {
			priceperl = controlhandler_->GetValueDouble(this, L"EF_price");
			fullprice = priceperl * volume;
			labelpricerem_->SetText(L"Full price: " + controlhandler_->CurrencyFormater(fullprice));
		} else {
			fullprice = controlhandler_->GetValueDouble(this, L"EF_price");
			if (volume > 0) {
			priceperl = fullprice / volume;
			} else {
			priceperl = 0;
			}
			labelpricerem_->SetText(L"Price per " + settingsdata_.volumeunitstr + ": " + controlhandler_->CurrencyFormater(priceperl));
		}
		if (labelpricerem_->IsVisible())
		labelpricerem_->RequestRedraw();
	}
}

void
FVnosPorabe::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{

}

bool FVnosPorabe::GetInputData(CarConData & data, bool validate) {
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
	int selectedtype(1);
	if ((controlhandler_->GetValueBool(this, L"CH_missed")) && (controlhandler_->GetValueBool(this, L"CH_partial"))) {
		selectedtype = CARCONDATATYPEMISSEDPARTIAL;
	} else if (controlhandler_->GetValueBool(this, L"CH_partial")) {
		selectedtype = CARCONDATATYPEPARTIAL;
	} else if (controlhandler_->GetValueBool(this, L"CH_missed")) {
		selectedtype = CARCONDATATYPEMISSED;
	}
	if (selectedodometertrip_type == SELECTEDTRIP) {
		if ((validate) && ((selectedtype == CARCONDATATYPEMISSED) || (selectedtype == CARCONDATATYPEMISSEDPARTIAL))) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Type error", "You can't input in trip value if you choose missed fill up!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return false;
		}
	}
	if ((validate) && ((odometervalue < minodometer) || ((odometervalue > maxodometer) && (maxodometer > 0)))) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Odometer error", "Odometer value must be larger than previous and smaller than next record odometer value!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		return false;
	}

	data.ID = this->itemid;
	data.CarID = curcarcondata_.CarID;
	data.FuelType = selectedfueltype;
	data.Location = selectedlocation;
	data.Type = selectedtype;
	data.StKm = odometervalue;
	data.StL = controlhandler_->GetValueDouble(this,L"EF_volume");
	data.Price = priceperl;
	data.Trip = 0;
	data.Con = 0;
	if (data.Type != CARCONDATATYPEMISSED) {
		data.Trip = tripvalue;
	}
	if (data.Type == CARCONDATATYPEOK) {
		if ((curcarcondata_.PreviusType == CARCONDATATYPEPARTIAL) || (curcarcondata_.PreviusType == CARCONDATATYPEMISSEDPARTIAL)) {
			data.Type = CARCONDATATYPEAFTERPARTIAL;
		}
	}
	if (data.Type == CARCONDATATYPEOK) {
		data.Con = (data.StL / data.Trip) * 100;
	}
	data.NextRecId = curcarcondata_.NextRecId;
	data.NextTrip = curcarcondata_.NextTrip;
	data.NextCon = curcarcondata_.NextCon;
	if ((maxodometer > 0) && (curcarcondata_.NextRecId > 0)) {
		data.NextTrip = maxodometer - data.StKm;
		if (curcarcondata_.NextType == CARCONDATATYPEOK) {
			data.NextCon = (curcarcondata_.NextStL / data.NextTrip) * 100;
		}
	}
	data.time = controlhandler_->GetValueDate(this, L"EF_date");
	data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
	data.Remark = controlhandler_->GetValue(this, L"EA_remark");
	if (validate) {
	data.Location = GetLocationId(controlhandler_->GetValue(this, L"CB_location"));
	}

	return true;
}

bool FVnosPorabe::CheckIfDataChanged(void) {
	CarConData data_;
	GetInputData(data_);
	if ((data_.FuelType == curcarcondata_.FuelType) && (data_.Location == curcarcondata_.Location) && (data_.Price == curcarcondata_.Price) && (data_.Remark == curcarcondata_.Remark) && (data_.StKm == curcarcondata_.StKm) && (data_.StL == curcarcondata_.StL) && (data_.Type == curcarcondata_.Type) && (data_.timejustdate == curcarcondata_.timejustdate)) {
		return false;
	} else {
		return true;
	}
	return false;
}

bool FVnosPorabe::Save(void) {
	CarConData data_;
	if (GetInputData(data_, true)) {
		carconclass_->SaveCarConData(data_);
		return true;
	} else {
		return false;
	}
	return false;
}

result
FVnosPorabe::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete pPopupSelectFuelType_;
	delete pAddnewfueltypepopup_;
	controlhandler_->Clear();
	delete pDatePicker_;
	delete buttontripodometerswitchbmptrip;
	delete buttontripodometerswitchbmpodo;
	delete buttontripodometerswitchbmppricepl;
	delete buttontripodometerswitchbmppricefull;

	delete SelectLocationComboItemFormat_;
	delete SelectLocationComboTimer_;

	delete pTimer_;

	return r;
}


void
FVnosPorabe::OnActionPerformed(const Control& source, int actionId)
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
			if (this->fromhome) {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			} else {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				ArrayList * list = new ArrayList;
				list->Construct();
				list->Add(*(new Integer(this->itemid)));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_CONFORM, list);
			}
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
	} else if (actionId == ID_SAVE) {
		if (Save() == false) {
			return;
		}
		if (!this->detailform) {
			if (this->fromhome) {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			} else {
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				ArrayList * list = new ArrayList;
				list->Construct();
				list->Add(*(new Integer(this->itemid)));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_CONFORM, list);
			}
		} else {
			OnActionPerformed(source, ID_DETAIL);
		}
	} else if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		popupmovetonextctrl = false;
		this->SetFocus();
		if (actionId == ID_DETAIL) {
			ReloadSavedData();
		}
		this->detailmode = (actionId == ID_DETAIL);
		DetailEditMode();
		RequestRedraw(true);
	} else if (actionId == ID_ODOMETERTRIPBTN) {
		if (selectedodometertrip_type == SELECTEDODOMETER) {
			Odometertripchangeselection(SELECTEDTRIP);
		} else {
			Odometertripchangeselection(SELECTEDODOMETER);
		}
	} else if (actionId == ID_PRICEUNITFULLBTN) {
		if (selectedpriceunitfull_type == SELECTEDUNITPRICE) {
			Priceunitfullchangeselection(SELECTEDFULLPRICE);
		} else {
			Priceunitfullchangeselection(SELECTEDUNITPRICE);
		}
	} else if (actionId == ID_ADDNEWFUELTYPEPOPUPCANCEL) {
		pAddnewfueltypepopup_->SetShowState(false);
		RequestRedraw(true);
	}  else if (actionId == ID_ADDNEWFUELTYPEPOPUPSAVE) {
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
			int selecteditemindex = pListSelectFuelTypes_->GetItemIndexFromItemId(-1);
			if (selecteditemindex > -1) {
				pListSelectFuelTypes_->InsertItemAt(selecteditemindex,&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
			} else {
				pListSelectFuelTypes_->AddItem(&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
			}
			selecteditemindex = pListSelectFuelTypes_->GetItemIndexFromItemId(fueltypedata_.ID);
			if (selecteditemindex > -1) {
				pListSelectFuelTypes_->SetItemChecked(selecteditemindex, true);
			}
			pListSelectFuelTypes_->ScrollToBottom();
			pAddnewfueltypepopup_->SetShowState(false);
			RequestRedraw(true);
			PopupSelectFuelTypeSelect(fueltypedata_.ID);
		}
	} else if (actionId == ID_SELECTFUELTYPEPOPUPADD) {
		Osp::Ui::Controls::EditField *pAddnewfueltypepopupEditField_ = static_cast<EditField *>(pAddnewfueltypepopup_->GetControl(L"IDC_EDITFIELD1"));
		pAddnewfueltypepopupEditField_->SetText(L"");
		pAddnewfueltypepopup_->SetShowState(true);
		pAddnewfueltypepopup_->Show();
	} else if (actionId == ID_SELECTFUELTYPEPOPUPCANCEL) {
		if (pPopupSelectFuelType_->IsVisible()) {
			pPopupSelectFuelType_->SetShowState(false);
			RequestRedraw(true);
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_DONE) {
		popupmovetonextctrl = true;
		int actiondone = controlhandler_->ActionDone(this, source);
		if ((actiondone == ControlHandler::ACTIONDONERESULT_HIDEKEYPAD) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) || (actiondone == ControlHandler::ACTIONDONERESULT_OPENDATEPICKER)) {
			ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
			vpScrollPanel->CloseOverlayWindow();
		}
		if (actiondone == ControlHandler::ACTIONDONERESULT_OPENDATEPICKER) {
			popupmovetonextctrl = true;
			pDatePicker_->SetDate(controlhandler_->GetValueDate(this, L"EF_date"));
			pDatePicker_->Show();
		}
		if (actiondone == ControlHandler::ACTIONDONERESULT_OPENCOMBOBOX) {
			popupmovetonextctrl = true;
			if (source.GetName() == L"EF_volume") {
				if (pListSelectFuelTypes_->GetItemCount() <= 2) {
					Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"CB_fueltype", true));
					OnActionPerformed(*ctrlel, ID_BUTTON_EDITFIELD_DONE);
				} else {
					pPopupSelectFuelType_->SetShowState(true);
					pPopupSelectFuelType_->Show();
				}
			}
		}
	} else if (actionId == ID_BUTTON_EDITFIELD_CLOSE) {
		popupmovetonextctrl = false;
		ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
		vpScrollPanel->CloseOverlayWindow();
	}
}

void
FVnosPorabe::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FVnosPorabe::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (!this->detailmode) {
	popupmovetonextctrl = false;
	if (source.GetName() == L"CB_fueltype") {
		pPopupSelectFuelType_->SetShowState(true);
		pPopupSelectFuelType_->Show();
	}
	if (source.GetName() == L"EF_date") {
		popupmovetonextctrl = true;
		pDatePicker_->SetDate(controlhandler_->GetValueDate(this, L"EF_date"));
		pDatePicker_->Show();
	}
	} else {
		OnFocusGained(source);
	}
}

void
FVnosPorabe::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (source.GetName() == "IDC_LIST1") {
		if (pPopupSelectFuelType_->IsVisible()) {
		if (itemId == -1) {
			pPopupSelectFuelType_->SetTitleText(L"Select fuel type [all]");
			int itemindex = pListSelectFuelTypes_->GetItemIndexFromItemId(-1);
			if (itemindex > -1) {
				pListSelectFuelTypes_->RemoveItemAt(itemindex);
			}
			FuelTypeData fueltypedata_;
			if (carconclass_->GetFuelTypeDataListStart(carconclass_->SelectedCar.ID, true)) {
				while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
					pListSelectFuelTypes_->AddItem(&fueltypedata_.Caption, null, null, null, fueltypedata_.ID);
				}
				carconclass_->GetFuelTypeDataListEnd();
			}
			pPopupSelectFuelType_->RequestRedraw(true);
		} else {
			PopupSelectFuelTypeSelect(itemId);
		}
		}
	}
}

void FVnosPorabe::OnOverlayControlCreated(const Osp::Ui::Control& source) {

}

void FVnosPorabe::OnOverlayControlOpened(const Osp::Ui::Control& source) {
	//
}

void FVnosPorabe::OnOverlayControlClosed(const Osp::Ui::Control& source) {
	SetFocus();
}

void FVnosPorabe::OnOtherControlSelected(const Osp::Ui::Control& source) {
	ScrollPanel * vpScrollPanel = static_cast<ScrollPanel *> (GetControl(L"IDC_SCROLLPANEL1"));
	vpScrollPanel->CloseOverlayWindow();
}

void FVnosPorabe::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status) {
	if ((elementId == SelectLocationItemBG) && (itemId > 0)) {
		PopupSelectLocationSelect(itemId);
	}
}
