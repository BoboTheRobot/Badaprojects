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
#include <FGraphics.h>
#include <FNet.h>
#include "DrawingClass.h"

class FormMgr :
	public Osp::Ui::Controls::Form,
	public Osp::Base::Runtime::ITimerEventListener
{
public:
	FormMgr(void);
	virtual ~FormMgr(void);
	result OnInitializing(void);
	result OnTerminating(void);

public:
	bool Initialize(void);
	static const RequestId REQUEST_MAINFORM = 100;
	static const RequestId REQUEST_SETTINGS = 101;
	static const RequestId REQUEST_FIRSTSTART = 102;
	static const RequestId REQUEST_ABOUT = 103;
	static const RequestId REQUEST_RESETC = 104;
	static const RequestId REQUEST_RESETCNOW = 105;

	static const int CALLBACKDONE = 1;

	static const int TRANSITIONRIGHT = 1;
	static const int TRANSITIONLEFT = 2;

	int TransitionDirection;

	DrawingClass * drawingclass_;

	Osp::Graphics::Bitmap * prevform;
	Osp::Graphics::Bitmap * curform;

	Osp::Base::Runtime::Timer * anitimer_;
	long long anilasttime, anicurtime;
	double anistep, anistepc;

	Osp::Base::Runtime::Timer * resetcshutdown_;

protected:
	void SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	Osp::Ui::Controls::Form *__pPreviousForm;

public:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	virtual result OnDraw();
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif /* FORMMGR_H_ */
