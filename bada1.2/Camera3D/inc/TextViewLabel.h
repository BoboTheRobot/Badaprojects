/*
 * TextViewLabel.h
 *
 *  Created on: 28.12.2010
 *      Author: mrak
 */

#ifndef TextViewLabel_H_
#define TextViewLabel_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FGraphics.h>
#include "DrawingClass.h"

class TextViewLabel:
	public Osp::Ui::Controls::Label
{
public:
	TextViewLabel();
	~TextViewLabel();
	bool Initialize(const Osp::Graphics::Rectangle &rect, Osp::Base::String & text);

protected:

public:
	DrawingClass * drawingclass_;

	Osp::Base::String text;

	result OnInitializing(void);
	result OnTerminating(void);
	result OnDraw(void);
};

#endif /* TextViewLabel_H_ */
