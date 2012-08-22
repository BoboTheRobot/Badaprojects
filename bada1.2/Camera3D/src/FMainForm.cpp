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
#include "FMainForm.h"
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

FMainForm::FMainForm(void)
{
}

FMainForm::~FMainForm(void)
{
}

bool
FMainForm::Initialize()
{

	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"MainForm");
	this->SetOrientation(ORIENTATION_LANDSCAPE);

	return true;
}

result
FMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	lastgalimg = null;

	if (Osp::Io::File::IsFileExist(L"/Home/Gallery")) {
		String lastfilename = L"";
		DateTime lastfiledate;
		Directory* pDir = new Directory;
		DirEnumerator* pDirEnum;
		pDir->Construct(L"/Home/Gallery");
		pDirEnum = pDir->ReadN();
		while(pDirEnum->MoveNext() == E_SUCCESS) {
			DirEntry entry = pDirEnum->GetCurrentDirEntry();
			if (Osp::Io::File::GetFileExtension(entry.GetName()) == L"c3d") {
				if (entry.GetDateTime() > lastfiledate) {
					lastfiledate = entry.GetDateTime();
					lastfilename = L"/Home/Gallery/" + entry.GetName();
				}
			}
		}
		if (lastfilename != L"") {
			Osp::Base::ByteBuffer * tmpimgbuffer = new Osp::Base::ByteBuffer();
			Cam3Dclass * cam3dclass_ = new Cam3Dclass();
			CAM3D_FILEINFO fileinfo_;
			cam3dclass_->ReadCam3Dfileinfo(lastfilename, fileinfo_);
			tmpimgbuffer->Construct(fileinfo_.leftimgsize);
			cam3dclass_->ReadCam3Dfileimg(lastfilename, cam3dclass_->LEFTIMG, *tmpimgbuffer);

			Image * pimage = new Image();
			pimage->Construct();
			lastgalimg = pimage->DecodeN(*tmpimgbuffer, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565, 220, 132);
			delete pimage;

			delete cam3dclass_;
			delete tmpimgbuffer;
		}
		AppLog("last %S", lastfilename.GetPointer());
		delete pDir;
		delete pDirEnum;
	}

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * camerabtn = new ButtonCostumEl(Rectangle(0,0,370,303), ACAMERA, 0, BIGMAINBTN, L"Camera");
	camerabtn->pressposition = Rectangle(camerabtn->position.x+45, camerabtn->position.y+45, camerabtn->position.width-80, camerabtn->position.height-40);
	buttons_->Add(*camerabtn);
	ButtonCostumEl * gallerybtn = new ButtonCostumEl(Rectangle(430,0,370,303), AGALERY, 0, BIGMAINBTN, L"Gallery");
	gallerybtn->pressposition = Rectangle(gallerybtn->position.x+45, gallerybtn->position.y+45, gallerybtn->position.width-80, gallerybtn->position.height-40);
	buttons_->Add(*gallerybtn);
	ButtonCostumEl * settingsbtn = new ButtonCostumEl(Rectangle(334,333,130,130), ASETTINGS, 0, ROUNDBTN, L"Settings");
	buttons_->Add(*settingsbtn);
	ButtonCostumEl * helpbtn = new ButtonCostumEl(Rectangle(43,333,130,130), AHELP, 0, ROUNDBTN, L"Help");
	buttons_->Add(*helpbtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);

	return r;
}

result
FMainForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,800,480), Rectangle(0,0,800,480));
		//draw overlay
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,298,800,182), Rectangle(0,480,800,182));
		//draw logo
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(538,335,203,127), Rectangle(0,670,203,127));
		//draw pagein mugo logo
		//drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(327,355,154,109), Rectangle(203,670,154,109));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == BIGMAINBTN) {
				int leftpos = 0;
				if (button_->action == ACAMERA) {
					leftpos = 812;
				} else if (button_->action == AGALERY) {
					leftpos = 1182;
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,button_->position.height,button_->position.width,button_->position.height));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,0,button_->position.width,button_->position.height));
				}
				if (button_->action == AGALERY) {
					//add picture in frame
					if (lastgalimg == null) {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+74,button_->position.y+90,220,132), Rectangle(leftpos,692,220,132));
					} else {
						pCanvas_->DrawBitmap(Rectangle(button_->position.x+74,button_->position.y+90,220,132),*lastgalimg);
					}
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+74,button_->position.y+90,220,132), Rectangle(leftpos,824,220,132));
					//btn overlay
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+63,button_->position.y+72,250-8,86), Rectangle(leftpos+8,606,250-8,86));
				}
				}
			}
			if (button_->type == ROUNDBTN) {
				int leftpos = 0;
				if (button_->action == AHELP) {
					leftpos = 812;
				} else if (button_->action == ASETTINGS) {
					leftpos = 942;
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,606+button_->position.height,button_->position.width,button_->position.height));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,606,button_->position.width,button_->position.height));
				}
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	buttons_->RemoveAll(true);
	delete buttons_;

	if (lastgalimg != null) {
		delete lastgalimg;
	}

	return r;
}

void OpenForm(int formid) {
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
	pFormMgr->SendUserEvent(formid, null);
}

void
FMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ACAMERA) {
		OpenForm(FormMgr::REQUEST_CAMERAFORM);
	}
	if (actionId == AGALERY) {
		if (lastgalimg == null) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Gallery empty", "Your gallery is empty!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		} else {
			OpenForm(FormMgr::REQUEST_GALLERYFORM);
		}
	}
	if (actionId == ASETTINGS) {
		OpenForm(FormMgr::REQUEST_SETTINGSFORM);
	}
	if (actionId == AHELP) {
		OpenForm(FormMgr::REQUEST_HELPFORM);
	}
}

void FMainForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FMainForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
		if ((isvisible) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				needsredraw = true;
			} else {
				currpressedbtn = n;
			}
			break;
		}
	}
	if (needsredraw) {
		RequestRedraw();
	}
}
void FMainForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FMainForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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

void FMainForm::OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FMainForm::OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FMainForm::OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{
	if ((keyCode == Osp::Ui::KEY_CAMERA)) {
		this->OnActionPerformed(source, ACAMERA);
	}
}
