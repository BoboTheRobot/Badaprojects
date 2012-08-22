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
/*
 * ControlHandlerItem.cpp
 *
 *  Created on: 16.10.2010
 *      Author: mrak
 */

#include "ControlHandlerItem.h"

ControlHandlerItem::ControlHandlerItem(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext, Osp::Base::String metext) {
	this->controlid = controlid;
	this->type = type;
	this->required = required;
	this->validateerrmsg = validateerrmsg;
	this->guidetext = guidetext;
	this->metext = metext;
}

ControlHandlerItem::~ControlHandlerItem(void) {

}

ControlHandler::ControlHandler() {
	controlslist_ = null;
}

ControlHandler::~ControlHandler(void) {
	if (controlslist_ != null) {
		controlslist_->RemoveAll(true);
		delete controlslist_;
	}
	controlslist_ = null;
}

bool ControlHandler::Construct(Osp::Base::String countrycode) {
	controlslist_ = new Osp::Base::Collection::ArrayList;

	Osp::Locales::LocaleManager localeManager;
	localeManager.Construct();
	Osp::Locales::Locale systemlocalesettings = localeManager.GetSystemLocale();
	if (countrycode != L"") {
		Osp::Base::String langcode(L""), countrycodestr(L"");
		Osp::Base::Utility::StringTokenizer countrycodetok(countrycode, L"_");
		if (countrycodetok.GetTokenCount() == 2) {
			countrycodetok.GetNextToken(langcode);
			countrycodetok.GetNextToken(countrycodestr);
		}
		if ((langcode != L"") && (countrycodestr != L"")) {
			Osp::Locales::Locale localesettings(Osp::Locales::Locale::StringToLanguageCode(langcode), Osp::Locales::Locale::StringToCountryCode(countrycodestr));
			this->pCurrencyFormatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(localesettings);
			this->pCurrency4Formatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(localesettings);
		} else {
			this->pCurrencyFormatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(systemlocalesettings);
			this->pCurrency4Formatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(systemlocalesettings);
		}
	} else {
		this->pCurrencyFormatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(systemlocalesettings);
		this->pCurrency4Formatter = Osp::Locales::NumberFormatter::CreateCurrencyFormatterN(systemlocalesettings);
	}
	this->pCurrency4Formatter->SetMaxFractionDigits(4);
	this->pCurrency4Formatter->SetMinFractionDigits(4);
	this->pDateFormatter = Osp::Locales::DateTimeFormatter::CreateDateFormatterN(systemlocalesettings,  Osp::Locales::DATE_TIME_STYLE_MEDIUM);
	this->pDateFormatter->ApplyPattern("d MMM yyyy");

	return true;
}

bool ControlHandler::Construct() {
	return this->Construct(L"");
}

bool ControlHandler::Reconstruct() {
	if (controlslist_ != null) {
		controlslist_->RemoveAll(true);
	}
	return true;
}

bool ControlHandler::Clear() {
	/*if (controlslist_ != null) {
		controlslist_->RemoveAll(true);
	}*/
	return true;
}

bool ControlHandler::Destroy() {
	if (controlslist_ != null) {
		controlslist_->RemoveAll(true);
		delete controlslist_;
	}
	controlslist_ = null;
	delete this->pCurrencyFormatter;
	delete this->pDateFormatter;
	delete this->pCurrency4Formatter;
	return true;
}

bool ControlHandler::Add(Osp::Base::String controlid, int type) {
	controlslist_->Add(*(new ControlHandlerItem(controlid, type, false, L"", L"", L"")));
	return true;
}

bool ControlHandler::Add(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext) {
	controlslist_->Add(*(new ControlHandlerItem(controlid, type, required, validateerrmsg, guidetext, L"")));
	return true;
}

bool ControlHandler::Add(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext, Osp::Base::String metext) {
	controlslist_->Add(*(new ControlHandlerItem(controlid, type, required, validateerrmsg, guidetext, metext)));
	return true;
}

