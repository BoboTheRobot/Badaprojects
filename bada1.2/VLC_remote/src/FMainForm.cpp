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
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Base::Utility;

FMainForm::FMainForm(void)
{
}

FMainForm::~FMainForm(void)
{
}

bool
FMainForm::Initialize()
{

	this->Construct(FORM_STYLE_NORMAL|FORM_STYLE_INDICATOR);
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
	publicfunc_ = pFormMgr->publicfunc_;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * playbtn = new ButtonCostumEl(Rectangle(12,13+38,233,233), APLAY, 0, BIGROUNDBTN, APAUSE, 0, L"Play/Pause");
	playbtn->toogle = false;
	buttons_->Add(*playbtn);
	ButtonCostumEl * prevbtn = new ButtonCostumEl(Rectangle(245,21+38,103,103), APREV, 0, SMALLROUNDSKYBTN, L"Previous");
	buttons_->Add(*prevbtn);
	ButtonCostumEl * nextbtn = new ButtonCostumEl(Rectangle(245,134+38,103,103), ANEXT, 0, SMALLROUNDSKYBTN, L"Next");
	buttons_->Add(*nextbtn);
	ButtonCostumEl * stopbtn = new ButtonCostumEl(Rectangle(356,77+38,103,103), ASTOP, 0, SMALLROUNDREDBTN, L"Stop");
	buttons_->Add(*stopbtn);
	ButtonCostumEl * playlistbtn = new ButtonCostumEl(Rectangle(12,667,128,124), APLAYLIST, 6, BOXBTN, L"Playlist");
	buttons_->Add(*playlistbtn);
	ButtonCostumEl * browsebtn = new ButtonCostumEl(Rectangle(140,667,128,124), ABROWSE, 0, BOXBTN, L"Browse");
	buttons_->Add(*browsebtn);
	ButtonCostumEl * morebtn = new ButtonCostumEl(Rectangle(268,667,128,124), AMORE, 5, BOXBTN, ALESS, 2, L"Show/Hide more");
	morebtn->toogle = false;
	buttons_->Add(*morebtn);

	ButtonCostumEl * streamoffbtn = new ButtonCostumEl(Rectangle(268,667,128,124), ASTREAMOFF, 7, BOXBTN, L"Streaming off");
	streamoffbtn->toogle = true;
	streamoffbtn->icoindextoogle = streamoffbtn->icoindex;
	buttons_->Add(*streamoffbtn);

	ButtonCostumEl * fullscreenbtn = new ButtonCostumEl(Rectangle(268,539,128,124), AFULLSCREEN, 3, BOXBTN, MOREMENI, L"Fullscreen on/off");
	buttons_->Add(*fullscreenbtn);
	/*ButtonCostumEl * streambtn = new ButtonCostumEl(Rectangle(268,411,128,124), ASTREAMON, 7, BOXBTN, MOREMENI, L"Streaming");
	buttons_->Add(*streambtn);*/
	ButtonCostumEl * clientsbtn = new ButtonCostumEl(Rectangle(140,539,128,124), ACLIENTS, 1, BOXBTN, MOREMENI, L"Clients");
	buttons_->Add(*clientsbtn);
	ButtonCostumEl * helpbtn = new ButtonCostumEl(Rectangle(140,411,128,124), AHELP, 4, BOXBTN, MOREMENI, L"Help");
	buttons_->Add(*helpbtn);

	moremenipos = Rectangle(138,403,260,268);
	moremeniopened = false;

	currpressedbtn = -1;
	currselslider = -1;
	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);

	displaybuffer_ = new Canvas();
	displaybuffer_->Construct(Rectangle(0,0,480,800));
	RedrawDisplayBuffer(displaybuffer_->GetBounds());

    playliststream = new Osp::Base::Collection::ArrayList;
    playliststream->Construct();

	if (publicfunc_->iswificonnected) {
		publicfunc_->StartListening();
	}

	streamplayerpreviewoverlay = null;
	streamplayerbufferoverlay = null;
	streamplayervolumeoverlay = null;
	streamplayertitleoverlay = null;
	streamplayer = null;

	StartToPlayTimer_ = new Osp::Base::Runtime::Timer();
	StartToPlayTimer_->Construct(*this);

	FullScreenStreamPlayerHideControls_ = new Osp::Base::Runtime::Timer();
	FullScreenStreamPlayerHideControls_->Construct(*this);

	isfullscreenplayer = false;
	isstreaming = false;
	streamingclearplaylistwait = false;
	streamingplaywait = false;
	streamplayervol = 20;

	isonbackground = false;

	return r;
}

