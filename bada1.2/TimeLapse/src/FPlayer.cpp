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
#include "FPlayer.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"
#include "TimeLapseClass.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Io;
using namespace Osp::Media;
using namespace Osp::Base::Utility;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Content;

FPlayer::FPlayer(void)
{
}

FPlayer::~FPlayer(void)
{
}

bool
FPlayer::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FPlayer");
	this->SetOrientation(ORIENTATION_LANDSCAPE);

	return true;
}

result
FPlayer::OnInitializing(void)
{
	result r = E_SUCCESS;

	autoplayonload = false;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	controlsarevisible = true;

	backbtn = new Osp::Ui::Controls::Button();
	backbtn->Construct(Rectangle(10,10,64,64),L"Back");
	backbtn->SetActionId(ABACK);
	backbtn->AddActionEventListener(*this);
	backbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(2*40),40,40));
	backbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(2*40),40,40));
	backbtn->SetNormalBackgroundBitmap(*backbtnbmpn_);
	backbtn->SetPressedBackgroundBitmap(*backbtnbmps_);
	backbtn->SetText(L"");
	this->AddControl(*backbtn);

	playpausebtn = new Osp::Ui::Controls::Button();
	playpausebtn->Construct(Rectangle(368,406,64,64),L"Play/pause");
	playpausebtn->SetActionId(APLAYPAUSE);
	playpausebtn->AddActionEventListener(*this);
	playpausebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(1*40),40,40));
	playpausebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(1*40),40,40));
	playpausepausebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(8*40),40,40));
	playpausepausebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(8*40),40,40));
	playpausebtn->SetNormalBackgroundBitmap(*playpausebtnbmpn_);
	playpausebtn->SetPressedBackgroundBitmap(*playpausebtnbmps_);
	playpausebtn->SetText(L"");
	this->AddControl(*playpausebtn);

	nextframebtn = new Osp::Ui::Controls::Button();
	nextframebtn->Construct(Rectangle(726,406,64,64),L"Next frame");
	nextframebtn->SetActionId(ANEXTFRAME);
	nextframebtn->AddActionEventListener(*this);
	nextframebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(9*40),40,40));
	nextframebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(9*40),40,40));
	nextframebtn->SetNormalBackgroundBitmap(*nextframebtnbmpn_);
	nextframebtn->SetPressedBackgroundBitmap(*nextframebtnbmps_);
	nextframebtn->SetText(L"");
	this->AddControl(*nextframebtn);

	prevframebtn = new Osp::Ui::Controls::Button();
	prevframebtn->Construct(Rectangle(630,406,64,64),L"Prev frame");
	prevframebtn->SetActionId(APREVFRAME);
	prevframebtn->AddActionEventListener(*this);
	prevframebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(10*40),40,40));
	prevframebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(10*40),40,40));
	prevframebtn->SetNormalBackgroundBitmap(*prevframebtnbmpn_);
	prevframebtn->SetPressedBackgroundBitmap(*prevframebtnbmps_);
	prevframebtn->SetText(L"");
	this->AddControl(*prevframebtn);

	infobtn = new Osp::Ui::Controls::Button();
	infobtn->Construct(Rectangle(726,10,64,64),L"Info");
	infobtn->SetActionId(ASHOWINFO);
	infobtn->AddActionEventListener(*this);
	infobtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(5*40),40,40));
	infobtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(5*40),40,40));
	infobtn->SetNormalBackgroundBitmap(*infobtnbmpn_);
	infobtn->SetPressedBackgroundBitmap(*infobtnbmps_);
	infobtn->SetText(L"");
	this->AddControl(*infobtn);

	exportbtn = new Osp::Ui::Controls::Button();
	exportbtn->Construct(Rectangle(726,106,64,64),L"Export");
	exportbtn->SetActionId(AEXPORT);
	exportbtn->AddActionEventListener(*this);
	exportbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(6*40),40,40));
	exportbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(6*40),40,40));
	exportbtn->SetNormalBackgroundBitmap(*exportbtnbmpn_);
	exportbtn->SetPressedBackgroundBitmap(*exportbtnbmps_);
	exportbtn->SetText(L"");
	this->AddControl(*exportbtn);

	/*editbtn = new Osp::Ui::Controls::Button();
	editbtn->Construct(Rectangle(726,106,64,64),L"Edit");
	editbtn->SetActionId(AEDIT);
	editbtn->AddActionEventListener(*this);
	editbtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(7*40),40,40));
	editbtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(7*40),40,40));
	editbtn->SetNormalBackgroundBitmap(*editbtnbmpn_);
	editbtn->SetPressedBackgroundBitmap(*editbtnbmps_);
	editbtn->SetText(L"");
	this->AddControl(*editbtn);*/

	deletebtn = new Osp::Ui::Controls::Button();
	deletebtn->Construct(Rectangle(726,202,64,64),L"Delete");
	deletebtn->SetActionId(ADELETEVIDEO);
	deletebtn->AddActionEventListener(*this);
	deletebtnbmpn_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196,64,64), Rectangle(1109,(4*40),40,40));
	deletebtnbmps_ = drawingclass_->FDrawButtonToBmpN(Rectangle(866,196+64,64,64), Rectangle(1109,(4*40),40,40));
	deletebtn->SetNormalBackgroundBitmap(*deletebtnbmpn_);
	deletebtn->SetPressedBackgroundBitmap(*deletebtnbmps_);
	deletebtn->SetText(L"");
	this->AddControl(*deletebtn);


	speedbtn = new Osp::Ui::Controls::Button();
	speedbtn->Construct(Rectangle(328,10,164,64),L"Speed change");
	speedbtn->SetActionId(ASPEEDCHANGE);
	speedbtn->AddActionEventListener(*this);
	speedbtnbmpn_ = CreateSpeedBtnBmp(164, L"? fps", false);
	speedbtnbmps_ = CreateSpeedBtnBmp(164, L"? fps", true);
	speedbtn->SetNormalBackgroundBitmap(*speedbtnbmpn_);
	speedbtn->SetPressedBackgroundBitmap(*speedbtnbmps_);
	speedbtn->SetText(L"");
	this->AddControl(*speedbtn);

	exportpopup_ = new Popup();
	exportpopup_->Construct(L"IDP_POPUP3");

	Osp::Ui::Controls::Button * exportpopupbtnCloseButton_ = static_cast<Button *>(exportpopup_->GetControl(L"IDC_BUTTON3"));
	exportpopupbtnCloseButton_->SetActionId(AEXPORTCANCEL);
	exportpopupbtnCloseButton_->AddActionEventListener(*this);

	Osp::Ui::Controls::Button * exportpopupbtnSaveButton_ = static_cast<Button *>(exportpopup_->GetControl(L"IDC_BUTTON1"));
	exportpopupbtnSaveButton_->SetActionId(AEXPORTSAVE);
	exportpopupbtnSaveButton_->AddActionEventListener(*this);

	Osp::Ui::Controls::Button * exportpopupbtnSaveSDButton_ = static_cast<Button *>(exportpopup_->GetControl(L"IDC_BUTTON2"));
	exportpopupbtnSaveSDButton_->SetActionId(AEXPORTSAVESD);
	exportpopupbtnSaveSDButton_->AddActionEventListener(*this);
	exportpopupbtnSaveSDButton_->SetShowState((Osp::Io::File::IsFileExist(L"/Storagecard/Media/Images") == true));

	Osp::Ui::Controls::Button * exportpopupbtnTypeButton_ = static_cast<Button *>(exportpopup_->GetControl(L"IDC_BUTTON5"));
	exportpopupbtnTypeButton_->SetActionId(AEXPORTTYPESWITCH);
	exportpopupbtnTypeButton_->AddActionEventListener(*this);

	exporttotype = EXPORTTYPE_VIDEO;
	exportfilenameset = false;

	savingpopup_ = new Popup();
	savingpopup_->Construct(L"IDP_POPUP1");

	infopopup_ = new Popup();
	infopopup_->Construct(L"IDP_POPUP2");

	Osp::Ui::Controls::Button * infopopupbtnTypeButton_ = static_cast<Button *>(infopopup_->GetControl(L"IDC_BUTTON1"));
	infopopupbtnTypeButton_->SetActionId(ACLOSEINFO);
	infopopupbtnTypeButton_->AddActionEventListener(*this);

	this->AddTouchEventListener(*this);

	HideControlsTimer_ = new Osp::Base::Runtime::Timer();
	HideControlsTimer_->Construct(*this);

	pPlayerThread_ = new FPlayerThread;
	pPlayerThread_->Construct();

	pExportThread_ = null;

	ExportProgressRefTimer_ = new Osp::Base::Runtime::Timer();
	ExportProgressRefTimer_->Construct(*this);

	displaybuffer_ = new Canvas();
	displaybuffer_->Construct(Rectangle(0,0,800,480));
	RedrawDisplayBuffer(displaybuffer_->GetBounds());

	return r;
}