bool ControlHandler::InitControls(Osp::Ui::Controls::Form * formobj, int DoneActionId, int CloseActionId, Osp::Ui::IActionEventListener * actioneventlistner, Osp::Ui::ITouchEventListener * toucheventlistner, Osp::Ui::ITextEventListener * texteventlistner, Osp::Ui::IFocusEventListener * focuseventlistner, Osp::Ui::IScrollPanelEventListener * scrollpaneleventlistner) {
	ControlHandlerItem* ctrlhandleritem_;
	Osp::Ui::Controls::EditField * fctrlef;
	Osp::Ui::Controls::EditArea * fctrlea;
	Osp::Ui::Controls::CheckButton * fctrlcb;
	for (int ictrl=0;ictrl < controlslist_->GetCount();ictrl++) {
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(ictrl));
		if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
			fctrlef = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlef->AddFocusEventListener(*focuseventlistner);
			fctrlef->AddActionEventListener(*actioneventlistner);
			fctrlef->AddScrollPanelEventListener(*scrollpaneleventlistner);
			if (ictrl == controlslist_->GetCount()-1) {
			fctrlef->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_LEFT, L"Done", DoneActionId);
			} else {
			fctrlef->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_LEFT, L"Next", DoneActionId);
			}
			fctrlef->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_RIGHT, L"Close", CloseActionId);
			if ((ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
				fctrlef->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_NUMERIC, true);
				fctrlef->AddTextEventListener(*texteventlistner);
			}
		}
		if (ctrlhandleritem_->type == COMBOBOX) {
			fctrlef = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlef->AddFocusEventListener(*focuseventlistner);
			fctrlef->SetKeypadEnabled(false);
			fctrlef->AddTouchEventListener(*toucheventlistner);
		}
		if (ctrlhandleritem_->type == DATETEXT) {
			fctrlef = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlef->AddFocusEventListener(*focuseventlistner);
			fctrlef->SetKeypadEnabled(false);
			fctrlef->AddTouchEventListener(*toucheventlistner);
		}
		if (ctrlhandleritem_->type == TEXTAREA) {
			fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlea->AddTouchEventListener(*toucheventlistner);
			fctrlea->AddActionEventListener(*actioneventlistner);
			fctrlea->AddScrollPanelEventListener(*scrollpaneleventlistner);
			if (ictrl == controlslist_->GetCount()-1) {
			fctrlea->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_LEFT, L"Done", DoneActionId);
			} else {
			fctrlea->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_LEFT, L"Next", DoneActionId);
			}
			fctrlea->SetOverlayKeypadCommandButton(Osp::Ui::Controls::COMMAND_BUTTON_POSITION_RIGHT, L"Close", CloseActionId);
		}
		if (ctrlhandleritem_->type == CHECKBOX) {
			fctrlcb = static_cast<Osp::Ui::Controls::CheckButton *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlcb->AddFocusEventListener(*focuseventlistner);
			fctrlcb->AddTouchEventListener(*toucheventlistner);
		}
	}
	return true;
}

int ControlHandler::FindControlIndex(Osp::Base::String controlid) {
	int findcontrol(-1);
	ControlHandlerItem* ctrlhandleritem_;
	for (int ictrl=0;ictrl < controlslist_->GetCount();ictrl++) {
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(ictrl));
		if (ctrlhandleritem_->controlid == controlid) {
			findcontrol = ictrl;
			break;
		}
	}
	return findcontrol;
}

bool ControlHandler::ControlDisable(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool disable) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX) || (ctrlhandleritem_->type == DATETEXT)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			ctrlel->SetEnabled(!disable);
		}
		if (ctrlhandleritem_->type == TEXTAREA) {
			Osp::Ui::Controls::EditArea * fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlea->SetEnabled(!disable);
		}
		if (ctrlhandleritem_->type == CHECKBOX) {
			Osp::Ui::Controls::CheckButton * fctrlcb = static_cast<Osp::Ui::Controls::CheckButton *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlcb->SetEnabled(!disable);
		}
		return true;
	} else {
		return false;
	}
}