void FMainForm::RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect) {
	if (isfullscreenplayer == true) {
		return;
	}
	displaybufferisindrawingstate = true;
	Osp::Graphics::Rectangle thisrect;
	if (moremeniopened) {
		redrawrect.x = 0;
		redrawrect.y = 0;
		redrawrect.width = displaybuffer_->GetBounds().width;
		redrawrect.height = displaybuffer_->GetBounds().height;
	}
	//bg
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, redrawrect, redrawrect);
	//icon
	thisrect = Rectangle(419,668,61,124);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, Rectangle(0,800,thisrect.width,thisrect.height));
	}
	//main top buttons background
	thisrect = Rectangle(10,10+38,460,240);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, thisrect);
		displaybuffer_->FillRoundRectangle(Color(0,0,0,128),thisrect,Dimension(120,120));
		displaybuffer_->FillRoundRectangle(Color(0,0,0,128),Rectangle(25,25+38,430,210),Dimension(105,105));
	}
	//seek slider
	thisrect = Rectangle(10,275+38-27,460,70+27);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, thisrect);
		displaybuffer_->FillRoundRectangle(Color(0,0,0,128),Rectangle(10,275+38,460,70),Dimension(20,20));
		displaybuffer_->FillRoundRectangle(Color(100,100,100,128),Rectangle(25,275+38+27,430,16),Dimension(8,8));
		if (publicfunc_->vlccurstatus.medialength > 0) {
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(393,275+38-27,62,40), Rectangle(62,800,62,40));
		}
		int seeksliderxpos = 25;
		if (currselslider == 1) {
			seeksliderxpos = seeksliderxpos + (int)(cursliderseekvalue * 388.0f);
		} else {
			seeksliderxpos = seeksliderxpos + (int)(publicfunc_->vlccurstatus.seekproc * 388.0f);
		}
		if (publicfunc_->vlccurstatus.medialength > 0) {
			displaybuffer_->FillRoundRectangle(Color(128,128,128),Rectangle(25,275+38+27,seeksliderxpos+10,16),Dimension(8,8));
			displaybuffer_->FillEllipse(Color(128,128,128), Rectangle(seeksliderxpos,275+38+14,42,42));
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(seeksliderxpos+12,275+38+14+5,16,30), Rectangle(78,880,16,30));
		} else {
			if (publicfunc_->vlccurstatus.mediacurtime > 0) {
				displaybuffer_->FillRoundRectangle(Color(128,128,128),Rectangle(25,275+38+27,430,16),Dimension(8,8));
			}
		}
	}
	//volume slider
	if (isstreaming == false) {
	thisrect = Rectangle(10,558,460,70-27+40);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, thisrect);
		displaybuffer_->FillRoundRectangle(Color(0,0,0,128),Rectangle(10,558,460,70),Dimension(20,20));
		displaybuffer_->FillRoundRectangle(Color(100,100,100,128),Rectangle(25,558+27,430,16),Dimension(8,8));
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(362,558+70-27,93,40), Rectangle(62,840,93,40));
		int volsliderxpos = 25;
		if (currselslider == 2) {
			volsliderxpos = volsliderxpos + (int)(curslidervolumevalue * 388.0f);
		} else {
			volsliderxpos = volsliderxpos + (int)(publicfunc_->vlccurstatus.volumeproc * 388.0f);
		}
		displaybuffer_->FillRoundRectangle(Color(128,128,128),Rectangle(25,558+27,volsliderxpos+10,16),Dimension(8,8));
		displaybuffer_->FillEllipse(Color(128,128,128), Rectangle(volsliderxpos,558+14,42,42));
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(volsliderxpos+10,558+14+5,16,30), Rectangle(62,880,16,30));
	}
	}
	//time
	thisrect = Rectangle(25,345+38,455,30);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		if (publicfunc_->vlccurstatus.mediaremainingtimestr != L"") {
		drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(25,345+38,100,30), publicfunc_->vlccurstatus.mediaremainingtimestr, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
		if (publicfunc_->vlccurstatus.mediatimestr != L"") {
		drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(125,345+38,330,30), publicfunc_->vlccurstatus.mediatimestr, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_RIGHT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
	}
	//streaming player frame
	if (isstreaming == true) {
	thisrect = Rectangle(10,384+30,460,277-30);
	if (redrawrect.IsIntersected(thisrect)) {
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, thisrect);
		redrawrect = redrawrect.GetUnion(thisrect);
		displaybuffer_->FillRoundRectangle(Color(0,0,0,128),Rectangle(10,384+30,460,277-30),Dimension(20,20));
		displaybuffer_->FillRectangle(Color(0,0,0), Rectangle(31,405+30,418,235-30));
	}
	}
	//title
	if (isstreaming == false) {
	thisrect = Rectangle(25,380+38,430,130-38);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		if (publicfunc_->vlccurstatus.mediaoutputtitlestr != L"") {
		drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(25,380+38,430,130-38), publicfunc_->vlccurstatus.mediaoutputtitlestr, 24, Osp::Graphics::FONT_STYLE_BOLD, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_TOP);
		}
	}
	}
	//buttons
	if (moremeniopened) {
	thisrect = Rectangle(266,657,132,136);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		displaybuffer_->FillRoundRectangle(Color(80,80,80),thisrect,Dimension(7,7));
	}
	thisrect = moremenipos;
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		displaybuffer_->FillRoundRectangle(Color(80,80,80),moremenipos,Dimension(7,7));
	}
	}
	ButtonCostumEl* button_;
	bool isvisible;
	Rectangle btnicorect;
	int toppos;
	Point btnicoshift;
	for (int n=0; n<buttons_->GetCount(); n++) {
		isvisible = true;
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		if (button_->group == MOREMENI) {
			isvisible = moremeniopened;
		} else {
			if (button_->action == ASTREAMOFF) {
				isvisible = isstreaming;
			} else if (button_->action == AMORE) {
				isvisible = !isstreaming;
			}
		}
		if (isvisible) {
		thisrect = Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			if (button_->type == BIGROUNDBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(584,234+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(584,234,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(584,700,100,110);
					btnicoshift.SetPosition(0,0);
					if ((button_->action == APLAY) && (button_->actiontoogle == APAUSE)) {
						if (button_->toogle) {
							btnicorect.x = 684;
						} else {
							btnicorect.x = 584;
							btnicoshift.x = 10;
						}
					}
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+66+btnicoshift.x,button_->position.y+61+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(584,0,button_->position.width,button_->position.height));
				}
			}
			if ((button_->type == SMALLROUNDREDBTN) || (button_->type == SMALLROUNDSKYBTN)) {
				if (isvisible) {
					toppos = 310;
					if (button_->type == SMALLROUNDREDBTN) {
						toppos = 104;
					}
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(480,toppos+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(480,toppos,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(480,516,40,40);
					btnicoshift.SetPosition(0,0);
					if ((button_->action == APREV) || (button_->action == ANEXT)) {
						btnicorect.y = 556;
						if (button_->action == ANEXT) {
							btnicorect.x = 520;
						}
					}
					if (button_->action == ASTOP) {
						btnicoshift.x = 2;
						btnicoshift.y = 1;
					}
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+31+btnicoshift.x,button_->position.y+31+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(480,0,button_->position.width,button_->position.height));
				}
			}
			if (button_->type == BOXBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(818,256,90,44);
					btnicoshift.SetPosition(0,0);
					if ((button_->icoindex == 5) && (!button_->toogle)) {
						btnicoshift.y = 5;
					} else if (button_->icoindex == 11) {
						btnicoshift.y = 6;
					}
					if (button_->toogle) {
						btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
					} else {
						btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
					}
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+19+btnicoshift.x,button_->position.y+38+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
		}
		}
	}
	canvasredrawrect_ = redrawrect;
	displaybufferisindrawingstate = false;
}

