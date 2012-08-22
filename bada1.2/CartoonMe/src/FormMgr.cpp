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
#include "FCartoonMe.h"
#include "FCamera.h"
#include "FGallery.h"

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

	curcamerasel = CAMERA_PRIMARY;
	__pGalleryForm = null;
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

	AppLog("test1");

	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (__pPreviousForm != null)
		pFrame->RemoveControl(*__pPreviousForm);
	__pPreviousForm = null;

	AppLog("test2");

	if (__pGalleryForm != null)
		pFrame->RemoveControl(*__pGalleryForm);
	__pGalleryForm = null;

	AppLog("test3");

	if (Osp::Io::File::IsFileExist(L"/Home/capturedimg.jpg")) {
		Osp::Io::File::Remove(L"/Home/capturedimg.jpg");
	}
	if (Osp::Io::File::IsFileExist(L"/Home/drawing.jpg")) {
		Osp::Io::File::Remove(L"/Home/drawing.jpg");
	}

	AppLog("test4");

	delete drawingclass_;

	AppLog("test5");

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

	if ((requestId == REQUEST_MAINFORM) || (requestId == REQUEST_PREVIEWFORM) || (requestId == REQUEST_SELECTFORM)) {
		FCartoonMe* pExeForm = new FCartoonMe();
		if (requestId == REQUEST_PREVIEWFORM) {
			pExeForm->FORMID = pExeForm->FORMID_PREVIEW;
		} else if (requestId == REQUEST_SELECTFORM) {
			pExeForm->FORMID = pExeForm->FORMID_SELECT;
		} else {
			pExeForm->FORMID = pExeForm->FORMID_FIRST;
		}
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_CAMERAFORM) {
		FCamera* pExeForm = new FCamera();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		pExeForm->StartCamera();
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_RESTARTCAMERAFORM) {
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		FCamera* pExeForm = new FCamera();
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		pExeForm->StartCamera();
		__pPreviousForm = pExeForm;
	} else if (requestId == REQUEST_GALLERYFORM) {
		if (__pGalleryForm == null) {
			__pGalleryForm = new FGallery();
			__pGalleryForm->Initialize();
			pFrame->AddControl(*__pGalleryForm);
			pFrame->SetCurrentForm(*__pGalleryForm);
			__pGalleryForm->Draw();
			__pGalleryForm->Show();
			__pGalleryForm->LoadThumbsPage(0);
		} else {
			pFrame->SetCurrentForm(*__pGalleryForm);
			__pGalleryForm->Draw();
			__pGalleryForm->Show();
		}
		if (__pPreviousForm != null)
			pFrame->RemoveControl(*__pPreviousForm);
		__pPreviousForm = null;
	}
}
