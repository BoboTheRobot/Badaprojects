
#ifndef _FGRAFZOOM_H_
#define _FGRAFZOOM_H_

#include <FBase.h>
#include <FUi.h>
#include "DrawingClass.h"

class FGrafZoom :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Base::Runtime::ITimerEventListener
{

// Construction
public:
	FGrafZoom(void);
	virtual ~FGrafZoom(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_CLOSE = 100;
	DrawingClass * drawingclass_;
	Osp::Graphics::Bitmap * pGrafBmp;
	bool showhint;
	Osp::Base::Runtime::Timer * pTimer_;

// Generated call-back functions
public:
	void onopen();
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
};

#endif
