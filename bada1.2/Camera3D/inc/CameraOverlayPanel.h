/*
 * CameraOverlayPanel.h
 *
 *  Created on: 28.12.2010
 *      Author: mrak
 */

#ifndef CAMERAOVERLAYPANEL_H_
#define CAMERAOVERLAYPANEL_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FGraphics.h>
#include "DrawingClass.h"

class CameraOverlayPanel:
	public Osp::Ui::Controls::OverlayPanel
{
public:
	CameraOverlayPanel();
	~CameraOverlayPanel();
	bool Initialize(const Osp::Graphics::Rectangle &rect);

protected:

public:
	static const int LEFTCAP = 0;
	static const int RIGHTCAP = 1;
	static const int ALIGN = 2;
	static const int VIEW = 3;
	static const int PAUSE = 4;

	int ftype;

	DrawingClass * drawingclass_;
	Osp::Graphics::Bitmap * bgbmp;
	Osp::Graphics::Rectangle bgbmpsizesrc;
	Osp::Graphics::Rectangle bgbmpsizedest;

	result OnInitializing(void);
	result OnTerminating(void);
	result OnDraw(void);
};

#endif /* CAMERAOVERLAYPANEL_H_ */
