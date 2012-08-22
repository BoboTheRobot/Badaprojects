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
#include "FDetailView.h"
#include "FormMgr.h"
#include "Cam3Dclass.h"
#include "ButtonCostumEl.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Io;
using namespace Osp::Media;
using namespace Osp::Base::Utility;
using namespace Osp::System;
using namespace Osp::Content;

FDetailView::FDetailView(void)
{

}

FDetailView::~FDetailView(void)
{
}

bool
FDetailView::Initialize()
{

	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"DetailView");
	this->SetOrientation(ORIENTATION_LANDSCAPE);
	this->buttonsvisible = true;

	return true;
}

result
FDetailView::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	cam3dclass_ = new Cam3Dclass();

	pimagedecoder_ = new Image();
	pimagedecoder_->Construct();

	curviewtype = VIEWTYPE_RC;

	imgbmp = null;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(30,30,64,64), ABACK, 0, ROUNDBTNBLUE, L"Back");
	backbtn->pressposition = backbtn->position;
	buttons_->Add(*backbtn);
	ButtonCostumEl * exportbtn = new ButtonCostumEl(Rectangle(706,30,64,64), AEXPORT, 0, ROUNDBTNBLUE, L"Export");
	exportbtn->pressposition = exportbtn->position;
	buttons_->Add(*exportbtn);
	ButtonCostumEl * deletebtn = new ButtonCostumEl(Rectangle(706,386,64,64), ADELETE, 0, ROUNDBTNRED, L"Delete");
	deletebtn->pressposition = deletebtn->position;
	buttons_->Add(*deletebtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);

	this->buttonsvisible = true;

	buttonsvisibleTimer_ = new Osp::Base::Runtime::Timer;
	buttonsvisibleTimer_->Construct(*this);

	exportpopup_ = new Popup();
	exportpopup_->Construct(L"IDP_POPUP1");

	Osp::Ui::Controls::Button * exportpopupbtnCloseButton_ = static_cast<Button *>(exportpopup_->GetControl(L"EXPORTBTN"));
	exportpopupbtnCloseButton_->SetActionId(AEXPORTOK);
	exportpopupbtnCloseButton_->AddActionEventListener(*this);

	Osp::Ui::Controls::Button * exportpopupbtnSaveButton_ = static_cast<Button *>(exportpopup_->GetControl(L"CANCELBTN"));
	exportpopupbtnSaveButton_->SetActionId(AEXPORTCANCEL);
	exportpopupbtnSaveButton_->AddActionEventListener(*this);

	return r;
}

void FDetailView::LoadFile(Osp::Base::String filename) {
	this->filename = filename;

	long long benchmarktime, benchmarktimeend;
	Osp::System::SystemTime::GetTicks(benchmarktime);

	Osp::Graphics::Bitmap * tmpimgbmpl = new Bitmap();
	Osp::Graphics::Bitmap * tmpimgbmpr = new Bitmap();
	Osp::Graphics::Bitmap * tmpimgbmp = new Bitmap();
	Osp::Base::ByteBuffer * tmpimgbufferl = new Osp::Base::ByteBuffer();
	Osp::Base::ByteBuffer * tmpimgbufferr = new Osp::Base::ByteBuffer();

	cam3dclass_->ReadCam3Dfileinfo(this->filename, fileinfo_);

	tmpimgbufferl->Construct(fileinfo_.leftimgsize);
	cam3dclass_->ReadCam3Dfileimg(this->filename, cam3dclass_->LEFTIMG, *tmpimgbufferl);

	tmpimgbmpl = pimagedecoder_->DecodeN(*tmpimgbufferl, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565);
	if (GetLastResult() == E_SUCCESS) {
	delete tmpimgbufferl;
	} else {
	if (Osp::Io::File::IsFileExist(L"/Home/tmpl.jpg")) {
		Osp::Io::File::Remove(L"/Home/tmpl.jpg");
	}
	File filel;
	filel.Construct(L"/Home/tmpl.jpg", L"w", true);
	filel.Write(*tmpimgbufferl);
	delete tmpimgbufferl;
	tmpimgbmpl = pimagedecoder_->DecodeN(L"/Home/tmpl.jpg", BITMAP_PIXEL_FORMAT_RGB565);
	if (Osp::Io::File::IsFileExist(L"/Home/tmpl.jpg")) {
		Osp::Io::File::Remove(L"/Home/tmpl.jpg");
	}
	}

	tmpimgbufferr->Construct(fileinfo_.rightimgsize);
	cam3dclass_->ReadCam3Dfileimg(this->filename, cam3dclass_->RIGHTIMG, *tmpimgbufferr);

	tmpimgbmpr = pimagedecoder_->DecodeN(*tmpimgbufferr, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565);
	if (GetLastResult() == E_SUCCESS) {
	delete tmpimgbufferr;
	} else {
	if (Osp::Io::File::IsFileExist(L"/Home/tmpr.jpg")) {
		Osp::Io::File::Remove(L"/Home/tmpr.jpg");
	}
	File filer;
	filer.Construct(L"/Home/tmpr.jpg", L"w", true);
	filer.Write(*tmpimgbufferr);
	delete tmpimgbufferr;
	tmpimgbmpr = pimagedecoder_->DecodeN(L"/Home/tmpr.jpg", BITMAP_PIXEL_FORMAT_RGB565);
	if (Osp::Io::File::IsFileExist(L"/Home/tmpr.jpg")) {
		Osp::Io::File::Remove(L"/Home/tmpr.jpg");
	}
	}

	Osp::Graphics::Rectangle croprect;
	tmpimgbmp->Construct(Dimension(tmpimgbmpl->GetWidth(),tmpimgbmpl->GetHeight()), BITMAP_PIXEL_FORMAT_RGB565);
	if (curviewtype == VIEWTYPE_RC) {
	croprect = cam3dclass_->MakeAnaglyphRC(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
	} else if (curviewtype == VIEWTYPE_RCGray) {
	croprect = cam3dclass_->MakeAnaglyphRCGray(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
	} else if (curviewtype == VIEWTYPE_PAR) {
	croprect = cam3dclass_->MakeAnaglyphParallel(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
	} else if (curviewtype == VIEWTYPE_CROS) {
	croprect = cam3dclass_->MakeAnaglyphCrossEyed(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
	}
	delete tmpimgbmpl;
	delete tmpimgbmpr;

	imgbmp = new Bitmap();
	imgbmp->Construct(Dimension(croprect.width,croprect.height), BITMAP_PIXEL_FORMAT_RGB565);
	imgbmp->Merge(Point(0,0), *tmpimgbmp, croprect);
	delete tmpimgbmp;


	this->buttonsvisible = true;

	buttonsvisibleTimer_->Cancel();
	buttonsvisibleTimer_->Start(3000);

	this->RequestRedraw();

	Osp::System::SystemTime::GetTicks(benchmarktimeend);
	AppLog("Done loading %d", (int)(benchmarktimeend - benchmarktime));
}

void FDetailView::ChangeView(int changeto) {
	if (curviewtype != changeto) {
		curviewtype = changeto;

		Osp::Graphics::Bitmap * tmpimgbmpl = new Bitmap();
		Osp::Graphics::Bitmap * tmpimgbmpr = new Bitmap();
		Osp::Graphics::Bitmap * tmpimgbmp = new Bitmap();
		Osp::Base::ByteBuffer * tmpimgbufferl = new Osp::Base::ByteBuffer();
		Osp::Base::ByteBuffer * tmpimgbufferr = new Osp::Base::ByteBuffer();

		tmpimgbufferl->Construct(fileinfo_.leftimgsize);
		cam3dclass_->ReadCam3Dfileimg(this->filename, cam3dclass_->LEFTIMG, *tmpimgbufferl);

		tmpimgbmpl = pimagedecoder_->DecodeN(*tmpimgbufferl, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_ARGB8888);
		if (GetLastResult() == E_SUCCESS) {
		delete tmpimgbufferl;
		} else {
		if (Osp::Io::File::IsFileExist(L"/Home/tmpl.jpg")) {
			Osp::Io::File::Remove(L"/Home/tmpl.jpg");
		}
		File filel;
		filel.Construct(L"/Home/tmpl.jpg", L"w", true);
		filel.Write(*tmpimgbufferl);
		delete tmpimgbufferl;
		tmpimgbmpl = pimagedecoder_->DecodeN(L"/Home/tmpl.jpg", BITMAP_PIXEL_FORMAT_ARGB8888);
		if (Osp::Io::File::IsFileExist(L"/Home/tmpl.jpg")) {
			Osp::Io::File::Remove(L"/Home/tmpl.jpg");
		}
		}

		tmpimgbufferr->Construct(fileinfo_.rightimgsize);
		cam3dclass_->ReadCam3Dfileimg(this->filename, cam3dclass_->RIGHTIMG, *tmpimgbufferr);

		tmpimgbmpr = pimagedecoder_->DecodeN(*tmpimgbufferr, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_ARGB8888);
		if (GetLastResult() == E_SUCCESS) {
		delete tmpimgbufferr;
		} else {
		if (Osp::Io::File::IsFileExist(L"/Home/tmpr.jpg")) {
			Osp::Io::File::Remove(L"/Home/tmpr.jpg");
		}
		File filer;
		filer.Construct(L"/Home/tmpr.jpg", L"w", true);
		filer.Write(*tmpimgbufferr);
		delete tmpimgbufferr;
		tmpimgbmpr = pimagedecoder_->DecodeN(L"/Home/tmpr.jpg", BITMAP_PIXEL_FORMAT_ARGB8888);
		if (Osp::Io::File::IsFileExist(L"/Home/tmpr.jpg")) {
			Osp::Io::File::Remove(L"/Home/tmpr.jpg");
		}
		}

		Osp::Graphics::Rectangle croprect;
		tmpimgbmp->Construct(Dimension(tmpimgbmpl->GetWidth(),tmpimgbmpl->GetHeight()), BITMAP_PIXEL_FORMAT_ARGB8888);
		if (curviewtype == VIEWTYPE_RC) {
		croprect = cam3dclass_->MakeAnaglyphRC(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
		} else if (curviewtype == VIEWTYPE_RCGray) {
		croprect = cam3dclass_->MakeAnaglyphRCGray(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
		} else if (curviewtype == VIEWTYPE_PAR) {
		croprect = cam3dclass_->MakeAnaglyphParallel(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
		} else if (curviewtype == VIEWTYPE_CROS) {
		croprect = cam3dclass_->MakeAnaglyphCrossEyed(tmpimgbmpl, tmpimgbmpr, tmpimgbmp, Point(fileinfo_.aligmentx,fileinfo_.aligmenty));
		}
		delete tmpimgbmpl;
		delete tmpimgbmpr;

		imgbmp->Merge(Point(0,0), *tmpimgbmp, croprect);
		delete tmpimgbmp;

		this->buttonsvisible = true;

		buttonsvisibleTimer_->Cancel();
		buttonsvisibleTimer_->Start(3000);

		this->RequestRedraw();

	}
}

void FDetailView::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (this->buttonsvisible == true) {
		this->buttonsvisible = false;
		this->RequestRedraw();
	}
}

result
FDetailView::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		if (imgbmp == null) {
		drawingclass_->FDrawTextToCanvas(pCanvas_, pCanvas_->GetBounds(), L"Loading...please wait...", 26, Osp::Graphics::FONT_STYLE_BOLD, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		} else {
		//draw img
		pCanvas_->DrawBitmap(Rectangle(0,0,800,480), *imgbmp, Rectangle(0,0,imgbmp->GetWidth(),imgbmp->GetHeight()));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == ROUNDBTNBLUE) {
				isvisible = buttonsvisible;
				int leftpos = 0;
				int toppos = 0;
				if (button_->action == ABACK) {
					leftpos = 472;
					toppos = 798;
				} else if (button_->action == AEXPORT) {
					leftpos = 512;
					toppos = 878;
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(472,670+button_->position.height,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,toppos+40,40,40));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(472,670,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,toppos,40,40));
				}
				}
			}
			if (button_->type == ROUNDBTNRED) {
				isvisible = buttonsvisible;
				int leftpos = 0;
				int toppos = 0;
				if (button_->action == ADELETE) {
					leftpos = 472;
					toppos = 878;
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(664,670+button_->position.height,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,toppos+40,40,40));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(664,670,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,toppos,40,40));
				}
				}
			}
		}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FDetailView::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete buttonsvisibleTimer_;
	delete cam3dclass_;
	delete imgbmp;
	delete pimagedecoder_;

	buttons_->RemoveAll(true);
	delete buttons_;

	delete exportpopup_;

	return r;
}