Osp::Graphics::Bitmap* FPlayer::CreateSpeedBtnBmp(int width, Osp::Base::String caption, bool selected) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(Rectangle(0,0,width,64));
	pcanvas_->FillRoundRectangle(Color(0,0,0,125),Rectangle(0,0,width,64),Dimension(32,32));
	drawingclass_->FDrawTextToCanvas(pcanvas_, Rectangle(10, 10, width - 20 - 64, 64 - 20), caption, 28, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	if (selected == true) {
		drawingclass_->FDrawSpriteToCanvas(pcanvas_, Rectangle(width - 64, 0, 64, 64), Rectangle(866,196+64,64,64));
	} else {
		drawingclass_->FDrawSpriteToCanvas(pcanvas_, Rectangle(width - 64, 0, 64, 64), Rectangle(866,196,64,64));
	}
	drawingclass_->FDrawSpriteToCanvas(pcanvas_, Rectangle(width - 64 + 12, 12, 40, 40), Rectangle(1109,(9*40),40,40));

	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}

void FPlayer::LoadFile(Osp::Base::String filename, bool autoplayonload) {
	this->autoplayonload = autoplayonload;
	pPlayerThread_->filename = filename;
	pPlayerThread_->Start();
}

void FPlayer::Background() {
	if (pPlayerThread_->playing == true) {
		OnActionPerformed(*this, APLAYPAUSE);
	}
}

