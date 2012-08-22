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
#include "FCamera.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Io;
using namespace Osp::System;
using namespace Osp::App;

FCamera::FCamera(void)
{
}

FCamera::~FCamera(void)
{
}

bool
FCamera::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FCamera");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FCamera::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;
	curcamerasel = pFormMgr->curcamerasel;

	resumecamTimer_ = null;
	framebmp = null;
	pOverlayPanelCanvas = null;
	camerabtnsenable = false;

	buttons_ = new Osp::Base::Collection::ArrayList;
	buttons_->Add(*new ButtonCostumEl(Rectangle(21,702,279,72), ACAPTURE, 0, GREENBTN, L"Take a picture"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(300,702,160,72), ABACK, 0, BLUEBTN, L"Back"));

	currpressedbtn = -1;

	pDrawPanel = new CameraOverlayPanel();
	pDrawPanel->Initialize(Rectangle(16+11,88+15,426,568));
	this->AddControl(*pDrawPanel);

	this->SetOrientation(ORIENTATION_PORTRAIT);

	cam = new Camera();
	if (curcamerasel == CAMERA_PRIMARY) {
		cam->Construct(*this, CAMERA_PRIMARY);
	} else {
		cam->Construct(*this, CAMERA_SECONDARY);
	}


	return r;
}

result
FCamera::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(75,16,330,55), Rectangle(950,400,330,55));
		//draw form frame
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16,88,448,598), Rectangle(480,0,448,598));
		pCanvas_->FillRectangle(Color::COLOR_BLACK, Rectangle(16+8,88+8,448-16,598-16));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		int leftpos = 0;
		Osp::Graphics::Rectangle btntextpos;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			isvisible = camerabtnsenable;
			if ((button_->type == GREENBTN) || (button_->type == BLUEBTN)) {
				leftpos = 480;
				if (button_->type == BLUEBTN) {
					leftpos = 848;
				}
				if (button_->action == ACAPTURE) {
					btntextpos.SetBounds(0,840,193,44);
				} else if (button_->action == ACAMERASEL) {
					btntextpos.SetBounds(193,840,88,48);
				} else if (button_->action == ABACK) {
					btntextpos.SetBounds(221,800,102,34);
				} else {
					btntextpos.SetBounds(0,0,0,0);
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598+button_->position.height,button_->position.width,button_->position.height));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598,button_->position.width,button_->position.height));
				}
				drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x + (int)((button_->position.width / 2.0f) - (btntextpos.width / 2.0f)),button_->position.y + (int)((button_->position.height / 2.0f) - (btntextpos.height / 2.0f)),btntextpos.width,btntextpos.height), btntextpos);
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FCamera::OnTerminating(void)
{
	result r = E_SUCCESS;
	camerabtnsenable = false;
	this->RemoveTouchEventListener(*this);
	this->RemoveKeyEventListener(*this);

	if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
		delete resumecamTimer_;
	}
	if (cam != null) {
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
			cam->StopPreview();
			cam->PowerOff();
		} else if ((cam->GetState() == CAMERA_STATE_INITIALIZED) || (cam->GetState() == CAMERA_STATE_CAPTURED) || (cam->GetState() == CAMERA_STATE_ERROR))  {
			cam->PowerOff();
		}
		delete cam;
	}
	Osp::System::PowerManager::KeepScreenOnState(false, true);

	delete framebmp;
	framebmp = null;
	delete pOverlayPanelCanvas;
	pOverlayPanelCanvas = null;

	buttons_->RemoveAll(true);
	delete buttons_;

	return r;
}


