
#ifndef _FCOMMONFORMLIST_H_
#define _FCOMMONFORMLIST_H_

#include <FBase.h>
#include <FUi.h>
#include "CarConClass.h"
#include "DrawingClass.h"

class FCommonFormList :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::ICustomItemEventListener
{

// Construction
public:
	FCommonFormList(int autoselectid=0, int FormTypeID=0);
	virtual ~FCommonFormList(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_BACK = 101;
	static const int ID_ADD = 102;
	static const int ID_EDIT = 103;
	static const int ID_DELETE = 104;
	static const int ID_DETAIL = 105;
	static const int ID_CLEARSEARCH = 108;
	static const int LEL_BG = 2;
	int selecteditemid;
	int autoselectid;
	int formtypeid;
	DrawingClass * drawingclass_;
	CarConClass * carconclass_;
	Osp::Ui::Controls::ContextMenu * pExListmenu_;
	int selectedctxmenuitemindex;
	Osp::Ui::Controls::CustomList * pExList;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemFormat;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemSearchFormat;
	Osp::Ui::Controls::Keypad * pKeypad;
	Osp::Base::String searchq;
	void AddListItem(Osp::Base::String col1, int itemId);
	void AddListItemSearch(Osp::Base::String col1, int itemId, int searchposcol1);

// Generated call-back functions
public:
	result OnDraw();
	void SearchAction(Osp::Base::String searchqstr);
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTextValueChanged(const Osp::Ui::Control& source);
	void OnTextValueChangeCanceled(const Osp::Ui::Control& source);
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, Osp::Ui::ItemStatus status);
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
    virtual void OnTouchDoublePressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusIn(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusOut(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchLongPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchMoved(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchReleased(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
};

#endif