void FPlayer::RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect) {
	displaybufferisindrawingstate = true;
	displaybuffer_->SetBackgroundColor(Color::COLOR_BLACK);
	displaybuffer_->Clear();
	if (pPlayerThread_->framecanvas != null) {
		int leftpos = 0;
		int width = 800;
		if (pPlayerThread_->resolution.width == 320) {
			leftpos = 80;
			width = 640;
		}
		displaybuffer_->Copy(Rectangle(leftpos,0,width,480),*pPlayerThread_->framecanvas,Rectangle(0,0,pPlayerThread_->vresolution.width,pPlayerThread_->vresolution.height));
	}
	if (controlsarevisible == true) {
		int progressw = (int)(pPlayerThread_->curposproc * 800.0f);
		if (progressw > 800) progressw = 800;
		displaybuffer_->FillRectangle(Color(0,0,0,100), Rectangle(0, 396, 800, 84));
		displaybuffer_->FillRectangle(Color(255,172,7), Rectangle(0, 475, progressw, 5));
		drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(10, 406, 340, 64), L"Frame no.: " + Osp::Base::Integer::ToString(pPlayerThread_->curframe) + L"\nTime: " + pPlayerThread_->curframedatetime.ToString(), 28, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	}
	canvasredrawrect_ = redrawrect;
	displaybufferisindrawingstate = false;
}

result
FPlayer::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		pCanvas_->Copy(Point(0,0),*displaybuffer_,Rectangle(0,0,800,480));
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void FPlayer::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (HideControlsTimer_->Equals(timer)) {
		controlsarevisible = false;
		backbtn->SetShowState(false);
		playpausebtn->SetShowState(false);
		infobtn->SetShowState(false);
		deletebtn->SetShowState(false);
		exportbtn->SetShowState(false);
		//editbtn->SetShowState(false);
		nextframebtn->SetShowState(false);
		prevframebtn->SetShowState(false);
		speedbtn->SetShowState(false);
		if (pPlayerThread_->playing == false) {
			RedrawDisplayBuffer(displaybuffer_->GetBounds());
			this->RequestRedraw();
		}
	}
	if (ExportProgressRefTimer_->Equals(timer)) {
		this->OnActionPerformed(*this, THREADCALLBACK_EXPORTPROGRESSUPDATE);
	}
}