void
FCamera::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == ACAMERASEL) {
		currpressedbtn = -1;
		if (curcamerasel == CAMERA_SECONDARY) {
			curcamerasel = CAMERA_PRIMARY;
		} else {
			curcamerasel = CAMERA_SECONDARY;
		}
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->curcamerasel = curcamerasel;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_RESTARTCAMERAFORM, null);
	} else if (actionId == ACAPTURE) {
		currpressedbtn = -1;
		if (curcamerasel == CAMERA_PRIMARY) {
			if (IsFailed(cam->SetAutoFocus(true))) {
				cam->Capture();
			}
		} else {
			cam->StopPreview();
			cam->PowerOff();
			result rr;
			Image * pimage = new Image();
			pimage->Construct();
			Osp::Graphics::Canvas * tmpcanvas;
			Osp::Graphics::Bitmap * capturedimg;
			//Osp::Graphics::Bitmap * capturedimgflip;
			tmpcanvas = new Canvas();
			rr = tmpcanvas->Construct(Rectangle(0,0,360,480));
			if (rr != E_SUCCESS) {
				if (rr == E_OUT_OF_MEMORY) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ACAMERAERROR);
					return;
				}
			}
			tmpcanvas->DrawBitmap(Rectangle(0,0,360,480),*framebmp,Rectangle(140,0,360,480));
			capturedimg = new Bitmap();
			rr = capturedimg->Construct(*tmpcanvas,Rectangle(0,0,360,480));
			if (rr != E_SUCCESS) {
				if (rr == E_OUT_OF_MEMORY) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ACAMERAERROR);
					return;
				}
			}
			rr = pimage->EncodeToFile(*capturedimg,IMG_FORMAT_JPG,L"/Home/capturedimg.jpg", true);
			if (rr != E_SUCCESS) {
				if ((rr == E_OUT_OF_MEMORY) || (rr == E_STORAGE_FULL)) {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ACAMERAERROR);
					return;
				} else {
					MessageBox msgbox;
					int modalResult = 0;
					msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
					this->OnActionPerformed(*this, ACAMERAERROR);
					return;
				}
			}
			delete tmpcanvas;
			delete pimage;
			this->OnActionPerformed(*this, ACAPTUREDONE);
		}
	} else if (actionId == ACAPTUREDONE) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->previewbackgotoform = 1;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_PREVIEWFORM, null);
	} else if (actionId == ACAMERAERROR) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_SELECTFORM, null);
	}
}

void
FCamera::OnKeyLongPressed(const Control &source, KeyCode keyCode)
{

}

void
FCamera::OnKeyPressed(const Control &source, KeyCode keyCode)
{
	if ((keyCode == Osp::Ui::KEY_CAMERA)) {
		if (camerabtnsenable) {
			this->OnActionPerformed(source, ACAPTURE);
		}
	}
}

void
FCamera::OnKeyReleased(const Control &source, KeyCode keyCode)
{

}

void
FCamera::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCamera::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCamera::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCamera::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCamera::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (!button_->pressposition.Contains(currentPosition)) {
			if (button_->pressed) {
				button_->pressed = false;
				buttons_->SetAt(*button_,currpressedbtn,false);
				currpressedbtn = -1;
				needsredraw = true;
			}
		}
	}
	bool isvisible = true;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = camerabtnsenable;
		if ((isvisible) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				needsredraw = true;
			}
			break;
		}
	}
	if (needsredraw) {
		RequestRedraw();
	}
}

void
FCamera::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FCamera::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			button_->pressed = false;
			buttons_->SetAt(*button_,currpressedbtn,false);
			this->OnActionPerformed(source, button_->action);
			needsredraw = true;
		}
	}
	currpressedbtn = -1;
	if (needsredraw) {
		RequestRedraw();
	}
}

