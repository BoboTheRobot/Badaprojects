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
#include "FBrowseForm.h"
#include "FPlaylistForm.h"
#include "FClientsForm.h"
#include "FHelpForm.h"

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
	publicfunc_ = new PublicFunctions();

	return r;
}

result
FormMgr::OnTerminating(void)
{
	result r = E_SUCCESS;

	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetControl("FMainForm") != null) {
		Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FMainForm"));
		pFrame->RemoveControl(*tmpform);
	}
	if (pFrame->GetControl("FPlaylistForm") != null) {
		Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FPlaylistForm"));
		pFrame->RemoveControl(*tmpform);
	}
	if (pFrame->GetControl("FBrowseForm") != null) {
		Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FBrowseForm"));
		pFrame->RemoveControl(*tmpform);
	}
	if (pFrame->GetControl("FClientsForm") != null) {
		Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FClientsForm"));
		pFrame->RemoveControl(*tmpform);
	}
	if (pFrame->GetControl("FHelpForm") != null) {
		Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FHelpForm"));
		pFrame->RemoveControl(*tmpform);
	}

	delete drawingclass_;
	delete publicfunc_;

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
	if ((requestId == REQUEST_CLIENTSFORM) || (requestId == REQUEST_CLIENTSFORMSTART)) {
		FClientsForm* pExeForm = new FClientsForm((requestId == REQUEST_CLIENTSFORMSTART));
		pExeForm->Initialize();
		pFrame->AddControl(*pExeForm);
		pFrame->SetCurrentForm(*pExeForm);
		pExeForm->Draw();
		pExeForm->Show();
		if (pFrame->GetControl("FMainForm") != null) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FMainForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if (pFrame->GetControl("FPlaylistForm") != null) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FPlaylistForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if (pFrame->GetControl("FBrowseForm") != null) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FBrowseForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if (pFrame->GetControl("FHelpForm") != null) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FHelpForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if (requestId == REQUEST_CLIENTSFORMSTART) {
			pExeForm->StartUpCheck();
		}
	} else {
		if (requestId == REQUEST_MAINFORM) {
			if (pFrame->GetControl("FMainForm") != null) {
				FMainForm* pExeForm = static_cast<FMainForm *>(pFrame->GetControl("FMainForm"));
				pFrame->SetCurrentForm(*pExeForm);
				pExeForm->Draw();
				pExeForm->Show();
			} else {
				FMainForm* pExeForm = new FMainForm();
				pExeForm->Initialize();
				pFrame->AddControl(*pExeForm);
				pFrame->SetCurrentForm(*pExeForm);
				pExeForm->Draw();
				pExeForm->Show();
			}
		} else if ((requestId == REQUEST_BROWSEFORM) || (requestId == REQUEST_BROWSEFORMADDTOPLAYLIST)) {
			FBrowseForm* pExeForm = new FBrowseForm((requestId == REQUEST_BROWSEFORMADDTOPLAYLIST));
			pExeForm->Initialize();
			pFrame->AddControl(*pExeForm);
			pFrame->SetCurrentForm(*pExeForm);
			pExeForm->Draw();
			pExeForm->Show();
		} else if (requestId == REQUEST_PLAYLISTFORM) {
			FPlaylistForm* pExeForm = new FPlaylistForm();
			pExeForm->Initialize();
			pFrame->AddControl(*pExeForm);
			pFrame->SetCurrentForm(*pExeForm);
			pExeForm->Draw();
			pExeForm->Show();
		} else if ((requestId == REQUEST_HELPFORM) || (requestId == REQUEST_HELPFORMCLIENTS) || (requestId == REQUEST_HELPFORMCLIENTSSTART)) {
			FHelpForm* pExeForm = new FHelpForm((requestId == REQUEST_HELPFORMCLIENTS), (requestId == REQUEST_HELPFORMCLIENTSSTART));
			pExeForm->Initialize();
			pFrame->AddControl(*pExeForm);
			pFrame->SetCurrentForm(*pExeForm);
			pExeForm->Draw();
			pExeForm->Show();
		}
		if ((pFrame->GetControl("FPlaylistForm") != null) && (requestId != REQUEST_PLAYLISTFORM)) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FPlaylistForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if ((pFrame->GetControl("FBrowseForm") != null) && (!((requestId == REQUEST_BROWSEFORM) || (requestId == REQUEST_BROWSEFORMADDTOPLAYLIST)))) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FBrowseForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if (pFrame->GetControl("FClientsForm") != null) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FClientsForm"));
			pFrame->RemoveControl(*tmpform);
		}
		if ((pFrame->GetControl("FHelpForm") != null) && (!((requestId == REQUEST_HELPFORM) || (requestId == REQUEST_HELPFORMCLIENTS) || (requestId == REQUEST_HELPFORMCLIENTSSTART)))) {
			Osp::Ui::Controls::Form * tmpform = static_cast<Osp::Ui::Controls::Form *>(pFrame->GetControl("FHelpForm"));
			pFrame->RemoveControl(*tmpform);
		}
	}
}
