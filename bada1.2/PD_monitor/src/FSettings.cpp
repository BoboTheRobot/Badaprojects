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
#include "FSettings.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::App;
using namespace Osp::Graphics;

FSettings::FSettings(void)
{
}

FSettings::~FSettings(void)
{
}

bool
FSettings::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FSettings");
	this->SetOrientation(ORIENTATION_PORTRAIT);
	return true;
}

result
FSettings::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * savebtn = new ButtonCostumEl(Rectangle(20,688,141,92), ACANCEL, 2, BOXBTN, L"Cancel");
	buttons_->Add(*savebtn);
	ButtonCostumEl * cancelbtn = new ButtonCostumEl(Rectangle(319,688,141,92), ASAVE, 3, BOXBTN, L"Save");
	buttons_->Add(*cancelbtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	Osp::Ui::Controls::EditField * limitef = new Osp::Ui::Controls::EditField();
	limitef->Construct(Rectangle(20,80,440,106), EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true, 6, GROUP_STYLE_TOP);
	limitef->SetName(L"limitel");
	limitef->SetGuideText(L"Enter monthly limit [MB]");
	limitef->SetTitleText(L"Monthly limit in MB");
	limitef->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_NUMERIC, true);
	int setlimit = 0;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"limit", setlimit) == E_SUCCESS) {
		limitef->SetText(Osp::Base::Integer::ToString(setlimit));
	}
	this->AddControl(*limitef);
	Osp::Ui::Controls::EditField * dayef = new Osp::Ui::Controls::EditField();
	dayef->Construct(Rectangle(20,186,440,106), EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true, 2, GROUP_STYLE_BOTTOM);
	dayef->SetName(L"dayel");
	dayef->SetGuideText(L"Day in month");
	dayef->SetTitleText(L"Day in month for reset");
	dayef->SetInputModeCategory(Osp::Ui::Controls::EDIT_INPUTMODE_NUMERIC, true);
	int setday = 1;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"day", setday) == E_SUCCESS) {
		dayef->SetText(Osp::Base::Integer::ToString(setday));
	}
	this->AddControl(*dayef);

	Osp::Ui::Controls::Label *infolabel_ = new Osp::Ui::Controls::Label();
	infolabel_->Construct(Rectangle(20,302,440,200),L"Application will auto start and shut down\nevery month on selected day\nto reset system packed data counter.");
	infolabel_->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	infolabel_->SetTextConfig(28, LABEL_TEXT_STYLE_NORMAL);
	this->AddControl(*infolabel_);

	return r;
}

result
FSettings::OnTerminating(void)
{
	result r = E_SUCCESS;

	buttons_->RemoveAll(true);
	delete buttons_;

	return r;
}

result
FSettings::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		//title
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,50), Rectangle(0,800,480,50));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(196,10,89,22), Rectangle(880,184,89,22));
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
FSettings::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ASAVE) {
		int tmpval;
		int setlimit = 0;
		int setday = 1;
		Osp::Ui::Controls::EditField * tmpef;
		tmpef = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"limitel", true));
		if (Osp::Base::Integer::Parse(tmpef->GetText(), setlimit) != E_SUCCESS) {
			setlimit = 0;
		}
		if (Application::GetInstance()->GetAppRegistry()->Get(L"limit", tmpval) == E_KEY_NOT_FOUND) {
			Application::GetInstance()->GetAppRegistry()->Add(L"limit", setlimit);
		} else {
			Application::GetInstance()->GetAppRegistry()->Set(L"limit", setlimit);
		}
		tmpef = static_cast<Osp::Ui::Controls::EditField *>(this->GetControl(L"dayel", true));
		if (Osp::Base::Integer::Parse(tmpef->GetText(), setday) != E_SUCCESS) {
			setday = 1;
		}
		if (Application::GetInstance()->GetAppRegistry()->Get(L"day", tmpval) == E_KEY_NOT_FOUND) {
			Application::GetInstance()->GetAppRegistry()->Add(L"day", setday);
		} else {
			Application::GetInstance()->GetAppRegistry()->Set(L"day", setday);
		}
		Application::GetInstance()->GetAppRegistry()->Save();
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->TransitionDirection = pFormMgr->TRANSITIONRIGHT;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ACANCEL) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->TransitionDirection = pFormMgr->TRANSITIONLEFT;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}

	DateTime today, nextrundate;

	Osp::System::SystemTime::GetCurrentTime(Osp::System::WALL_TIME, today);
	int dayinm = 1;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"day", dayinm) != E_SUCCESS) {
		dayinm = 1;
	}
	nextrundate.SetValue(today.GetYear(),today.GetMonth(),dayinm,0,0,0);
	if (today.GetDay() >= dayinm) {
		nextrundate.AddMonths(1);
	}

	AppManager * appmgr = Osp::App::AppManager::GetInstance();
	if (appmgr->IsAppLaunchRegistered() == true) {
		appmgr->UnregisterAppLaunch();
	}
	appmgr->RegisterAppLaunch(L"DateTime='" + nextrundate.ToString() + "'", null, AppManager::LAUNCH_OPTION_DEFAULT);

	AppLog("set next run %S", nextrundate.ToString().GetPointer());

}

void FSettings::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FSettings::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FSettings::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FSettings::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FSettings::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
void FSettings::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FSettings::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
