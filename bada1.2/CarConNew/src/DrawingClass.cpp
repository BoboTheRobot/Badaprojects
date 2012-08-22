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

Osp::Base::Collection::IList * DrawingClass::CreateBlankMountains(Osp::Graphics::Rectangle destdim) {
	Osp::Base::Collection::IList* mountainpolyline = new Osp::Base::Collection::ArrayList;
	mountainpolyline->Add(*(new Point(destdim.x+0,destdim.y+(destdim.height * 0.5))));
	mountainpolyline->Add(*(new Point(destdim.x+100,destdim.y+destdim.height)));
	mountainpolyline->Add(*(new Point(destdim.x+162,destdim.y+(destdim.height * 0.7))));
	mountainpolyline->Add(*(new Point(destdim.x+240,destdim.y)));
	mountainpolyline->Add(*(new Point(destdim.x+290,destdim.y+(destdim.height * 0.6))));
	mountainpolyline->Add(*(new Point(destdim.x+351,destdim.y+(destdim.height * 0.9))));
	mountainpolyline->Add(*(new Point(destdim.x+445,destdim.y+(destdim.height * 0.2))));
	mountainpolyline->Add(*(new Point(destdim.x+480,destdim.y+(destdim.height * 0.5))));
	return mountainpolyline;
}

void DrawingClass::FDrawMountainsToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Base::Collection::IList* mountainpolyline, Osp::Graphics::Point peakpoint, Osp::Base::String peakpointval, Osp::Base::String peakpointme) {
	oncanvas->SetForegroundColor(Color(35, 86, 142));
	oncanvas->SetLineWidth(2);
	mountainpolyline->Add(*(new Point(destdim.x+destdim.width,destdim.y+destdim.height)));
	mountainpolyline->Add(*(new Point(destdim.x+0,destdim.y+destdim.height)));
	oncanvas->FillPolygon(Color(229, 229, 229), *mountainpolyline);
	mountainpolyline->RemoveItems(mountainpolyline->GetCount()-2,2,true);
	oncanvas->DrawPolyline(*mountainpolyline);
	if (peakpointval != L"") {
		oncanvas->FillEllipse(Color(35, 86, 142),Rectangle(peakpoint.x-7,peakpoint.y-7,14,14));
		oncanvas->DrawLine(Point(peakpoint.x,peakpoint.y+7),Point(peakpoint.x,peakpoint.y+7+15));
		Font fontgrafval;
		fontgrafval.Construct(FONT_STYLE_PLAIN, 20);
		Font fontgrafme;
		fontgrafme.Construct(FONT_STYLE_PLAIN, 14);
		Dimension textdim;
		fontgrafval.GetTextExtent(peakpointval,peakpointval.GetLength(),textdim);
		Dimension textdim2;
		fontgrafme.GetTextExtent(peakpointme,peakpointme.GetLength(),textdim2);
		bool isright = ((peakpoint.x+15+textdim.width+textdim2.width+15-destdim.x+50) > destdim.width);
		if (isright) {
			oncanvas->DrawLine(Point(peakpoint.x,peakpoint.y+7+15),Point(peakpoint.x-15,peakpoint.y+7+15));
			oncanvas->FillRoundRectangle(Color(0, 0, 0, 130),Rectangle(peakpoint.x-15-(textdim.width+textdim2.width+15),peakpoint.y+7+15-13,textdim.width+textdim2.width+15,26),Dimension(5,5));
			oncanvas->SetForegroundColor(Color::COLOR_WHITE);
			oncanvas->SetFont(fontgrafval);
			oncanvas->DrawText(Point(peakpoint.x-10-(textdim.width+textdim2.width+15),peakpoint.y+7+15-(textdim.height / 2)), peakpointval);
			oncanvas->SetFont(fontgrafme);
			oncanvas->DrawText(Point((peakpoint.x-10-(textdim.width+textdim2.width+15))+textdim.width+5,peakpoint.y+7+15-(textdim2.height / 2)), peakpointme);
		} else {
			oncanvas->DrawLine(Point(peakpoint.x,peakpoint.y+7+15),Point(peakpoint.x+15,peakpoint.y+7+15));
			oncanvas->FillRoundRectangle(Color(0, 0, 0, 130),Rectangle(peakpoint.x+15,peakpoint.y+7+15-13,textdim.width+textdim2.width+15,26),Dimension(5,5));
			oncanvas->SetForegroundColor(Color::COLOR_WHITE);
			oncanvas->SetFont(fontgrafval);
			oncanvas->DrawText(Point(peakpoint.x+20,peakpoint.y+7+15-(textdim.height / 2)), peakpointval);
			oncanvas->SetFont(fontgrafme);
			oncanvas->DrawText(Point(peakpoint.x+20+textdim.width+5,peakpoint.y+7+15-(textdim2.height / 2)), peakpointme);
		}
	}
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

void DrawingClass::FDrawButtonIcoToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Point destpoint, int buttontype, int icoindex, bool pressed) {
	Rectangle spriteposdef = Rectangle(130,0,115,115);
	Rectangle spritepossel = Rectangle(spriteposdef.x,spriteposdef.height,spriteposdef.width,spriteposdef.height);
	Rectangle spriteposover = Rectangle(spriteposdef.x+10,230,94,46);
	Point spriteposoverind = Point(10,56);
	Rectangle spriteposico = Rectangle(600,0,84,84);
	Point spriteindico = Point(15,15);
	if ((buttontype == 3) || (buttontype == 4) || (buttontype == 5) || (buttontype == 6)) {
		spriteposdef = Rectangle(250,0,235,80);
		if ((buttontype == 5) || (buttontype == 6)) {
			spriteposdef = Rectangle(spriteposdef.x,200,spriteposdef.width,spriteposdef.height);
		}
		spritepossel = Rectangle(spriteposdef.x,spriteposdef.y+spriteposdef.height,spriteposdef.width,spriteposdef.height);
		spriteposover = Rectangle(spriteposdef.x+10,160,225,32);
		spriteposoverind = Point(10,40);
		spriteposico = Rectangle(250,360,182,42);
		spriteindico = Point(25, 18);
	}
	if (pressed == true) {
		/* draw pressed */
		oncanvas->DrawBitmap(Rectangle(destpoint.x,destpoint.y,spriteposdef.width,spriteposdef.height), *this->spritebmp, spritepossel);
	} else {
		/* draw normal */
		oncanvas->DrawBitmap(Rectangle(destpoint.x,destpoint.y,spriteposdef.width,spriteposdef.height), *this->spritebmp, spriteposdef);
	}
	/* draw icon */
	oncanvas->DrawBitmap(Rectangle(destpoint.x+spriteindico.x,destpoint.y+spriteindico.y,spriteposico.width,spriteposico.height), *this->spritebmp, Rectangle(spriteposico.x,spriteposico.y+((icoindex-1) * spriteposico.height),spriteposico.width,spriteposico.height));
	/* draw overlay */
	oncanvas->DrawBitmap(Rectangle(destpoint.x+spriteposoverind.x,destpoint.y+spriteposoverind.y,spriteposover.width,spriteposover.height), *this->spritebmp, spriteposover);
}

