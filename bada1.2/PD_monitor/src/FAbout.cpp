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
#include "FAbout.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::App;
using namespace Osp::Graphics;

FAbout::FAbout(void)
{
}

FAbout::~FAbout(void)
{
}

bool
FAbout::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FAbout");
	this->SetOrientation(ORIENTATION_PORTRAIT);
	return true;
}

result
FAbout::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	Osp::Media::Image * imgdecoder = new Osp::Media::Image();
	imgdecoder->Construct();

	logobmp = imgdecoder->DecodeN(L"/Res/logo.png", BITMAP_PIXEL_FORMAT_ARGB8888);

	delete imgdecoder;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * closebtn = new ButtonCostumEl(Rectangle(20,688,141,92), ACLOSE, 4, BOXBTN, L"Back");
	buttons_->Add(*closebtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	return r;
}

result
FAbout::OnTerminating(void)
{
	result r = E_SUCCESS;

	buttons_->RemoveAll(true);
	delete buttons_;

	delete logobmp;

	return r;
}

result
FAbout::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		//title
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,50), Rectangle(0,800,480,50));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(196,10,89,22), Rectangle(880,206,89,22));
		//text
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,80,440,200), L"Programing & Design:\nBostjan Mrak\n\nBussines partner:\nPagein, d.o.o.", 30, Osp::Graphics::FONT_STYLE_BOLD, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_TOP);
		//logo
		pCanvas_->DrawBitmap(Point(120,280), *logobmp);
		//buttons
		ButtonCostumEl* button_;
		bool isvisible;
		Rectangle btnicorect;
		Point btnicoshift;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (isvisible) {
				if (button_->type == BOXBTN) {
					if (isvisible) {
						if ((button_->pressed) || (button_->toogle)) {
							drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(880,0+button_->position.height,button_->position.width,button_->position.height));
						} else {
							drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(880,0,button_->position.width,button_->position.height));
						}
						btnicorect.SetBounds(880,184,89,22);
						btnicoshift.SetPosition(0,0);
						if (button_->toogle) {
							btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
						} else {
							btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
						}
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+26+btnicoshift.x,button_->position.y+35+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
					}
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void
FAbout::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ACLOSE) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->TransitionDirection = pFormMgr->TRANSITIONRIGHT;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
}

void FAbout::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FAbout::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FAbout::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FAbout::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FAbout::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
			}
			break;
		}
	}
	if (needsredraw) {
		RequestRedraw();
	}
}
void FAbout::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FAbout::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
