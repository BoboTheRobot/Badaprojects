#ifndef _FDetailView_H_
#define _FDetailView_H_

#include <FBase.h>
#include <FUi.h>
#include <FApp.h>
#include <FMedia.h>
#include <FBaseByteBuffer.h>
#include <FSystem.h>
#include <FContent.h>

#include "DrawingClass.h"
#include "Cam3Dclass.h"

class FDetailView :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IKeyEventListener,
	public Osp::Base::Runtime::ITimerEventListener
{

// Construction
public:
	FDetailView(void);
	virtual ~FDetailView(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ABACK = 100;
	static const int AEXPORT = 101;
	static const int ADELETE = 102;
	static const int AEXPORTOK = 201;
	static const int AEXPORTCANCEL = 202;

	static const int ROUNDBTNBLUE = 1;
	static const int ROUNDBTNRED = 3;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;

	Osp::Base::String filename;

	Cam3Dclass * cam3dclass_;
	Osp::Graphics::Bitmap * imgbmp;
	Osp::Media::Image * pimagedecoder_;
	bool buttonsvisible;

	Osp::Base::Runtime::Timer * buttonsvisibleTimer_;

	Osp::Ui::Controls::Popup* exportpopup_;

	int curviewtype;

	CAM3D_FILEINFO fileinfo_;

	static const int VIEWTYPE_RC = 0;
	static const int VIEWTYPE_RCGray = 2;
	static const int VIEWTYPE_ANIM = 3;
	static const int VIEWTYPE_PAR = 4;
	static const int VIEWTYPE_CROS = 5;

private:

public:
	void ChangeView(int changeto);
	void LoadFile(Osp::Base::String filename);
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
	void OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
};

#endif	//_FDetailView_H_
