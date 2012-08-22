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
/*
 * CameraOverlayPanel.cpp
 *
 *  Created on: 28.12.2010
 *      Author: mrak
 */

#include "CameraOverlayPanel.h"
#include "FormMgr.h"
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

CameraOverlayPanel::CameraOverlayPanel() {
	// TODO Auto-generated constructor stub

}

CameraOverlayPanel::~CameraOverlayPanel() {
	// TODO Auto-generated destructor stub
}

bool
CameraOverlayPanel::Initialize(const Osp::Graphics::Rectangle &rect) {
	OverlayPanel::Construct(rect);

	return true;
}

result
CameraOverlayPanel::OnInitializing(void)
{
	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	bgbmp = null;

	return E_SUCCESS;
}

result
CameraOverlayPanel::OnTerminating(void)
{

	return E_SUCCESS;
}

result
CameraOverlayPanel::OnDraw(void) {
	Canvas * pCanvas = GetCanvasN();
	pCanvas->Clear();

	if (bgbmp != null) {
		pCanvas->DrawBitmap(bgbmpsizedest, *bgbmp, bgbmpsizesrc);
	}

	pCanvas->FillRectangle(Color(0,0,0,100), Rectangle(0, 420, 800, 60));
	if (ftype == LEFTCAP) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(0,420,800,60), L"Left eye capture", 28, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_RED, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		pCanvas->SetForegroundColor(Color::COLOR_RED);
	} else if (ftype == RIGHTCAP) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(0,420,800,60), L"Right eye capture", 28, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_BLUE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		pCanvas->SetForegroundColor(Color::COLOR_BLUE);
	} else if (ftype == ALIGN) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(0,420,800,60), L"Alignment", 28, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_GREEN, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		pCanvas->SetForegroundColor(Color::COLOR_GREEN);
	} else if (ftype == PAUSE) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(0,420,800,60), L"Paused [resume in 2 sec]", 28, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_GREEN, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		pCanvas->SetForegroundColor(Color::COLOR_RED);
	}
	if ((ftype == LEFTCAP) || (ftype == RIGHTCAP)) {
	pCanvas->SetLineWidth(2);
	/*int boxw = 200;
	int boxh = 120;*/
	int boxw2 = 100;
	int boxh2 = 60;
	int gapw = 50;
	int gaph = 30;
	//top left corner
	pCanvas->DrawLine(Point(400-boxw2,240-boxh2), Point(400-boxw2+gapw, 240-boxh2));
	pCanvas->DrawLine(Point(400-boxw2,240-boxh2), Point(400-boxw2, 240-boxh2+gaph));
	//top right corner
	pCanvas->DrawLine(Point(400+boxw2-gapw,240-boxh2), Point(400+boxw2, 240-boxh2));
	pCanvas->DrawLine(Point(400+boxw2,240-boxh2), Point(400+boxw2, 240-boxh2+gaph));
	//bottom left corner
	pCanvas->DrawLine(Point(400-boxw2,240+boxh2), Point(400-boxw2+gapw, 240+boxh2));
	pCanvas->DrawLine(Point(400-boxw2,240+boxh2), Point(400-boxw2, 240+boxh2-gaph));
	//bottom right corner
	pCanvas->DrawLine(Point(400+boxw2-gapw,240+boxh2), Point(400+boxw2, 240+boxh2));
	pCanvas->DrawLine(Point(400+boxw2,240+boxh2), Point(400+boxw2, 240+boxh2-gaph));
	}
	delete pCanvas;
	return E_SUCCESS;
}
