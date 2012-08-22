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
#include "FHelpForm.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

FHelpForm::FHelpForm(bool fromclients, bool fromclientsstart)
{
	this->fromclients = fromclients;
	this->fromclientsstart = fromclientsstart;
}

FHelpForm::~FHelpForm(void)
{
}

bool
FHelpForm::Initialize()
{
	Form::Construct(L"IDF_FHELPFORM");
	this->SetName(L"FHelpForm");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FHelpForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(12,667,128,124), ABACK, 10, BOXBTN, L"Back");
	buttons_->Add(*backbtn);

	String helplinestart = L"Before you can connect to VLC player, you need to follow instructions bellow.\nIf help doesn't help you, search on Internet: \"VLC enable HTTP remote control interface\" and \"How to Computer Local IP address\".";
	String helplinesetvlc = L"1. Open VLC on your computer.\n2. Navigate to: Tools > Preferences.\n3. In preferences window choose: Show settings All (in bottom left of window).\n4.In left menu navigate to Interface > Main interfaces.\n5. Check option HTTP remote control interface.\n6. Save Preferences.\n7. Restart VLC player (Close & Open).";
	String helplinegetip = L"1. Open Start menu, choose Run or if you have option search on bottom of start menu, enter CMD inside and press return (enter).\n2. In CMD window (black screen) enter ipconfig and press return.\n3. Computer local IP address is shown in line starts with IPv4 Address.";

	Osp::Ui::Controls::Label *helplinesetvlc_ = static_cast<Label *>(GetControl(L"IDPC_LABEL2", true));
	helplinesetvlc_->SetText(helplinesetvlc);
	helplinesetvlc_->Draw();

	Osp::Ui::Controls::Label *helplinegetip_ = static_cast<Label *>(GetControl(L"IDPC_LABEL4", true));
	helplinegetip_->SetText(helplinegetip);
	helplinegetip_->Draw();

	Osp::Ui::Controls::Label *helplinestart_ = static_cast<Label *>(GetControl(L"IDPC_LABEL7", true));
	helplinestart_->SetText(helplinestart);
	helplinestart_->Draw();

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	return r;
}

result
FHelpForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		pCanvas_->FillRectangle(Color(0,0,0,50),Rectangle(0,660,480,140));
		//draw title
		pCanvas_->FillRectangle(Color(0,0,0,100),Rectangle(0,38,480,50));
		pCanvas_->FillRectangle(Color(255,255,255,100),Rectangle(0,38+50-2,480,2));
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(0,38,480,50),L"Help",28,FONT_STYLE_BOLD,false,Color::COLOR_WHITE,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		//draw icon
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(419,668,61,124), Rectangle(0,800,61,124));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		Rectangle btnicorect;
		Point btnicoshift;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == BOXBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(818,256,90,44);
					btnicoshift.SetPosition(0,0);
					if (button_->toogle) {
						btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
					} else {
						btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
					}
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+19+btnicoshift.x,button_->position.y+38+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FHelpForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	buttons_->RemoveAll(true);
	delete buttons_;

	return r;
}

void
FHelpForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		if (fromclientsstart) {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_CLIENTSFORMSTART, null);
		} else if (fromclients) {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_CLIENTSFORM, null);
		} else {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
		}
	}
}

void
FHelpForm::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FHelpForm::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FHelpForm::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FHelpForm::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	
}

void
FHelpForm::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
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
		isvisible = true;
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
FHelpForm::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FHelpForm::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
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