void FMainForm::RefreshDataUpdate() {
	bool needsredraw = false;
	Osp::Graphics::Rectangle redrawrect(0,0,-1,-1);
	Osp::Graphics::Rectangle thisrect;
	int buttonindexfound = -1;

	if ((isstreaming == true) && (streamplayer != null) && (publicfunc_->VLCAPIVER >= 2)) {
		//AppLog("playerpos %d",(int)streamplayer->GetPosition());
	} else {

	if (publicfunc_->statusdataischanged) {

		ButtonCostumEl* button_;
		if (publicfunc_->vlccurstatus.state != publicfunc_->vlcoldstatus.state) {
			buttonindexfound = -1;
			for (int n=0; n<buttons_->GetCount(); n++) {
				button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
				if (button_->action == APLAY) {
					buttonindexfound = n;
					break;
				}
			}
			if (buttonindexfound > -1) {
				if ((publicfunc_->vlccurstatus.state == L"playing") != button_->toogle) {
					button_->toogle = (publicfunc_->vlccurstatus.state == L"playing");
					buttons_->SetAt(*button_,buttonindexfound,false);
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(button_->position);
					} else {
						redrawrect = button_->position;
					}
					needsredraw = true;
				}
			}
		}
		if (publicfunc_->vlcoldstatus.isfullscreen != publicfunc_->vlccurstatus.isfullscreen) {
			buttonindexfound = -1;
			for (int n=0; n<buttons_->GetCount(); n++) {
				button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
				if (button_->action == AFULLSCREEN) {
					buttonindexfound = n;
					break;
				}
			}
			if (buttonindexfound > -1) {
				if (button_->toogle != publicfunc_->vlccurstatus.isfullscreen) {
					button_->toogle = publicfunc_->vlccurstatus.isfullscreen;
					buttons_->SetAt(*button_,buttonindexfound,false);
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(button_->position);
					} else {
						redrawrect = button_->position;
					}
					needsredraw = true;
				}
			}
		}
		if (publicfunc_->vlcoldstatus.volume != publicfunc_->vlccurstatus.volume) {
			thisrect = Rectangle(10,558,460,70-27+40);
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(thisrect);
			} else {
				redrawrect = thisrect;
			}
			needsredraw = true;
		}
		if ((publicfunc_->vlcoldstatus.mediacurtime != publicfunc_->vlccurstatus.mediacurtime) || (publicfunc_->vlcoldstatus.medialength != publicfunc_->vlccurstatus.medialength)) {
			thisrect = Rectangle(25,345+38,455,30);
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(thisrect);
			} else {
				redrawrect = thisrect;
			}
			needsredraw = true;
		}

		if (publicfunc_->vlcoldstatus.mediaoutputtitlestr != publicfunc_->vlccurstatus.mediaoutputtitlestr) {
			thisrect = Rectangle(25,380+38,430,130-38);
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(thisrect);
			} else {
				redrawrect = thisrect;
			}
			needsredraw = true;
			if ((isstreaming) && (streamplayerpreviewoverlay != null)) {
				Osp::Base::String overlaytitlestr = publicfunc_->vlccurstatus.mediaoutputtitlestr;
				if (overlaytitlestr != L"") overlaytitlestr.Replace(L"\n",L"-");
				streamplayertitleoverlay->SetText(overlaytitlestr);
				if (this->IsVisible()) {
				streamplayertitleoverlay->Draw();
				streamplayertitleoverlay->Show();
				}
			}
		}

		if (publicfunc_->vlcoldstatus.seekproc != publicfunc_->vlccurstatus.seekproc) {
			thisrect = Rectangle(10,275+38-27,460,70+27);
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(thisrect);
			} else {
				redrawrect = thisrect;
			}
			needsredraw = true;
		}
		if (needsredraw) {
			if (moremeniopened == false) {
				RedrawDisplayBuffer(redrawrect);
				if (this->IsVisible()) {
				if (canvasredrawrect_.y < 100) {
					RequestRedraw();
				} else {
					if (isfullscreenplayer == false) {
						Canvas* formCanvas;
						formCanvas = GetCanvasN();
						formCanvas->Copy(Point(canvasredrawrect_.x, canvasredrawrect_.y), *displaybuffer_, canvasredrawrect_);
						formCanvas->Show(canvasredrawrect_);
						delete formCanvas;
					}
				}
				}
			}
		}
	}
	}

	publicfunc_->RefreshDataUpdateRecived();
}

result
FMainForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		pCanvas_->Copy(Point(0,0),*displaybuffer_,Rectangle(0,0,480,800));
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	Osp::System::PowerManager::KeepScreenOnState(false, true);

	StartToPlayTimer_->Cancel();
	delete StartToPlayTimer_;

	buttons_->RemoveAll(true);
	delete buttons_;

	delete displaybuffer_;

	publicfunc_->StopListening();

	playliststream->RemoveAll(true);
	delete playliststream;

	if (streamplayer != null) {
		streamplayer->Stop();
	}
	delete streamplayer;

	return r;
}

void
FMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if ((actionId == AMORE) || (actionId == ALESS)) {
		moremeniopened = (actionId == AMORE);
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == APLAY) {
		AppLog("play");
		if (publicfunc_->vlccurstatus.state == L"paused") {
			if ((isstreaming == true) && (streamplayer != null)) {
				streamingplaywait = true;
			}
			publicfunc_->SendCommand("pl_pause");
		} else {
			if ((isstreaming == true) && (streamplayer != null)) {
				streamingplaywait = true;
			}
			publicfunc_->SendCommand("pl_play");
		}
	} else if (actionId == APLAYSTREAMFROMBG) {
		publicfunc_->StopListening();
		publicfunc_->SendCommand("pl_pause");
		streamplayerbufferoverlay->SetText(L"Preparing media for streaming");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
		streamingplaywait = true;
		publicfunc_->StartListening();
	} else if (actionId == APAUSE) {
		if ((isstreaming == true) && (streamplayer != null)) {
			streamingpausewait = true;
		}
		publicfunc_->SendCommand("pl_pause");
	} else if (actionId == APREV) {
		if (isstreaming == true) {
			if ((streamingcurrentitemindex > 0) && (playliststream->GetCount() > 1)) {
				if (streamplayer != null) {
					streamplayer->Stop();
				}
				streamingcurrentitemindex--;
				publicfunc_->StopListening();
				StreamPlayItemIndex(streamingcurrentitemindex);
			}
		} else {
			publicfunc_->SendCommand("pl_previous");
		}
	} else if (actionId == ANEXT) {
		if (isstreaming == true) {
			if ((streamingcurrentitemindex < (playliststream->GetCount()-2)) && (playliststream->GetCount() > 1)) {
				if (streamplayer != null) {
					streamplayer->Stop();
				}
				streamingcurrentitemindex++;
				publicfunc_->StopListening();
				StreamPlayItemIndex(streamingcurrentitemindex);
			}
		} else {
			publicfunc_->SendCommand("pl_next");
		}
	} else if (actionId == ASTOP) {
		publicfunc_->SendCommand("pl_stop");
		if ((isstreaming == true) && (streamplayer != null)) {
			streamplayer->Stop();
			if (publicfunc_->VLCAPIVER >= 2) {
				publicfunc_->SendCommandVLM(L"control%20VLCremote%20stop");
			}
		}
	} else if (actionId == AFULLSCREEN) {
		ButtonCostumEl* button_;
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->action == AMORE) {
				if (button_->toogle) {
					button_->toogle = false;
					buttons_->SetAt(*button_,n,false);
					moremeniopened = false;
					RedrawDisplayBuffer(displaybuffer_->GetBounds());
					RequestRedraw();
				}
				break;
			}
		}
		publicfunc_->SendCommand("fullscreen");
	} else if (actionId == APLAYLIST) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_PLAYLISTFORM, null);
	} else if (actionId == ABROWSE) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_BROWSEFORM, null);
	} else if (actionId == ACLIENTS) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CLIENTSFORM, null);
	} else if (actionId == AHELP) {
		ButtonCostumEl* button_;
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->action == AMORE) {
				if (button_->toogle) {
					button_->toogle = false;
					buttons_->SetAt(*button_,n,false);
					moremeniopened = false;
					RedrawDisplayBuffer(displaybuffer_->GetBounds());
					RequestRedraw();
				}
				break;
			}
		}
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_HELPFORM, null);
	} else if (actionId == ASTREAM) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_VIDEOPLAYER, null);
	} else if (actionId == ASTREAMON) {
		ButtonCostumEl* button_;
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->action == AMORE) {
				if (button_->toogle) {
					button_->toogle = false;
					buttons_->SetAt(*button_,n,false);
					moremeniopened = false;
				}
				break;
			}
		}
		isstreaming = true;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		streamplayerpreviewoverlay = new OverlayPanel();
		streamplayerpreviewoverlay->Construct(Rectangle(31,405-38+30,418,235-30)); //240,135 : 240,180
		streamplayerpreviewoverlay->SetRendererAspectRatio(true);
		streamplayerpreviewoverlay->AddTouchEventListener(*this);
		streamplayerbufferoverlay = new Label();
		streamplayerbufferoverlay->Construct(Rectangle(120,0,290,25),L"");
		streamplayerbufferoverlay->SetTextConfig(22,LABEL_TEXT_STYLE_NORMAL);
		streamplayerbufferoverlay->SetTextHorizontalAlignment(ALIGNMENT_RIGHT);
		streamplayerbufferoverlay->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		streamplayerpreviewoverlay->AddControl(*streamplayerbufferoverlay);
		streamplayervolumeoverlay = new Label();
		streamplayervolumeoverlay->Construct(Rectangle(0,0,120,25),L"Vol: " + Osp::Base::Integer::ToString(streamplayervol) + L"%");
		streamplayervolumeoverlay->SetBackgroundColor(Color(0,0,0,125));
		if (streamplayervol == 0) {
			streamplayervolumeoverlay->SetText(L"Vol: Mute");
		} else if (streamplayervol == 100) {
			streamplayervolumeoverlay->SetText(L"Vol: Max");
		}
		streamplayervolumeoverlay->SetTextConfig(22,LABEL_TEXT_STYLE_NORMAL);
		streamplayervolumeoverlay->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
		streamplayervolumeoverlay->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		streamplayerpreviewoverlay->AddControl(*streamplayervolumeoverlay);
		streamplayertitleoverlay = new Label();
		Osp::Base::String overlaytitlestr = publicfunc_->vlccurstatus.mediaoutputtitlestr;
		if (overlaytitlestr != L"") overlaytitlestr.Replace(L"\n",L"-");
		streamplayertitleoverlay->Construct(Rectangle(0,210-30,418,25),overlaytitlestr);
		streamplayertitleoverlay->SetBackgroundColor(Color(0,0,0,125));
		streamplayertitleoverlay->SetTextConfig(22,LABEL_TEXT_STYLE_NORMAL);
		streamplayertitleoverlay->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
		streamplayertitleoverlay->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		streamplayerpreviewoverlay->AddControl(*streamplayertitleoverlay);
		this->AddControl(*streamplayerpreviewoverlay);
		this->RequestRedraw();
		Osp::Graphics::BufferInfo bufferInfo;
		Osp::Graphics::Dimension renderdim = Osp::Graphics::Dimension(streamplayerpreviewoverlay->GetWidth(),streamplayerpreviewoverlay->GetHeight());
		streamplayerpreviewoverlay->SetRendererSize(renderdim);
		streamplayerpreviewoverlay->SetRendererRotation(Osp::Ui::Controls::OverlayPanel::ROTATION_NONE);
		streamplayerpreviewoverlay->GetBackgroundBufferInfo(bufferInfo);
		delete streamplayer;
		streamplayer = null;
		streamplayer = new Player();
		streamplayer->Construct(*this, &bufferInfo);
		streamplayer->SetVolume(streamplayervol);
		Osp::System::PowerManager::KeepScreenOnState(true, false);
		publicfunc_->StopListening();
		playliststream->RemoveAll(true);
		publicfunc_->LoadPlaylist();
	} else if (actionId == ASTREAMOFF) {
		Osp::System::PowerManager::KeepScreenOnState(false, true);
		isstreaming = false;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		if (streamplayer != null) {
			streamplayer->Stop();
			streamplayer->Close();
			delete streamplayer;
			streamplayer = null;
		}
		this->RemoveControl(*streamplayerpreviewoverlay);
		streamplayerpreviewoverlay = null;
		this->RequestRedraw();
		if (publicfunc_->VLCAPIVER >= 2) {
			publicfunc_->SendCommandVLM(L"control%20VLCremote%20stop");
		} else {
			publicfunc_->SendCommand("pl_empty");
		}
	} else if (actionId == ASTREAMFULLSCREEN) {
		if (streamingcurmediaaudio == false) {
			if (isfullscreenplayer == false) {
				isfullscreenplayer = true;
				this->SetOrientation(ORIENTATION_LANDSCAPE);
				this->SetFormStyle(FORM_STYLE_NORMAL);
				streamplayervolumeoverlay->SetShowState(true);
				streamplayertitleoverlay->SetShowState(true);
				streamplayerpreviewoverlay->RequestRedraw();
				streamplayerpreviewoverlay->SetBounds(Rectangle(0,0,800,480));
				Osp::Graphics::Dimension renderdim = Osp::Graphics::Dimension(streamplayerpreviewoverlay->GetWidth(),streamplayerpreviewoverlay->GetHeight());
				streamplayerpreviewoverlay->SetRendererSize(renderdim);
				streamplayerbufferoverlay->SetBounds(Rectangle(382,0,410,25));
				streamplayervolumeoverlay->SetBounds(Rectangle(0,0,120,25));
				streamplayertitleoverlay->SetBounds(Rectangle(0,455,800,25));
				FullScreenStreamPlayerHideControls_->Cancel();
				FullScreenStreamPlayerHideControls_->Start(3000);
				this->RequestRedraw();
			} else {
				FullScreenStreamPlayerHideControls_->Cancel();
				streamplayervolumeoverlay->SetShowState(true);
				streamplayertitleoverlay->SetShowState(true);
				streamplayerpreviewoverlay->RequestRedraw();
				isfullscreenplayer = false;
				RedrawDisplayBuffer(displaybuffer_->GetBounds());
				this->SetOrientation(ORIENTATION_PORTRAIT);
				this->SetFormStyle(FORM_STYLE_NORMAL|FORM_STYLE_INDICATOR);
				streamplayerpreviewoverlay->SetBounds(Rectangle(31,405-38+30,418,235-30));
				Osp::Graphics::Dimension renderdim = Osp::Graphics::Dimension(streamplayerpreviewoverlay->GetWidth(),streamplayerpreviewoverlay->GetHeight());
				streamplayerpreviewoverlay->SetRendererSize(renderdim);
				streamplayerbufferoverlay->SetBounds(Rectangle(120,0,290,25));
				streamplayervolumeoverlay->SetBounds(Rectangle(0,0,120,25));
				streamplayertitleoverlay->SetBounds(Rectangle(0,210-30,418,25));
				this->RequestRedraw();
			}
		}
	}
}

void FMainForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if (streamplayerpreviewoverlay != null) {
		if (streamplayerpreviewoverlay->Equals(source)) {
			this->OnActionPerformed(source, ASTREAMFULLSCREEN);
		}
	}
}
void FMainForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FMainForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if (streamplayerpreviewoverlay != null) {
		if (streamplayerpreviewoverlay->Equals(source)) {
			return;
		}
		if ((isfullscreenplayer) && (isstreaming)) {
			return;
		}
	}
	if ((Osp::Base::Utility::Math::Abs(oldpresspos.x - currentPosition.x) < 15) && (Osp::Base::Utility::Math::Abs(oldpresspos.y - currentPosition.y) < 15)) {
		return;
	}
	bool needsredraw = false;
	Osp::Graphics::Rectangle redrawrect(0,0,-1,-1);
	if (currselslider == -1) {
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (!button_->pressposition.Contains(currentPosition)) {
			if (button_->pressed) {
				button_->pressed = false;
				buttons_->SetAt(*button_,currpressedbtn,false);
				currpressedbtn = -1;
				if (needsredraw == true) {
					redrawrect = redrawrect.GetUnion(button_->position);
				} else {
					redrawrect = button_->position;
				}
				needsredraw = true;
			}
		}
	}
	bool isvisible = true;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = true;
		if (button_->group == MOREMENI) {
			isvisible = moremeniopened;
		} else {
			if (button_->action != AMORE) {
				isvisible = !moremeniopened;
			}
			if (button_->action == ASTREAMOFF) {
				isvisible = isstreaming;
			} else if (button_->action == AMORE) {
				isvisible = !isstreaming;
			}
		}
		if ((isvisible) && (publicfunc_->sendingcommand == false) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				if (needsredraw == true) {
					redrawrect = redrawrect.GetUnion(button_->position);
				} else {
					redrawrect = button_->position;
				}
				needsredraw = true;
			}
			break;
		}
	}
	}
	if ((currpressedbtn == -1) && (!moremeniopened)) {
		Osp::Graphics::Rectangle sliderpos;
		sliderpos.SetBounds(10,275+38,460,70);
		if (publicfunc_->vlccurstatus.medialength > 0) {
		if (((currentPosition.y >= sliderpos.y) && (currentPosition.y <= (sliderpos.y+sliderpos.height)) && (currselslider == -1)) || (currselslider == 1)) {
			if (((currentPosition.x >= sliderpos.x+25) && (currentPosition.x <= sliderpos.x+25+388) && (currselslider == -1)) || (currselslider == 1)) {
				if (currselslider != 1) {
					cursliderseekvalue = publicfunc_->vlccurstatus.seekproc;
				}
				double sliderp = ((double)(currentPosition.x - 25 - (sliderpos.x+25)) / (double)(388));
				if (sliderp > 1) sliderp = 1;
				if (sliderp < 0) sliderp = 0;
				if (sliderp != cursliderseekvalue) {
					cursliderseekvalue = sliderp;
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(sliderpos);
					} else {
						redrawrect = sliderpos;
					}
					needsredraw = true;
				}
				currselslider = 1;
			}
		}
		}
		if (isstreaming == false) {
		sliderpos.SetBounds(10,558,460,70);
		if (((currentPosition.y >= sliderpos.y) && (currentPosition.y <= (sliderpos.y+sliderpos.height)) && (currselslider == -1)) || (currselslider == 2)) {
			if (((currentPosition.x >= sliderpos.x+25) && (currentPosition.x <= sliderpos.x+25+388) && (currselslider == -1)) || (currselslider == 2)) {
				if (currselslider != 2) {
					curslidervolumevalue = publicfunc_->vlccurstatus.volumeproc;
				}
				double sliderp = ((double)(currentPosition.x - 25 - (sliderpos.x+25)) / (double)(388));
				if (sliderp > 1) sliderp = 1;
				if (sliderp < 0) sliderp = 0;
				if (sliderp != curslidervolumevalue) {
					curslidervolumevalue = sliderp;
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(sliderpos);
					} else {
						redrawrect = sliderpos;
					}
					needsredraw = true;
				}
				currselslider = 2;
			}
		}
		}
	}
	oldpresspos = currentPosition;
	if (needsredraw) {
		if (displaybufferisindrawingstate == false) {
			RedrawDisplayBuffer(redrawrect); //redraw only changed region
			/*if (canvasredrawrect_.y < 100) {
				RequestRedraw();
			} else {*/
				Canvas* formCanvas;
				formCanvas = GetCanvasN();
				formCanvas->Copy(Point(canvasredrawrect_.x, canvasredrawrect_.y), *displaybuffer_, canvasredrawrect_);
				formCanvas->Show(canvasredrawrect_);
				delete formCanvas;
			/*}*/
		}
	}
}
void FMainForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if (streamplayerpreviewoverlay != null) {
		if (isfullscreenplayer) {
			streamplayervolumeoverlay->SetShowState(true);
			streamplayertitleoverlay->SetShowState(true);
			streamplayerpreviewoverlay->RequestRedraw();
			FullScreenStreamPlayerHideControls_->Cancel();
			FullScreenStreamPlayerHideControls_->Start(3000);
		}
		if (streamplayerpreviewoverlay->Equals(source)) {
			return;
		}
		if ((isfullscreenplayer) && (isstreaming)) {
			return;
		}
	}
	currpressedbtn = -1;
	currselslider = -1;
	curslidervolumevalue = 0;
	cursliderseekvalue = 0;
	oldpresspos.SetPosition(-1,-1);
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FMainForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if (streamplayerpreviewoverlay != null) {
		if (streamplayerpreviewoverlay->Equals(source)) {
			return;
		}
		if ((isfullscreenplayer) && (isstreaming)) {
			return;
		}
	}
	bool needsredraw = false;
	Osp::Graphics::Rectangle redrawrect(0,0,-1,-1);
	bool istoogle = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			istoogle = ((button_->action == APLAY) || (button_->action == AMORE));
			button_->pressed = false;
			if (istoogle) {
				button_->toogle = !button_->toogle;
			}
			buttons_->SetAt(*button_,currpressedbtn,false);
			if (istoogle) {
				if (!button_->toogle) {
					this->OnActionPerformed(source, button_->actiontoogle);
				} else {
					this->OnActionPerformed(source, button_->action);
				}
			} else {
				this->OnActionPerformed(source, button_->action);
			}
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(button_->position);
			} else {
				redrawrect = button_->position;
			}
			needsredraw = true;
		}
	}
	if ((!moremenipos.Contains(currentPosition)) && (moremeniopened)) {
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->action == AMORE) {
				if (!(button_->position.Contains(currentPosition))) {
					button_->toogle = !button_->toogle;
					buttons_->SetAt(*button_,n,false);
					OnActionPerformed(source, ALESS);
				}
				break;
			}
		}

	}
	if (currselslider == 1) {
		publicfunc_->vlccurstatus.seekproc = cursliderseekvalue;
		publicfunc_->RefreshDataUpdate();
		int seekproc = (int)Math::Round(cursliderseekvalue * 100.0f);
		if (seekproc < 0) seekproc = 0;
		if (seekproc > 100) seekproc = 100;
		publicfunc_->SendCommand(L"seek&val=" + Osp::Base::Integer::ToString(seekproc) + "%25");
		if (needsredraw == true) {
			redrawrect = redrawrect.GetUnion(Rectangle(10,275+38,460,70));
		} else {
			redrawrect = Rectangle(10,275+38,460,70);
		}
		needsredraw = true;
	}
	if (currselslider == 2) {
		publicfunc_->vlccurstatus.volumeproc = curslidervolumevalue;
		publicfunc_->vlccurstatus.volume = Math::Round(curslidervolumevalue * 512.0f);
		if (publicfunc_->vlccurstatus.volume > 512) publicfunc_->vlccurstatus.volume = 512;
		if (publicfunc_->vlccurstatus.volume < 0) publicfunc_->vlccurstatus.volume = 0;
		publicfunc_->RefreshDataUpdate();
		publicfunc_->SendCommand(L"volume&val=" + Osp::Base::Integer::ToString(publicfunc_->vlccurstatus.volume));
		if (needsredraw == true) {
			redrawrect = redrawrect.GetUnion(Rectangle(10,558,460,70));
		} else {
			redrawrect = Rectangle(10,558,460,70);
		}
		needsredraw = true;
	}
	currselslider = -1;
	currpressedbtn = -1;
	cursliderseekvalue = 0;
	curslidervolumevalue = 0;
	if (needsredraw) {
		RedrawDisplayBuffer(redrawrect);
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
	if ((keyCode == Osp::Ui::KEY_SIDE_UP) || (keyCode == Osp::Ui::KEY_SIDE_DOWN)) {
		if (isstreaming == true) {
			int setvol = streamplayervol;
			if (keyCode == Osp::Ui::KEY_SIDE_UP) {
				streamplayervol = streamplayervol + 10;
			} else {
				streamplayervol = streamplayervol - 10;
			}
			if (streamplayervol > 100) streamplayervol = 100;
			if (streamplayervol < 0) streamplayervol = 0;
			setvol = streamplayervol;
			if (setvol > 99) setvol = 99;
			streamplayer->SetVolume(setvol);
			if (streamplayerpreviewoverlay != null) {

				streamplayervolumeoverlay->SetShowState(true);
				streamplayertitleoverlay->SetShowState(true);
				streamplayerpreviewoverlay->RequestRedraw();
				if (streamplayervol == 0) {
					streamplayervolumeoverlay->SetText(L"Vol: Mute");
				} else if (streamplayervol == 100) {
					streamplayervolumeoverlay->SetText(L"Vol: Max");
				} else {
					streamplayervolumeoverlay->SetText(L"Vol: " + Osp::Base::Integer::ToString(streamplayervol) + L"%");
				}
				streamplayervolumeoverlay->Draw();
				streamplayervolumeoverlay->Show();

				if (isfullscreenplayer) {
				FullScreenStreamPlayerHideControls_->Cancel();
				FullScreenStreamPlayerHideControls_->Start(3000);
				}
			}
		} else {
			if (keyCode == Osp::Ui::KEY_SIDE_UP) {
				publicfunc_->vlccurstatus.volume = publicfunc_->vlccurstatus.volume + 50;
			} else {
				publicfunc_->vlccurstatus.volume = publicfunc_->vlccurstatus.volume - 50;
			}
			if (publicfunc_->vlccurstatus.volume > 512) publicfunc_->vlccurstatus.volume = 512;
			if (publicfunc_->vlccurstatus.volume < 0) publicfunc_->vlccurstatus.volume = 0;
			publicfunc_->RefreshDataUpdate();
			publicfunc_->SendCommand(L"volume&val=" + Osp::Base::Integer::ToString(publicfunc_->vlccurstatus.volume));
		}
	}
}

void FMainForm::StreamPlayItemIndex(int itemindex) {
	if ((itemindex >= 0) && (itemindex < playliststream->GetCount())) {
		streamingcurmediaaudio = false;
		streamplayerbufferoverlay->SetText(L"Preparing media for streaming");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
		PlayListItem * streamplaylistitem_ = static_cast<PlayListItem *> (playliststream->GetAt(itemindex));
		String streamstr = L"";
		String streamext = L"";
		int extpos;
		if (streamplaylistitem_->uri.LastIndexOf(L".",streamplaylistitem_->uri.GetLength()-1,extpos) == E_SUCCESS) {
			streamplaylistitem_->uri.SubString(extpos+1,3,streamext);
		}
		int mediafiletype = publicfunc_->GetMediaFileType(streamext);
		if (mediafiletype == 0) {
			if (streamplaylistitem_->uri.GetLength() > 6) {
				streamplaylistitem_->uri.SubString(0,6,streamext);
				if ((streamext == L"http:/") || (streamext == L"rtp://") || (streamext == L"mms://") || (streamext == L"rtsp:/")) {
					mediafiletype = publicfunc_->MEDIAFILETYPE_AUDIO;
				}
			}
		}
		if (mediafiletype == publicfunc_->MEDIAFILETYPE_AUDIO) {
			streamstr = L":sout=#transcode{vcodec=none,acodec=mp4a,ab=192,channels=2,samplerate=44100,threads=16,high-priority=1}:rtp{dst=" + publicfunc_->GetMyIp() + ",sdp=rtsp://:5544/stream.sdp} :no-sout-rtp-sap :no-sout-standard-sap :ttl=10 :sout-keep";
			streaminguri = L"rtsp://" + publicfunc_->clientip + ":5544/stream.sdp";
			streamingcurmediaaudio = true;
		} else if (mediafiletype == publicfunc_->MEDIAFILETYPE_VIDEO) {
			streamstr = L":sout=#transcode{vcodec=h264,venc=x264{profile=baseline,keyint=15},vb=512,scale=1,height=480,deinterlace,audio-sync,acodec=mp4a,ab=128,channels=2,samplerate=44100,threads=16,high-priority=1}:rtp{dst=" + publicfunc_->GetMyIp() + ",sdp=rtsp://:8081/stream.mp4,mp4a-latm} :no-sout-rtp-sap :no-sout-standard-sap :ttl=10 :sout-keep";
			vlmstreamstr = publicfunc_->prepareurl(L"new VLCremote broadcast enabled input \"" + streamplaylistitem_->uri + L"\" output #transcode{vcodec=h264,venc=x264,vb=512,scale=1,height=480,deinterlace,acodec=mp4a,ab=128,channels=2,samplerate=44100}:rtp{dst=" + publicfunc_->GetMyIp() + ",sdp=rtsp://:8081/stream.mp4,mp4a-latm} option no-sout-rtp-sap option no-sout-standard-sap option sout-keep option ttl=10");
			//vlmstreamstr = publicfunc_->prepareurl(L"new VLCremote broadcast enabled input \"" + streamplaylistitem_->uri + L"\" output #transcode{vcodec=mp4v,vb=512,scale=1,height=480,deinterlace,acodec=mp4a,ab=128,channels=2,samplerate=44100}:std{access=http,mux=mp4,dst=" + publicfunc_->GetMyIp() + ":8081/stream.mp4} option no-sout-rtp-sap option no-sout-standard-sap option sout-keep option ttl=10");
			/*
			 * first remove broadcast element
			 * vlm_cmd.xml?command=del%20VLCremote
			 *
			 * create new broadcast element
			 * vlm_cmd.xml?command=new VLCremote broadcast enabled input "file:///C:/Users/Mrak/Documents%2F20111111_2147_58.avi" output #transcode{vcodec=h264,venc=x264{profile=baseline,keyint=15},vb=512,scale=1,height=480,deinterlace,audio-sync,acodec=mp4a,ab=128,channels=2,samplerate=44100,threads=16,high-priority=1}:rtp{dst=192.168.1.101,sdp=rtsp://:5544/stream.mp4,mp4a-latm}
			 *
			 * play broadcast element
			 * vlm_cmd.xml?command=control%20VLCremote%20play
			 */

			streaminguri = L"rtsp://" + publicfunc_->clientip + ":8081/stream.mp4";
		}
		if (streamstr != L"") {
			streamingplaywait = true;
			if (publicfunc_->VLCAPIVER >= 2) {
				vlmstreamcom = 2;
				publicfunc_->SendCommandVLM(L"del%20VLCremote");
			} else {
				publicfunc_->SendCommand(L"in_play&input=" + publicfunc_->prepareurl(streamplaylistitem_->uri + L" " + streamstr));
			}
			AppLog("send play command");
		}
	}
}