bool ControlHandler::ControlReadOnly(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool readonly) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX) || (ctrlhandleritem_->type == DATETEXT)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			if ((ctrlhandleritem_->type != COMBOBOX) && (ctrlhandleritem_->type != DATETEXT)) {
			ctrlel->SetKeypadEnabled(!readonly);
			}
			if (ctrlhandleritem_->required) {
			Osp::Base::String settitle = ctrlel->GetTitleText();
			if (readonly) {
				if (settitle.EndsWith(L"*")) {
					settitle.SubString(0, settitle.GetLength()-1, settitle);
					ctrlel->SetTitleText(settitle);
				}
				ctrlel->SetGuideText(L"");
			} else {
				if (!settitle.EndsWith(L"*")) {
					settitle.Append(L"*");
					ctrlel->SetTitleText(settitle);
				}
				ctrlel->SetGuideText(ctrlhandleritem_->guidetext);
			}
			} else {
				if (readonly) {
					ctrlel->SetGuideText(L"");
				} else {
					ctrlel->SetGuideText(ctrlhandleritem_->guidetext);
				}
			}
		}
		if (ctrlhandleritem_->type == TEXTAREA) {
			Osp::Ui::Controls::EditArea * fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlea->SetKeypadEnabled(!readonly);
			if (readonly) {
				fctrlea->SetGuideText(L"");
			} else {
				fctrlea->SetGuideText(ctrlhandleritem_->guidetext);
			}
		}
		if (ctrlhandleritem_->type == CHECKBOX) {
			Osp::Ui::Controls::CheckButton * fctrlcb = static_cast<Osp::Ui::Controls::CheckButton *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlcb->SetEnabled(!readonly);
		}
		return true;
	} else {
		return false;
	}
}

bool ControlHandler::SetValueString(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		value.Trim();
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX) || (ctrlhandleritem_->type == DATETEXT)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			if (ctrlhandleritem_->metext != L"") {
				ctrlel->SetText(value + L" " + ctrlhandleritem_->metext);
			} else {
				ctrlel->SetText(value);
			}
			if (ctrlel->IsVisible()) {
				ctrlel->RequestRedraw();
			}
		}
		if (ctrlhandleritem_->type == TEXTAREA) {
			Osp::Ui::Controls::EditArea * fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlea->SetText(value);
			if (fctrlea->IsVisible()) {
				fctrlea->RequestRedraw();
			}
		}
		return true;
	} else {
		return false;
	}
}

bool ControlHandler::SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value) {
	return SetValueString(formobj, controlid, value);
}

bool ControlHandler::SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, int value) {
	Osp::Base::String strvalue;
	strvalue = Osp::Base::Integer::ToString(value);
	return SetValueString(formobj, controlid, strvalue);
}

bool ControlHandler::SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, double value) {
	Osp::Base::String strvalue;
	strvalue.Format(25,L"%4.2f",value);
	return SetValueString(formobj, controlid, strvalue);
}

bool ControlHandler::SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool value) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == CHECKBOX) {
			Osp::Ui::Controls::CheckButton * fctrlcb = static_cast<Osp::Ui::Controls::CheckButton *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			fctrlcb->SetSelected(value);
			if (fctrlcb->IsVisible()) {
				fctrlcb->RequestRedraw();
			}
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool ControlHandler::SetValueCurrency(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, double value) {
	Osp::Base::String strvalue;
	bool iscurrency4(false);
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		iscurrency4 = (ctrlhandleritem_->type == CURRENCY4);
	}
	if (iscurrency4) {
		this->pCurrency4Formatter->Format(value, strvalue);
	} else {
		this->pCurrencyFormatter->Format(value, strvalue);
	}
	return SetValueString(formobj, controlid, strvalue);
}

bool ControlHandler::SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::DateTime value) {
	Osp::Base::String strvalue = this->DateFormater(value);
	return SetValueString(formobj, controlid, strvalue);
}

bool ControlHandler::SetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::DateTime value) {
	return SetValue(formobj, controlid, value);
}

