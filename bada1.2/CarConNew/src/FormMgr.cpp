/*
 Copyright (C) 2012  Boštjan Mrak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * FormMgr.cpp
 *
 *  Created on: 16.8.2010
 *      Author: mrak
 */
#include "FormMgr.h"
#include "FMainForm.h"
#include "FCarSelect.h"
#include "FCarAddEdit.h"
#include "FCommonFormList.h"
#include "FCommonFormAddEdit.h"
#include "FVnosPorabe.h"
#include "FPregled.h"
#include "FSettings.h"
#include "FHelp.h"
#include "FStroski.h"
#include "FStroskiAddEdit.h"
#include "FWellcome.h"
#include "FCon.h"
#include "FTrip.h"
#include "FTripAddEdit.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::System;

FormMgr::FormMgr(void) :
__pPreviousForm(null)
{
}

FormMgr::~FormMgr(void)
{
}

bool
FormMgr::Initialize(void)
{
	result r = E_SUCCESS;
	r = Form::Construct(FORM_STYLE_NORMAL);
	SetName(L"FormMgr");

	return true;
}

result
FormMgr::OnInitializing(void)
{
	result r = E_SUCCESS;
	carconclass_ = new CarConClass();
	SettingsData settingsdata_;
	carconclass_->Create(carconclass_->GetLastSelectedID());
	carconclass_->GetSettingsData(settingsdata_);
	drawingclass_ = new DrawingClass(L"/Res/sprite.png");
	controlhandler_ = new ControlHandler();
	controlhandler_->Construct(settingsdata_.currencycountrycode);
	return r;
}

result
FormMgr::OnTerminating(void)
{
	result r = E_SUCCESS;

	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (__pPreviousForm != null)
		pFrame->RemoveControl(*__pPreviousForm);
	__pPreviousForm = null;

	delete drawingclass_;
	controlhandler_->Destroy();
	delete controlhandler_;
	delete carconclass_;

	return r;
}


void FormMgr::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	SwitchToForm(requestId, pArgs);
}

void FormMgr::RunCommonFormList(RequestId requestId, Osp::Base::Collection::IList* pArgs, int formtype) {
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	int autoselectid(0);
	if (pArgs != null) {
		Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
		autoselectid = par->ToInt();
		pArgs->RemoveAll(true);
		delete pArgs;
	}
	if (formtype == COMMONFORMTYPELIST) {
		FCommonFormList* pExeForm = new FCommonFormList(autoselectid, requestId);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	}
	if ((formtype == COMMONFORMTYPEADDEDIT) || (formtype == COMMONFORMTYPEDETAIL)) {
		FCommonFormAddEdit* pExeForm = new FCommonFormAddEdit(autoselectid, requestId, (formtype == COMMONFORMTYPEDETAIL));
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	}
}

