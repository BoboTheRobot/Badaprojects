/*
 * ControlHandlerItem.h
 *
 *  Created on: 16.10.2010
 *      Author: mrak
 */

#ifndef CONTROLHANDLERITEM_H_
#define CONTROLHANDLERITEM_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>

class ControlHandlerItem : public Osp::Base::Object {
public:
	ControlHandlerItem(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext, Osp::Base::String metext);
	virtual ~ControlHandlerItem(void);
private:
	//
public:
	Osp::Base::String controlid;
	int type;
	bool required;
	Osp::Base::String validateerrmsg;
	Osp::Base::String guidetext;
	Osp::Base::String metext;
};

class ControlHandler {
public:
	ControlHandler();
	virtual ~ControlHandler(void);
protected:

private:
	Osp::Base::Collection::IList* controlslist_;
	Osp::Locales::NumberFormatter * pCurrencyFormatter;
	Osp::Locales::NumberFormatter * pCurrency4Formatter;
	Osp::Locales::DateTimeFormatter * pDateFormatter;
	int FindControlIndex(Osp::Base::String controlid);
	bool SetValueString(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value);
	double ParseFloat(Osp::Base::String & tmps, bool roundto4=false);
	int ParseInteger(Osp::Base::String & tmps);

public:
	static const int TEXT = 1;
	static const int NUMBER = 2;
	static const int COMBOBOX = 3;
	static const int TEXTAREA = 4;
	static const int DATE = 5;
	static const int CURRENCY = 6;
	static const int DOUBLE = 7;
	static const int DATETEXT = 8;
	static const int CHECKBOX = 9;
	static const int CURRENCY4 = 10;
	static const int ACTIONDONERESULT_HIDEKEYPAD = 1;
	static const int ACTIONDONERESULT_OPENCOMBOBOX = 4;
	static const int ACTIONDONERESULT_OPENDATEPICKER = 5;
	bool Construct(Osp::Base::String countrycode);
	bool Construct();
	bool Reconstruct();
	bool Clear();
	bool Destroy();
	bool Add(Osp::Base::String controlid, int type);
	bool Add(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext);
	bool Add(Osp::Base::String controlid, int type, bool required, Osp::Base::String validateerrmsg, Osp::Base::String guidetext, Osp::Base::String metext);
	bool InitControls(Osp::Ui::Controls::Form * formobj, int DoneActionId, int CloseActionId, Osp::Ui::IActionEventListener * actioneventlistner, Osp::Ui::ITouchEventListener * toucheventlistner, Osp::Ui::ITextEventListener * texteventlistner, Osp::Ui::IFocusEventListener * focuseventlistner, Osp::Ui::IScrollPanelEventListener * scrollpaneleventlistner);
	bool ControlReadOnly(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool readonly);
	bool ControlDisable(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool disable);
	bool SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value);
	bool SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, int value);
	bool SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, double value);
	bool SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::DateTime value);
	bool SetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, bool value);
	bool SetValueCurrency(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, double value);
	bool SetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::String value);
	bool SetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid, Osp::Base::DateTime value);
	Osp::Base::String GetValue(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid);
	int GetValueInt(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid);
	double GetValueDouble(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid);
	Osp::Base::DateTime GetValueDate(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid);
	bool GetValueBool(Osp::Ui::Controls::Form * formobj, Osp::Base::String controlid);
	bool ControlUnformatValue(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source);
	bool ControlFormatValue(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source);
	bool ControlFormatValueOnInput(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source);
	Osp::Base::String ValidateFields(Osp::Ui::Controls::Form * formobj);
	int ActionDone(Osp::Ui::Controls::Form * formobj, const Osp::Ui::Control& source);
	Osp::Base::String DateFormater(Osp::Base::DateTime datetime, bool minimize=false);
	Osp::Base::String CurrencyFormater(double value);
	Osp::Base::String Currency4Formater(double value);
	Osp::Base::String GetCurrencyStr();
	Osp::Base::DateTime ParseDateTime(Osp::Base::String value);
};

#endif /* CONTROLHANDLERITEM_H_ */