bool FMainForm::StreamingBrowseAddMedia(Osp::Base::String uri, bool startplay) {
	String streamext = L"";
	int extpos;
	bool status = false;
	if (uri.LastIndexOf(L".",uri.GetLength()-1,extpos) == E_SUCCESS) {
		uri.SubString(extpos+1,3,streamext);
	}
	int mediafiletype = publicfunc_->GetMediaFileType(streamext);
	if (mediafiletype == 0) {
		if (uri.GetLength() > 6) {
			uri.SubString(0,6,streamext);
			if ((streamext == L"http:/") || (streamext == L"rtp://") || (streamext == L"mms://") || (streamext == L"rtsp:/")) {
				mediafiletype = publicfunc_->MEDIAFILETYPE_AUDIO;
			}
		}
	}
	if (mediafiletype == publicfunc_->MEDIAFILETYPE_AUDIO) {
		PlayListItem * playlistitemnew_ = new PlayListItem(L"", L"", uri, 0, false);
		playliststream->Add(*playlistitemnew_);
		if (startplay == true) {
			if (streamplayer != null) {
				streamplayer->Stop();
			}
			publicfunc_->StopListening();
			streamingcurrentitemindex = playliststream->GetCount()-1;
			StreamPlayItemIndex(streamingcurrentitemindex);
		}
		status = true;
	} else if (mediafiletype == publicfunc_->MEDIAFILETYPE_VIDEO) {
		PlayListItem * playlistitemnew_ = new PlayListItem(L"", L"", uri, 0, false);
		playliststream->Add(*playlistitemnew_);
		if (startplay == true) {
			if (streamplayer != null) {
				streamplayer->Stop();
			}
			publicfunc_->StopListening();
			streamingcurrentitemindex = playliststream->GetCount()-1;
			StreamPlayItemIndex(streamingcurrentitemindex);
		}
		status = true;
	} else {
		status = false;
	}
	return status;
}

void FMainForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == publicfunc_->EVENTREFRESHDATAUPDATE) {
		RefreshDataUpdate();
		if ((isstreaming == true) && (publicfunc_->VLCAPIVER >= 2)) {
			//do nothing
		} else {
			if ((streamingplaywait == true) && (isstreaming == true)) {
				if (publicfunc_->vlccurstatus.state == L"playing") {
					streamingplaywait = false;
					streamingendoffile = false;
					StartToPlayTimer_->Start(200);
				}
			}
			if ((streamingpausewait == true) && (isstreaming == true)) {
				if (publicfunc_->vlccurstatus.state == L"paused") {
					streamingpausewait = false;
					streamplayer->Stop();
				}
			}
		}
		/*if ((isstreaming == true) && (streamplayer != null)) {
			if ((streamplayer->GetState() == PLAYER_STATE_PLAYING) || (streamplayer->GetState() == PLAYER_STATE_ERROR)) {

			}
		}*/
	} else if (requestId == publicfunc_->EVENTREFRESHDATAUPDATEPLAYLIST) {
		publicfunc_->StopListening();
		Osp::Base::String mediauri = L"";
		Osp::Base::String mediaext = L"";
		int soutpos(0);
		int extpos(0);
		int mediafiletype(0);
		streamingcurrentitemindex = 0;
		int curitemindex(0);
		playliststream->RemoveAll(true);
		Osp::Base::Collection::IEnumerator * pEnum = publicfunc_->playlist->GetEnumeratorN();
		PlayListItem * playlistitem_ = null;
		while (pEnum->MoveNext() == E_SUCCESS) {
			playlistitem_ = static_cast<PlayListItem *> (pEnum->GetCurrent());
			mediauri = playlistitem_->uri;
			mediaext = L"";
			if (playlistitem_->uri.IndexOf(L":sout=#",0,soutpos) == E_SUCCESS) {
				playlistitem_->uri.SubString(0,soutpos,mediauri);
				mediauri.Trim();
			}
			if (mediaext == L"") {
				if (mediauri.LastIndexOf(L".",mediauri.GetLength()-1,extpos) == E_SUCCESS) {
					mediauri.SubString(extpos+1,3,mediaext);
				}
			}
			mediafiletype = publicfunc_->GetMediaFileType(mediaext);
			if (mediafiletype == 0) {
				if (mediauri.GetLength() > 6) {
					mediauri.SubString(0,6,mediaext);
					if ((mediaext == L"http:/") || (mediaext == L"rtp://") || (mediaext == L"mms://") || (mediaext == L"rtsp:/")) {
						mediafiletype = publicfunc_->MEDIAFILETYPE_AUDIO;
					}
				}
			}
			if (mediafiletype == publicfunc_->MEDIAFILETYPE_AUDIO) {
				AppLog("added audio in stream playlist %S", mediauri.GetPointer());
				if (playlistitem_->iscurrent) {
					streamingcurrentitemindex = curitemindex;
				}
				PlayListItem * playlistitemnew_ = new PlayListItem(playlistitem_->title, playlistitem_->artist, playlistitem_->uri, playlistitem_->id, playlistitem_->iscurrent);
				playliststream->Add(*playlistitemnew_);
				curitemindex++;
			} else if (mediafiletype == publicfunc_->MEDIAFILETYPE_VIDEO) {
				AppLog("added video in stream playlist %S", mediauri.GetPointer());
				if (playlistitem_->iscurrent) {
					streamingcurrentitemindex = curitemindex;
				}
				PlayListItem * playlistitemnew_ = new PlayListItem(playlistitem_->title, playlistitem_->artist, playlistitem_->uri, playlistitem_->id, playlistitem_->iscurrent);
				playliststream->Add(*playlistitemnew_);
				curitemindex++;
			} else {
				AppLog("unknown format %S %S", mediauri.GetPointer(), mediaext.GetPointer());
			}
		}
		delete pEnum;
		if (playliststream->GetCount() < 1) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Playlist empty!", "No supported media in playlist for streaming!\nDo you want to start stream mode anyway?", MSGBOX_STYLE_YESNO, 10000);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				streamingclearplaylistwait = true;
				publicfunc_->SendCommand(L"pl_empty");
			} else {
				this->OnActionPerformed(*this, ASTREAMOFF);
			}
		} else {
			streamingclearplaylistwait = true;
			publicfunc_->SendCommand(L"pl_empty");
		}
	} else if (requestId == publicfunc_->EVENTCOMMANDRETURN) {
		if (streamingclearplaylistwait) {
			publicfunc_->StopListening();
			streamingclearplaylistwait = false;
			StreamPlayItemIndex(streamingcurrentitemindex);
		} else if (streamingplaywait) {
			if (publicfunc_->VLCAPIVER >= 2) {
				if (vlmstreamcom == 2) {
					AppLog("create stream");
					publicfunc_->SendCommandVLM(vlmstreamstr);
					vlmstreamcom--;
				} else if (vlmstreamcom == 1) {
					AppLog("play stream");
					publicfunc_->SendCommandVLM(L"control%20VLCremote%20play");
					vlmstreamcom--;
				} else {
					AppLog("start ref");
					publicfunc_->StartListening();
					streamingplaywait = false;
					streamingendoffile = false;
					StartToPlayTimer_->Start(200);
				}
			} else {
				publicfunc_->StartListening();
			}
		}
	}
}

