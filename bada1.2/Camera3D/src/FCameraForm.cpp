/*
 Copyright (C) 2012  Boštjan Mrak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FCameraForm.h"
#include "FormMgr.h"
#include "Cam3Dclass.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Io;
using namespace Osp::App;
using namespace Osp::Content;
using namespace Osp::Base::Utility;


class MyPlayerListener
    : public IPlayerEventListener
 {
    public:

        void OnPlayerOpened(result r);
        void OnPlayerEndOfClip(void);
        void OnPlayerBuffering(int percent);
        void OnPlayerErrorOccurred(PlayerErrorReason r );
        void OnPlayerInterrupted();
        void OnPlayerReleased();
        void OnPlayerSeekCompleted(result r);

 };

 static MyPlayerListener* pListener = null;

void MyPlayerListener::OnPlayerOpened(result r)
 {

 }


 void MyPlayerListener::OnPlayerEndOfClip(void)
 {

 }


 void MyPlayerListener::OnPlayerBuffering(int percent)
 {
    //Insert your code to operate while the buffering for a remote resource is going on.
 }

 void MyPlayerListener::OnPlayerErrorOccurred(PlayerErrorReason r )
 {
    //Insert your code to operate after any error occurred.
 }

    void MyPlayerListener::OnPlayerInterrupted()
    {
        //Insert your code here
    }

    void MyPlayerListener::OnPlayerReleased()
    {
        //Insert your code here
    }

    void MyPlayerListener::OnPlayerSeekCompleted(result r)
    {
        //Insert your code here
    }


FCameraForm::FCameraForm(void)
{
}

FCameraForm::~FCameraForm(void)
{
}

bool
FCameraForm::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"CameraForm");
	this->SetOrientation(ORIENTATION_LANDSCAPE);
	return true;
}

void FCameraForm::LoadSettings(int &isolevel, int &exposure, int &brightness, int &contrast) {
	isolevel = 200;
	exposure = 5;
	brightness = 5;
	contrast = 5;
	flashenable = false;
	shuttersound = true;
	Osp::Base::String value;
	int intvalue;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"isolevel", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		isolevel = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"exposure", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		exposure = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"brightness", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		brightness = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"contrast", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		contrast = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"flash", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		flashenable = (intvalue == 1);
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"shuttersound", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		shuttersound = (intvalue == 1);
	}
}

result
FCameraForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	pCapturedFirst = null;
	pCapturedSecond = null;
	pCapturedCombine = null;

	resumecamTimer_ = null;

	if (Osp::Io::File::IsFileExist(L"/Home/capturedfirst.jpg")) {
		Osp::Io::File::Remove(L"/Home/capturedfirst.jpg");
	}
	if (Osp::Io::File::IsFileExist(L"/Home/capturedsecond.jpg")) {
		Osp::Io::File::Remove(L"/Home/capturedsecond.jpg");
	}

	pDrawPanel = new CameraOverlayPanel();
	pDrawPanel->Initialize(Rectangle(0,0,800,480));
	this->AddControl(*pDrawPanel);

	pDrawPanel->ftype = pDrawPanel->LEFTCAP;
	pDrawPanel->SetRendererRotation(Osp::Ui::Controls::OverlayPanel::ROTATION_90);
	pDrawPanel->SetRendererRotation(Osp::Ui::Controls::OverlayPanel::ROTATION_NONE);

	capturebtn = new Osp::Ui::Controls::Button();
	capturebtn->Construct(Rectangle(800-140-20,20,140,74),L"Capture");
	capturebtn->SetActionId(ACAMCAPTURE);
	capturebtn->AddActionEventListener(*this);
	pDrawPanel->AddControl(*capturebtn);

	Osp::Ui::Controls::Button *backbtn = new Osp::Ui::Controls::Button();
	backbtn->Construct(Rectangle(20,20,64,64),L"Back");
	backbtn->SetActionId(ABACK);
	backbtn->AddActionEventListener(*this);
	backbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(472,670,64,64), Rectangle(472,798,40,40));
	backbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(472,670+64,64,64), Rectangle(472,798+40,40,40));
	backbtn->SetNormalBackgroundBitmap(*backbtnbmpn_);
	backbtn->SetPressedBackgroundBitmap(*backbtnbmps_);
	backbtn->SetText(L"");
	pDrawPanel->AddControl(*backbtn);

	savebtn = new Osp::Ui::Controls::Button();
	savebtn->Construct(Rectangle(20,336,64,64),L"Save");
	savebtn->SetActionId(ASAVETOGALLERY);
	savebtn->AddActionEventListener(*this);
	savebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(536,670,64,64), Rectangle(512,798,40,40));
	savebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(536,670+64,64,64), Rectangle(512,798+40,40,40));
	savebtn->SetNormalBackgroundBitmap(*savebtnbmpn_);
	savebtn->SetPressedBackgroundBitmap(*savebtnbmps_);
	savebtn->SetText(L"");
	savebtn->SetShowState(false);
	pDrawPanel->AddControl(*savebtn);

	albtn = new Osp::Ui::Controls::Button();
	albtn->Construct(Rectangle(556,336,64,64),L"L");
	albtn->SetActionId(AMLEFT);
	albtn->AddActionEventListener(*this);
	albtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670,64,64), Rectangle(552,798,40,40));
	albtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670+64,64,64), Rectangle(632,798,40,40));
	albtn->SetNormalBackgroundBitmap(*albtnbmpn_);
	albtn->SetPressedBackgroundBitmap(*albtnbmps_);
	albtn->SetText(L"");
	albtn->SetShowState(false);
	pDrawPanel->AddControl(*albtn);

	abbtn = new Osp::Ui::Controls::Button();
	abbtn->Construct(Rectangle(636,336,64,64),L"B");
	abbtn->SetActionId(AMBOTTOM);
	abbtn->AddActionEventListener(*this);
	abbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670,64,64), Rectangle(712,798,40,40));
	abbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670+64,64,64), Rectangle(712,798+40,40,40));
	abbtn->SetNormalBackgroundBitmap(*abbtnbmpn_);
	abbtn->SetPressedBackgroundBitmap(*abbtnbmps_);
	abbtn->SetText(L"");
	abbtn->SetShowState(false);
	pDrawPanel->AddControl(*abbtn);

	arbtn = new Osp::Ui::Controls::Button();
	arbtn->Construct(Rectangle(716,336,64,64),L"R");
	arbtn->SetActionId(AMRIGHT);
	arbtn->AddActionEventListener(*this);
	arbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670,64,64), Rectangle(592,798,40,40));
	arbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670+64,64,64), Rectangle(632,798+40,40,40));
	arbtn->SetNormalBackgroundBitmap(*arbtnbmpn_);
	arbtn->SetPressedBackgroundBitmap(*arbtnbmps_);
	arbtn->SetText(L"");
	arbtn->SetShowState(false);
	pDrawPanel->AddControl(*arbtn);

	atbtn = new Osp::Ui::Controls::Button();
	atbtn->Construct(Rectangle(636,256,64,64),L"T");
	atbtn->SetActionId(AMTOP);
	atbtn->AddActionEventListener(*this);
	atbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670,64,64), Rectangle(672,798,40,40));
	atbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(600,670+64,64,64), Rectangle(672,798+40,40,40));
	atbtn->SetNormalBackgroundBitmap(*atbtnbmpn_);
	atbtn->SetPressedBackgroundBitmap(*atbtnbmps_);
	atbtn->SetText(L"");
	atbtn->SetShowState(false);
	pDrawPanel->AddControl(*atbtn);

	cam = new Camera();
	cam->Construct(*this, CAMERA_PRIMARY);

	cam3dclass_ = new Cam3Dclass();

	LoadSettings(isolevel,exposure,brightness,contrast);

	shuttersoundplayer = new Player();
	pListener = new MyPlayerListener();
	shuttersoundplayer->Construct(*pListener, null);
	shuttersoundplayer->OpenFile(L"/Res/shutter.wav", false);

	shuttersoundplayer->SetVolume(99);

	if (shuttersound == false) {
		shuttersoundplayer->SetMute(true);
	} else {
		bool issilent = false;
		Osp::System::SettingInfo::GetValue(L"SilentMode", issilent);
		shuttersoundplayer->SetMute(issilent);
	}

	this->AddKeyEventListener(*this);

	return r;
}

void FCameraForm::Start() {

	this->RequestRedraw();

	OnActionPerformed(*this, ASTARTCAM);

}

void FCameraForm::TerminateCam() {
	if (pCapturedCombine == null) {
		OnActionPerformed(*this, ABACKWOQ);
	}
}

void FCameraForm::PauseCam() {
	if (pCapturedCombine == null) {
		if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
		}
		this->RemoveKeyEventListener(*this);
		capturebtn->SetEnabled(false);
		this->RequestRedraw();
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
			cam->StopPreview();
			cam->PowerOff();
		} else if ((cam->GetState() == CAMERA_STATE_INITIALIZED) || (cam->GetState() == CAMERA_STATE_CAPTURED) || (cam->GetState() == CAMERA_STATE_ERROR))  {
			cam->PowerOff();
		}
		Osp::System::PowerManager::KeepScreenOnState(false, true);
		pDrawPanel->ftype = pDrawPanel->PAUSE;
		this->RequestRedraw();
	}
}

void FCameraForm::ResumeCamProcess() {
	if (pCapturedCombine == null) {
		if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
		}
		this->AddKeyEventListener(*this);
		if (pCapturedFirst == null) {
			pDrawPanel->ftype = pDrawPanel->LEFTCAP;
		} else if (pCapturedSecond == null) {
			pDrawPanel->ftype = pDrawPanel->RIGHTCAP;
		}
		capturebtn->SetEnabled(true);
		this->RequestRedraw();
		StartCamera();
	}
}

void FCameraForm::ResumeCam() {
	if (pCapturedCombine == null) {
		if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
		}
		if (resumecamTimer_ == null) {
		resumecamTimer_ = new Osp::Base::Runtime::Timer;
		resumecamTimer_->Construct(*this);
		}
		resumecamTimer_->Start(2000);
	}
}

void FCameraForm::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	ResumeCamProcess();
}

result
FCameraForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	Osp::System::PowerManager::KeepScreenOnState(false, true);

	if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
		delete resumecamTimer_;
	}

	if (cam->GetState() == CAMERA_STATE_PREVIEW) {
		cam->StopPreview();
		cam->PowerOff();
	} else if ((cam->GetState() == CAMERA_STATE_INITIALIZED) || (cam->GetState() == CAMERA_STATE_CAPTURED) || (cam->GetState() == CAMERA_STATE_ERROR))  {
		cam->PowerOff();
	}
	delete cam;

	delete pCapturedCombine;
	delete pCapturedFirst;
	delete pCapturedSecond;

	if (Osp::Io::File::IsFileExist(L"/Home/capturedfirst.jpg")) {
		Osp::Io::File::Remove(L"/Home/capturedfirst.jpg");
	}
	if (Osp::Io::File::IsFileExist(L"/Home/capturedsecond.jpg")) {
		Osp::Io::File::Remove(L"/Home/capturedsecond.jpg");
	}

	delete backbtnbmpn_;
	delete backbtnbmps_;
	delete savebtnbmpn_;
	delete savebtnbmps_;
	delete albtnbmpn_;
	delete albtnbmps_;
	delete arbtnbmpn_;
	delete arbtnbmps_;
	delete atbtnbmpn_;
	delete atbtnbmps_;
	delete abbtnbmpn_;
	delete abbtnbmps_;

	delete cam3dclass_;

	shuttersoundplayer->Stop();
	shuttersoundplayer->Close();
	delete shuttersoundplayer;
	delete pListener;

	return r;
}

void FCameraForm::StartCamera() {
	if (cam->GetState() == CAMERA_STATE_INITIALIZED) {
		AppLog("camera state init");
	} else if (cam->GetState() == CAMERA_STATE_PREVIEW) {
		AppLog("camera state preview");
	} else if (cam->GetState() == CAMERA_STATE_AUTO_FOCUSING) {
		AppLog("camera state focusing");
	} else if (cam->GetState() == CAMERA_STATE_CAPTURING) {
		AppLog("camera state capturing");
	} else if (cam->GetState() == CAMERA_STATE_CAPTURED) {
		AppLog("camera state captured");
	} else if (cam->GetState() == CAMERA_STATE_ERROR) {
		AppLog("camera state error");
	}
	if ((cam->GetState() != CAMERA_STATE_PREVIEW) && (cam->GetState() != CAMERA_STATE_AUTO_FOCUSING) && (cam->GetState() != CAMERA_STATE_CAPTURING) && (cam->GetState() != CAMERA_STATE_CAPTURED) && (cam->GetState() != CAMERA_STATE_ERROR)) {
		cam->PowerOn();
		Osp::Graphics::Dimension dimpreview(800, 480);
		captureimgdim.SetSize(1600, 960);
		Osp::Graphics::Dimension dimrender(800, 480);
		cam->SetWhiteBalance(CAMERA_WHITE_BALANCE_AUTO);
		if (isolevel == 50) {
			cam->SetIsoLevel(CAMERA_ISO_50);
		} else if (isolevel == 100) {
			cam->SetIsoLevel(CAMERA_ISO_100);
		} else if (isolevel == 200) {
			cam->SetIsoLevel(CAMERA_ISO_200);
		} else if (isolevel == 400) {
			cam->SetIsoLevel(CAMERA_ISO_400);
		} else if (isolevel == 800) {
			cam->SetIsoLevel(CAMERA_ISO_800);
		} else if (isolevel == 1600) {
			cam->SetIsoLevel(CAMERA_ISO_1600);
		} else {
			cam->SetIsoLevel(CAMERA_ISO_50);
		}
		cam->SetExposure(exposure);
		cam->SetBrightness(brightness);
		cam->SetContrast(contrast);
		cam->SetQuality(CAMERA_QUALITY_SUPER_FINE);
		cam->SetPreviewFormat(Osp::Graphics::PIXEL_FORMAT_YCbCr420_PLANAR);
		cam->SetCaptureFormat(Osp::Graphics::PIXEL_FORMAT_JPEG);
		cam->SetPreviewResolution(dimpreview);
		cam->SetCaptureResolution(captureimgdim);
		pDrawPanel->SetRendererSize(dimrender);
		pDrawPanel->GetBackgroundBufferInfo(cambufferInfo);
		cam->SetExifOrientation(CAMERA_EXIF_ORIENTATION_TOP_LEFT);
		cam->SetFlash(flashenable);
		if (cam->StartPreview(&cambufferInfo, false) == E_SUCCESS) {
			Osp::System::PowerManager::KeepScreenOnState(true, false);
		}
		this->RequestRedraw();
	}
}

void
FCameraForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ASTARTCAM) {
		StartCamera();
	}
	if (actionId == ACAMCAPTURE) {
		capturebtn->SetEnabled(false);
		AppLog("cam capture autofocus");
		if (IsFailed(cam->SetAutoFocus(true))) {
			cam->Capture();
			shuttersoundplayer->SeekTo(0);
			shuttersoundplayer->Play();
		}
		AppLog("cam capture autofocus end");
	}
	if (actionId == ALEFTCAM) {
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
		cam->StopPreview();
		cam->PowerOff();
		}
	}
	if (actionId == ARIGHTCAM) {
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
		cam->StopPreview();
		cam->PowerOff();
		}
	}
	if (actionId == AALIGMENT) {
		if ((pCapturedFirst != null) && (pCapturedSecond != null)) {
		Osp::System::PowerManager::KeepScreenOnState(false, true);
		savebtn->SetShowState(true);
		albtn->SetShowState(true);
		arbtn->SetShowState(true);
		atbtn->SetShowState(true);
		abbtn->SetShowState(true);
		capturebtn->SetShowState(false);
		Osp::Graphics::Rectangle croprect;
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
		cam->StopPreview();
		cam->PowerOff();
		}
		if (pCapturedCombine == null) {
			pCapturedCombine = new Bitmap();
			result r;
			r = pCapturedCombine->Construct(Dimension(800,480), BITMAP_PIXEL_FORMAT_RGB565);
			if (r != E_SUCCESS) {
				if (r == E_OUT_OF_MEMORY) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ABACKWOQ);
				} else {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Unable to create bitmap!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ABACKWOQ);
				}
			}
		}
		croprect = cam3dclass_->MakeAnaglyphRC(pCapturedFirst, pCapturedSecond, pCapturedCombine, Point(combineleft,combinetop));
		pDrawPanel->ftype = pDrawPanel->ALIGN;
		pDrawPanel->bgbmpsizedest = Rectangle(0,0,800,480);
		pDrawPanel->bgbmpsizesrc = croprect;
		pDrawPanel->bgbmp = pCapturedCombine;
		this->RequestRedraw();
		}
	}
	if (actionId == AMLEFT) {
		combineleft = combineleft - 1;
		OnActionPerformed(source, AALIGMENT);
	}
	if (actionId == AMRIGHT) {
		combineleft = combineleft + 1;
		OnActionPerformed(source, AALIGMENT);
	}
	if (actionId == AMTOP) {
		combinetop = combinetop - 1;
		OnActionPerformed(source, AALIGMENT);
	}
	if (actionId == AMBOTTOM) {
		combinetop = combinetop + 1;
		OnActionPerformed(source, AALIGMENT);
	}
	if ((actionId == ABACK) || (actionId == ABACKWOQ)) {
		if ((pCapturedFirst != null) && (actionId != ABACKWOQ)) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Discard?", "Do you want to discard captured photo?", MSGBOX_STYLE_YESNO, 0);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_NO) {
				return;
			}
		}
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
	if (actionId == ASAVETOGALLERY) {
		if (pCapturedCombine != null) {
		if ((Osp::Io::File::IsFileExist(L"/Home/capturedfirst.jpg")) && (Osp::Io::File::IsFileExist(L"/Home/capturedsecond.jpg"))) {

			if (Osp::Io::File::IsFileExist(L"/Home/Gallery") == false) {
				Osp::Io::Directory::Create(L"/Home/Gallery");
			}

			DateTime today;
			Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, today);
			long authorid = 0;

			int filenumindex = 0;
			String filenamed;
			String filename;
			filenamed.Format(12,L"%04d%02d%02d%02d%02d",today.GetYear(),today.GetMonth(),today.GetDay(),today.GetHour(),today.GetMinute());
			filename = L"cam3d_" + filenamed + ".c3d";
			if (Osp::Io::File::IsFileExist(L"/Home/Gallery/" + filename)) {
				filenumindex++;
				filename = L"cam3d_" + filenamed + "_" + Osp::Base::Integer::ToString(filenumindex) + ".c3d";
				while (Osp::Io::File::IsFileExist(L"/Home/Gallery/" + filename) == true) {
					filename = L"cam3d_" + filenamed + "_" + Osp::Base::Integer::ToString(filenumindex) + ".c3d";
					filenumindex++;
				}
			}

			CAM3D_FILEINFO fileinfo_;

			result r;
			r = cam3dclass_->SaveCam3Dfile(L"/Home/capturedfirst.jpg", L"/Home/capturedsecond.jpg", today, authorid, combineleft*2, combinetop*2, L"/Home/Gallery/" + filename);
			if (r != E_SUCCESS) {
				if ((r == E_MAX_EXCEEDED) || (r == E_STORAGE_FULL) || (r == E_IO)) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ABACKWOQ);
				} else {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ABACKWOQ);
				}
			}
			OnActionPerformed(source, ABACKWOQ);
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Take picture first.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		}
	}
}

void FCameraForm::OnCameraAutoFocused(bool completeCondition) {
	cam->Capture();
	shuttersoundplayer->SeekTo(0);
	shuttersoundplayer->Play();
	/*if (completeCondition) {
		AppLog("Focused");
		cam->Capture();
	} else {
		AppLog("not focused");
	}*/
}