void
FDetailView::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		Osp::Base::Collection::ArrayList * list = new Osp::Base::Collection::ArrayList;
		list->Construct();
		list->Add(*(new Osp::Base::String(this->filename)));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_GALLERYFORM, list);
	}
	if (actionId == ADELETE) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Delete?", "Do you want to delete photo?", MSGBOX_STYLE_YESNO, 0);
		msgbox.ShowAndWait(modalResult);
		if (modalResult == MSGBOX_RESULT_YES) {
			if (Osp::Io::File::IsFileExist(this->filename)) {
				Osp::Io::File::Remove(this->filename);
			}
			bool galleryhasimages = false;
			if (Osp::Io::File::IsFileExist(L"/Home/Gallery")) {
				Directory* pDir = new Directory;
				DirEnumerator* pDirEnum;
				pDir->Construct(L"/Home/Gallery");
				pDirEnum = pDir->ReadN();
				while(pDirEnum->MoveNext() == E_SUCCESS) {
					DirEntry entry = pDirEnum->GetCurrentDirEntry();
					if (Osp::Io::File::GetFileExtension(entry.GetName()) == L"c3d") {
						galleryhasimages = true;
						break;
					}
				}
				delete pDir;
				delete pDirEnum;
			}
			if (galleryhasimages == true) {
				OnActionPerformed(source, ABACK);
			} else {
				Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			}
		}
	}
	if (actionId == AEXPORT) {
		Osp::Ui::Controls::EditField * exportpopupfilenameel_ = static_cast<EditField *>(exportpopup_->GetControl(L"FILENAMEEF"));
		Osp::Base::String outfn;
		outfn = Osp::Io::File::GetFileName(this->filename);
		outfn.Replace(L".c3d", L".jpg");
		exportpopupfilenameel_->SetText(outfn);
		exportpopupfilenameel_->RequestRedraw();
		exportpopup_->SetShowState(true);
		exportpopup_->Show();
	}
	if (actionId == AEXPORTCANCEL) {
		exportpopup_->SetShowState(false);
		this->RequestRedraw();
	}
	if (actionId == AEXPORTOK) {
		String setfilename;
		Osp::Ui::Controls::EditField * exportpopupfilenameel_ = static_cast<EditField *>(exportpopup_->GetControl(L"FILENAMEEF"));
		setfilename = exportpopupfilenameel_->GetText();
		setfilename.Trim();
		if (setfilename == L"") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Invalid filename", "Please enter file name!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		String filename;
		filename = L"/Media/Images/" + setfilename;
		if (Osp::Io::File::GetFileExtension(filename) != L"jpg") {
			filename = filename + L".jpg";
		}
		if (Osp::Io::File::IsFileExist(filename) == true) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("File name exists", "File with that filename already exists!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		Image * pimage = new Image();
		pimage->Construct();
		if (pimage->EncodeToFile(*imgbmp, IMG_FORMAT_JPG, L"/Home/viewimg.jpg", true) != E_SUCCESS) {
			delete pimage;
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error!", "Can't encode image!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		delete pimage;

		ContentId contentId;
        ContentManager contentManager;
        contentManager.Construct();
		if (Osp::Content::ContentManagerUtil::CopyToMediaDirectory(L"/Home/viewimg.jpg", filename) != E_SUCCESS) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Error", "Error saving file! Maybe out of disk space.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		}
		if (Osp::Io::File::IsFileExist(L"/Home/viewimg.jpg")) {
			Osp::Io::File::Remove(L"/Home/viewimg.jpg");
		}
		ImageContentInfo imageContentInfo;
        imageContentInfo.Construct(filename);
        contentId = contentManager.CreateContent(imageContentInfo);
        imageContentInfo.SetAuthor(L"Camera3D");
        imageContentInfo.SetDescription(L"Camera3D app");
        imageContentInfo.SetKeyword(L"Camera3D");
        contentManager.UpdateContent(imageContentInfo);
		exportpopup_->SetShowState(false);
		this->RequestRedraw();
		MessageBox msgbox;
        int modalResult = 0;
		msgbox.Construct("Saved", L"File saved to:\n" + filename, MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
	}
}

void FDetailView::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FDetailView::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FDetailView::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FDetailView::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FDetailView::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	buttonsvisibleTimer_->Cancel();
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
		isvisible = buttonsvisible;
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
	if (buttonsvisible == false) {
		buttonsvisible = true;
		needsredraw = true;
	}
	if (needsredraw) {
		RequestRedraw();
	}
}
void FDetailView::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FDetailView::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
	buttonsvisibleTimer_->Cancel();
	buttonsvisibleTimer_->Start(3000);
}

void FDetailView::OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FDetailView::OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FDetailView::OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}
