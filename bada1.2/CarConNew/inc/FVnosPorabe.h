
#ifndef _FVNOSPORABE_H_
#define _FVNOSPORABE_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class FVnosPorabe :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::IItemEventListener
	,public Osp::Ui::IScrollPanelEventListener
	,public Osp::Ui::IDateChangeEventListener
	,public Osp::Ui::IFocusEventListener
	,public Osp::Base::Runtime::ITimerEventListener
	,public Osp::Ui::ICustomItemEventListener
{

// Construction
public:
	FVnosPorabe(int addeditid, bool detailform, bool fromhome);
	virtual ~FVnosPorabe(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CANCEL = 100;
	static const int ID_SAVE = 101;
	static const int ID_ODOMETERTRIPBTN = 102;
	static const int ID_PRICEUNITFULLBTN = 103;
	static const int ID_EDIT = 104;
	static const int ID_DETAIL = 105;
	static const int ID_SELECTFUELTYPEPOPUPADD = 111;
	static const int ID_SELECTFUELTYPEPOPUPCANCEL = 113;
	static const int ID_ADDNEWFUELTYPEPOPUPSAVE = 115;
	static const int ID_ADDNEWFUELTYPEPOPUPCANCEL = 116;
	static const int ID_BUTTON_EDITFIELD_DONE = 11;
	static const int ID_BUTTON_EDITFIELD_CLOSE = 12;
	int itemid;
	static const int SELECTEDODOMETER = 1;
	static const int SELECTEDTRIP = 2;
	static const int SELECTEDUNITPRICE = 1;
	static const int SELECTEDFULLPRICE = 2;
	int selectedfueltype;
	int selectedlocation;
	int selectedodometertrip_type;
	int selectedpriceunitfull_type;
	int minodometer, maxodometer;
	Osp::Base::DateTime prevtime, nexttime;
	double lastpriceperl;
	int odometervalue, tripvalue;
	double priceperl, fullprice;
	Osp::Ui::Controls::Popup* pPopupSelectFuelType_;
	Osp::Ui::Controls::List* pListSelectFuelTypes_;
	Osp::Ui::Controls::Popup* pAddnewfueltypepopup_;
	CarConClass * carconclass_;
	SettingsData settingsdata_;
	CarConData curcarcondata_;
	ControlHandler * controlhandler_;
	DrawingClass * drawingclass_;
	Osp::Ui::Controls::DatePicker* pDatePicker_;
	bool popupmovetonextctrl;
	Osp::Graphics::Bitmap * buttontripodometerswitchbmptrip;
	Osp::Graphics::Bitmap * buttontripodometerswitchbmpodo;
	Osp::Graphics::Bitmap * buttontripodometerswitchbmppricepl;
	Osp::Graphics::Bitmap * buttontripodometerswitchbmppricefull;
	Osp::Ui::Controls::CustomList * SelectLocationCombo_;
	Osp::Ui::Controls::CustomListItemFormat * SelectLocationComboItemFormat_;
	static const int SelectLocationItemBG = 2;
	Osp::Base::Runtime::Timer * SelectLocationComboTimer_;
	bool SelectLocationComboDonthide;
	bool isfirstdraw;

	bool detailform;
	bool detailmode;
	bool fromhome;
	Osp::Base::Runtime::Timer * pTimer_;
	int timerstep;
	bool initformelhideondraw;


private:
	void SelectLocationComboItemAdd(Osp::Base::String col1, int itemId, int searchql=0, int searchposcol1=0);
	bool GetInputData(CarConData & data, bool validate=false);
	bool CheckIfDataChanged(void);
	bool Save(void);
	void PopupSelectFuelTypeSelect(int itemid);
	void Odometertripchangeselection(int setselection);
	void Priceunitfullchangeselection(int setselection);
	void PopupSelectLocationSelect(int itemid);
	int GetLocationId(Osp::Base::String caption);
	void ReloadLocationCombo();
	void ReloadSavedData();
	void DetailEditMode();
// Generated call-back functions
public:
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
	virtual void OnDateChanged(const Osp::Ui::Control& source, int year, int month, int day);
    virtual void OnDateChangeCanceled(const Osp::Ui::Control& source);
	void OnFocusGained(const Osp::Ui::Control& source);
	void OnFocusLost(const Osp::Ui::Control& source);
	void OnTextValueChanged(const Osp::Ui::Control& source);
	void OnTextValueChangeCanceled(const Osp::Ui::Control& source);
	void OnActionPerformed(const Osp::Ui::Control&, int);
    virtual void OnTouchDoublePressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusIn(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusOut(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchLongPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchMoved(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchReleased(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
    virtual void OnOverlayControlCreated(const Osp::Ui::Control& source);
	virtual void OnOverlayControlOpened(const Osp::Ui::Control& source);
	virtual void OnOverlayControlClosed(const Osp::Ui::Control& source);
	virtual void OnOtherControlSelected(const Osp::Ui::Control& source);
};

#endif
