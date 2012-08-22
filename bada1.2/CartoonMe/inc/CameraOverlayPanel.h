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

class CameraOverlayPanel:
	public Osp::Ui::Controls::OverlayPanel
{
public:
	CameraOverlayPanel();
	~CameraOverlayPanel();
	bool Initialize(const Osp::Graphics::Rectangle &rect);

protected:

public:
	static const int EMPTY = 0;
	static const int PREVIEW = 1;
	static const int PAUSE = 2;

	int ftype;

	result OnInitializing(void);
	result OnTerminating(void);
	result OnDraw(void);
};

#endif /* CAMERAOVERLAYPANEL_H_ */
