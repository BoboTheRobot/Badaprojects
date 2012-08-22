
#ifndef _FCARADDEDIT_H_
#define _FCARADDEDIT_H_

#include <FBase.h>
#include <FUi.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"


class FCarAddEdit :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::IFocusEventListener
	,public Osp::Base::Runtime::ITimerEventListener
	,public Osp::Ui::IScrollPanelEventListener
{

// Construction
public:
	FCarAddEdit(int addeditid, bool detailform, int formtype=0);
	virtual ~FCarAddEdit(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CANCEL = 100;
	static const int ID_SAVE = 101;
	static const int ID_EDIT = 102;
	static const int ID_DETAIL = 104;
	static const int ID_SELECTFUELTYPEPOPUPOK = 110;
	static const int ID_SELECTFUELTYPEPOPUPADD = 111;
	static const int ID_ADDNEWFUELTYPEPOPUPSAVE = 112;
	static const int ID_ADDNEWFUELTYPEPOPUPCANCEL = 113;
	static const int ID_BUTTON_EDITFIELD_DONE = 1;
	static const int ID_BUTTON_EDITFIELD_CLOSE = 2;
	int itemid;
	bool detailform;
	bool detailmode;
	CarData curcardata_;
	Osp::Ui::Controls::Popup* pPopup_;
	Osp::Ui::Controls::List* pListSelectFuelTypes_;
	Osp::Ui::Controls::Popup* pAddnewfueltypepopup_;
	ArrayListT<int> fueltypesselectedarray_;
	Osp::Base::Collection::IList* controlslist_;
	ControlHandler * controlhandler_;
	CarConClass * carconclass_;
	DrawingClass * drawingclass_;
	Osp::Base::Runtime::Timer * pTimer_;
	int timerstep;
	int formtype;
// Generated call-back functions
private:
	void ReloadSavedData();
	void DetailEditMode();
	bool GetInputData(CarData & data, bool validate=false);
	bool CheckIfDataChanged();
	bool CheckIfDataChangedFuelTypes();
	String GetFuelTypesString();
	bool Save();
public:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnFocusGained(const Osp::Ui::Control& source);
	void OnFocusLost(const Osp::Ui::Control& source);
	void OnTextValueChanged(const Osp::Ui::Control& source);
	void OnTextValueChangeCanceled(const Osp::Ui::Control& source);
    virtual void OnTouchDoublePressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusIn(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusOut(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchLongPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchMoved(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchReleased(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnOverlayControlCreated(const Osp::Ui::Control& source);
    virtual void OnOverlayControlOpened(const Osp::Ui::Control& source);
    virtual void OnOverlayControlClosed(const Osp::Ui::Control& source);
    virtual void OnOtherControlSelected(const Osp::Ui::Control& source);
};

#endif