void FCamera::StartCamera() {
	this->SetOrientation(ORIENTATION_PORTRAIT);
	pDrawPanel->ftype = pDrawPanel->PREVIEW;
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
		if (curcamerasel == CAMERA_PRIMARY) {
			Osp::Graphics::BufferInfo cambufferInfo;
			pDrawPanel->SetRendererRotation(Osp::Ui::Controls::OverlayPanel::ROTATION_90);
			pDrawPanel->SetRendererAspectRatio(true);
			Osp::Graphics::Dimension renderdim(pDrawPanel->GetWidth(),pDrawPanel->GetHeight());
			pDrawPanel->SetRendererSize(renderdim);
			pDrawPanel->GetBackgroundBufferInfo(cambufferInfo);
			cam->PowerOn();
			cam->SetFlash(true);
			cam->SetWhiteBalance(CAMERA_WHITE_BALANCE_AUTO);
			cam->SetQuality(CAMERA_QUALITY_SUPER_FINE);
			cam->SetCaptureFormat(Osp::Graphics::PIXEL_FORMAT_JPEG);
			cam->SetPreviewResolution(Dimension(640,480));
			cam->SetCaptureResolution(Dimension(640,480));
			if (cam->StartPreview(&cambufferInfo, false) == E_SUCCESS) {
				Osp::System::PowerManager::KeepScreenOnState(true, false);
			}
		} else {
			pDrawPanel->SetRendererRotation(Osp::Ui::Controls::OverlayPanel::ROTATION_NONE);
			pDrawPanel->SetRendererAspectRatio(false);
			Osp::Graphics::Dimension renderdim(pDrawPanel->GetWidth(),pDrawPanel->GetHeight());
			pDrawPanel->SetRendererSize(renderdim);
			if (framebmp == null) {
				framebmp = new Bitmap();
				framebmp->Construct(Dimension(640,480), BITMAP_PIXEL_FORMAT_ARGB8888);
			}
			pOverlayPanelCanvas = pDrawPanel->GetCanvasN();
			cam->PowerOn();
			cam->SetPreviewResolution(Dimension(640,480));
			if (cam->StartPreview(null, true) == E_SUCCESS) {
				Osp::System::PowerManager::KeepScreenOnState(true, false);
			}
		}
	}
	camerabtnsenable = true;
	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);
	this->SetOrientation(ORIENTATION_PORTRAIT);
	this->RequestRedraw();
	this->SetOrientation(ORIENTATION_PORTRAIT);
}

void FCamera::TerminateCam() {
	OnActionPerformed(*this, ACAMERAERROR);
}

void FCamera::PauseCam() {
	if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
	}
	this->RemoveKeyEventListener(*this);
	if (cam != null) {
		if (cam->GetState() == CAMERA_STATE_PREVIEW) {
			cam->StopPreview();
			cam->PowerOff();
		} else if ((cam->GetState() == CAMERA_STATE_INITIALIZED) || (cam->GetState() == CAMERA_STATE_CAPTURED) || (cam->GetState() == CAMERA_STATE_ERROR))  {
			cam->PowerOff();
		}
	}
	Osp::System::PowerManager::KeepScreenOnState(false, true);
	pDrawPanel->ftype = pDrawPanel->PAUSE;
	camerabtnsenable = false;
	this->RemoveTouchEventListener(*this);
	this->RemoveKeyEventListener(*this);
	this->RequestRedraw();
}

void FCamera::ResumeCamProcess() {
	if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
	}
	this->AddKeyEventListener(*this);
	pDrawPanel->ftype = pDrawPanel->PREVIEW;
	this->RequestRedraw();
	StartCamera();
}

void FCamera::ResumeCam() {
	if (resumecamTimer_ != null) {
		resumecamTimer_->Cancel();
	}
	if (resumecamTimer_ == null) {
	resumecamTimer_ = new Osp::Base::Runtime::Timer;
	resumecamTimer_->Construct(*this);
	}
	resumecamTimer_->Start(2000);
}

void FCamera::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	ResumeCamProcess();
}

void FCamera::OnCameraAutoFocused(bool completeCondition) {
	cam->Capture();
}

void FCamera::OnCameraPreviewed( Osp::Base::ByteBuffer& previewedData, result r) {
	ConvertYCbCr420p2RGB888(&previewedData);

	//pDrawPanel->SetInputBuffer(Dimension(pDrawPanel->GetWidth(),pDrawPanel->GetHeight()), previewedData, Dimension(cam->GetPreviewResolution().width,cam->GetPreviewResolution().height), OverlayPanel::BUFFER_PIXEL_FORMAT_YCbCr420_PLANAR);
	//pDrawPanel->SetInputBuffer(Dimension(pDrawPanel->GetWidth(),pDrawPanel->GetHeight()), previewedData, Dimension(cam->GetPreviewResolution().width,cam->GetPreviewResolution().height), OverlayPanel::BUFFER_PIXEL_FORMAT_YCbCr420_PLANAR);

	pOverlayPanelCanvas->Clear();
	pOverlayPanelCanvas->DrawBitmap(Rectangle(0,0,pDrawPanel->GetWidth(),pDrawPanel->GetHeight()), *framebmp, Rectangle(140,0,360,480));
	pOverlayPanelCanvas->Show();
	r = E_SUCCESS;
}