bool ControlHandler::SetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value) {
	Osp::Base::DateTime datevalue;
	if (value == L"TODAY") {
		Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, datevalue);
	} else {
		Osp::Base::DateTime::Parse(value, datevalue);
	}
	return SetValue(formobj, controlid, datevalue);
}

Osp::Base::String ControlHandler::GetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		Osp::Base::String value = L"";
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			value = ctrlel->GetText();
			value.Trim();
			if (ctrlhandleritem_->type == NUMBER) {
				this->ParseInteger(value);
			} else if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
				this->ParseFloat(value);
			}
		}
		if (ctrlhandleritem_->type == TEXTAREA) {
			Osp::Ui::Controls::EditArea * fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			value = fctrlea->GetText();
			value.Trim();
		}
		return value;
	} else {
		return L"";
	}
}

int ControlHandler::GetValueInt(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		int value = 0;
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == NUMBER) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String strvalue = ctrlel->GetText();
			strvalue.Trim();
			value = this->ParseInteger(strvalue);
		}
		return value;
	} else {
		return 0;
	}
}

double ControlHandler::GetValueDouble(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		double value = 0;
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String strvalue = ctrlel->GetText();
			strvalue.Trim();
			value = this->ParseFloat(strvalue);
		}
		return value;
	} else {
		return 0.0f;
	}
}

bool ControlHandler::GetValueBool(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		bool value = false;
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == CHECKBOX) {
			Osp::Ui::Controls::CheckButton * ctrlcb = static_cast<Osp::Ui::Controls::CheckButton *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			value = ctrlcb->IsSelected();
		}
		return value;
	} else {
		return false;
	}
}

Osp::Base::DateTime ControlHandler::ParseDateTime(Osp::Base::String value) {
	Osp::Base::DateTime datevalue;
	Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, datevalue);
	Osp::Base::String datef;
	datef = pDateFormatter->GetPattern();
	int month = datevalue.GetMonth();
	int day = datevalue.GetDay();
	int year = datevalue.GetYear();

	Osp::Base::Utility::StringTokenizer strTok(value, L" ");

	Osp::Base::String daystr;
	Osp::Base::String monthstr;
	Osp::Base::String yearstr;

    strTok.GetNextToken(daystr);
    Osp::Base::Integer::Parse(daystr,10,day);

    strTok.GetNextToken(monthstr);
    Osp::Locales::LocaleManager localeManager;
	localeManager.Construct();
    Osp::Locales::DateTimeFormatter * tmpdateformater = Osp::Locales::DateTimeFormatter::CreateDateFormatterN(localeManager.GetSystemLocale());
	tmpdateformater->ApplyPattern(L"MMM");
	Osp::Base::String monthname;
	Osp::Base::DateTime tmpdate;
	for (int monthnamei=1;monthnamei <= 12;monthnamei++) {
		tmpdate.SetValue(2010,monthnamei,1,0,0,0);
		monthname = L"";
		tmpdateformater->Format(tmpdate, monthname);
		if (monthname == monthstr) {
			month = monthnamei;
			break;
		}
	}
	delete tmpdateformater;

    strTok.GetNextToken(yearstr);
    Osp::Base::Integer::Parse(yearstr,10,year);

	datevalue.SetValue(year,month,day,0,0,0);
	return datevalue;
}

Osp::Base::DateTime ControlHandler::GetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid) {
	int findcontrol = FindControlIndex(controlid);
	if (findcontrol >= 0) {
		Osp::Base::DateTime value;
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == DATETEXT) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String strvalue = ctrlel->GetText();
			strvalue.Trim();
			value = ParseDateTime(strvalue);
		}
		return value;
	} else {
		Osp::Base::DateTime value;
		return value;
	}
}

