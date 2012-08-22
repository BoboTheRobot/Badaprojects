
#ifndef _FSETTINGS_H_
#define _FSETTINGS_H_

#include <FBase.h>
#include <FUi.h>
#include <FApp.h>
#include <FGraphics.h>
#include <FMedia.h>
#include "DrawingClass.h"
#include "FormMgr.h"

class FSettings :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FSettings(void);
	virtual ~FSettings(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ASAVE = 101;
	static const int ACANCEL = 102;

	static const int BOXBTN = 1;

	DrawingClass * drawingclass_;
	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

// Generated call-back functions
public:
	virtual result OnDraw();
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
