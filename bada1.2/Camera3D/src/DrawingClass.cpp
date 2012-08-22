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
 * DrawingClass.cpp
 *
 *  Created on: 10.10.2010
 *      Author: mrak
 */

#include "DrawingClass.h"

using namespace Osp::Base::Utility;
using namespace Osp::Base;
using namespace Osp::Base::Collection;
using namespace Osp::Media;
using namespace Osp::Graphics;

DrawingClass::DrawingClass(String spritefn) {
	Image *pImage_ = new Image();
	pImage_->Construct();
	spritebmp = pImage_->DecodeN(spritefn, BITMAP_PIXEL_FORMAT_ARGB8888);
	delete pImage_;
}

DrawingClass::~DrawingClass() {
	delete spritebmp;
}

void DrawingClass::FDrawTextToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, String text, int fontsize, int fontstyle, bool wordwrap, Osp::Graphics::Color fontcolor, Osp::Graphics::TextHorizontalAlignment halign, Osp::Graphics::TextVerticalAlignment valign) {
	Font font_;
	font_.Construct(fontstyle, fontsize);
	EnrichedText texteel;
	texteel.Construct(Dimension(destdim.width, destdim.height));
	texteel.SetHorizontalAlignment(halign);
	texteel.SetVerticalAlignment(valign);
	if (wordwrap) {
		texteel.SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
	}
	TextElement textel;
	textel.Construct(text);
	textel.SetTextColor(fontcolor);
	textel.SetFont(font_);
	texteel.Add(textel);
	oncanvas->DrawText(Point(destdim.x,destdim.y), texteel);
}

void DrawingClass::FDrawSpriteToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Graphics::Rectangle srcdim) {
	if ((destdim.width == 0) || (destdim.height == 0)) {
		destdim.SetSize(srcdim.width, srcdim.height);
	}
	oncanvas->DrawBitmap(destdim, *this->spritebmp, srcdim);
}

Osp::Graphics::Bitmap* DrawingClass::FDrawSpriteToBmpN(Osp::Graphics::Rectangle srcdim) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(Rectangle(0,0,srcdim.width,srcdim.height));
	this->FDrawSpriteToCanvas(pcanvas_, Rectangle(0,0,srcdim.width,srcdim.height), srcdim);
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}

Osp::Graphics::Bitmap* DrawingClass::FDrawTextToBmpN(Osp::Graphics::Rectangle dim, Osp::Base::String text, int fontsize, Osp::Graphics::Color fontcolor) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(dim);
	this->FDrawTextToCanvas(pcanvas_, dim, text, fontsize, FONT_STYLE_PLAIN, true, fontcolor, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}

Osp::Graphics::Bitmap* DrawingClass::FDrawButtonToBmpN(Osp::Graphics::Rectangle srcdim, Osp::Graphics::Rectangle icodim) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(Rectangle(0,0,srcdim.width,srcdim.height));
	this->FDrawSpriteToCanvas(pcanvas_, Rectangle(0,0,srcdim.width,srcdim.height), srcdim);
	this->FDrawSpriteToCanvas(pcanvas_, Rectangle(12,12,icodim.width,icodim.height), icodim);
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}
