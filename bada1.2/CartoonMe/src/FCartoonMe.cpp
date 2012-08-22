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
#include "FCartoonMe.h"
#include "ImageFilters.h"
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
using namespace Osp::Content;
using namespace Osp::Base::Utility;
using namespace Osp::Base::Runtime;
//using namespace Osp::Ads::Controls;

FCartoonMe::FCartoonMe(void)
{
}

FCartoonMe::~FCartoonMe(void)
{
}

bool
FCartoonMe::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FCartoonMe");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FCartoonMe::OnInitializing(void)
{
	result r = E_SUCCESS;

	showdonetimer_ = null;

	showdonetimer_ = new Osp::Base::Runtime::Timer;
	showdonetimer_->Construct(*this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	imagep_ = new Image();
	imagep_->Construct();

	srcbmp = null;
	destbmp = null;
	papertexture = null;
	imagefilter_ = null;

	if (FORMID != FORMID_FIRST) {
	papertexture = new Bitmap();
	papertexture = imagep_->DecodeN(L"/Res/paper.jpg", BITMAP_PIXEL_FORMAT_ARGB8888);

	if (Osp::Io::File::IsFileExist(L"/Home/capturedimg.jpg")) {
		srcbmp = imagep_->DecodeN(L"/Home/capturedimg.jpg", BITMAP_PIXEL_FORMAT_ARGB8888);
	}
	}

	buttons_ = new Osp::Base::Collection::ArrayList;

	//first form
	buttons_->Add(*new ButtonCostumEl(Rectangle(21,702,279,72), ACONTINUE, 0, GREENBTN, FORMID_FIRST, L"Start"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(300,702,160,72), AABOUT, 0, BLUEBTN, FORMID_FIRST, L"About"));
	//about form
	buttons_->Add(*new ButtonCostumEl(Rectangle(295,702,165,72), AABOUTCLOSE, 0, REDBTN, FORMID_ABOUT, L"Back"));
	//preview form
	buttons_->Add(*new ButtonCostumEl(Rectangle(21,702,279,72), ADRAW, 0, GREENBTN, FORMID_PREVIEW, L"Draw"));
	if (pFormMgr->previewbackgotoform == 1) {
		buttons_->Add(*new ButtonCostumEl(Rectangle(300,702,160,72), APREVIEWBACKCAMERA, 0, BLUEBTN, FORMID_PREVIEW, L"Back"));
	} else {
		buttons_->Add(*new ButtonCostumEl(Rectangle(300,702,160,72), APREVIEWBACKGALLERY, 0, BLUEBTN, FORMID_PREVIEW, L"Back"));
	}
	//progress form
	buttons_->Add(*new ButtonCostumEl(Rectangle(295,702,165,72), APROGRESSCANCEL, 0, REDBTN, FORMID_PROGRESS, L"Cancel"));
	//done form
	buttons_->Add(*new ButtonCostumEl(Rectangle(21,702,279,72), ASAVEBTN, 0, GREENBTN, FORMID_DONE, L"Save"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(300,702,160,72), ADONEBACK, 0, BLUEBTN, FORMID_DONE, L"Back"));
	//select form
	buttons_->Add(*new ButtonCostumEl(Rectangle(55,190,355,110), ASELECTCAMF, 0, SELBTN, FORMID_SELECT, L"Main camera"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(55,350,355,110), ASELECTCAMB, 0, SELBTN, FORMID_SELECT, L"Front camera"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(55,510,355,110), ASELECTGAL, 0, SELBTN, FORMID_SELECT, L"Gallery"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(295,702,165,72), ADONEBACK, 0, REDBTN, FORMID_SELECT, L"Back"));

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);

	pProcessThreadWorker_ = null;

	saveaspopup_ = new Popup();
	saveaspopup_->Construct(L"IDP_POPUP3");

	Osp::Ui::Controls::Button * saveaspopupCloseButton_ = static_cast<Button *>(saveaspopup_->GetControl(L"IDC_BUTTON3"));
	saveaspopupCloseButton_->SetActionId(AID_CLOSESAVEASPOPUP);
	saveaspopupCloseButton_->AddActionEventListener(*this);

	Osp::Ui::Controls::Button * saveaspopupSDCardButton_ = static_cast<Button *>(saveaspopup_->GetControl(L"IDC_BUTTON2"));
	saveaspopupSDCardButton_->SetActionId(AID_SAVEASCARD);
	saveaspopupSDCardButton_->AddActionEventListener(*this);

	Osp::Ui::Controls::Button * saveaspopupPhoneButton_ = static_cast<Button *>(saveaspopup_->GetControl(L"IDC_BUTTON1"));
	saveaspopupPhoneButton_->SetActionId(AID_SAVEASPHONE);
	saveaspopupPhoneButton_->AddActionEventListener(*this);

	/*__pAd = new Ad();
	__pAd->Construct(Osp::Graphics::Rectangle(0, 0, 480, 80), L"2100000387_001", this);
	__pAd->SetBackgroundColor(Color::COLOR_WHITE);
	__pAd->SetTextColor(Color::COLOR_BLACK);
	__pAd->SetDefaultPortraitImage(L"/Res/bannerblank.png");
	Osp::Base::Collection::ArrayList* pKeywordsList = null;
	pKeywordsList = new Osp::Base::Collection::ArrayList;
	pKeywordsList->Construct();
	pKeywordsList->Add(*(new String(L"cartoon")));
	pKeywordsList->Add(*(new String(L"comics")));
	pKeywordsList->Add(*(new String(L"photography")));
	__pAd->SetKeywords(pKeywordsList);
	if (pKeywordsList) {
		pKeywordsList->RemoveAll(true);
		delete pKeywordsList;
		pKeywordsList = null;
	}
	this->AddControl(*__pAd);
	RequestId reqId  = INVALID_REQUEST_ID;
	__pAd->RequestAd(reqId);*/

	Osp::Graphics::Bitmap * bannerbmp_ = imagep_->DecodeN(L"/Res/bannerblank2.png",BITMAP_PIXEL_FORMAT_ARGB8888);
	Osp::Ui::Controls::Button * bannerbtn_ = new Button();
	bannerbtn_->Construct(Rectangle(0,0,480,80),L"");
	bannerbtn_->SetActionId(ABANNER);
	bannerbtn_->AddActionEventListener(*this);
	bannerbtn_->SetNormalBackgroundBitmap(*bannerbmp_);
	bannerbtn_->SetPressedBackgroundBitmap(*bannerbmp_);
	bannerbtn_->SetHighlightedBackgroundBitmap(*bannerbmp_);
	this->AddControl(*bannerbtn_);
	delete bannerbmp_;

	return r;
}

