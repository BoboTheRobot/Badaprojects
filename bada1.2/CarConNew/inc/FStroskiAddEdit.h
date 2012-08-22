
#ifndef _FSTROSKIADDEDIT_H_
#define _FSTROSKIADDEDIT_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class FStroskiAddEdit :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::IFocusEventListener
	,public Osp::Ui::IScrollPanelEventListener
	,public Osp::Base::Runtime::ITimerEventListener
	,public Osp::Ui::IDateChangeEventListener
	,public Osp::Ui::IItemEventListener
{

// Construction
public:
	FStroskiAddEdit(int addeditid, bool detailform);
	virtual ~FStroskiAddEdit(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CANCEL = 100;
	static const int ID_SAVE = 101;
	static const int ID_EDIT = 102;
	static const int ID_DETAIL = 104;
	static const int ID_SETCURODOMETER = 200;
	static const int ID_BUTTON_EDITFIELD_DONE = 1;
	static const int ID_BUTTON_EDITFIELD_CLOSE = 2;
	static const int ID_SELECTKATPOPUPOK = 110;
	static const int ID_SELECTKATPOPUPADD = 111;
	static const int ID_SELECTKATPOPUPCANCEL = 114;
	static const int ID_ADDNEWKATPOPUPSAVE = 112;
	static const int ID_ADDNEWKATPOPUPCANCEL = 113;
	int itemid;
	bool detailform;
	bool detailmode;
	CarExpenseData curdata_;
	CarConClass * carconclass_;
	ControlHandler * controlhandler_;
	DrawingClass * drawingclass_;
	int selectedkat;
	Osp::Ui::Controls::DatePicker* pDatePicker_;
	Osp::Base::Runtime::Timer * pTimer_;
	Osp::Ui::Controls::Popup* pPopupSelectKat_;
	Osp::Ui::Controls::List* pListSelectKat_;
	Osp::Ui::Controls::Popup* pAddnewkatpopup_;
	int timerstep;
	bool initformelhideondraw;
	bool popupmovetonextctrl;

// Generated call-back functions

private:
	void ReloadSavedData();
	void DetailEditMode();
	bool GetInputData(CarExpenseData & data, bool validate=false);
	bool CheckIfDataChanged(void);
	bool Save(void);
	void PopupSelectKatSelect(int itemid);

public:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
	virtual void OnDateChanged(const Osp::Ui::Control& source, int year, int month, int day);
    virtual void OnDateChangeCanceled(const Osp::Ui::Control& source);
	void OnFocusGained(const Osp::Ui::Control& source);
	void OnFocusLost(const Osp::Ui::Control& source);
	void OnTextValueChanged(const Osp::Ui::Control& source);
	void OnTextValueChangeCanceled(const Osp::Ui::Control& source);
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	virtual void OnOverlayControlCreated(const Osp::Ui::Control& source);
	virtual void OnOverlayControlOpened(const Osp::Ui::Control& source);
	virtual void OnOverlayControlClosed(const Osp::Ui::Control& source);
	virtual void OnOtherControlSelected(const Osp::Ui::Control& source);
	virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
};

#endif
