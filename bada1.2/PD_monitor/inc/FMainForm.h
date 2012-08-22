#ifndef _FMAINFORM_H_
#define _FMAINFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FNet.h>
#include <FApp.h>
#include <FGraphics.h>
#include <FMedia.h>
#include "DrawingClass.h"
#include "FormMgr.h"

class FMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FMainForm(void);
	virtual ~FMainForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ASETTINGS = 101;
	static const int AABOUT = 102;

	static const int BOXBTN = 1;

	DrawingClass * drawingclass_;
	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Graphics::Bitmap * greenpointer;
	Osp::Graphics::Bitmap * redpointer;

	double download, upload, limit, overlimit, remaining, pointerdegree;

	Osp::Graphics::Canvas * displaybuffer_;

	Osp::Base::Runtime::Timer * anitimer_;
	long long anilasttime, anicurtime;
	double anistep, anistepc;

public:
	void StartAnim();
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	void CreateDisplayBuffer();
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
	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif	//_FMAINFORM_H_
