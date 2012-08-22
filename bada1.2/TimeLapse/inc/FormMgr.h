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
	static const RequestId REQUEST_PLAYERFORM = 102;

	DrawingClass * drawingclass_;

protected:
	void SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	Osp::Ui::Controls::Form *__pPreviousForm;

public:
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif /* FORMMGR_H_ */
