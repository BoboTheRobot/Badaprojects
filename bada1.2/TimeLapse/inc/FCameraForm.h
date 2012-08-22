#ifndef _FCAMERAFORM_H_
#define _FCAMERAFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include <FIo.h>
#include <FContent.h>
#include "DrawingClass.h"
#include "CameraOverlayPanel.h"
#include "FCameraThread.h"

class FCameraForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::ITouchEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Ui::IKeyEventListener,
	public Osp::Media::ICameraEventListener
{

// Construction
public:
	FCameraForm(void);
	virtual ~FCameraForm(void);
	bool Initialize();

// Implementation
protected:
	static const int ASTOP = 101;
	static const int ABACK = 102;
	static const int ASTART = 103;
	static const int ASTARTTIMEOUT = 104;

	Osp::Media::Camera * pCamera;
	CameraOverlayPanel * previewpanel;
	Osp::Graphics::Canvas * pOverlayPanelCanvas;
	Osp::Graphics::BufferInfo bufferInfo;

	Osp::Base::Runtime::Timer * StartTimer_;
	Osp::Base::Runtime::Timer * SecRefTimer_;

	Osp::Graphics::Bitmap * backbtnbmpn_;
	Osp::Graphics::Bitmap * backbtnbmps_;
	Osp::Graphics::Bitmap * stopbtnbmpn_;
	Osp::Graphics::Bitmap * stopbtnbmps_;

	bool tempexternalstorage;

	long long frameinterval;
	long long frametime_last;
	long long recstarttime;

	Osp::Ui::Controls::Button *backbtn;
	Osp::Ui::Controls::Button *stopbtn;

	DrawingClass * drawingclass_;

	Osp::Base::String recodingfilename;
	Osp::Graphics::Dimension resolution;
	int stopafterframeno;
	int stopafterframecountdown;

	FCameraThread * pCameraThread_;

// Generated call-back functions
public:
	static const int INTERUPTED_BACKGROUND = 200;
	static const int INTERUPTED_BATTERY = 201;
	static const int INTERUPTED_MEMORY = 202;
	void Interupt(int interuptedtype);
	void UpdatePreviewPanelValues();
	void StartCamera(int setframeinterval, Osp::Graphics::Dimension setresolution, int setstopafterframeno);
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	void OnCameraAutoFocused(bool completeCondition);
	void OnCameraPreviewed( Osp::Base::ByteBuffer& previewedData, result r) ;
	void OnCameraCaptured( Osp::Base::ByteBuffer& capturedData, result r);
	void OnCameraErrorOccurred(Osp::Media::CameraErrorReason r );
	virtual void OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	void OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
};

#endif
