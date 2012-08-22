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
#include "FGrafZoom.h"
#include "FPregled.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

FGrafZoom::FGrafZoom(void)
{
}

FGrafZoom::~FGrafZoom(void)
{
}

bool
FGrafZoom::Initialize()
{
	Form::Construct(L"IDF_FGRAFZOOM");
	SetName(L"FGrafZoom");
	this->showhint = true;
	return true;
}

result
FGrafZoom::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	pGrafBmp = null;
	pTimer_ = new Osp::Base::Runtime::Timer;
	pTimer_->Construct(*this);

	AddTouchEventListener(*this);

	return r;
}

void FGrafZoom::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	this->showhint = false;
	this->RequestRedraw();
}

void FGrafZoom::onopen() {
	if (pGrafBmp == null) {
		AppLog("create zoom bmp");
		FPregled *pFPregled = static_cast<FPregled *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FPregled"));
		Rectangle grafimgrect = Rectangle(0,0,800,480);
		pGrafBmp = new Bitmap;
		pFPregled->GetGraph(pGrafBmp, grafimgrect, true);
	}

	pTimer_->Cancel();
	this->showhint = true;
	pTimer_->Start(2000);
}

result
FGrafZoom::OnDraw()
{
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		pCanvas_->DrawBitmap(Point(0,0), *pGrafBmp);
		if (this->showhint == true) {
			Rectangle tooltippos = Rectangle(10,10,780,50);
			pCanvas_->SetForegroundColor(Color(255, 255, 255));
			pCanvas_->SetLineWidth(2);
			pCanvas_->FillRoundRectangle(Color(0, 172, 1, 180), tooltippos, Dimension(10,10));
			pCanvas_->DrawRoundRectangle(tooltippos, Dimension(10,10));
			drawingclass_->FDrawTextToCanvas(pCanvas_, tooltippos, "Double tab for close", 28, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FGrafZoom::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete pGrafBmp;
	pGrafBmp = null;
	delete pTimer_;


	return r;
}


void
FGrafZoom::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == ID_CLOSE) {
		FPregled *pFPregled = static_cast<FPregled *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FPregled"));
		Application::GetInstance()->GetAppFrame()->GetFrame()->SetCurrentForm(*pFPregled);
		pFPregled->Draw();
		pFPregled->Show();
		delete pGrafBmp;
		pGrafBmp = null;
	}
}

void
FGrafZoom::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	OnActionPerformed(source, ID_CLOSE);
}

void
FGrafZoom::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGrafZoom::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGrafZoom::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGrafZoom::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGrafZoom::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (this->showhint == false) {
		this->showhint = true;
		this->RequestRedraw();
	}
}

void
FGrafZoom::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (this->showhint == true) {
		pTimer_->Cancel();
		pTimer_->Start(500);
		this->RequestRedraw();
	}
}