/*void
FCartoonMe::OnAdReceived(RequestId reqId, const Osp::Ads::Controls::Ad& source, result r, const Osp::Base::String& errorCode, const Osp::Base::String& errorMsg)
{
	AppLog("RequestAd response : (reqId=%d, r=%s, errorCode=%S, errorMsg=%S)", reqId,
			GetErrorMessage(r), errorCode.GetPointer(), errorMsg.GetPointer());
}*/

void FCartoonMe::BackgroundPause() {
	if (FORMID == FORMID_PROGRESS) {
		//stop process
		//pProcessThreadWorker_->StopMe();
	}
}

void FCartoonMe::BackgroundResume() {
	if (FORMID == FORMID_PROGRESS) {
		//resume thread
	}
}

result
FCartoonMe::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(75,16,330,55), Rectangle(950,400,330,55));
		//draw form frame
		if ((FORMID == FORMID_FIRST) || (FORMID == FORMID_ABOUT) || (FORMID == FORMID_PROGRESS) || (FORMID == FORMID_SELECT)) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16,88,448,598), Rectangle(480,0,448,598));
		}
		if ((FORMID == FORMID_CAMERA) || (FORMID == FORMID_PREVIEW) || (FORMID == FORMID_DONE)) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16,88,448,598), Rectangle(480,0,448,598));
			pCanvas_->FillRectangle(Color::COLOR_BLACK, Rectangle(16+8,88+8,448-16,598-16));
		}
		//draw form
		if (FORMID == FORMID_FIRST) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16+34,88+58,380,481), Rectangle(1376,0,380,481));
		} else if (FORMID == FORMID_SELECT) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16+72,88+30,305,51), Rectangle(928,0,305,51));
		} else if (FORMID == FORMID_ABOUT) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16+44,88+52,360,494), Rectangle(1376,481,360,494));
		} else if (FORMID == FORMID_PREVIEW) {
			if (srcbmp != null) {
				pCanvas_->DrawBitmap(Rectangle(16+11,88+15,426,568), *srcbmp, Rectangle(0,0,srcbmp->GetWidth(),srcbmp->GetHeight()));
			}
		} else if (FORMID == FORMID_PROGRESS) {
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16+71,88+50,306,100), Rectangle(989,742,306,100));
			//progress bar
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44,394,395,235), Rectangle(480,742,395,235));
			int progressw = (int)(395.0f*processingprocdone);
			if (progressw < 40) {
				drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44,394,progressw,235), Rectangle(875,742,progressw,235));
			} else {
				drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44,394,40,235), Rectangle(875,742,40,235));
				if (progressw > (395-40)) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44+40,394,(395-80),235), Rectangle(875+40,742,5,235));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44+(395-40),394,progressw-(395-40),235), Rectangle(875+45,742,progressw-(395-40),235));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44+40,394,progressw-40,235), Rectangle(875+40,742,5,235));
				}
			}
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(44+progressw-15,394-150,29,235), Rectangle(960,742,29,150));
		} else if (FORMID == FORMID_DONE) {
			if (destbmp != null) {
				pCanvas_->DrawBitmap(Rectangle(16+11,88+15,426,568), *destbmp, Rectangle(0,0,destbmp->GetWidth(),destbmp->GetHeight()));
			}
		}
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		int leftpos = 0;
		int toppos = 0;
		Osp::Graphics::Rectangle btntextpos;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			isvisible = (FORMID == button_->group);
			if ((button_->type == GREENBTN) || (button_->type == BLUEBTN) || (button_->type == REDBTN)) {
				leftpos = 480;
				if (button_->type == BLUEBTN) {
					leftpos = 848;
				} else if (button_->type == REDBTN) {
					leftpos = 1008;
				}
				if (button_->action == ACONTINUE) {
					btntextpos.SetBounds(0,800,132,31);
				} else if (button_->action == AABOUT) {
					btntextpos.SetBounds(132,800,89,40);
				} else if ((button_->action == AABOUTCLOSE) || (button_->action == APREVIEWBACK) || (button_->action == APREVIEWBACKCAMERA) || (button_->action == APREVIEWBACKGALLERY) || (button_->action == ADONEBACK)) {
					btntextpos.SetBounds(221,800,102,34);
				} else if (button_->action == ADRAW) {
					btntextpos.SetBounds(281,840,80,30);
				} else if (button_->action == APROGRESSCANCEL) {
					btntextpos.SetBounds(323,800,91,32);
				} else if (button_->action == ASAVEBTN) {
					btntextpos.SetBounds(360,840,73,29);
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
			if (button_->type == SELBTN) {
				if (button_->action == ASELECTCAMF) {
					toppos = 51;
				} else if (button_->action == ASELECTCAMB) {
					toppos = 161;
				} else if (button_->action == ASELECTGAL) {
					toppos = 271;
				}
				if (isvisible) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(928,toppos,button_->position.width,button_->position.height));
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FCartoonMe::OnTerminating(void)
{
	result r = E_SUCCESS;

	AppLog("terminating mainform");
	if (showdonetimer_ != null) {
		showdonetimer_->Cancel();
	}
	if (pProcessThreadWorker_ != null) {
		pProcessThreadWorker_->StopMe();
	}

	buttons_->RemoveAll(true);
	delete buttons_;

	delete srcbmp;
	delete destbmp;
	delete imagefilter_;
	delete papertexture;
	delete imagep_;

	return r;
}

void
FCartoonMe::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == AABOUT) {
		currpressedbtn = -1;
		FORMID = FORMID_ABOUT;
		this->RequestRedraw();
	} else if (actionId == AABOUTCLOSE) {
		currpressedbtn = -1;
		FORMID = FORMID_FIRST;
		this->RequestRedraw();
	} else if (actionId == ASELECTCAMF) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->curcamerasel = CAMERA_PRIMARY;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CAMERAFORM, null);
	} else if (actionId == ASELECTCAMB) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->curcamerasel = CAMERA_SECONDARY;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CAMERAFORM, null);
	} else if (actionId == ASELECTGAL) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_GALLERYFORM, null);
	} else if ((actionId == ACONTINUE) || (actionId == APREVIEWBACK)) {
		currpressedbtn = -1;
		FORMID = FORMID_SELECT;
		this->RequestRedraw();
	} else if (actionId == APREVIEWBACKCAMERA) {
		currpressedbtn = -1;
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CAMERAFORM, null);
	} else if (actionId == APREVIEWBACKGALLERY) {
		currpressedbtn = -1;
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_GALLERYFORM, null);
	} else if (actionId == ADRAW) {
		if (imagefilter_ == null) {
		imagefilter_ = new ImageFilters();
		imagefilter_->Construct();
		}
		currpressedbtn = -1;
		destbmp = new Bitmap();
		destbmp->Construct(Dimension(srcbmp->GetWidth(), srcbmp->GetHeight()), BITMAP_PIXEL_FORMAT_ARGB8888);
		processingprocdone = 0;
		pProcessThreadWorker_ = new ProcessThread();
		pProcessThreadWorker_->Construct();
		pProcessThreadWorker_->Start();
		FORMID = FORMID_PROGRESS;
		this->RequestRedraw();
	} else if (actionId == APROGRESSCANCEL) {
		currpressedbtn = -1;
		pProcessThreadWorker_->StopMe();
	} else if (actionId == APROGRESSDONE) {
		AppLog("test1");
		currpressedbtn = -1;
		if (imagefilter_ != null) {
		delete imagefilter_;
		imagefilter_ = null;
		}
		imagep_->EncodeToFile(*destbmp,IMG_FORMAT_JPG,L"/Home/drawing.jpg", true);
		bool hasmediacard = false;
		hasmediacard = (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Images") == true);
		if (hasmediacard) {
			AppLog("has media card");
		} else {
			AppLog("no media card");
		}
		int filenumindex = 1;
		while (filenumindex < 100) {
			if (Osp::Io::File::IsFileExist(L"/Media/Images/cartoonme_" + Osp::Base::Integer::ToString(filenumindex) + ".jpg") == false) {
				if (hasmediacard) {
					if (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Images/cartoonme_" + Osp::Base::Integer::ToString(filenumindex) + ".jpg") == false) {
						break;
					}
				} else {
					break;
				}
			}
			filenumindex++;
		}
		Osp::Ui::Controls::Button * saveaspopupSDCardButton_ = static_cast<Button *>(saveaspopup_->GetControl(L"IDC_BUTTON2"));
		saveaspopupSDCardButton_->SetEnabled(hasmediacard);
		Osp::Ui::Controls::EditField * saveaspopupfilenameel_ = static_cast<EditField *>(saveaspopup_->GetControl(L"IDC_EDITFIELD1"));
		saveaspopupfilenameel_->SetText(L"cartoonme_" + Osp::Base::Integer::ToString(filenumindex));
		saveaspopupfilenameel_->RequestRedraw();
		AppLog("test2");
		if (showdonetimer_ != null) {
			AppLog("cancel timer");
			showdonetimer_->Cancel();
		}
		AppLog("start timer");
		showdonetimer_->Start(1000);
	} else if (actionId == ASAVEBTN) {
		AppLog("save pressed");
		saveaspopup_->SetShowState(true);
		saveaspopup_->Show();
	} else if (actionId == ADONEBACK) {
		currpressedbtn = -1;
		if (Osp::Io::File::IsFileExist(L"/Home/capturedimg.jpg")) {
			Osp::Io::File::Remove(L"/Home/capturedimg.jpg");
		}
		if (Osp::Io::File::IsFileExist(L"/Home/drawing.jpg")) {
			Osp::Io::File::Remove(L"/Home/drawing.jpg");
		}
		delete srcbmp;
		srcbmp = null;
		delete destbmp;
		destbmp = null;
		if (imagefilter_ != null) {
		delete imagefilter_;
		imagefilter_ = null;
		}
		OnActionPerformed(source, AABOUTCLOSE);
	} else if (actionId == AID_CLOSESAVEASPOPUP) {
		saveaspopup_->SetShowState(false);
		this->RequestRedraw();
	} else if ((actionId == AID_SAVEASPHONE) || (actionId == AID_SAVEASCARD)) {
		String setfilename;
		Osp::Ui::Controls::EditField * saveaspopupfilenameel_ = static_cast<EditField *>(saveaspopup_->GetControl(L"IDC_EDITFIELD1"));
		setfilename = saveaspopupfilenameel_->GetText();
		setfilename.Trim();
		if (setfilename == L"") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Invalid filename", "Please enter file name!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		String filename;
		if (actionId == AID_SAVEASCARD) {
			filename = L"/Storagecard/Media/Images/" + setfilename + ".jpg";
		} else {
			filename = L"/Media/Images/" + setfilename + ".jpg";
		}
		if (Osp::Io::File::IsFileExist(filename) == true) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("File name exists", "File with that filename already exists!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		ContentId contentId;
        ContentManager contentManager;
        contentManager.Construct();
		if (Osp::Content::ContentManagerUtil::CopyToMediaDirectory(L"/Home/drawing.jpg", filename) != E_SUCCESS) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Error saving file! Maybe out of disk space.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		ImageContentInfo imageContentInfo;
        imageContentInfo.Construct(filename);
        contentId = contentManager.CreateContent(imageContentInfo);
        imageContentInfo.SetAuthor(L"CartoonMe");
        imageContentInfo.SetDescription(L"CartoonMe app");
        imageContentInfo.SetKeyword(L"CartoonMe");
        contentManager.UpdateContent(imageContentInfo);
		saveaspopup_->SetShowState(false);
		this->RequestRedraw();
        MessageBox msgbox;
        int modalResult = 0;
		msgbox.Construct("Saved", L"File saved to:\n" + filename, MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
	} else if (actionId == ABANNER) {
		AppId samsungAppsAppId;
		Osp::System::SystemInfo::GetValue("SamsungAppsAppId", samsungAppsAppId);
		Osp::Base::Collection::ArrayList *pArgList = new Osp::Base::Collection::ArrayList();
		pArgList->Construct();
		String *aArg = new String(L"samsungapps://ProductDetail/uz18c2vi72");
		pArgList->Add(*aArg);
		AppManager *pAppManager = AppManager::GetInstance();
		pAppManager->LaunchApplication(samsungAppsAppId, pArgList, AppManager::LAUNCH_OPTION_DEFAULT);
	}
}

void FCartoonMe::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCartoonMe::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FCartoonMe::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCartoonMe::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FCartoonMe::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
		isvisible = (FORMID == button_->group);
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
void FCartoonMe::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FCartoonMe::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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

void FCartoonMe::OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FCartoonMe::OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FCartoonMe::OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{
	if ((keyCode == Osp::Ui::KEY_CAMERA)) {
		if (FORMID == FORMID_FIRST) {
			this->OnActionPerformed(source, ACONTINUE);
		}
	}
}

void FCartoonMe::ProcessThreadCallback(int action, double procdone) {
	if (action == 1) {
		processingprocdone = 0;
		RequestRedraw();
	} else if (action == 2) {
		processingprocdone = procdone;
		RequestRedraw();
	} else if (action == 3) {
		procdone = 1;
		processingprocdone = procdone;
		RequestRedraw();
		OnActionPerformed(*this, APROGRESSDONE);
	} else if (action == 4) {
		OnActionPerformed(*this, ADONEBACK);
	}
}

void FCartoonMe::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	AppLog("timer ended");
	FORMID = FORMID_DONE;
	this->RequestRedraw();
}
