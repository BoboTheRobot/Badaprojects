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
#include "TimeLapseClass.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Io;
using namespace Osp::Content;
using namespace Osp::App;

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
	this->SetName(L"FCameraForm");
	this->SetOrientation(ORIENTATION_LANDSCAPE);
	return true;
}

result
FCameraForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	if (File::IsFileExist(L"/Home/lastcapture.tmp")) {
		File::Remove(L"/Home/lastcapture.tmp");
	}

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	StartTimer_ = null;
	SecRefTimer_ = null;

	pCamera = null;

	previewpanel = new CameraOverlayPanel();
	previewpanel->Initialize(Rectangle(0,0,800,480));
	previewpanel->AddTouchEventListener(*this);
	this->AddControl(*previewpanel);

	backbtn = new Osp::Ui::Controls::Button();
	backbtn->Construct(Rectangle(10,10,64,64),L"Back");
	backbtn->SetActionId(ABACK);
	backbtn->AddActionEventListener(*this);
	backbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(2*40),40,40));
	backbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(2*40),40,40));
	backbtn->SetNormalBackgroundBitmap(*backbtnbmpn_);
	backbtn->SetPressedBackgroundBitmap(*backbtnbmps_);
	backbtn->SetText(L"");
	previewpanel->AddControl(*backbtn);

	stopbtn = new Osp::Ui::Controls::Button();
	stopbtn->Construct(Rectangle(726,10,64,64),L"Stop");
	stopbtn->SetShowState(false);
	stopbtn->SetActionId(ASTOP);
	stopbtn->AddActionEventListener(*this);
	stopbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(3*40),40,40));
	stopbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(3*40),40,40));
	stopbtn->SetNormalBackgroundBitmap(*stopbtnbmpn_);
	stopbtn->SetPressedBackgroundBitmap(*stopbtnbmps_);
	stopbtn->SetText(L"");
	previewpanel->AddControl(*stopbtn);

	frametime_last = 0;

	recstarttime = 0;

	recodingfilename = L"/Home/lastcapture.tmp";

	frameinterval = 1000;

	pCameraThread_ = new FCameraThread;
	pCameraThread_->Construct();

	tempexternalstorage = false;

	File testfile;
	String testfilename = L"";
	for (int i=0;i<20;i++) {
	testfilename.Append(L"aaaaaaaaaa");
	}

	if (File::IsFileExist(L"/Home/" + testfilename)) {
		File::Remove(L"/Home/" + testfilename);
	}
	if (testfile.Construct(L"/Home/" + testfilename,"w+",false) == E_INVALID_ARG) {
		tempexternalstorage = true;
	} else {
		if (File::IsFileExist(L"/Home/" + testfilename)) {
			File::Remove(L"/Home/" + testfilename);
		}
	}

	SecRefTimer_ = new Osp::Base::Runtime::Timer();
	SecRefTimer_->Construct(*this);
	SecRefTimer_->Start(500);

	this->AddKeyEventListener(*this);

	return r;
}

