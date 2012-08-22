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
#include "FCameraForm.h"
#include "FPlayer.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::System;
using namespace Osp::Media;


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

	__pPreviousForm = null;

	return true;
}

result
FormMgr::OnInitializing(void)
{
	result r = E_SUCCESS;
	drawingclass_ = new DrawingClass(L"/Res/sprite.png");

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

	return r;
}

void FormMgr::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	SwitchToForm(requestId, pArgs);
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
	} else if (requestId == REQUEST_CAMERAFORM) {
		Integer * arg1 = static_cast<Integer*>(pArgs->GetAt(0));
		int sinterval = arg1->ToInt();
		Osp::Graphics::Dimension * arg2 = static_cast<Osp::Graphics::Dimension*>(pArgs->GetAt(1));
		Osp::Graphics::Dimension sresolution;
		sresolution.SetSize(arg2->width,arg2->height);
		Integer * arg3 = static_cast<Integer*>(pArgs->GetAt(2));
		int sstopafter = arg3->ToInt();
		FCameraForm* pExeForm = new FCameraForm();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		pExeForm->StartCamera(sinterval, sresolution, sstopafter);
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_PLAYERFORM) {
		Osp::Base::String filename = L"";
		bool autoplay = false;
		Osp::Base::String * arg1 = static_cast<Osp::Base::String*>(pArgs->GetAt(0));
		if (pArgs->GetCount() >= 2) {
			Osp::Base::Boolean * arg2 = static_cast<Osp::Base::Boolean*>(pArgs->GetAt(1));
			autoplay = arg2->ToBool();
		}
		filename.Append(*arg1);
		FPlayer* pExeForm = new FPlayer();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		pExeForm->LoadFile(filename, autoplay);
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	}
}