void FMainForm::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (StartToPlayTimer_->Equals(timer)) {
		streamplayer->Close();
		Uri streamuri;
		streamuri.SetUri(streaminguri);
		streamplayer->OpenUrl(streamuri, true);
	}
	if (FullScreenStreamPlayerHideControls_->Equals(timer)) {
		if ((streamplayerpreviewoverlay != null) && (isstreaming) && (isfullscreenplayer)) {
			streamplayervolumeoverlay->SetShowState(false);
			streamplayertitleoverlay->SetShowState(false);
			streamplayerpreviewoverlay->RequestRedraw();
		}
	}
}

void FMainForm::OnPlayerOpened(result r) {
	streamplayerbufferoverlay->SetText(L"Starting player");
	streamplayerbufferoverlay->Draw();
	streamplayerbufferoverlay->Show();
    result rt = E_SUCCESS;
    rt = streamplayer->Play();
    if (IsFailed(rt)) {
    	streamplayerbufferoverlay->SetText(L"Player error");
    	AppLog("player play error");
    	if (rt == E_DEVICE_BUSY) {
    		streamplayerbufferoverlay->SetText(L"Player error: device busy");
    		AppLog("player play devicebusy");
    	} else if (rt == E_INVALID_STATE) {
    		streamplayerbufferoverlay->SetText(L"Player error: invalid state");
    		AppLog("player play invalidstate");
    	} else if (rt == E_SYSTEM ) {
    		streamplayerbufferoverlay->SetText(L"Player error: system");
    		AppLog("player play system");
    	}
    	streamplayerbufferoverlay->Draw();
    	streamplayerbufferoverlay->Show();

        //Need to handle the exception.
    }
}


void FMainForm::OnPlayerEndOfClip(void) {
	AppLog("OnPlayerEndOfClip");
	streamplayerbufferoverlay->SetText(L"End of clip");
	streamplayerbufferoverlay->Draw();
	streamplayerbufferoverlay->Show();
    result r = E_SUCCESS;
    r = streamplayer->Close();
    if (IsFailed(r)) {
        //Handle the exception.
    }
    this->OnActionPerformed(*this,ANEXT);
}

void FMainForm::OnPlayerBuffering(int percent) {
	if (percent >= 100) {
		streamplayerbufferoverlay->SetText(L"");
	} else {
		streamplayerbufferoverlay->SetText(L"Buffering " + Osp::Base::Integer::ToString(percent) + L"%");
	}
	streamplayerbufferoverlay->Draw();
	streamplayerbufferoverlay->Show();
}

void FMainForm::OnPlayerErrorOccurred(PlayerErrorReason r ) {
	AppLog("OnPlayerErrorOccurred");
	if (r == PLAYER_ERROR_INVALID_DATA) {
		streamplayerbufferoverlay->SetText(L"Player error: Invalid data");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else if (r == PLAYER_ERROR_CONNECTION_LOST ) {
		streamplayerbufferoverlay->SetText(L"Player error: Connection lost");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else if (r == PLAYER_ERROR_STREAMING_TIMEOUT ) {
		streamplayerbufferoverlay->SetText(L"Player error: Timeout");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else if (r == PLAYER_ERROR_TRANSPORT  ) {
		streamplayerbufferoverlay->SetText(L"Player error: Transport");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else if (r == PLAYER_ERROR_SERVER  ) {
		streamplayerbufferoverlay->SetText(L"Player error: Server failed");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else if (r == PLAYER_ERROR_DEVICE_FAILED   ) {
		streamplayerbufferoverlay->SetText(L"Player error: Device failed");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	} else {
		streamplayerbufferoverlay->SetText(L"Player error: Undefined error");
		streamplayerbufferoverlay->Draw();
		streamplayerbufferoverlay->Show();
	}
}

void FMainForm::OnPlayerInterrupted() {
	streamplayerbufferoverlay->SetText(L"Interrupted");
	streamplayerbufferoverlay->Draw();
	streamplayerbufferoverlay->Show();
	this->OnActionPerformed(*this,APAUSE);
	if (streamplayer != null) {
		streamplayer->Stop();
	}
}

void FMainForm::OnPlayerReleased() {
	streamplayerbufferoverlay->SetText(L"Released");
	streamplayerbufferoverlay->Draw();
	streamplayerbufferoverlay->Show();
	this->OnActionPerformed(*this,APLAY);
}

void FMainForm::OnPlayerSeekCompleted(result r) {
	AppLog("OnPlayerSeekCompleted");
}

void FMainForm::Background() {
	AppLog("Background");
	if ((isstreaming) && (streamplayer != null) && (streamplayerpreviewoverlay != null)) {
		if (streamplayer->GetState() == PLAYER_STATE_PLAYING) {
			streamplayerbufferoverlay->SetText(L"Background pause");
			streamplayerbufferoverlay->Draw();
			streamplayerbufferoverlay->Show();
			this->OnActionPerformed(*this,APAUSE);
			Osp::System::PowerManager::KeepScreenOnState(false, true);
			if (streamplayer != null) {
				streamplayer->Stop();
			}
			isonbackground = true;
		}
	}
}

void FMainForm::Foreground() {
	AppLog("Foreground");
	if ((isstreaming) && (streamplayer != null) && (streamplayerpreviewoverlay != null)) {
		if (isonbackground == true) {
			if (streamplayer->GetState() == PLAYER_STATE_STOPPED) {
				isonbackground = false;
				streamplayerbufferoverlay->SetText(L"");
				streamplayerbufferoverlay->Draw();
				streamplayerbufferoverlay->Show();
				this->OnActionPerformed(*this,APLAYSTREAMFROMBG);
				Osp::System::PowerManager::KeepScreenOnState(true, false);
			}
		}
	}
}

void FMainForm::StreamingPlayListClear() {
	playliststream->RemoveAll(true);
	publicfunc_->SendCommand("pl_stop");
	/*if (publicfunc_->VLCAPIVER >= 2) {
		publicfunc_->SendCommand("pl_stop");
	}*/
	if ((isstreaming == true) && (streamplayer != null)) {
		streamplayer->Stop();
	}
}

void FMainForm::StreamingPlayListRemoveItem(int itemindex) {
	if ((itemindex >= 0) && (itemindex < playliststream->GetCount())) {
		if (itemindex == streamingcurrentitemindex) {
			publicfunc_->SendCommand("pl_stop");
			if ((isstreaming == true) && (streamplayer != null)) {
				streamplayer->Stop();
			}
		}
		playliststream->RemoveAt(itemindex, true);
	}
}

void FMainForm::StreamingPlayListPlayItem(int itemindex) {
	if ((itemindex >= 0) && (itemindex < playliststream->GetCount())) {
		streamingcurrentitemindex = itemindex;
		if (streamplayer != null) {
			streamplayer->Stop();
		}
		publicfunc_->StopListening();
		StreamPlayItemIndex(streamingcurrentitemindex);
	}
}
