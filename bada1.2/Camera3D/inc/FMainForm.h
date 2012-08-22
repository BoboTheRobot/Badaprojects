#ifndef _FMAINFORM_H_
#define _FMAINFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FApp.h>
#include <FMedia.h>
#include <FBaseByteBuffer.h>
#include "DrawingClass.h"

class FMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IKeyEventListener
{

// Construction
public:
	FMainForm(void);
	virtual ~FMainForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ACAMERA = 100;
	static const int AGALERY = 101;
	static const int AHELP = 102;
	static const int ASETTINGS = 103;

	static const int BIGMAINBTN = 1;
	static const int ROUNDBTN = 2;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Graphics::Bitmap * lastgalimg;

private:

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

#endif	//_FMAINFORM_H_