void FCamera::OnCameraCaptured( Osp::Base::ByteBuffer& capturedData, result r) {
	//shrani image v file
	File file;
	result rr;
	rr = file.Construct(L"/Home/capturedimg.jpg", L"w", true);
	if (rr != E_SUCCESS) {
		if ((rr == E_OUT_OF_MEMORY) || (rr == E_STORAGE_FULL)) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			this->OnActionPerformed(*this, ACAMERAERROR);
			return;
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			this->OnActionPerformed(*this, ACAMERAERROR);
			return;
		}
	}
	rr = file.Write(capturedData);
	if (rr != E_SUCCESS) {
		if ((rr == E_OUT_OF_MEMORY) || (rr == E_STORAGE_FULL)) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			this->OnActionPerformed(*this, ACAMERAERROR);
			return;
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Can't save image!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			this->OnActionPerformed(*this, ACAMERAERROR);
			return;
		}
	}
	file.Flush();
	//preberi metatag orientation
	Osp::Content::ImageMetadata* pImgMeta = Osp::Content::ContentManagerUtil::GetImageMetaN(L"/Home/capturedimg.jpg");
	bool rotateimage = false;
	bool fliphimage = false;
	bool flipvimage = false;
	if (pImgMeta->GetOrientation() == 1) {
		//rotate image > tested ok
		rotateimage = true;
	} else if (pImgMeta->GetOrientation() == 2) {
		//rotate image + flip h > NOT TESTED
		rotateimage = true;
		fliphimage = true;
	} else if (pImgMeta->GetOrientation() == 3) {
		//rotate image + flip h + flip v > tested ok
		rotateimage = true;
	} else if (pImgMeta->GetOrientation() == 4) {
		//rotate image + flip v > NOT TESTED
		rotateimage = true;
		flipvimage = true;
	} else if (pImgMeta->GetOrientation() == 5) {
		//flip v > NOT TESTED
		flipvimage = true;
	} else if (pImgMeta->GetOrientation() == 7) {
		//flip h > NOT TESTED
		fliphimage = true;
	} else if (pImgMeta->GetOrientation() == 8) {
		//flip v + flip h > tested ok
		rotateimage = true;
	}
	AppLog("image orientation %d", pImgMeta->GetOrientation());
	delete pImgMeta;
	if ((rotateimage == true) || (fliphimage == true) || (flipvimage == true)) {
		Image * pimage = new Image();
		pimage->Construct();
		Osp::Graphics::Bitmap * capturedimg = pimage->DecodeN("/Home/capturedimg.jpg", BITMAP_PIXEL_FORMAT_RGB565);
		Osp::Graphics::Bitmap * capturedimgdone = new Bitmap();
		capturedimgdone->Construct(Dimension(480,640), BITMAP_PIXEL_FORMAT_RGB565 );
		Osp::Graphics::Canvas * tmpcanvas;
		tmpcanvas = new Canvas();
		rr = tmpcanvas->Construct(Rectangle(0,0,480,640));
		if (rotateimage == true) {
			tmpcanvas->Clear();
			tmpcanvas->DrawBitmap(Point(capturedimg->GetHeight(),capturedimg->GetWidth()),*capturedimg,Point(capturedimg->GetWidth(),0),-90);
			Osp::Graphics::Bitmap * rotatedimage = new Bitmap();
			rr = rotatedimage->Construct(*tmpcanvas,tmpcanvas->GetBounds());
			capturedimgdone->Merge(Point(0,0), *rotatedimage, tmpcanvas->GetBounds());
			delete rotatedimage;
		} else {
			capturedimgdone->Merge(Point(0,0), *capturedimg, tmpcanvas->GetBounds());
		}
		if (fliphimage == true) {
			tmpcanvas->Clear();
			tmpcanvas->DrawBitmap(Point(0,0),*capturedimgdone, FLIP_DIRECTION_HORIZONTAL);
			Osp::Graphics::Bitmap * flipedimage = new Bitmap();
			rr = flipedimage->Construct(*tmpcanvas,tmpcanvas->GetBounds());
			capturedimgdone->Merge(Point(0,0), *flipedimage, tmpcanvas->GetBounds());
			delete flipedimage;
		}
		if (flipvimage == true) {
			tmpcanvas->Clear();
			tmpcanvas->DrawBitmap(Point(0,0),*capturedimgdone, FLIP_DIRECTION_VERTICAL);
			Osp::Graphics::Bitmap * flipedimage2 = new Bitmap();
			rr = flipedimage2->Construct(*tmpcanvas,tmpcanvas->GetBounds());
			capturedimgdone->Merge(Point(0,0), *flipedimage2, tmpcanvas->GetBounds());
			delete flipedimage2;
		}
		rr = pimage->EncodeToFile(*capturedimgdone,IMG_FORMAT_JPG,L"/Home/capturedimg.jpg", true);
		delete tmpcanvas;
		delete capturedimg;
		delete pimage;
		delete capturedimgdone;
	}

	AppLog("capture done");
	this->OnActionPerformed(*this, ACAPTUREDONE);
}