void FCameraForm::OnCameraPreviewed( Osp::Base::ByteBuffer& previewedData, result r) {

}

void FCameraForm::OnCameraCaptured( Osp::Base::ByteBuffer& capturedData, result r) {
	AppLog("captured");
	cameraerrcount = 0;
	if (pCapturedFirst == null) {
		Image * pimage = new Image();
		pimage->Construct();
		pCapturedFirst = pimage->DecodeN(capturedData, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565, 800, 480);
		result r;
		r = GetLastResult();
		if (r != E_SUCCESS) {
			if (r == E_OUT_OF_MEMORY) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't decode image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		if (Osp::Io::File::IsFileExist(L"/Home/capturedfirst.jpg")) {
			Osp::Io::File::Remove(L"/Home/capturedfirst.jpg");
		}
		File filel;
		r = filel.Construct(L"/Home/capturedfirst.jpg", L"w", true);
		if (r != E_SUCCESS) {
			if ((r == E_MAX_EXCEEDED) || (r == E_STORAGE_FULL) || (r == E_IO)) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		r = filel.Write(capturedData);
		if (r != E_SUCCESS) {
			if ((r == E_MAX_EXCEEDED) || (r == E_STORAGE_FULL) || (r == E_IO)) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		delete pimage;
	} else {
		Image * pimage = new Image();
		pimage->Construct();
		pCapturedSecond = pimage->DecodeN(capturedData, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565, 800, 480);
		result r;
		r = GetLastResult();
		if (r != E_SUCCESS) {
			if (r == E_OUT_OF_MEMORY) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't decode image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		if (Osp::Io::File::IsFileExist(L"/Home/capturedsecond.jpg")) {
			Osp::Io::File::Remove(L"/Home/capturedsecond.jpg");
		}
		File filer;
		r = filer.Construct(L"/Home/capturedsecond.jpg", L"w", true);
		if (r != E_SUCCESS) {
			if ((r == E_MAX_EXCEEDED) || (r == E_STORAGE_FULL) || (r == E_IO)) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		r = filer.Write(capturedData);
		if (r != E_SUCCESS) {
			if ((r == E_MAX_EXCEEDED) || (r == E_STORAGE_FULL) || (r == E_IO)) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			} else {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
				this->OnActionPerformed(*this, ABACKWOQ);
			}
		}
		delete pimage;
	}
	if (pCapturedSecond == null) {
		if (pCapturedFirst == null) {
			pDrawPanel->ftype = pDrawPanel->LEFTCAP;
		} else if (pCapturedSecond == null) {
			pDrawPanel->ftype = pDrawPanel->RIGHTCAP;
		}
		capturebtn->SetEnabled(true);
		this->RequestRedraw();
		cam->StartPreview(&cambufferInfo, true);
	} else {
		pDrawPanel->ftype = pDrawPanel->ALIGN;
		capturebtn->SetEnabled(false);
		this->RequestRedraw();
		this->OnActionPerformed(*this, AALIGMENT);
	}
}

void FCameraForm::OnCameraErrorOccurred(Osp::Media::CameraErrorReason r ) {
	MessageBox msgbox;
	int modalResult = 0;
	if (r == CAMERA_ERROR_OUT_OF_MEMORY) {
		msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		this->OnActionPerformed(*this, ABACKWOQ);
	} else if (r == CAMERA_ERROR_DEVICE_FAILED) {
		msgbox.Construct("Error", "Camera device failed!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		this->OnActionPerformed(*this, ABACKWOQ);
	} else {
		AppLog("camera error");
		cameraerrcount++;
		if (cameraerrcount > 0) {
			msgbox.Construct("Error", "Camera error!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			this->OnActionPerformed(*this, ABACKWOQ);
		} else {
			cam->StartPreview(&cambufferInfo, true);
			this->OnActionPerformed(*this, ACAMCAPTURE);
		}
	}
}

void FCameraForm::ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB, Osp::Graphics::Bitmap * imgbmp)
{
	int __w = 800;
	int __h = 480;
	int hw = __w/2;
	int hh = __h/2;
	byte* pOriY= (byte *)pB->GetPointer();
	byte * pOriCb= pOriY + __w*__h;
	byte * pOriCr = pOriCb + hw*hh;

	BufferInfo bi;
	imgbmp->Lock(bi);
	for( int j=0;j<__h;j+=2)
	{
		byte * pY0,*pY1, *pCb, *pCr;
		pY0 = pOriY + j*__w;
		pY1 = pOriY + (j+1)*__w;
		pCb = pOriCb + hw*(j/2);
		pCr = pOriCr + hw*(j/2);

		unsigned char * buf = (unsigned char *)bi.pPixels + bi.pitch * j;
		unsigned int * pB0= (unsigned int *)buf;

		buf = (unsigned char *)bi.pPixels + bi.pitch * (j+1);
		unsigned int * pB1= (unsigned int *)buf;
		for( int i=0;i<__w;i+=2)
		{
			int Y[4],Cb,Cr;
			Cb = pCb[i/2] - 128;
			Cr = pCr[i/2] - 128;

			Y[0]=pY0[i];
			Y[1]=pY0[i+1];
			Y[2]=pY1[i];
			Y[3]=pY1[i+1];

			int A,B,C;
			A= Cr + (Cr>>2) + (Cr>>3) + (Cr>>5);
			B= (Cb>>2) + (Cb>>4) + (Cb>>5) + (Cr>>1) + (Cr>>3) + (Cr>>4) + (Cr>>5);
			C= Cb + (Cb>>1) + (Cb>>2) + (Cb>>6);

			#define CL(A) 	((A>255)? 255:( (A<0)? 0:A ))
			#define RGBA(R, G, B, A) ( (A <<24) | (R<<16) | (G<<8) | (B) )

			pB0[i]   = RGBA(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C), 0XFF);
			pB0[i+1] = RGBA(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C), 0XFF);
			pB1[i]   = RGBA(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C), 0XFF);
			pB1[i+1] = RGBA(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C), 0XFF);
		}
	}

	imgbmp->Unlock();
}

void FCameraForm::OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FCameraForm::OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FCameraForm::OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{
	if ((keyCode == Osp::Ui::KEY_CAMERA)) {
		this->OnActionPerformed(source, ACAMCAPTURE);
	}
	/*if ((keyCode == Osp::Ui::KEY_SIDE_UP)) {
		this->OnActionPerformed(source, ALEFTCAM);
	}
	if ((keyCode == Osp::Ui::KEY_SIDE_DOWN)) {
		this->OnActionPerformed(source, ARIGHTCAM);
	}*/
}