void FCameraForm::UpdatePreviewPanelValues() {
	Osp::Base::DateTime time;
	long long storageremains;
	long long timenow;
	int BatteryLevel;
	Osp::Base::String strvalue;

	Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, time);
	Osp::System::SystemTime::GetTicks(timenow);
	TimeSpan rectimespan((timenow - recstarttime));
	if (tempexternalstorage == true) {
		Osp::System::RuntimeInfo::GetValue(L"AvailableExternalStorage", storageremains);
	} else {
		Osp::System::RuntimeInfo::GetValue(L"AvailableInternalStorage", storageremains);
	}
	Osp::System::RuntimeInfo::GetValue(L"BatteryLevel", BatteryLevel);

	previewpanel->framenumber = pCameraThread_->frameindex;
	previewpanel->rectime = L"";
	previewpanel->rectime.Append(rectimespan.GetHours());
	previewpanel->rectime.Append(L":");
	if (rectimespan.GetMinutes() < 10) previewpanel->rectime.Append(L"0");
	previewpanel->rectime.Append(rectimespan.GetMinutes());
	previewpanel->rectime.Append(L":");
	if (rectimespan.GetSeconds() < 10) previewpanel->rectime.Append(L"0");
	previewpanel->rectime.Append(rectimespan.GetSeconds());
	if (pCameraThread_->filesize > 1048576) {
		strvalue = L"";
		strvalue.Format(25,L"%.2f",(double)(pCameraThread_->filesize / 1048576.0f));
		previewpanel->filesize = strvalue + L" Mb";
	} else {
		previewpanel->filesize = Osp::Base::Integer::ToString((int)(pCameraThread_->filesize / 1024)) + L" Kb";
	}
	if (storageremains > 1048576) {
		strvalue = L"";
		strvalue.Format(25,L"%.2f",(double)(storageremains / 1048576.0f));
		previewpanel->remainingmemory = strvalue + L" Mb";
	} else {
		previewpanel->remainingmemory = Osp::Base::Integer::ToString((int)(storageremains / 1024)) + L" Kb";
	}
	previewpanel->systemtime = L"";
	previewpanel->systemtime.Append(time.GetHour());
	previewpanel->systemtime.Append(L":");
	if (time.GetMinute() < 10) previewpanel->systemtime.Append(L"0");
	previewpanel->systemtime.Append(time.GetMinute());
	previewpanel->batterylevel = BatteryLevel;
}

void FCameraForm::StartCamera(int setframeinterval, Osp::Graphics::Dimension setresolution, int setstopafterframeno) {
	long long storageremains;
	int BatteryLevel;
	if (tempexternalstorage == true) {
		Osp::System::RuntimeInfo::GetValue(L"AvailableExternalStorage", storageremains);
	} else {
		Osp::System::RuntimeInfo::GetValue(L"AvailableInternalStorage", storageremains);
	}
	Osp::System::RuntimeInfo::GetValue(L"BatteryLevel", BatteryLevel);

	if (BatteryLevel <= 30) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Low battery!!!", "It's recommended that you charge battery before recording, otherwise your recording can be lost!\nDo you wish to continue anyway?", MSGBOX_STYLE_YESNO, 0);
		msgbox.ShowAndWait(modalResult);
		if (modalResult != MSGBOX_RESULT_YES) {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			return;
		}
	}
	if (storageremains <= 15728640) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Memory low!!!", "It's recommended that you made some free space (at least 15mb) before recording, otherwise your recording can be lost!\nDo you wish to continue anyway?", MSGBOX_STYLE_YESNO, 0);
		msgbox.ShowAndWait(modalResult);
		if (modalResult != MSGBOX_RESULT_YES) {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			return;
		}
	}

	Osp::System::PowerManager::KeepScreenOnState(true, true);

	frameinterval = ((long long)setframeinterval) * 1000;
	resolution.SetSize(setresolution.width, setresolution.height);
	stopafterframeno = setstopafterframeno;
	stopafterframecountdown = stopafterframeno;
	if (stopafterframeno < 1) stopafterframecountdown = -1;

	previewpanel->ftype = previewpanel->INIT;
	previewpanel->starttimer = 3;
	//startnotice = memory low, battery low
	previewpanel->startnotice = L"";
	previewpanel->SetRendererAspectRatio(true);

	UpdatePreviewPanelValues();
	this->RequestRedraw();

	pOverlayPanelCanvas = previewpanel->GetCanvasN();
	pCamera = new Camera();
	pCamera->Construct(*this, CAMERA_PRIMARY);
	pCamera->PowerOn();
	pCamera->SetPreviewResolution(resolution);
	pCamera->SetPreviewFrameRate(15);
	pCamera->SetWhiteBalance(CAMERA_WHITE_BALANCE_AUTO);
	pCamera->SetQuality(CAMERA_QUALITY_SUPER_FINE);
	pCamera->SetCaptureFormat(Osp::Graphics::PIXEL_FORMAT_JPEG);
	pCamera->SetCaptureResolution(Dimension(800,480));
	pCamera->SetFlash(false);
	previewpanel->GetBackgroundBufferInfo(bufferInfo);
	pCamera->StartPreview(&bufferInfo, true);

}

