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
	this->ftype = INIT;

	return true;
}

result
CameraOverlayPanel::OnInitializing(void)
{
	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

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

	//overlay box top
	pCanvas->FillRectangle(Color(0,0,0,100), Rectangle(0, 0, 800, 84));
	//frame no, rectime, filesize
	if (this->ftype == RECORDING) {
	Osp::Base::String textline1 = L"";
	textline1.Append(L"Frame no.: ");
	textline1.Append(this->framenumber);
	textline1.Append(L"\nRec time: ");
	textline1.Append(this->rectime);
	textline1.Append(L"    File size: ");
	textline1.Append(this->filesize);
	drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 10, 620, 64), textline1, 30, Osp::Graphics::FONT_STYLE_BOLD, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	}

	//overlay box bottom
	pCanvas->FillRectangle(Color(0,0,0,100), Rectangle(0, 420, 800, 60));
	//remaining memory, time, battery
	Osp::Base::String textline2 = L"";
	Osp::Base::String textline3 = L"";
	textline2.Append(L"Remaining memory: ");
	textline2.Append(this->remainingmemory);
	textline3.Append(L"Time: ");
	textline3.Append(this->systemtime);
	textline3.Append(L"    Battery: ");
	textline3.Append(this->batterylevel);
	textline3.Append(L"%");
	drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 430, 400, 40), textline2, 26, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(410, 430, 380, 40), textline3, 26, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_RIGHT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);

	if (this->ftype == INIT) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 100, 780, 280), L"Camera initializing...", 24, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_RED, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	} else if (this->ftype == START) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 100, 780, 280), L"Double tap to start recording!", 30, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_GREEN, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	} else if (this->ftype == STARTTIMER) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 100, 780, 280), L"Start in " + Osp::Base::Integer::ToString(starttimer) + " sec", 30, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_BLUE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	} else if (this->ftype == RECORDING) {
		drawingclass_->FDrawTextToCanvas(pCanvas, Rectangle(10, 100, 780, 280), L"Recording...", 30, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_RED, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
	}

	delete pCanvas;
	return E_SUCCESS;
}