void
FormMgr::SwitchToForm(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();

	if (requestId == REQUEST_MAINFORM) {
		FMainForm* pExeForm = new FMainForm();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_CARSELECTFORM) {
		int autoselectid(0);
		int formtype(0);
		if (pArgs != null) {
			Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
			autoselectid = par->ToInt();
			if (pArgs->GetCount() > 1) {
				Integer * p2 = static_cast<Integer*>(pArgs->GetAt(1));
				formtype = p2->ToInt();
			}
			pArgs->RemoveAll(true);
			delete pArgs;
		}
		FCarSelect* pExeForm = new FCarSelect(autoselectid, formtype);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if ((requestId == REQUEST_ADDEDITCAR) || (requestId == REQUEST_DETAILCAR)) {
		int itemid(0);
		int formtype(0);
		if (pArgs != null) {
			Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
			itemid = par->ToInt();
			if (pArgs->GetCount() > 1) {
				Integer * p2 = static_cast<Integer*>(pArgs->GetAt(1));
				formtype = p2->ToInt();
			}
			pArgs->RemoveAll(true);
			delete pArgs;
		}
		FCarAddEdit* pExeForm = new FCarAddEdit(itemid, (requestId == REQUEST_DETAILCAR), formtype);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_FUELTYPEFORM) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPELIST);
	} else if (requestId == REQUEST_ADDEDITFUELTYPE) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPEADDEDIT);
	} else if (requestId == REQUEST_FUELTYPEDETAIL) {
		RunCommonFormList(REQUEST_ADDEDITFUELTYPE, pArgs, COMMONFORMTYPEDETAIL);
	} else if (requestId == REQUEST_LOCATIONFORM) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPELIST);
	} else if (requestId == REQUEST_ADDEDITLOCATION) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPEADDEDIT);
	} else if (requestId == REQUEST_LOCATIONDETAIL) {
		RunCommonFormList(REQUEST_ADDEDITLOCATION, pArgs, COMMONFORMTYPEDETAIL);
	} else if (requestId == REQUEST_EXPENSEKATFORM) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPELIST);
	} else if (requestId == REQUEST_ADDEDITEXPENSEKAT) {
		RunCommonFormList(requestId, pArgs, COMMONFORMTYPEADDEDIT);
	} else if (requestId == REQUEST_EXPENSEKATDETAIL) {
		RunCommonFormList(REQUEST_ADDEDITEXPENSEKAT, pArgs, COMMONFORMTYPEDETAIL);
	} else if ((requestId == REQUEST_VNOSPORABEFORM) || (requestId == REQUEST_DETAILPORABE) || (requestId == REQUEST_ADDEDITPORABA)) {
		int addeditid(0);
		if (pArgs != null) {
		Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
		addeditid = par->ToInt();
		pArgs->RemoveAll(true);
		delete pArgs;
		}
		FVnosPorabe* pExeForm = new FVnosPorabe(addeditid, (requestId == REQUEST_DETAILPORABE), (requestId == REQUEST_VNOSPORABEFORM));
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if ((requestId == REQUEST_PREGLEDFORM) || (requestId == REQUEST_PREGLEDEXPENSESFORM) || (requestId == REQUEST_PREGLEDCONFORM) || (requestId == REQUEST_PREGLEDFUELPRICEFORM)) {
		DateTime fromdate;
		DateTime todate;
		int formtype(0);
		if (requestId == REQUEST_PREGLEDEXPENSESFORM) {
			formtype = 3;
		} else if (requestId == REQUEST_PREGLEDFUELPRICEFORM) {
			formtype = 2;
		} else if (requestId == REQUEST_PREGLEDCONFORM) {
			formtype = 1;
		}
		if (pArgs != null) {
			if (pArgs->GetCount() > 1) {
				DateTime * p1 = static_cast<DateTime*>(pArgs->GetAt(0));
				DateTime * p2 = static_cast<DateTime*>(pArgs->GetAt(1));
				if (pArgs->GetCount() > 2) {
				Integer * p3 = static_cast<Integer*>(pArgs->GetAt(2));
				formtype = p3->ToInt();
				}
				fromdate.SetValue(*p1);
				todate.SetValue(*p2);
			} else {
				Integer * p1 = static_cast<Integer*>(pArgs->GetAt(0));
				formtype = p1->ToInt();
				Osp::System::SystemTime::GetCurrentTime(STANDARD_TIME, fromdate);
				fromdate.AddYears(-1);
				Osp::System::SystemTime::GetCurrentTime(STANDARD_TIME, todate);
			}
			pArgs->RemoveAll(true);
			delete pArgs;
		} else {
			Osp::System::SystemTime::GetCurrentTime(STANDARD_TIME, fromdate);
			fromdate.AddYears(-1);
			Osp::System::SystemTime::GetCurrentTime(STANDARD_TIME, todate);
		}
		FPregled* pExeForm = new FPregled(fromdate, todate, formtype);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_SETTINGSFORM) {
		int formtype(0);
		if (pArgs != null) {
		Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
		formtype = par->ToInt();
		pArgs->RemoveAll(true);
		delete pArgs;
		}
		FSettings* pExeForm = new FSettings(formtype);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
		pExeForm->LoadOnShow();
	} else if (requestId == REQUEST_HELPFORM) {
		FHelp* pExeForm = new FHelp();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_EXPENSESFORM) {
		int autoselectid(0);
		if (pArgs != null) {
			Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
			autoselectid = par->ToInt();
			pArgs->RemoveAll(true);
			delete pArgs;
		}
		FStroski* pExeForm = new FStroski(autoselectid);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if ((requestId == REQUEST_EXPENSEADDEDIT) || (requestId == REQUEST_DETAILEXPENSE)) {
		int addeditid(0);
		if (pArgs != null) {
		Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
		addeditid = par->ToInt();
		pArgs->RemoveAll(true);
		delete pArgs;
		}
		FStroskiAddEdit* pExeForm = new FStroskiAddEdit(addeditid, (requestId == REQUEST_DETAILEXPENSE));
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_WELLCOMEFORM) {
		FWellcome* pExeForm = new FWellcome();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_CONFORM) {
		int autoselectid(0);
		if (pArgs != null) {
			Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
			autoselectid = par->ToInt();
			pArgs->RemoveAll(true);
			delete pArgs;
		}
		FCon* pExeForm = new FCon(autoselectid);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_TRIPFORM) {
		int autoselectid(0);
		if (pArgs != null) {
			Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
			autoselectid = par->ToInt();
			pArgs->RemoveAll(true);
			delete pArgs;
		}
		FTrip* pExeForm = new FTrip(autoselectid);
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if ((requestId == REQUEST_TRIPADDEDIT) || (requestId == REQUEST_TRIPDETAIL)) {
		int addeditid(0);
		if (pArgs != null) {
		Integer * par = static_cast<Integer*>(pArgs->GetAt(0));
		addeditid = par->ToInt();
		pArgs->RemoveAll(true);
		delete pArgs;
		}
		FTripAddEdit* pExeForm = new FTripAddEdit(addeditid, (requestId == REQUEST_TRIPDETAIL));
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	}
	return;
}
