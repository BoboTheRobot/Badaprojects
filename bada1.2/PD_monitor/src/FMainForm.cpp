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
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Base::Collection;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Net;
using namespace Osp::App;
using namespace Osp::Graphics;

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
	this->SetName(L"FMainForm");
	this->SetOrientation(ORIENTATION_PORTRAIT);
	return true;
}

result
FMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	buttons_ = new Osp::Base::Collection::ArrayList;

	greenpointer = drawingclass_->FDrawSpriteToBmpN(Rectangle(880,486,53,158));
	redpointer = drawingclass_->FDrawSpriteToBmpN(Rectangle(943,486,53,158));

	ButtonCostumEl * aboutbtn = new ButtonCostumEl(Rectangle(20,688,141,92), AABOUT, 1, BOXBTN, L"About");
	buttons_->Add(*aboutbtn);
	ButtonCostumEl * settingsbtn = new ButtonCostumEl(Rectangle(319,688,141,92), ASETTINGS, 0, BOXBTN, L"Settings");
	buttons_->Add(*settingsbtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	NetStatistics * netstat = new NetStatistics();
	netstat->Construct();

	long long pd_download = netstat->GetNetStatisticsInfo(NET_BEARER_PS, NET_STAT_TOTAL_RCVD_SIZE);
	long long pd_upload = netstat->GetNetStatisticsInfo(NET_BEARER_PS, NET_STAT_TOTAL_SENT_SIZE);

	delete netstat;

	download = ((double)pd_download / 1048576.0f);
	upload = ((double)pd_upload / 1048576.0f);
	limit = 0.0f;
	int setlimit = 0;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"limit", setlimit) == E_SUCCESS) {
		limit = (double)setlimit;
	}

	if (limit > 0) {
		overlimit = limit * 0.3f;
	} else {
		overlimit = 10.0f;
	}
	remaining = limit - (download+upload);

	pointerdegree = 142;
	if (((remaining) >= 0) && ((limit) > 0)) {
		pointerdegree = (142+39) * (((remaining)/(limit))) - 39;
		if (pointerdegree > 142) pointerdegree = 142;
		if (pointerdegree < -39) pointerdegree = -39;
	} else {
		pointerdegree = -(142-39) * (((-(remaining))/(overlimit))) - 39;
		if (pointerdegree > -39) pointerdegree = -39;
		if (pointerdegree < -142) pointerdegree = -142;
		if ((download+upload) <= 0) {
			pointerdegree = 142;
		}
	}

	anistep = 0;
	anistepc = 0;
	displaybuffer_ = new Canvas();
	displaybuffer_->Construct(Rectangle(0,0,480,800));
	CreateDisplayBuffer();

	anitimer_ = new Osp::Base::Runtime::Timer;
	anitimer_->Construct(*this);

	/*Osp::Base::Collection::IList * startarglist = new Osp::Base::Collection::ArrayList();
	Application * thisapp = Application::GetInstance();
	startarglist = thisapp->GetAppArgumentListN();
	String * arg1 = static_cast<String*>(startarglist->GetAt(1));
	//osp.appcontrol.CONDMGR
	if (arg1->Equals(L"osp.appcontrol.CONDMGR", false)) {
	AppLog("autostart");
	}
	startarglist->RemoveAll(true);
	delete startarglist;

	AppManager * appmgr = Osp::App::AppManager::GetInstance();
	appmgr->RegisterAppLaunch(L"DateTime='05/20/2011 17:35:00'", null, AppManager::LAUNCH_OPTION_DEFAULT);*/

	return r;
}

result
FMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	anitimer_->Cancel();
	delete anitimer_;

	buttons_->RemoveAll(true);
	delete buttons_;
	
	delete greenpointer;
	delete redpointer;

	delete displaybuffer_;

	return r;
}