void DrawingClass::FDrawSpriteToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Graphics::Rectangle srcdim) {
	if ((destdim.width == 0) || (destdim.height == 0)) {
		destdim.SetSize(srcdim.width, srcdim.height);
	}
	oncanvas->DrawBitmap(destdim, *this->spritebmp, srcdim);
}

void DrawingClass::FDrawNumbersToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, String text, int size) {
	Rectangle charposfirst;
	int shadowsize;
	if (size == 1) {
		charposfirst = Rectangle(559,12,32,35);
		shadowsize = 4;
	} else {
		charposfirst = Rectangle(484,7,72,80);
		shadowsize = 7;
	}
	mchar tchar;
	Rectangle charpos, charposdest;
	Point curpos = Point(destdim.x + destdim.width + shadowsize, destdim.y + destdim.height + shadowsize);
	int charw = 0;
	int chary = 0;
	for (int n=text.GetLength()-1; n>=0; n--) {
		text.GetCharAt(n,tchar);
		if (Character::ToString(tchar) == L"0") {
			if (size == 1) {
				charw = 30;
			} else {
				charw = 64;
			}
			chary = charposfirst.y;
		} else if (Character::ToString(tchar) == L"1") {
			if (size == 1) {
				charw = 22;
			} else {
				charw = 46;
			}
			chary = charposfirst.y+(charposfirst.height * 1);
		} else if (Character::ToString(tchar) == L"2") {
			if (size == 1) {
				charw = 25;
			} else {
				charw = 58;
			}
			chary = charposfirst.y+(charposfirst.height * 2);
		} else if (Character::ToString(tchar) == L"3") {
			if (size == 1) {
				charw = 25;
			} else {
				charw = 60;
			}
			chary = charposfirst.y+(charposfirst.height * 3);
		} else if (Character::ToString(tchar) == L"4") {
			if (size == 1) {
				charw = 26;
			} else {
				charw = 60;
			}
			chary = charposfirst.y+(charposfirst.height * 4);
		} else if (Character::ToString(tchar) == L"5") {
			if (size == 1) {
				charw = 26;
			} else {
				charw = 60;
			}
			chary = charposfirst.y+(charposfirst.height * 5);
		} else if (Character::ToString(tchar) == L"6") {
			if (size == 1) {
				charw = 26;
			} else {
				charw = 62;
			}
			chary = charposfirst.y+(charposfirst.height * 6);
		} else if (Character::ToString(tchar) == L"7") {
			if (size == 1) {
				charw = 25;
			} else {
				charw = 60;
			}
			chary = charposfirst.y+(charposfirst.height * 7);
		} else if (Character::ToString(tchar) == L"8") {
			if (size == 1) {
				charw = 26;
			} else {
				charw = 62;
			}
			chary = charposfirst.y+(charposfirst.height * 8);
		} else if (Character::ToString(tchar) == L"9") {
			if (size == 1) {
				charw = 27;
			} else {
				charw = 62;
			}
			chary = charposfirst.y+(charposfirst.height * 9);
		} else {
			if (size == 1) {
				charw = 14;
			} else {
				charw = 26;
			}
			chary = charposfirst.y+(charposfirst.height * 10);
		}
		charpos = Rectangle(charposfirst.x+Math::Round((charposfirst.width - charw) / 2.0f),chary,charw,charposfirst.height-1);
		charposdest = Rectangle(curpos.x - charpos.width, curpos.y - charpos.height, charpos.width, charpos.height);
		oncanvas->DrawBitmap(charposdest, *this->spritebmp, charpos);
		curpos.SetPosition(curpos.x - charpos.width, curpos.y);
	}
}

