
#ifndef _FCON_H_
#define _FCON_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class FCon :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ITextEventListener
	,public Osp::Ui::ICustomItemEventListener
{

// Construction
public:
	FCon(int autoselectid=0);
	virtual ~FCon(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_OPTIONKEY = 100;
	static const int ID_BACK = 101;
	static const int ID_ADD = 102;
	static const int ID_EDIT = 103;
	static const int ID_DELETE = 104;
	static const int ID_DETAIL = 105;
	static const int ID_CONPREGLED = 107;
	static const int ID_CLEARSEARCH = 108;
	static const int LEL_BG = 4;
	int selecteditemid;
	int autoselectid;
	CarConClass * carconclass_;
	SettingsData settingsdata_;
	DrawingClass * drawingclass_;
	ControlHandler * controlhandler_;
	Osp::Ui::Controls::ContextMenu * pExListmenu_;
	int selectedctxmenuitemindex;
	Osp::Ui::Controls::CustomList * pExList;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemFormat;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemSearchFormat;
	Osp::Ui::Controls::Keypad * pKeypad;
	Osp::Base::String searchq;
	Osp::Ui::Controls::OptionMenu* __pOptionMenu;
	void AddListItem(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId);
	void AddListItemSearch(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, Osp::Base::String col4, int itemId, int searchposcol1, int searchposcol2);

private:

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
