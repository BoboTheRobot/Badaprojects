
#ifndef _FGallery_H_
#define _FGallery_H_

#include <FBase.h>
#include <FUi.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>
#include <FIo.h>
#include <FSystem.h>
#include <FContent.h>
#include "DrawingClass.h"
#include "CameraOverlayPanel.h"

class ThumbsListItem : public Osp::Base::Object {
public:
	ThumbsListItem(Osp::Base::String filename, Osp::Base::DateTime filetime);
	virtual ~ThumbsListItem(void);

public:
	Osp::Base::String filename;
	Osp::Base::DateTime filetime;
};

class FGallery :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IItemEventListener,
	public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FGallery(void);
	virtual ~FGallery(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int AIMGSEL = 101;
	static const int ABACK = 102;
	static const int APREVPAGE = 103;
	static const int ANEXTPAGE = 104;

	static const int REDBTN = 3;
	static const int PAGERBTN = 4;

	DrawingClass * drawingclass_;
	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Ui::Controls::IconList *__pIconList;
	Osp::Base::Collection::ArrayList * thumbslist;
	Osp::Base::String selectedthumb;
	Osp::Ui::Controls::Popup* loadingpopup_;
	int pagescount, curpage;

	bool actioninprogress;

// Generated call-back functions
public:
	void LoadThumbsPage(int page);
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
};

#endif