void
FPlayer::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == THREADCALLBACK_REDRAW) {
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		if (controlsarevisible == false) {
			Canvas* formCanvas;
			formCanvas = GetCanvasN();
			formCanvas->Copy(Point(canvasredrawrect_.x, canvasredrawrect_.y), *displaybuffer_, canvasredrawrect_);
			formCanvas->Show(canvasredrawrect_);
			delete formCanvas;
		} else {
			this->RequestRedraw();
		}
		/*long long fpsbenchmarktimeold = fpsbenchmarktime;
		Osp::System::SystemTime::GetTicks(fpsbenchmarktime);
		AppLog("render frame %d ms", (int)(fpsbenchmarktime - fpsbenchmarktimeold));*/
		pPlayerThread_->SendUserEvent(pPlayerThread_->THREAD_FRAMEDRAWED, null);
	} else if (requestId == THREADCALLBACK_FILELOADED) {
		delete speedbtnbmpn_;
		delete speedbtnbmps_;
		speedbtnbmpn_ = CreateSpeedBtnBmp(164, Osp::Base::Integer::ToString((int)pPlayerThread_->fileinfo_.playframerate) + L" fps", false);
		speedbtnbmps_ = CreateSpeedBtnBmp(164, Osp::Base::Integer::ToString((int)pPlayerThread_->fileinfo_.playframerate) + L" fps", true);
		speedbtn->SetNormalBackgroundBitmap(*speedbtnbmpn_);
		speedbtn->SetPressedBackgroundBitmap(*speedbtnbmps_);
		this->RequestRedraw();
		if (this->autoplayonload == true) {
			this->OnActionPerformed(*this, APLAYPAUSE);
			/*pPlayerThread_->playing = false;
			pPlayerThread_->SendUserEvent(pPlayerThread_->THREAD_PLAYPAUSE, null);*/
		}
	} else if ((requestId == THREADCALLBACK_EXPORTDONE) || (requestId == THREADCALLBACK_EXPORTPROGRESSUPDATE)) {
		AppLog("player event recived");
		this->OnActionPerformed(*this, requestId);
	}
}

result
FPlayer::OnTerminating(void)
{
	result r = E_SUCCESS;

	Osp::System::PowerManager::KeepScreenOnState(false, true);

	HideControlsTimer_->Cancel();
	delete HideControlsTimer_;

	delete backbtnbmpn_;
	delete backbtnbmps_;
	delete playpausebtnbmpn_;
	delete playpausebtnbmps_;
	delete playpausepausebtnbmpn_;
	delete playpausepausebtnbmps_;
	delete nextframebtnbmpn_;
	delete nextframebtnbmps_;
	delete prevframebtnbmpn_;
	delete prevframebtnbmps_;
	delete infobtnbmpn_;
	delete infobtnbmps_;
	delete exportbtnbmpn_;
	delete exportbtnbmps_;
	/*delete editbtnbmpn_;
	delete editbtnbmps_;*/
	delete deletebtnbmpn_;
	delete deletebtnbmps_;
	delete speedbtnbmpn_;
	delete speedbtnbmps_;

	if (pPlayerThread_ != null) {
		pPlayerThread_->Stop();
		pPlayerThread_->Join();
		delete pPlayerThread_;
		pPlayerThread_ = null;
	}

	ExportProgressRefTimer_->Cancel();
	delete ExportProgressRefTimer_;
	if (pExportThread_ != null) {
		pExportThread_->Stop();
		pExportThread_->Join();
		delete pExportThread_;
		pExportThread_ = null;
	}

	if (Osp::Io::File::IsFileExist(L"/Home/export.avi")) {
		Osp::Io::File::Remove(L"/Home/export.avi");
	}
	if (Osp::Io::File::IsFileExist(L"/Home/export.jpg")) {
		Osp::Io::File::Remove(L"/Home/export.jpg");
	}

	delete exportpopup_;
	delete savingpopup_;
	delete infopopup_;
	delete displaybuffer_;

	return r;
}

