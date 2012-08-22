
#ifndef _FCARSELECT_H_
#define _FCARSELECT_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FGraphics.h>
#include "CarConClass.h"
#include "DrawingClass.h"

class FCarSelect :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ICustomItemEventListener
{

// Construction
public:
	FCarSelect(int autoselectid=0, int formtype=0);
	virtual ~FCarSelect(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CANCELSELECT = 100;
	static const int ID_SELECT = 101;
	static const int ID_ADD = 102;
	static const int ID_EDIT = 103;
	static const int ID_DELETE = 104;
	static const int ID_DETAIL = 105;
	static const int LEL_COL1 = 1;
	static const int LEL_COL2 = 2;
	static const int LEL_CB = 3;
	static const int LEL_BG = 4;
	static const int LEL_CBBG = 5;
	Osp::Ui::Controls::ContextMenu * pExListmenu_;
	Osp::Graphics::Bitmap * checkboximg;
	int selectedctxmenuitemindex;
	int selectedcarid;
	int autoselectid;
	CarConClass * carconclass_;
	int formtype;
	DrawingClass * drawingclass_;
	Osp::Ui::Controls::CustomList * pExList;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemFormat;
	void AddListItem(Osp::Base::String col1, Osp::Base::String col2, int itemId);

public:
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
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