Osp::Base::String ControlHandler::ValidateFields(Osp::Ui::Controls::Form * formobj) {
	ControlHandlerItem* ctrlhandleritem_;
	for (int ictrl=0;ictrl < controlslist_->GetCount();ictrl++) {
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(ictrl));
		if (ctrlhandleritem_->required) {
			if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX)) {
				if (ctrlhandleritem_->type == NUMBER) {
					if (this->GetValueInt(formobj, ctrlhandleritem_->controlid) == 0) {
						return ctrlhandleritem_->validateerrmsg;
					}
				} else if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
					if (this->GetValueDouble(formobj, ctrlhandleritem_->controlid) == 0) {
						return ctrlhandleritem_->validateerrmsg;
					}
				} else {
					if (this->GetValue(formobj, ctrlhandleritem_->controlid) == L"") {
						return ctrlhandleritem_->validateerrmsg;
					}
				}

			}
		}
	}
	return L"";
}

double ControlHandler::ParseFloat(Osp::Base::String & tmps, bool roundto4) {
	double floatval;
	if (tmps.GetLength() < 1) {
	floatval = 0;
	} else {
	tmps.Replace(L"#",L"#");
	tmps.Replace(L"*",L"#");
	tmps.Replace(L".",L"#");
	tmps.Replace(L",",L"#");
	int decimalkapos=0;
	if (tmps.LastIndexOf(L"#",tmps.GetLength()-1,decimalkapos) == E_SUCCESS) {
		tmps.Replace(L"#",L".",decimalkapos);
		tmps.Replace(L"#","");
	}
	mchar tchar;
	Osp::Base::String outputstr;
	for (int n=0; n<tmps.GetLength(); n++) {
		tmps.GetCharAt(n,tchar);
		if ((Osp::Base::Character::IsDigit(tchar)) || (Osp::Base::Character::ToString(tchar) == L".")) {
			outputstr += Osp::Base::Character::ToString(tchar);
		}
	}
	tmps = outputstr;
	if (tmps.GetLength() < 1) {
	tmps = L"0";
	} else {
	if (tmps.IndexOf(L".",0,decimalkapos) == E_SUCCESS) {
		if (decimalkapos == tmps.GetLength()-1) {
			tmps.Replace(L".","");
		}
	}
	}
	if (Osp::Base::Double::Parse(tmps, floatval) == E_SUCCESS) {
		if (roundto4) {
			tmps.Format(25,L"%4.4f", floatval);
		} else {
			tmps.Format(25,L"%4.2f", floatval);
		}
	} else {
		floatval = 0;
		tmps = L"0";
	}
	}
	return floatval;
}

int ControlHandler::ParseInteger(Osp::Base::String & tmps) {
	int intval;
	tmps = Osp::Base::Double::ToString(this->ParseFloat(tmps));
	int decimalkapos=0;
	if (tmps.IndexOf(L".",0,decimalkapos) == E_SUCCESS) {
	tmps.SetLength(decimalkapos);
	}
	if (Osp::Base::Integer::Parse(tmps, intval) != E_SUCCESS) {
		intval = 0;
		tmps = L"0";
	}
	return intval;
}

bool ControlHandler::ControlUnformatValue(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source) {
	int findcontrol = FindControlIndex(source.GetName());
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == NUMBER) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			int value = this->ParseInteger(tmps);
			if (value == 0) {
				ctrlel->SetText(L"");
			} else {
				ctrlel->SetText(tmps);
			}
		} else if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			double value(0);
			if (ctrlhandleritem_->type == CURRENCY4) {
				value = this->ParseFloat(tmps, true);
			} else {
				value = this->ParseFloat(tmps);
			}
			if (value == 0) {
				ctrlel->SetText(L"");
			} else {
				ctrlel->SetText(tmps);
			}
		}
		return true;
	} else {
		return false;
	}
	return false;
}