void FPlayer::OnActionPerformed(const Osp::Ui::Control& source, int actionId) {
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == APLAYPAUSE) {
		HideControlsTimer_->Cancel();
		infobtn->SetShowState(pPlayerThread_->playing);
		deletebtn->SetShowState(pPlayerThread_->playing);
		exportbtn->SetShowState(pPlayerThread_->playing);
		//editbtn->SetShowState(pPlayerThread_->playing);
		nextframebtn->SetShowState(pPlayerThread_->playing);
		prevframebtn->SetShowState(pPlayerThread_->playing);
		speedbtn->SetShowState(pPlayerThread_->playing);
		if (pPlayerThread_->playing == true) {
			playpausebtn->SetNormalBackgroundBitmap(*playpausebtnbmpn_);
			playpausebtn->SetPressedBackgroundBitmap(*playpausebtnbmps_);
			controlsarevisible = true;
			backbtn->SetShowState(true);
			playpausebtn->SetShowState(true);
		} else {
			playpausebtn->SetNormalBackgroundBitmap(*playpausepausebtnbmpn_);
			playpausebtn->SetPressedBackgroundBitmap(*playpausepausebtnbmps_);
		}
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		this->RequestRedraw();
		if (pPlayerThread_->playing != true) {
			Osp::System::PowerManager::KeepScreenOnState(true, false);
			HideControlsTimer_->Start(2000);
		} else {
			Osp::System::PowerManager::KeepScreenOnState(false, true);
		}
		pPlayerThread_->SendUserEvent(pPlayerThread_->THREAD_PLAYPAUSE, null);
	} else if (actionId == APREVFRAME) {
		if (pPlayerThread_->playing != true) {
			pPlayerThread_->SendUserEvent(pPlayerThread_->THREAD_PREVFRAME, null);
		}
	} else if (actionId == ANEXTFRAME) {
		if (pPlayerThread_->playing != true) {
			pPlayerThread_->SendUserEvent(pPlayerThread_->THREAD_NEXTFRAME, null);
		}
	} else if (actionId == ADELETEVIDEO) {
		if (pPlayerThread_->playing != true) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Delete?", "Do you want to delete video?", MSGBOX_STYLE_YESNO, 0);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				Osp::Base::String filename = pPlayerThread_->filename;
				pPlayerThread_->Stop();
				pPlayerThread_->Join();
				delete pPlayerThread_;
				pPlayerThread_ = null;
				if (Osp::Io::File::IsFileExist(filename)) {
					Osp::Io::File::Remove(filename);
				}
				Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			}
		}
	} else if (actionId == ASHOWINFO) {
		if (pPlayerThread_->playing != true) {
			Osp::Base::String strvalue;

			Osp::Ui::Controls::Label * infopopuplresel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLRES"));
			Osp::Ui::Controls::Label * infopopupstimeel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLSTIME"));
			Osp::Ui::Controls::Label * infopopupetimeel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLETIME"));
			Osp::Ui::Controls::Label * infopopupframesel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLFRAMES"));
			Osp::Ui::Controls::Label * infopopupfilesizeel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLFILESIZE"));
			Osp::Ui::Controls::Label * infopopupftimeel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLFTIME"));
			Osp::Ui::Controls::Label * infopopupframenel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLFRAMEN"));
			Osp::Ui::Controls::Label * infopopupframesizeel_ = static_cast<Label *>(infopopup_->GetControl(L"LBLFRAMESIZE"));

			infopopuplresel_->SetText(Osp::Base::Integer::ToString(pPlayerThread_->resolution.width) + L"x" + Osp::Base::Integer::ToString(pPlayerThread_->resolution.height));
			infopopupstimeel_->SetText(pPlayerThread_->startframetime.ToString());
			infopopupetimeel_->SetText(pPlayerThread_->endframetime.ToString());
			infopopupframesel_->SetText(Osp::Base::Integer::ToString(pPlayerThread_->framescount));

			if (pPlayerThread_->filesize > 1048576) {
				strvalue = L"";
				strvalue.Format(25,L"%.2f",(double)(pPlayerThread_->filesize / 1048576.0f));
				strvalue.Append(L" Mb");
			} else {
				strvalue = L"";
				strvalue.Append((int)(pPlayerThread_->filesize / 1024));
				strvalue.Append(L" Kb");
			}
			infopopupfilesizeel_->SetText(strvalue);

			infopopupftimeel_->SetText(pPlayerThread_->curframedatetime.ToString());
			infopopupframenel_->SetText(Osp::Base::Integer::ToString(pPlayerThread_->curframe));

			if (pPlayerThread_->curframesize > 1048576) {
				strvalue = L"";
				strvalue.Format(25,L"%.2f",(double)(pPlayerThread_->curframesize / 1048576.0f));
				strvalue.Append(L" Mb");
			} else {
				strvalue = L"";
				strvalue.Append((int)(pPlayerThread_->curframesize / 1024));
				strvalue.Append(L" Kb");
			}
			infopopupframesizeel_->SetText(strvalue);

			infopopup_->SetShowState(true);
			infopopup_->Show();
		}
	} else if (actionId == ACLOSEINFO) {
		infopopup_->SetShowState(false);
		this->RequestRedraw();
	} else if (actionId == AEXPORT) {
		if (pPlayerThread_->playing != true) {
			if (exportfilenameset == false) {
				Osp::Ui::Controls::EditField * exportpopupfilenameel_ = static_cast<EditField *>(exportpopup_->GetControl(L"IDC_EDITFIELD1"));
				Osp::Base::String outfn;
				outfn = Osp::Io::File::GetFileName(pPlayerThread_->filename);
				outfn.Replace(L".tlv", L".avi");
				exportpopupfilenameel_->SetText(outfn);
				exportpopupfilenameel_->RequestRedraw();
				exportfilenameset = true;
			}
			exportpopup_->SetShowState(true);
			exportpopup_->Show();
		}
	} else if (actionId == AEXPORTTYPESWITCH) {
		Osp::Ui::Controls::EditField * exportpopupfilenameel_ = static_cast<EditField *>(exportpopup_->GetControl(L"IDC_EDITFIELD1"));
		Osp::Base::String outfn;
		outfn = Osp::Io::File::GetFileName(pPlayerThread_->filename);
		Osp::Ui::Controls::Button * exportpopupswitchbtn_ = static_cast<Button *>(exportpopup_->GetControl(L"IDC_BUTTON5"));
		Osp::Ui::Controls::Label * exportpopupswitchlabel_ = static_cast<Label *>(exportpopup_->GetControl(L"IDC_LABEL2"));
		if (exporttotype == EXPORTTYPE_VIDEO) {
			exporttotype = EXPORTTYPE_FRAME;
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			exporttotype = EXPORTTYPE_VIDEO;
		}
		if (exporttotype == EXPORTTYPE_VIDEO) {
			exportpopupswitchlabel_->SetText(L"Video file");
			exportpopupswitchbtn_->SetText(L"Frame image");
			outfn.Replace(L".tlv", L".avi");
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			exportpopupswitchlabel_->SetText(L"Frame image");
			exportpopupswitchbtn_->SetText(L"Video file");
			outfn.Replace(L".tlv", L".jpg");
		}
		exportpopupswitchlabel_->Draw();
		exportpopupswitchlabel_->Show();
		exportpopupswitchbtn_->Draw();
		exportpopupswitchbtn_->Show();
		exportpopupfilenameel_->SetText(outfn);
		exportpopupfilenameel_->RequestRedraw();
	} else if (actionId == AEDIT) {
		if (pPlayerThread_->playing != true) {
			//open edit form
		}
	} else if (actionId == AEXPORTCANCEL) {
		exportpopup_->SetShowState(false);
		this->RequestRedraw();
	} else if ((actionId == AEXPORTSAVE) || (actionId == AEXPORTSAVESD)) {
		String setfilename;
		Osp::Ui::Controls::EditField * exportpopupfilenameel_ = static_cast<EditField *>(exportpopup_->GetControl(L"IDC_EDITFIELD1"));
		setfilename = exportpopupfilenameel_->GetText();
		setfilename.Trim();
		if (setfilename == L"") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Invalid filename", "Please enter file name!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		exportfilename = L"";
		if (actionId == AEXPORTSAVESD) {
			exportfilename = L"/Storagecard/Media/";
		} else {
			exportfilename = L"/Media/";
		}
		exporttempfilename = L"/Home/";
		if (exporttotype == EXPORTTYPE_VIDEO) {
			exportfilename.Append(L"Others/" + setfilename);
			exporttempfilename.Append(L"export.avi");
			if (Osp::Io::File::GetFileExtension(exportfilename) != L"avi") {
				exportfilename = exportfilename + L".avi";
				exportpopupfilenameel_->SetText(setfilename + L".avi");
				exportpopupfilenameel_->RequestRedraw();
			}
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			exportfilename.Append(L"Images/" + setfilename);
			exporttempfilename.Append(L"export.jpg");
			if (Osp::Io::File::GetFileExtension(exportfilename) != L"jpg") {
				exportfilename = exportfilename + L".jpg";
				exportpopupfilenameel_->SetText(setfilename + L".jpg");
				exportpopupfilenameel_->RequestRedraw();
			}
		}
		if (Osp::Io::File::IsFileExist(exportfilename) == true) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("File name exists", "File with that filename already exists!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		if (exporttotype == EXPORTTYPE_VIDEO) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Notice", "Video file will not be playable on Wave phone's player, export only for playing on other media players (PC,Youtube,...).\nVideo will be exported in Others folder.", MSGBOX_STYLE_OKCANCEL, 60000);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_CANCEL) {
				return;
			}
		}
		if (Osp::Io::File::IsFileExist(exporttempfilename)) {
			Osp::Io::File::Remove(exporttempfilename);
		}
		exportpopup_->SetShowState(false);
		this->RequestRedraw();

		savingpopup_->SetShowState(true);
		Osp::Ui::Controls::Progress * savingpopupprogressel_ = static_cast<Progress *>(savingpopup_->GetControl(L"IDC_PROGRESS1"));
		savingpopupprogressel_->SetValue(0);
		savingpopup_->Show();

		/*TimeLapseClass * TimeLapseClass_ = new TimeLapseClass();

		if (exporttotype == EXPORTTYPE_VIDEO) {
			TimeLapseClass_->ExportToMJPEGfile(pPlayerThread_->filename, tempfilename);
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			TimeLapseClass_->ExportToJPEGframefile(pPlayerThread_->filename, pPlayerThread_->curframefilepos, tempfilename);
		}

		delete TimeLapseClass_;*/

		pExportThread_ = new FExportThread;
		pExportThread_->Construct();
		pExportThread_->filename = pPlayerThread_->filename;
		pExportThread_->filenameout = exporttempfilename;
		pExportThread_->framefilepos = pPlayerThread_->curframefilepos;
		pExportThread_->Start();
		ExportProgressRefTimer_->Start(500);

		if (exporttotype == EXPORTTYPE_VIDEO) {
			pExportThread_->SendUserEvent(pExportThread_->THREAD_EXPORTMJPEG, null);
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			pExportThread_->SendUserEvent(pExportThread_->THREAD_EXPORTFRAME, null);
		}

	} else if (actionId == THREADCALLBACK_EXPORTPROGRESSUPDATE) {
		if (pExportThread_ != null) {
			double progress = ((double)pExportThread_->TimeLapseClass_->exportfileframecurno / (double)pExportThread_->TimeLapseClass_->exportfileframescount);
			if (progress > 1) progress = 1;
			if (progress < 1) {
				ExportProgressRefTimer_->Start(500);
			}
			int progressint = (int)(progress * 100.0f);
			if (progressint < 0) progressint = 0;
			if (progressint > 100) progressint = 100;
			Osp::Ui::Controls::Progress * savingpopupprogressel_ = static_cast<Progress *>(savingpopup_->GetControl(L"IDC_PROGRESS1"));
			savingpopupprogressel_->SetValue(progressint);
			savingpopupprogressel_->Draw();
			savingpopupprogressel_->Show();
		}
	} else if (actionId == THREADCALLBACK_EXPORTDONE) {
		ExportProgressRefTimer_->Cancel();
		Osp::Ui::Controls::Progress * savingpopupprogressel_ = static_cast<Progress *>(savingpopup_->GetControl(L"IDC_PROGRESS1"));
		savingpopupprogressel_->SetValue(100);
		savingpopupprogressel_->Draw();
		savingpopupprogressel_->Show();

		pExportThread_->Stop();
		pExportThread_->Join();
		delete pExportThread_;
		pExportThread_ = null;

		ContentId contentId;
		ContentManager contentManager;
		contentManager.Construct();
		if (Osp::Content::ContentManagerUtil::CopyToMediaDirectory(exporttempfilename, exportfilename) != E_SUCCESS) {
			savingpopup_->SetShowState(false);
			this->RequestRedraw();
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Error saving file! Maybe out of disk space.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		if (Osp::Io::File::IsFileExist(exporttempfilename)) {
			Osp::Io::File::Remove(exporttempfilename);
		}

		if (exporttotype == EXPORTTYPE_VIDEO) {
			VideoContentInfo videoContentInfo;
			videoContentInfo.Construct(exportfilename);
			contentId = contentManager.CreateContent(videoContentInfo);
			videoContentInfo.SetAuthor(L"Timelapse");
			videoContentInfo.SetDescription(L"Timelapse app");
			videoContentInfo.SetKeyword(L"Timelapse");
			contentManager.UpdateContent(videoContentInfo);
		} else if (exporttotype == EXPORTTYPE_FRAME) {
			ImageContentInfo imageContentInfo;
			imageContentInfo.Construct(exportfilename);
			contentId = contentManager.CreateContent(imageContentInfo);
			imageContentInfo.SetAuthor(L"Timelapse");
			imageContentInfo.SetDescription(L"Timelapse app");
			imageContentInfo.SetKeyword(L"Timelapse");
			contentManager.UpdateContent(imageContentInfo);
		}
		savingpopup_->SetShowState(false);
		this->RequestRedraw();
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Saved", L"File saved to:\n" + exportfilename, MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
	} else if (actionId == ASPEEDCHANGE) {
		if ((int)pPlayerThread_->fileinfo_.playframerate == 10) {
			pPlayerThread_->SetFrameRate(15);
		} else if ((int)pPlayerThread_->fileinfo_.playframerate == 15) {
			pPlayerThread_->SetFrameRate(20);
		} else if ((int)pPlayerThread_->fileinfo_.playframerate == 20) {
			pPlayerThread_->SetFrameRate(25);
		} else if ((int)pPlayerThread_->fileinfo_.playframerate == 25) {
			pPlayerThread_->SetFrameRate(30);
		} else {
			pPlayerThread_->SetFrameRate(10);
		}

		delete speedbtnbmpn_;
		delete speedbtnbmps_;
		speedbtnbmpn_ = CreateSpeedBtnBmp(164, Osp::Base::Integer::ToString((int)pPlayerThread_->fileinfo_.playframerate) + L" fps", false);
		speedbtnbmps_ = CreateSpeedBtnBmp(164, Osp::Base::Integer::ToString((int)pPlayerThread_->fileinfo_.playframerate) + L" fps", true);
		speedbtn->SetNormalBackgroundBitmap(*speedbtnbmpn_);
		speedbtn->SetPressedBackgroundBitmap(*speedbtnbmps_);
		this->RequestRedraw();

	}
}

void FPlayer::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FPlayer::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FPlayer::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FPlayer::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FPlayer::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FPlayer::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	HideControlsTimer_->Cancel();
	if (controlsarevisible == false) {
		controlsarevisible = true;
		backbtn->SetShowState(true);
		playpausebtn->SetShowState(true);
		infobtn->SetShowState(not pPlayerThread_->playing);
		deletebtn->SetShowState(not pPlayerThread_->playing);
		exportbtn->SetShowState(not pPlayerThread_->playing);
		//editbtn->SetShowState(not pPlayerThread_->playing);
		nextframebtn->SetShowState(not pPlayerThread_->playing);
		prevframebtn->SetShowState(not pPlayerThread_->playing);
		speedbtn->SetShowState(not pPlayerThread_->playing);
		if (pPlayerThread_->playing == false) {
			RedrawDisplayBuffer(displaybuffer_->GetBounds());
			this->RequestRedraw();
		}
	}
	if (pPlayerThread_->playing) {
		HideControlsTimer_->Start(2000);
	}
}
void FPlayer::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
