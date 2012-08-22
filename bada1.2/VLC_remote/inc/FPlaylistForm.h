#ifndef _FPlaylistForm_H_
#define _FPlaylistForm_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "PublicFunctions.h"

class FPlaylistForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::ICustomItemEventListener,
	public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FPlaylistForm(void);
	virtual ~FPlaylistForm(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int APLAY = 1;
	static const int AREMOVE = 2;
	static const int ACLEAR = 3;
	static const int AADD = 4;
	static const int ABACK = 5;

	static const int BOXBTN = 4;

	static const int ID_LIST_TEXTTITLE = 101;
	static const int ID_LIST_TEXTARTIST = 102;
	static const int ID_LIST_ICON = 103;

	Osp::Ui::Controls::CustomListItemFormat* __pCustomListItemFormat;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Ui::Controls::CustomList * UList_;
	Osp::Graphics::Bitmap * currenticon;

	Osp::Ui::Controls::ContextMenu * UListContextmenu_;
	int curselitemid;
	int curplayingitemid;

	PublicFunctions * publicfunc_;

// Generated call-back functions
public:
	result AddListItem(int itemid, Osp::Base::String itemTextArtist, Osp::Base::String itemTextTitle, Osp::Graphics::Bitmap* pBitmapNormal);
	void FillListItems();
	void LoadPlaylist();
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif
