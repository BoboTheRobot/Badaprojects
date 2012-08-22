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
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

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
	static const RequestId REQUEST_WELLCOMEFORM = 99;
	static const RequestId REQUEST_MAINFORM = 100;
	static const RequestId REQUEST_CARSELECTFORM = 101;
	static const RequestId REQUEST_ADDEDITCAR = 102;
	static const RequestId REQUEST_FUELTYPEFORM = 103;
	static const RequestId REQUEST_ADDEDITFUELTYPE = 104;
	static const RequestId REQUEST_LOCATIONFORM = 105;
	static const RequestId REQUEST_ADDEDITLOCATION = 106;
	static const RequestId REQUEST_VNOSPORABEFORM = 107;
	static const RequestId REQUEST_PREGLEDFORM = 108;
	static const RequestId REQUEST_SETTINGSFORM = 109;
	static const RequestId REQUEST_HELPFORM = 110;
	static const RequestId REQUEST_EXPENSESFORM = 112;
	static const RequestId REQUEST_EXPENSEADDEDIT = 113;
	static const RequestId REQUEST_DETAILCAR = 114;
	static const RequestId REQUEST_DETAILEXPENSE = 115;
	static const RequestId REQUEST_FUELTYPEDETAIL = 116;
	static const RequestId REQUEST_LOCATIONDETAIL = 117;
	static const RequestId REQUEST_EXPENSEKATFORM = 118;
	static const RequestId REQUEST_ADDEDITEXPENSEKAT = 119;
	static const RequestId REQUEST_EXPENSEKATDETAIL = 120;
	static const RequestId REQUEST_CONFORM = 121;
	static const RequestId REQUEST_DETAILCON = 122;
	static const RequestId REQUEST_CONEDIT = 123;
	static const RequestId REQUEST_PREGLEDEXPENSESFORM = 124;
	static const RequestId REQUEST_PREGLEDCONFORM = 125;
	static const RequestId REQUEST_PREGLEDFUELPRICEFORM = 126;
	static const RequestId REQUEST_DETAILPORABE = 127;
	static const RequestId REQUEST_ADDEDITPORABA = 128;
	static const RequestId REQUEST_TRIPFORM = 129;
	static const RequestId REQUEST_TRIPADDEDIT = 130;
	static const RequestId REQUEST_TRIPDETAIL = 131;

	DrawingClass * drawingclass_;
	ControlHandler * controlhandler_;
	CarConClass * carconclass_;

private:
	static const int COMMONFORMTYPELIST = 1;
	static const int COMMONFORMTYPEADDEDIT = 2;
	static const int COMMONFORMTYPEDETAIL = 3;

protected:
	void SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	void RunCommonFormList(RequestId requestId, Osp::Base::Collection::IList* pArgs, int formtype);
	Osp::Ui::Controls::Form *__pPreviousForm;

public:
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif /* FORMMGR_H_ */