bool ControlHandler::ControlFormatValue(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source) {
	int findcontrol = FindControlIndex(source.GetName());
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == NUMBER) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			int value = this->ParseInteger(tmps);
			if (value == 0) {
				ctrlel->SetText(L"");
			} else {
				if (ctrlhandleritem_->metext != L"") {
					tmps = tmps + L" " + ctrlhandleritem_->metext;
				}
				ctrlel->SetText(tmps);
			}
		} else if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			double value(0);
			if (ctrlhandleritem_->type == CURRENCY4) {
				value = this->ParseFloat(tmps, true);
			} else {
				value = this->ParseFloat(tmps);
			}
			tmps = L"";
			if (ctrlhandleritem_->type == CURRENCY4) {
				this->pCurrency4Formatter->Format(value, tmps);
			} else {
				this->pCurrencyFormatter->Format(value, tmps);
			}
			if (value == 0) {
				ctrlel->SetText(L"");
			} else {
				if (ctrlhandleritem_->metext != L"") {
					tmps = tmps + L" " + ctrlhandleritem_->metext;
				}
				ctrlel->SetText(tmps);
			}
		} else if (ctrlhandleritem_->type == DOUBLE) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			double value = this->ParseFloat(tmps);
			if (value == 0) {
				ctrlel->SetText(L"");
			} else {
				if (ctrlhandleritem_->metext != L"") {
					tmps = tmps + L" " + ctrlhandleritem_->metext;
				}
				ctrlel->SetText(tmps);
			}
		}
		return true;
	} else {
		return false;
	}
}

bool ControlHandler::ControlFormatValueOnInput(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source) {
	int findcontrol = FindControlIndex(source.GetName());
	if (findcontrol >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(findcontrol));
		if (ctrlhandleritem_->type == NUMBER) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			int value = this->ParseInteger(tmps);
			if ((value == 0)) {
				ctrlel->SetText(L"");
			} else if (tmps != ctrlel->GetText()) {
				ctrlel->SetText(tmps);
			}
		} else if ((ctrlhandleritem_->type == CURRENCY) || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE)) {
			Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
			Osp::Base::String tmps = ctrlel->GetText();
			Osp::Base::String tmps2 = tmps;
			double value(0);
			if (ctrlhandleritem_->type == CURRENCY4) {
				value = this->ParseFloat(tmps, true);
			} else {
				value = this->ParseFloat(tmps);
			}
			if (tmps2.GetLength() > 0) {
			int decimalkapos=0;
			tmps2.Replace(L"#",L"#");
			tmps2.Replace(L"*",L"#");
			if (tmps2.IndexOf(L".",0,decimalkapos) != E_SUCCESS) {
			tmps2.Replace(L".",L"#");
			tmps2.Replace(L",",L"#");
			if (tmps2.LastIndexOf(L"#",tmps2.GetLength()-1,decimalkapos) == E_SUCCESS) {
				tmps2.Replace(L"#",L".",decimalkapos);
				tmps2.Replace(L"#","");
			}
			}
			mchar tchar;
			Osp::Base::String outputstr;
			for (int n=0; n<tmps2.GetLength(); n++) {
				tmps2.GetCharAt(n,tchar);
				if ((Osp::Base::Character::IsDigit(tchar)) || (Osp::Base::Character::ToString(tchar) == L".")) {
					outputstr += Osp::Base::Character::ToString(tchar);
				}
			}
			tmps2 = outputstr;
			if (tmps2.GetLength() > 0) {
			if (tmps2.IndexOf(L".",0,decimalkapos) == E_SUCCESS) {
				if (ctrlhandleritem_->type == CURRENCY4) {
					if (decimalkapos < tmps2.GetLength()-5) {
						tmps2.SubString(0,decimalkapos+5,tmps2);
					}
				} else {
					if (decimalkapos < tmps2.GetLength()-3) {
						tmps2.SubString(0,decimalkapos+3,tmps2);
					}
				}
			}
			}
			tmps = tmps2;
			}
			if ((value == 0) && (ctrlel->GetText() != L"")) {
				ctrlel->SetText(L"");
			} else if (tmps != ctrlel->GetText()) {
				ctrlel->SetText(tmps);
			}
		}
		return true;
	} else {
		return false;
	}
}

