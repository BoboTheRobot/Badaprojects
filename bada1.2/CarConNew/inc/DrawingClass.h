/*
 * DrawingClass.h
 *
 *  Created on: 10.10.2010
 *      Author: mrak
 */

#ifndef DRAWINGCLASS_H_
#define DRAWINGCLASS_H_

#include <FBase.h>
#include <FMedia.h>

class DrawingClass {
public:
	DrawingClass(Osp::Base::String spritefn);
	virtual ~DrawingClass();
	Osp::Graphics::Bitmap* spritebmp;
public:
	static const int BUTTONSPECIALGROUP_LEFT = 1;
	static const int BUTTONSPECIALGROUP_MIDDLE = 2;
	static const int BUTTONSPECIALGROUP_RIGHT = 3;
	Osp::Base::Collection::IList * CreateBlankMountains(Osp::Graphics::Rectangle destdim);
	void FDrawMountainsToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Base::Collection::IList* mountainpolyline, Osp::Graphics::Point peakpoint, Osp::Base::String peakpointval, Osp::Base::String peakpointme);
	void FDrawTextToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Base::String text, int fontsize, int fontstyle, bool wordwrap, Osp::Graphics::Color fontcolor, Osp::Graphics::TextHorizontalAlignment halign, Osp::Graphics::TextVerticalAlignment valign);
	void FDrawButtonIcoToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Point destpoint, int buttontype, int icoindex, bool pressed);
	void FDrawSpriteToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Graphics::Rectangle srcdim);
	void FDrawNumbersToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Base::String text, int size);
	Osp::Graphics::Bitmap* FDrawFormTitleIconN(int icoindex);
	Osp::Graphics::Bitmap* FDrawSpriteToBmpN(Osp::Graphics::Rectangle srcdim);
	Osp::Graphics::Bitmap* FDrawTextToBmpN(Osp::Graphics::Rectangle dim, Osp::Base::String text, int fontsize, Osp::Graphics::Color fontcolor);
	Osp::Graphics::Bitmap* FDrawButtonSwitchToBmpN(bool status, Osp::Base::String caption1, Osp::Base::String caption2);
	Osp::Graphics::Bitmap* FDrawButtonSpecialToBmpN(Osp::Graphics::Rectangle dim, bool status, Osp::Base::String caption, int groupstyle=0);
};

#endif /* DRAWINGCLASS_H_ */
