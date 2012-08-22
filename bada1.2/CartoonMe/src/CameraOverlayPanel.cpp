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
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

CameraOverlayPanel::CameraOverlayPanel() {

}

CameraOverlayPanel::~CameraOverlayPanel() {

}

bool
CameraOverlayPanel::Initialize(const Osp::Graphics::Rectangle &rect) {
	OverlayPanel::Construct(rect);

	return true;
}

result
CameraOverlayPanel::OnInitializing(void)
{

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


	if (this->ftype == PAUSE) {
		Font font_;
		font_.Construct(Osp::Graphics::FONT_STYLE_BOLD, 30);
		EnrichedText texteel;
		texteel.Construct(Dimension(pCanvas->GetBounds().width, pCanvas->GetBounds().height));
		texteel.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_CENTER);
		texteel.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		texteel.SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
		TextElement textel;
		textel.Construct(L"Paused\n[resume in 2 sec]");
		textel.SetTextColor(Color::COLOR_GREEN);
		textel.SetFont(font_);
		texteel.Add(textel);
		pCanvas->DrawText(Point(0,0), texteel);
	}

	delete pCanvas;
	return E_SUCCESS;
}
