
#ifndef _FCAMERA_H_
#define _FCAMERA_H_

#include <FBase.h>
#include <FUi.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>
#include <FIo.h>
#include <FSystem.h>
#include <FContent.h>
#include "DrawingClass.h"
#include "CameraOverlayPanel.h"

class FCamera :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IKeyEventListener,
	public Osp::Media::ICameraEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FCamera(void);
	virtual ~FCamera(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ACAPTURE = 104;
	static const int ACAMERASEL = 105;
	static const int ACAMERAERROR = 112;
	static const int ACAPTUREDONE = 113;
	static const int ABACK = 114;

	static const int GREENBTN = 1;
	static const int BLUEBTN = 2;

	Osp::Base::Runtime::Timer * resumecamTimer_;
	CameraOverlayPanel * pDrawPanel;
	Osp::Media::Camera * cam;
	DrawingClass * drawingclass_;
	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	Osp::Media::CameraSelection curcamerasel;
	Osp::Graphics::Bitmap * framebmp;
	Osp::Graphics::Canvas * pOverlayPanelCanvas;
	bool camerabtnsenable;

// Generated call-back functions
public:
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnKeyLongPressed(const Osp::Ui::Control&, Osp::Ui::KeyCode);
	void OnKeyPressed(const Osp::Ui::Control&, Osp::Ui::KeyCode);
	void OnKeyReleased(const Osp::Ui::Control&, Osp::Ui::KeyCode);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	void StartCamera();
	void TerminateCam();
	void PauseCam();
	void ResumeCamProcess();
	void ResumeCam();
	void Start();
	void OnCameraAutoFocused(bool completeCondition);
    void OnCameraPreviewed( Osp::Base::ByteBuffer& previewedData, result r) ;
    void OnCameraCaptured( Osp::Base::ByteBuffer& capturedData, result r);
    void OnCameraErrorOccurred(Osp::Media::CameraErrorReason r );
    void ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB);
};

#endif