result
FCameraForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	if (pCamera != null) {
		pCamera->StopPreview();
		pCamera->PowerOff();
	}
	delete pCamera;

	if (pCameraThread_ != null) {
		if (pCameraThread_->isconstructed == true) {
			pCameraThread_->Stop();
			pCameraThread_->Join();
		}
		delete pCameraThread_;
		pCameraThread_ = null;
	}

	delete backbtnbmpn_;
	delete backbtnbmps_;
	delete stopbtnbmpn_;
	delete stopbtnbmps_;

	if (StartTimer_ != null) {
		StartTimer_->Cancel();
	}
	delete StartTimer_;
	if (SecRefTimer_ != null) {
		SecRefTimer_->Cancel();
	}
	delete SecRefTimer_;

	if (File::IsFileExist(recodingfilename)) {
		File::Remove(recodingfilename);
	}

	Osp::System::PowerManager::KeepScreenOnState(false, true);

	return r;
}

void FCameraForm::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (StartTimer_ != null) {
		if (StartTimer_->Equals(timer)) {
			if (previewpanel->starttimer > 1) {
				previewpanel->starttimer = previewpanel->starttimer-1;
				UpdatePreviewPanelValues();
				this->RequestRedraw();
				StartTimer_->Start(1000);
			} else {
				previewpanel->starttimer = 0;
				this->OnActionPerformed(*this, ASTART);
			}
		}
	}
	if (SecRefTimer_->Equals(timer)) {
		if (pCamera != null) {
			if (previewpanel->ftype == previewpanel->INIT) {
				if (pCamera->GetState() == CAMERA_STATE_PREVIEW) {
					previewpanel->ftype = previewpanel->START;
				}
			}
		}
		UpdatePreviewPanelValues();
		this->RequestRedraw();
		SecRefTimer_->Start(500);
	}
}

void
FCameraForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId) {
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ASTARTTIMEOUT) {
		previewpanel->starttimer = 3;
		previewpanel->ftype = previewpanel->STARTTIMER;
		UpdatePreviewPanelValues();
		this->RequestRedraw();
		StartTimer_ = new Osp::Base::Runtime::Timer();
		StartTimer_->Construct(*this);
		StartTimer_->Start(1000);
	} else if (actionId == ASTART) {
		pCameraThread_->interval = frameinterval;
		pCameraThread_->playframerate = 10;
		pCameraThread_->filename = recodingfilename;
		pCameraThread_->resolution.SetSize(resolution.width, resolution.height);
		pCameraThread_->Start();
		previewpanel->ftype = previewpanel->RECORDING;
		Osp::System::SystemTime::GetTicks(recstarttime);
		backbtn->SetShowState(false);
		stopbtn->SetShowState(true);
		UpdatePreviewPanelValues();
		this->RequestRedraw();
	} else if ((actionId == ASTOP) || (actionId == INTERUPTED_BACKGROUND) || (actionId == INTERUPTED_BATTERY) || (actionId == INTERUPTED_MEMORY)) {
		if (previewpanel->ftype == previewpanel->RECORDING) {
			if ((File::IsFileExist(recodingfilename)) && (recodingfilename != L"")) {
				if (pCameraThread_->frameindex > 0) {
					previewpanel->ftype = previewpanel->START;
					//stop thread
					if (pCameraThread_ != null) {
						pCameraThread_->Stop();
						pCameraThread_->Join();
						delete pCameraThread_;
						pCameraThread_ = null;
					}
					//save to gallery
					if (Osp::Io::File::IsFileExist(L"/Home/Gallery") == false) {
						Osp::Io::Directory::Create(L"/Home/Gallery");
					}
					DateTime today;
					Osp::System::SystemTime::GetCurrentTime(Osp::System::STANDARD_TIME, today);
					int filenumindex = 0;
					String filenamed;
					String filename;
					filenamed.Format(12,L"%04d%02d%02d%02d%02d",today.GetYear(),today.GetMonth(),today.GetDay(),today.GetHour(),today.GetMinute());
					filename = L"timelapse_" + filenamed + ".tlv";
					if (Osp::Io::File::IsFileExist(L"/Home/Gallery/" + filename)) {
						filenumindex++;
						filename = L"timelapse_" + filenamed + "_" + Osp::Base::Integer::ToString(filenumindex) + ".tlv";
						while (Osp::Io::File::IsFileExist(L"/Home/Gallery/" + filename) == true) {
							filename = L"timelapse_" + filenamed + "_" + Osp::Base::Integer::ToString(filenumindex) + ".tlv";
							filenumindex++;
						}
					}
					Osp::Io::File::Move(recodingfilename, L"/Home/Gallery/" + filename);

					Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
					FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));

					if ((actionId == INTERUPTED_BACKGROUND) || (actionId == INTERUPTED_BATTERY) || (actionId == INTERUPTED_MEMORY)) {
						pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
					} else {
						Osp::Base::Collection::ArrayList * list = new Osp::Base::Collection::ArrayList;
						list->Construct();
						list->Add(*(new Osp::Base::String(L"/Home/Gallery/" + filename)));
						list->Add(*(new Osp::Base::Boolean(true)));
						pFormMgr->SendUserEvent(FormMgr::REQUEST_PLAYERFORM, list);
					}
				} else {
					//no frames
					if ((actionId == INTERUPTED_BACKGROUND) || (actionId == INTERUPTED_BATTERY) || (actionId == INTERUPTED_MEMORY)) {
						Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
						FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
						pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
					}
				}
			} else {
				//no file
				if ((actionId == INTERUPTED_BACKGROUND) || (actionId == INTERUPTED_BATTERY) || (actionId == INTERUPTED_MEMORY)) {
					Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
					FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
					pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
				}
			}
		} else {
			//incorect state
			if ((actionId == INTERUPTED_BACKGROUND) || (actionId == INTERUPTED_BATTERY) || (actionId == INTERUPTED_MEMORY)) {
				Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			}
		}
	}
}

