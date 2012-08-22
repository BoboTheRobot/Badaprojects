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
 * TextViewLabel.cpp
 *
 *  Created on: 28.12.2010
 *      Author: mrak
 */

#include "TextViewLabel.h"
#include "FormMgr.h"
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

TextViewLabel::TextViewLabel() {
	// TODO Auto-generated constructor stub

}

TextViewLabel::~TextViewLabel() {
	// TODO Auto-generated destructor stub
}

bool
TextViewLabel::Initialize(const Osp::Graphics::Rectangle &rect, Osp::Base::String & text) {
	this->text = text;
	text = L"";
	Label::Construct(rect, text);
	return true;
}

result
TextViewLabel::OnInitializing(void)
{
	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	return E_SUCCESS;
}

result
TextViewLabel::OnTerminating(void)
{

	return E_SUCCESS;
}

result
TextViewLabel::OnDraw(void) {
	Canvas * pCanvas = GetCanvasN();
	pCanvas->Clear();
	
	drawingclass_->FDrawTextToCanvas(pCanvas, pCanvas->GetBounds(), this->text, 26, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_TOP);

	delete pCanvas;
	return E_SUCCESS;
}
