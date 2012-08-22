#ifndef _FCAMERAFORM_H_
#define _FCAMERAFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FGraphics.h>
#include <FIo.h>
#include <FSystem.h>
#include <FContent.h>
#include <FBaseByteBuffer.h>
#include "DrawingClass.h"
#include "CameraOverlayPanel.h"
#include "Cam3Dclass.h"

class FCameraForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Media::ICameraEventListener,
	public Osp::Ui::IKeyEventListener,
	public Osp::Base::Runtime::ITimerEventListener
{

// Construction
public:
	FCameraForm(void);
	virtual ~FCameraForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ABACKWOQ = 99;
	static const int ABACK = 100;
	static const int ASTARTCAM = 105;
	static const int ACAMCAPTURE = 106;
	static const int ALEFTCAM = 111;
	static const int ARIGHTCAM = 112;
	static const int AALIGMENT = 113;
	static const int AMLEFT = 121;
	static const int AMRIGHT = 122;
	static const int AMTOP = 123;
	static const int AMBOTTOM = 124;
	static const int ASAVETOGALLERY = 125;
	CameraOverlayPanel * pDrawPanel;
	Osp::Graphics::BufferInfo cambufferInfo;
	Osp::Media::Camera * cam;
	Osp::Graphics::Bitmap* pCapturedFirst;
	Osp::Graphics::Bitmap* pCapturedSecond;
	Osp::Graphics::Bitmap* pCapturedCombine;
	int combinetop, combineleft;
	Osp::Graphics::Dimension captureimgdim;

	DrawingClass * drawingclass_;

	Osp::Graphics::Bitmap * backbtnbmpn_;
	Osp::Graphics::Bitmap * backbtnbmps_;
	Osp::Graphics::Bitmap * savebtnbmpn_;
	Osp::Graphics::Bitmap * savebtnbmps_;
	Osp::Graphics::Bitmap * albtnbmpn_;
	Osp::Graphics::Bitmap * albtnbmps_;
	Osp::Graphics::Bitmap * arbtnbmpn_;
	Osp::Graphics::Bitmap * arbtnbmps_;
	Osp::Graphics::Bitmap * atbtnbmpn_;
	Osp::Graphics::Bitmap * atbtnbmps_;
	Osp::Graphics::Bitmap * abbtnbmpn_;
	Osp::Graphics::Bitmap * abbtnbmps_;

	Osp::Ui::Controls::Button *albtn;
	Osp::Ui::Controls::Button *abbtn;
	Osp::Ui::Controls::Button *arbtn;
	Osp::Ui::Controls::Button *atbtn;
	Osp::Ui::Controls::Button *savebtn;
	Osp::Ui::Controls::Button *capturebtn;

	int cameraerrcount;

	Cam3Dclass * cam3dclass_;

	int isolevel,exposure,brightness,contrast;
	bool flashenable, shuttersound;

	Osp::Media::Player * shuttersoundplayer;

	Osp::Base::Runtime::Timer * resumecamTimer_;

private:
	void ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB, Osp::Graphics::Bitmap * imgbmp);
	void LoadSettings(int &isolevel, int &exposure, int &brightness, int &contrast);

public:
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
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	void OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
};

#endif	//_FCAMERAFORM_H_
