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
#include "PublicFunctions.h"
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
	static const RequestId REQUEST_VIDEOPLAYER = 101;
	static const RequestId REQUEST_BROWSEFORM = 102;
	static const RequestId REQUEST_PLAYLISTFORM = 103;
	static const RequestId REQUEST_BROWSEFORMADDTOPLAYLIST = 104;
	static const RequestId REQUEST_CLIENTSFORM = 105;
	static const RequestId REQUEST_CLIENTSFORMSTART = 106;
	static const RequestId REQUEST_HELPFORM = 107;
	static const RequestId REQUEST_HELPFORMCLIENTS = 108;
	static const RequestId REQUEST_HELPFORMCLIENTSSTART = 109;

	DrawingClass * drawingclass_;

	PublicFunctions * publicfunc_;

protected:
	void SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	Osp::Ui::Controls::Form *__pPreviousForm;

public:
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif /* FORMMGR_H_ */
