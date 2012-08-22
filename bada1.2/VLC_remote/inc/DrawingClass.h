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
	void FDrawTextToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Base::String text, int fontsize, int fontstyle, bool wordwrap, Osp::Graphics::Color fontcolor, Osp::Graphics::TextHorizontalAlignment halign, Osp::Graphics::TextVerticalAlignment valign);
	void FDrawSpriteToCanvas(Osp::Graphics::Canvas * oncanvas, Osp::Graphics::Rectangle destdim, Osp::Graphics::Rectangle srcdim);
	Osp::Graphics::Bitmap* FDrawSpriteToBmpN(Osp::Graphics::Rectangle srcdim);
	Osp::Graphics::Bitmap* FDrawTextToBmpN(Osp::Graphics::Rectangle dim, Osp::Base::String text, int fontsize, Osp::Graphics::Color fontcolor);
	Osp::Graphics::Bitmap* FDrawButtonToBmpN(Osp::Graphics::Rectangle srcdim, Osp::Graphics::Rectangle icodim, Osp::Graphics::Point icopos);
};

#endif /* DRAWINGCLASS_H_ */
