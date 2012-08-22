#ifndef _FMAINFORM_H_
#define _FMAINFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "TimeLapseClass.h"

class IntervalValueItem : public Osp::Base::Object {
public:
	IntervalValueItem(int sec, Osp::Base::String caption);
	virtual ~IntervalValueItem(void);

public:
	int sec;
	Osp::Base::String caption;
};

class ResolutionValueItem : public Osp::Base::Object {
public:
	ResolutionValueItem(int width, int height);
	virtual ~ResolutionValueItem(void);

public:
	int width;
	int height;
};

class StopAfterValueItem : public Osp::Base::Object {
public:
	StopAfterValueItem(int frames);
	virtual ~StopAfterValueItem(void);

public:
	int frames;
};

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
	ThumbsListItemThumb(Osp::Base::String filename, Osp::Graphics::Rectangle pos, TimeLapseClass * TimeLapseClass_, Osp::Media::Image * pimagedecoder_);
	virtual ~ThumbsListItemThumb(void);

public:
	Osp::Base::String filename;
	Osp::Base::String title;
	Osp::Graphics::Bitmap * thumbimg;
	Osp::Graphics::Rectangle pos;
};

class FMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FMainForm(void);
	virtual ~FMainForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ASTART = 1;
	static const int AHELP = 2;
	static const int AINFO = 3;
	static const int ASETTINGS = 4;
	static const int AGALLERYNEXT = 5;
	static const int AGALLERYPREV = 6;
	static const int ASAVESETTINGS = 7;
	static const int ACLOSEINFO = 8;
	static const int ACLOSEHELP = 9;

	static const int BOXBTN = 1;
	static const int ROUNDBTN = 2;
	static const int BIGBTN = 3;
	static const int THUMB = 4;

	static const int FORMTYPE_NORMAL = 1;
	static const int FORMTYPE_SETTINGS = 2;
	static const int FORMTYPE_INFO = 3;
	static const int FORMTYPE_HELP = 4;

	int formtype;

	DrawingClass * drawingclass_;

	Osp::Graphics::Canvas * displaybuffer_;
	bool displaybufferisindrawingstate;
	Osp::Graphics::Rectangle canvasredrawrect_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	int currselslider;
	double cursliderintervalvalue;
	double cursliderresolutinvalue;
	double cursliderstopaftervalue;
	Osp::Graphics::Point oldpresspos;
	
	double intervalprocvalue;
	int intervalselected;
	Osp::Base::Collection::ArrayList * intervalvalueslist;

	double resolutionprocvalue;
	int resolutionselected;
	Osp::Base::Collection::ArrayList * resolutionvalueslist;

	double stopafterprocvalue;
	int stopafterselected;
	Osp::Base::Collection::ArrayList * stopaftervalueslist;

	Osp::Base::String settingstext;

	int curpage;
	int pagescount;

	Osp::Base::Collection::ArrayList * thumbslist;
	Osp::Base::Collection::ArrayList * thumbslistthumbs;

	Osp::Media::Image * pimagedecoder_;
	TimeLapseClass * TimeLapseClass_;

public:
	void SaveSettings();
	void LoadSettings();
	void LoadThumbsPage(int page);
	void RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect);
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
};

#endif	//_FMAINFORM_H_
