/*
 * FormMgr.h
 *
 *  Created on: 16.8.2010
 *      Author: mrak
 */

#ifndef FORMMGR_H_
#define FORMMGR_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FSystem.h>
#include "DrawingClass.h"
#include "FGallery.h"

class FormMgr :
	public Osp::Ui::Controls::Form
{
public:
	FormMgr(void);
	virtual ~FormMgr(void);
	result OnInitializing(void);
	result OnTerminating(void);

public:
	bool Initialize(void);
	static const RequestId REQUEST_MAINFORM = 100;
	static const RequestId REQUEST_CAMERAFORM = 101;
	static const RequestId REQUEST_PREVIEWFORM = 102;
	static const RequestId REQUEST_RESTARTCAMERAFORM = 103;
	static const RequestId REQUEST_SELECTFORM = 104;
	static const RequestId REQUEST_GALLERYFORM = 105;

	DrawingClass * drawingclass_;

	Osp::Media::CameraSelection curcamerasel;
	int previewbackgotoform;

protected:
	void SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	Osp::Ui::Controls::Form *__pPreviousForm;
	FGallery *__pGalleryForm;

public:
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif /* FORMMGR_H_ */
