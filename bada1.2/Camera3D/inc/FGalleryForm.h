#ifndef _FGalleryForm_H_
#define _FGalleryForm_H_

#include <FBase.h>
#include <FUi.h>
#include <FApp.h>
#include <FMedia.h>
#include <FBaseByteBuffer.h>
#include "DrawingClass.h"
#include "Cam3Dclass.h"

class ThumbsListItem : public Osp::Base::Object {
public:
	ThumbsListItem(Osp::Base::String filename, Osp::Base::DateTime filetime);
	virtual ~ThumbsListItem(void);

public:
	Osp::Base::String filename;
	Osp::Base::DateTime filetime;
};

class ThumbsListItemThumb : public Osp::Base::Object {
public:
	ThumbsListItemThumb(Osp::Base::String filename, Osp::Graphics::Rectangle pos, Cam3Dclass * cam3dclass_, Osp::Media::Image * pimagedecoder_);
	virtual ~ThumbsListItemThumb(void);

public:
	Osp::Base::String filename;
	Osp::Graphics::Bitmap * thumbimg;
	Osp::Graphics::Rectangle pos;
};

class FGalleryForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IKeyEventListener
{

// Construction
public:
	FGalleryForm(Osp::Base::String returnfilename=L"");
	virtual ~FGalleryForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ABACK = 100;
	static const int ANEXTPAGE = 101;
	static const int APREVPAGE = 102;

	static const int ROUNDBTNBLUE = 1;
	static const int ROUNDBTNGREEN = 2;
	static const int THUMBBTN = 3;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Base::Collection::ArrayList * thumbslist;
	Osp::Base::Collection::ArrayList * thumbslistthumbs;
	int curpage, pagescount;
	Osp::Base::String returnfilename;

	Cam3Dclass * cam3dclass_;
	Osp::Media::Image * pimagedecoder_;

private:
	void LoadThumbsPage(int page);

public:
	virtual result OnDraw();
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	virtual void OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	void OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
};

#endif	//_FGalleryForm_H_
