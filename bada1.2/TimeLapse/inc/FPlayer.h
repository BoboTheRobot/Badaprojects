#ifndef _FPLAYER_H_
#define _FPLAYER_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include <FContent.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "TimeLapseClass.h"
#include "FPlayerThread.h"
#include "FExportThread.h"

class FPlayer :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FPlayer(void);
	virtual ~FPlayer(void);
	bool Initialize();

// Implementation
protected:
	static const int ABACK = 100;
	static const int APLAYPAUSE = 101;
	static const int APREVFRAME = 102;
	static const int ANEXTFRAME = 103;
	static const int ADELETEVIDEO = 105;
	static const int ASHOWINFO = 106;
	static const int ACLOSEINFO = 107;
	static const int AEXPORT = 108;
	static const int AEDIT = 109;
	static const int AEXPORTCANCEL = 110;
	static const int AEXPORTSAVE = 111;
	static const int AEXPORTSAVESD = 112;
	static const int AEXPORTTYPESWITCH = 113;
	static const int ASPEEDCHANGE = 200;

	Osp::Ui::Controls::Popup * exportpopup_;
	Osp::Ui::Controls::Popup * savingpopup_;
	Osp::Ui::Controls::Popup * infopopup_;

	Osp::Graphics::Bitmap * backbtnbmpn_;
	Osp::Graphics::Bitmap * backbtnbmps_;
	Osp::Graphics::Bitmap * playpausebtnbmpn_;
	Osp::Graphics::Bitmap * playpausebtnbmps_;
	Osp::Graphics::Bitmap * playpausepausebtnbmpn_;
	Osp::Graphics::Bitmap * playpausepausebtnbmps_;
	Osp::Graphics::Bitmap * nextframebtnbmpn_;
	Osp::Graphics::Bitmap * nextframebtnbmps_;
	Osp::Graphics::Bitmap * prevframebtnbmpn_;
	Osp::Graphics::Bitmap * prevframebtnbmps_;
	Osp::Graphics::Bitmap * infobtnbmpn_;
	Osp::Graphics::Bitmap * infobtnbmps_;
	Osp::Graphics::Bitmap * exportbtnbmpn_;
	Osp::Graphics::Bitmap * exportbtnbmps_;
	/*Osp::Graphics::Bitmap * editbtnbmpn_;
	Osp::Graphics::Bitmap * editbtnbmps_;*/
	Osp::Graphics::Bitmap * deletebtnbmpn_;
	Osp::Graphics::Bitmap * deletebtnbmps_;
	Osp::Graphics::Bitmap * speedbtnbmpn_;
	Osp::Graphics::Bitmap * speedbtnbmps_;

	Osp::Ui::Controls::Button *backbtn;
	Osp::Ui::Controls::Button *playpausebtn;
	Osp::Ui::Controls::Button *infobtn;
	Osp::Ui::Controls::Button *deletebtn;
	Osp::Ui::Controls::Button *exportbtn;
	Osp::Ui::Controls::Button *editbtn;
	Osp::Ui::Controls::Button *nextframebtn;
	Osp::Ui::Controls::Button *prevframebtn;
	Osp::Ui::Controls::Button *speedbtn;

	FPlayerThread * pPlayerThread_;

	FExportThread * pExportThread_;

	DrawingClass * drawingclass_;

	bool controlsarevisible;

	Osp::Base::Runtime::Timer * HideControlsTimer_;

	long long fpsbenchmarktime;

	static const int EXPORTTYPE_VIDEO = 1;
	static const int EXPORTTYPE_FRAME = 2;
	int exporttotype;

	bool exportfilenameset;

	bool autoplayonload;

	Osp::Graphics::Canvas * displaybuffer_;
	bool displaybufferisindrawingstate;
	Osp::Graphics::Rectangle canvasredrawrect_;

	Osp::Base::String exporttempfilename;
	Osp::Base::String exportfilename;

	Osp::Base::Runtime::Timer * ExportProgressRefTimer_;

// Generated call-back functions
public:
	static const int THREADCALLBACK_REDRAW = 200;
	static const int THREADCALLBACK_FILELOADED = 201;
	static const int THREADCALLBACK_EXPORTDONE = 202;
	static const int THREADCALLBACK_EXPORTPROGRESSUPDATE = 203;

	Osp::Graphics::Bitmap* CreateSpeedBtnBmp(int width, Osp::Base::String caption, bool selected);

	void RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect);
	void Background();
	void LoadFile(Osp::Base::String filename, bool autoplayonload);
	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
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

#endif
