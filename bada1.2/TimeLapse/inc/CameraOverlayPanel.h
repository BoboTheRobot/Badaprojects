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
	static const int INIT = 0;
	static const int START = 1;
	static const int STARTTIMER = 2;
	static const int RECORDING = 3;

	int ftype;

	int starttimer;
	Osp::Base::String startnotice;

	int framenumber;
	Osp::Base::String rectime;
	Osp::Base::String filesize;
	Osp::Base::String remainingmemory;
	Osp::Base::String systemtime;
	int batterylevel;

	DrawingClass * drawingclass_;

	result OnInitializing(void);
	result OnTerminating(void);
	result OnDraw(void);
};

#endif /* CAMERAOVERLAYPANEL_H_ */