void FMainForm::CreateDisplayBuffer() {
	//displaybuffer_
	//bg
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
	//gauge
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,20,400,400), Rectangle(480,0,400,400));
	//data
	int databoxtop = 438;
	displaybuffer_->FillRoundRectangle(Color(0,0,0,100), Rectangle(20,databoxtop,440,230), Dimension(20,20));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25,134,27), Rectangle(880,294,134,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25+38,92,27), Rectangle(880,459,92,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25+76,73,27), Rectangle(880,429,73,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25+114,67,27), Rectangle(880,321,67,27));
	if (remaining >= 0) {
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25+152,134,27), Rectangle(880,375,134,27));
	} else {
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(40,databoxtop+25+152,138,27), Rectangle(880,402,138,27));
	}

	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25,38,27), Rectangle(880,348,38,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25+38,38,27), Rectangle(880,348,38,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25+76,38,27), Rectangle(880,348,38,27));
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25+114,38,27), Rectangle(880,348,38,27));
	if (remaining >= 0) {
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25+152,38,27), Rectangle(956,348,38,27));
	} else {
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(382,databoxtop+25+152,38,27), Rectangle(918,348,38,27));
	}

}

result
FMainForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {

		pCanvas_->Copy(Point(0,0),*displaybuffer_,Rectangle(0,0,480,668));

		//gauge pointer
		double degree = 142-(anistepc*(142-pointerdegree));

		if (((remaining) >= 0) && ((limit) > 0)) {
			pCanvas_->DrawBitmap(Point(240,220), *greenpointer, Point(26,152), degree);
		} else {
			pCanvas_->DrawBitmap(Point(240,220), *redpointer, Point(26,152), degree);
		}

		//gauge overlay
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(40,20,400,400), Rectangle(480,400,400,400));

		//data
		int databoxtop = 438;
		//numbers
		String numf;
		numf = L"";
		numf.Format(25,L"%0.2f",(anistepc*download));
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25,187,27), numf, 1);
		numf = L"";
		numf.Format(25,L"%0.2f",(anistepc*upload));
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25+38,187,27), numf, 1);
		numf = L"";
		numf.Format(25,L"%0.2f",(anistepc*(download+upload)));
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25+76,187,27), numf, 1);
		numf = L"";
		numf.Format(25,L"%0.2f",(anistepc*limit));
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25+114,187,27), numf, 1);
		numf = L"";
		if (remaining >= 0) {
			numf.Format(25,L"%0.2f",(anistepc*remaining));
			drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25+152,187,27), numf, 2);
		} else {
			numf.Format(25,L"%0.2f",-(anistepc*remaining));
			drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(180,databoxtop+25+152,187,27), numf, 3);
		}

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

void FMainForm::StartAnim() {
	Osp::System::SystemTime::GetTicks(anilasttime);
	anitimer_->Start(20);
}

void FMainForm::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	Osp::System::SystemTime::GetTicks(anicurtime);
	int timepassed = anicurtime - anilasttime;
	double anistepproc = (double)timepassed / 2000.0f;
	anilasttime = anicurtime;
	anistep += anistepproc;
	if (anistep > 1) anistep = 1;
	double anipowf = 1.5f;

	anistepc = (1 - Osp::Base::Utility::Math::Pow(1 - anistep, anipowf*2)); //easeout
	//anistepc = Osp::Base::Utility::Math::Pow(anistep, anipowf*2); //easein

	//anistepc = ((-Osp::Base::Utility::Math::Cos(anistep*Osp::Base::Utility::Math::GetPi())/2.0f) + 0.5f); //easeinout

	RequestRedraw();
	if (anistep == 1) {
		//stop anim
	} else {
		anitimer_->Start(20);
	}
}

void
FMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ASETTINGS) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->TransitionDirection = pFormMgr->TRANSITIONRIGHT;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_SETTINGS, null);
	} else if (actionId == AABOUT) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->TransitionDirection = pFormMgr->TRANSITIONLEFT;
		pFormMgr->SendUserEvent(FormMgr::REQUEST_ABOUT, null);
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

void FMainForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs) {
	if (requestId == FormMgr::CALLBACKDONE) {
		this->StartAnim();
	}
}
