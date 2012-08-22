#ifndef _FCARTOONME_H_
#define _FCARTOONME_H_

#include <FBase.h>
#include <FUi.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>
#include <FIo.h>
#include <FSystem.h>
#include <FContent.h>
//#include <FAds.h>
#include "ImageFilters.h"
#include "DrawingClass.h"
#include "ProcessThread.h"

class FCartoonMe :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::ITouchEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Ui::IKeyEventListener
	//public Osp::Ads::Controls::IAdListener
{

// Construction
public:
	FCartoonMe(void);
	virtual ~FCartoonMe(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ACONTINUE = 101;
	static const int AABOUT = 102;
	static const int AABOUTCLOSE = 103;
	static const int ADRAW = 106;
	static const int APREVIEWBACK = 107;
	static const int APROGRESSCANCEL = 108;
	static const int APROGRESSDONE = 109;
	static const int ASAVEBTN = 110;
	static const int ADONEBACK = 111;
	static const int AID_CLOSESAVEASPOPUP = 112;
	static const int AID_SAVEASCARD = 113;
	static const int AID_SAVEASPHONE = 114;
	static const int ASELECTCAMF = 115;
	static const int ASELECTCAMB = 116;
	static const int ASELECTGAL = 117;
	static const int APREVIEWBACKCAMERA = 118;
	static const int APREVIEWBACKGALLERY = 119;
	static const int ABANNER = 120;

	static const int ALOADSRC = 121;
	static const int APROC = 122;
	static const int ASAVE = 123;

	static const int GREENBTN = 1;
	static const int BLUEBTN = 2;
	static const int REDBTN = 3;
	static const int SELBTN = 4;


	Osp::Media::Image * imagep_;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	ProcessThread* pProcessThreadWorker_;

	double processingprocdone;

	Osp::Ui::Controls::Popup* saveaspopup_;

	Osp::Base::Runtime::Timer * showdonetimer_;

	//Osp::Ads::Controls::Ad* 		__pAd;

public:
	static const int FORMID_FIRST = 1;
	static const int FORMID_ABOUT = 2;
	static const int FORMID_CAMERA = 3;
	static const int FORMID_PREVIEW = 4;
	static const int FORMID_PROGRESS = 5;
	static const int FORMID_DONE = 6;
	static const int FORMID_SELECT = 7;
	int FORMID;
	Osp::Graphics::Bitmap * srcbmp;
	Osp::Graphics::Bitmap * destbmp;
	Osp::Graphics::Bitmap * papertexture;
	ImageFilters * imagefilter_;

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
	void ProcessThreadCallback(int action, double procdone);
	void BackgroundPause();
	void BackgroundResume();
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	//void OnAdReceived(RequestId reqId, const Osp::Ads::Controls::Ad& source, result r, const Osp::Base::String& errorCode, const Osp::Base::String& errorMsg);
};

#endif	//_FCARTOONME_H_