Osp::Graphics::Bitmap* DrawingClass::FDrawFormTitleIconN(int icoindex) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(Rectangle(0,0,32,32));
	this->FDrawSpriteToCanvas(pcanvas_, Rectangle(0,0,32,32), Rectangle(700,((icoindex-1)*32),32,32));
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
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

Osp::Graphics::Bitmap* DrawingClass::FDrawButtonSwitchToBmpN(bool status, Osp::Base::String caption1, Osp::Base::String caption2) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(Rectangle(0,0,156,110));
	if (status) {
		this->FDrawSpriteToCanvas(pcanvas_, Rectangle(0,0,156,110), Rectangle(760,110,156,110));
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,7,146,47), caption1, 26, FONT_STYLE_PLAIN, false, Color::COLOR_BLACK, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,60,146,47), caption2, 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	} else {
		this->FDrawSpriteToCanvas(pcanvas_, Rectangle(0,0,156,110), Rectangle(760,0,156,110));
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,7,146,47), caption1, 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,60,146,47), caption2, 26, FONT_STYLE_PLAIN, false, Color::COLOR_BLACK, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	}
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}

Osp::Graphics::Bitmap* DrawingClass::FDrawButtonSpecialToBmpN(Osp::Graphics::Rectangle dim, bool status, Osp::Base::String caption, int groupstyle) {
	Bitmap* pBitmap = new Bitmap;
	Canvas * pcanvas_ = new Canvas;
	pcanvas_->Construct(dim);
	if (status) {
		this->FDrawSpriteToCanvas(pcanvas_, dim, Rectangle(765,230,5,103));
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,5,dim.width-10,dim.height-10), caption, 26, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	} else {
		this->FDrawSpriteToCanvas(pcanvas_, dim, Rectangle(760,230,5,103));
		this->FDrawTextToCanvas(pcanvas_, Rectangle(5,5,dim.width-10,dim.height-10), caption, 26, FONT_STYLE_PLAIN, true, Color::COLOR_BLACK, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	}
	pcanvas_->SetLineWidth(1);
	pcanvas_->SetForegroundColor(Color(91, 104, 116));
	if ((groupstyle == this->BUTTONSPECIALGROUP_LEFT) || (groupstyle == this->BUTTONSPECIALGROUP_MIDDLE)) {
		pcanvas_->DrawLine(Point(0,0),Point(0,dim.height));
		pcanvas_->DrawLine(Point(0,dim.height-1),Point(dim.width,dim.height-1));
	} else if (groupstyle == this->BUTTONSPECIALGROUP_RIGHT) {
		pcanvas_->DrawLine(Point(0,0),Point(0,dim.height));
		pcanvas_->DrawLine(Point(0,dim.height-1),Point(dim.width,dim.height-1));
		pcanvas_->DrawLine(Point(dim.width-1,0),Point(dim.width-1,dim.height));
	} else {
		pcanvas_->DrawRectangle(dim);
	}
	pBitmap->Construct(*pcanvas_, pcanvas_->GetBounds());
	delete pcanvas_;
	return pBitmap;
}