void FCamera::OnCameraErrorOccurred(Osp::Media::CameraErrorReason r ) {
	MessageBox msgbox;
	int modalResult = 0;
	if (r == CAMERA_ERROR_OUT_OF_MEMORY) {
		msgbox.Construct("Error", "Out of memory!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		this->OnActionPerformed(*this, ACAMERAERROR);
	} else if (r == CAMERA_ERROR_DEVICE_FAILED) {
		msgbox.Construct("Error", "Camera device failed!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		this->OnActionPerformed(*this, ACAMERAERROR);
	} else {
		AppLog("camera error");
		msgbox.Construct("Error", "Camera error!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		this->OnActionPerformed(*this, ACAMERAERROR);
	}
}

void FCamera::ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB)
{
	BufferInfo bi;
	framebmp->Lock(bi);
	int __w = bi.width;
	int __h = bi.height;

	int hw = __w/2;
	int hh = __h/2;
	byte* pOriY= (byte *)pB->GetPointer();
	byte * pOriCb= pOriY + __w*__h;
	byte * pOriCr = pOriCb + hw*hh;
	bool flipoutput = true;

	for(int j=0;j<__h;j+=2)
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
		for( int i=0;i <__w;i+=2)
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
			B= (Cb>>2) + (Cb>>4) + (Cb>>5) + (Cr>>1) + (Cr>>3)
				+ (Cr>>4) + (Cr>>5);
			C= Cb + (Cb>>1) + (Cb>>2) + (Cb>>6);

#define CL(A) 	((A>255)? 255:( (A<0)? 0:A ))
//#define RGBA(R, G, B, A) ( (A <<24) | (B<<16) | (G<<8) | (R) )//FOR OPENGLES
#define RGBA(R, G, B, A) ( (A <<24) | (R<<16) | (G<<8) | (B) )//FOR BITMAP
			if (flipoutput == true) {
				pB0[__w-i-1]   = RGBA(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C), 0XFF);
				pB0[__w-i-2] = RGBA(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C), 0XFF);
				pB1[__w-i-1]   = RGBA(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C), 0XFF);
				pB1[__w-i-2] = RGBA(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C), 0XFF);
			} else {
				pB0[i]   = RGBA(CL(Y[0]+A), CL(Y[0]-B), CL(Y[0]+C), 0XFF);
				pB0[i+1] = RGBA(CL(Y[1]+A), CL(Y[1]-B), CL(Y[1]+C), 0XFF);
				pB1[i]   = RGBA(CL(Y[2]+A), CL(Y[2]-B), CL(Y[2]+C), 0XFF);
				pB1[i+1] = RGBA(CL(Y[3]+A), CL(Y[3]-B), CL(Y[3]+C), 0XFF);
			}
		}
	}
	framebmp->Unlock();
}
