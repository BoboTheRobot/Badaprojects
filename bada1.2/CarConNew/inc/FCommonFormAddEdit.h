
#ifndef _FCOMMONFORMADDEDIT_H_
#define _FCOMMONFORMADDEDIT_H_

#include <FBase.h>
#include <FUi.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class FCommonFormAddEdit :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::IFocusEventListener
	,public Osp::Ui::IScrollPanelEventListener
	,public Osp::Base::Runtime::ITimerEventListener
{

// Construction
public:
	FCommonFormAddEdit(int addeditid, int FormTypeID, bool detailform);
	virtual ~FCommonFormAddEdit(void);
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
	FuelTypeData fueltypedatacurdata_;
	LocationData locationdatacurdata_;
	CarExpenseKatData expensekatdata_;

	int formtypeid;
	ControlHandler * controlhandler_;
	DrawingClass * drawingclass_;
	CarConClass * carconclass_;
	Osp::Base::Runtime::Timer * pTimer_;
	int timerstep;

private:
	void ReloadSavedData();
	void DetailEditMode();
	bool GetInputDataFuelType(FuelTypeData & data, bool validate=false);
	bool GetInputDataLocation(LocationData & data, bool validate=false);
	bool GetInputDataExpenseKat(CarExpenseKatData & data, bool validate=false);
	bool CheckIfDataChanged(void);
	bool Save(void);

// Generated call-back functions
public:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
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