int ControlHandler::ActionDone(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source) {
	int sourcetabindex = FindControlIndex(source.GetName());
	if (sourcetabindex >= 0) {
		ControlHandlerItem* ctrlhandleritem_;
		sourcetabindex++;
		while (sourcetabindex < controlslist_->GetCount()) {
			ctrlhandleritem_ = static_cast<ControlHandlerItem *> (controlslist_->GetAt(sourcetabindex));
			if ((ctrlhandleritem_->type == TEXT) || (ctrlhandleritem_->type == NUMBER) || (ctrlhandleritem_->type == CURRENCY)  || (ctrlhandleritem_->type == CURRENCY4) || (ctrlhandleritem_->type == DOUBLE) || (ctrlhandleritem_->type == COMBOBOX) || (ctrlhandleritem_->type == DATETEXT)) {
				Osp::Ui::Controls::EditField * ctrlel = static_cast<Osp::Ui::Controls::EditField *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
				if (ctrlhandleritem_->type == COMBOBOX) {
					ctrlel->SetFocus();
					return ACTIONDONERESULT_OPENCOMBOBOX;
				} else if (ctrlhandleritem_->type == DATETEXT) {
					ctrlel->SetFocus();
					return ACTIONDONERESULT_OPENDATEPICKER;
				} else {
					if (ctrlel->IsEnabled()) {
						ctrlel->SetFocus();
						ctrlel->ShowKeypad();
						return 0;
					} else {
						sourcetabindex++;
					}
				}
			}
			if (ctrlhandleritem_->type == TEXTAREA) {
				Osp::Ui::Controls::EditArea * fctrlea = static_cast<Osp::Ui::Controls::EditArea *>(formobj->GetControl(ctrlhandleritem_->controlid, true));
				if (fctrlea->IsEnabled()) {
					fctrlea->SetFocus();
					fctrlea->ShowKeypad();
					return 0;
				} else {
					sourcetabindex++;
				}
			}
			if (ctrlhandleritem_->type == CHECKBOX) {
				sourcetabindex++;
			}
		}
	}
	return ACTIONDONERESULT_HIDEKEYPAD;
}

Osp::Base::String ControlHandler::DateFormater(Osp::Base::DateTime datetime, bool minimize) {
	Osp::Base::DateTime todaydate, tmpdate;
	Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, todaydate);
	todaydate.SetValue(todaydate.GetYear(), todaydate.GetMonth(), todaydate.GetDay(), 0, 0, 0);
	Osp::Base::String timestr = L"";
	this->pDateFormatter->Format(datetime, timestr);
	tmpdate.SetValue(datetime.GetYear(), datetime.GetMonth(), datetime.GetDay(), 0, 0, 0);
	int daysspan = 0;
	if (tmpdate != todaydate) {
		while ((tmpdate != todaydate) && (daysspan < 10)) {
			daysspan++;
			tmpdate.AddDays(1);
		}
	}
	if (daysspan == 0) {
		if (minimize) {
			timestr = L"Today";
		} else {
			timestr.Append(L" - Today");
		}
	} else if (daysspan == 1) {
		if (minimize) {
			timestr = L"Yesterday";
		} else {
			timestr.Append(L" - Yesterday");
		}
	} else if (daysspan < 10) {
		if (!minimize) {
		timestr.Append(L" - ");
		timestr.Append(daysspan);
		timestr.Append(L" days ago");
		}
	}
	return timestr;
}

Osp::Base::String ControlHandler::CurrencyFormater(double value) {
	Osp::Base::String pricestr = L"";
	this->pCurrencyFormatter->Format(value, pricestr);
	return pricestr;
}

Osp::Base::String ControlHandler::Currency4Formater(double value) {
	Osp::Base::String pricestr = L"";
	this->pCurrency4Formatter->Format(value, pricestr);
	return pricestr;
}

Osp::Base::String ControlHandler::GetCurrencyStr() {
	Osp::Base::String currencysign = this->pCurrencyFormatter->GetPositiveSuffix() + this->pCurrencyFormatter->GetPositivePrefix();
	mchar tchar;
	Osp::Base::String outputstr;
	for (int n=0; n<currencysign.GetLength(); n++) {
		currencysign.GetCharAt(n,tchar);
		if (Osp::Base::Character::GetUnicodeCategory(tchar) != Osp::Base::UNICODE_SPACE) {
			outputstr += Osp::Base::Character::ToString(tchar);
		}
	}
	return outputstr;
}