void FCameraForm::OnCameraAutoFocused(bool completeCondition)
 {
    //Insert your code to operate after auto focusing is completed.
 }

 void FCameraForm::OnCameraPreviewed( Osp::Base::ByteBuffer& previewedData, result r)
 {
	 if ((previewpanel->ftype == previewpanel->RECORDING) && (pCameraThread_->issaving == false) && (pCameraThread_->isconstructed == true)) {
		 long long thistime;
		 Osp::System::SystemTime::GetTicks(thistime);

		 if ((thistime - frametime_last) >= frameinterval) {
			 frametime_last = thistime;

			 if (pCameraThread_->framepreview != null) {
				 delete pCameraThread_->framepreview;
				 pCameraThread_->framepreview = null;
			 }
			 previewedData.Rewind();
			 pCameraThread_->framepreview = new ByteBuffer();
			 pCameraThread_->framepreview->Construct(previewedData);
			 pCameraThread_->framepreview->Rewind();

			 pCameraThread_->SendUserEvent(pCameraThread_->THREAD_SAVEFRAME, null);

			 stopafterframecountdown--;
			 if (stopafterframecountdown == 0) {
				 OnActionPerformed(*this, ASTOP);
			 }
		 }
	 }

 }

 void FCameraForm::OnCameraCaptured( Osp::Base::ByteBuffer& capturedData, result r)
 {

 }

 void FCameraForm::OnCameraErrorOccurred(CameraErrorReason r )
 {
    //Insert your code to operate after the error occurred.
 }

void FCameraForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if (previewpanel->Equals(source)) {
		if (previewpanel->ftype == previewpanel->START) {
			this->OnActionPerformed(source, ASTARTTIMEOUT);
		}
	}
}
void FCameraForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FCameraForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCameraForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCameraForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCameraForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCameraForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

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
		if (previewpanel->ftype == previewpanel->START) {
			this->OnActionPerformed(source, ASTARTTIMEOUT);
		} else if (previewpanel->ftype == previewpanel->RECORDING) {
			this->OnActionPerformed(source, ASTOP);
		}
	}
}

void FCameraForm::Interupt(int interuptedtype) {
	AppLog("interupt");
	OnActionPerformed(*this, interuptedtype);
}
