
#ifndef _FTRIPADDEDIT_H_
#define _FTRIPADDEDIT_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class FTripAddEdit :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::IFocusEventListener
	,public Osp::Ui::IScrollPanelEventListener
	,public Osp::Base::Runtime::ITimerEventListener
	,public Osp::Ui::IDateChangeEventListener
	,public Osp::Ui::ICustomItemEventListener
{

// Construction
public:
	FTripAddEdit(int addeditid, bool detailform);
	virtual ~FTripAddEdit(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CANCEL = 100;
	static const int ID_SAVE = 101;
	static const int ID_EDIT = 102;
	static const int ID_DETAIL = 104;
	static const int ID_BUTTON_EDITFIELD_DONE = 1;
	static const int ID_BUTTON_EDITFIELD_CLOSE = 2;
	int itemid;
	bool detailform;
	bool detailmode;
	TripData curdata_;
	CarConClass * carconclass_;
	ControlHandler * controlhandler_;
	DrawingClass * drawingclass_;
	Osp::Ui::Controls::DatePicker* pDatePicker_;
	Osp::Base::Runtime::Timer * pTimer_;
	int timerstep;
	bool popupmovetonextctrl;
	String datefieldcurrentid;
	Osp::Ui::Controls::CustomList * SelectLocationStartCombo_;
	Osp::Ui::Controls::CustomList * SelectLocationEndCombo_;
	Osp::Ui::Controls::CustomListItemFormat * SelectLocationComboItemFormat_;
	static const int SelectLocationItemBG = 2;
	Osp::Base::Runtime::Timer * SelectLocationComboTimer_;
	bool SelectLocationComboDonthide;
	bool isfirstdraw;
	double AvgCon;
	int ConType;

// Generated call-back functions

private:
	void SelectLocationComboItemAdd(int type, Osp::Base::String col1, int itemId, int searchql=0, int searchposcol1=0);
	void ReloadSavedData();
	void DetailEditMode();
	bool GetInputData(TripData & data, bool validate=false);
	bool CheckIfDataChanged(void);
	bool Save(void);
	void PopupSelectLocationSelect(int type, int itemid);
	int GetLocationId(Osp::Base::String caption);
	void ReloadLocationCombo(int type);

public:
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
	virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
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
};

#endif
