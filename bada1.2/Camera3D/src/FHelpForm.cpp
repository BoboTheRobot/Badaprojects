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
#include "FHelpForm.h"
#include "FormMgr.h"
#include "TextViewLabel.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

FHelpForm::FHelpForm(void)
{
}

FHelpForm::~FHelpForm(void)
{
}

bool
FHelpForm::Initialize()
{
	Form::Construct(L"IDF_FHELP");

	return true;
}

result
FHelpForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_0, ABACK);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	Tab* pTab = GetTab();
	pTab->AddItem(L"How to", TABHOWTO);
	pTab->AddItem(L"Gallery", TABVIEW);
	//pTab->AddItem(L"About", TABABOUT);
	pTab->AddActionEventListener(*this);
	pTab->SetSelectedItem(0);

	pTabHowToScrollPanel = new ScrollPanel();
	pTabHowToScrollPanel->Construct(Rectangle(0,0,800,317));
	pTabHowToScrollPanel->SetName(L"TABHOWTO");
	Label * pTabHowToTextLabel = new Label();
	Osp::Base::String howtotext = L"";
	howtotext.Append(L"When you open camera form (you can open camera from main form with camera capture button), camera will start previewing image for left eye.\n");
	howtotext.Append(L"You choose your object and press hardware camera capture button to capture left eye's picture.\n");
	howtotext.Append(L"Then you move camera slightly to right (approx. 4cm - depends of distance of object) and press camera capture button again.\n");
	howtotext.Append(L"In last step you are able to align right image to get best 3D image possible (arrow buttons on right bottom of screen).\n");
	howtotext.Append(L"The last step is in analygraph red/cyan mode, so you can see actual result.\n");
	howtotext.Append(L"When you are satisfied with image you save it with pressing on save button (left bottom of screen).");
	pTabHowToTextLabel->Construct(Rectangle(20,20,760,480), howtotext);
	pTabHowToTextLabel->SetTextConfig(30, LABEL_TEXT_STYLE_NORMAL);
	pTabHowToTextLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pTabHowToTextLabel->SetTextVerticalAlignment(ALIGNMENT_TOP);
	pTabHowToScrollPanel->AddControl(*pTabHowToTextLabel);
	this->AddControl(*pTabHowToScrollPanel);

	pTabViewScrollPanel = new ScrollPanel();
	pTabViewScrollPanel->Construct(Rectangle(0,0,800,317));
	pTabHowToScrollPanel->SetName(L"TABVIEW");
	Label * pTabViewTextLabel = new Label();
	Osp::Base::String viewtext = L"";
	viewtext.Append(L"You can browse and view pictures in library by open gallery form.\n");
	viewtext.Append(L"You navigate through pages with prev/next buttons (bottom middle of screen) or with hardware buttons for volume.\n");
	viewtext.Append(L"Pictures in thumbnails are only left eye pictures, not 3D.\n");
	viewtext.Append(L"You open full view of 3D picture with press on thumbnail.\n");
	viewtext.Append(L"You can delete picture with delete button (bottom right of screen) or you can export picture to your phone's library with export button (top right of screen).\n");
	pTabViewTextLabel->Construct(Rectangle(20,20,760,480), viewtext);
	pTabViewTextLabel->SetTextConfig(30, LABEL_TEXT_STYLE_NORMAL);
	pTabViewTextLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	pTabViewTextLabel->SetTextVerticalAlignment(ALIGNMENT_TOP);
	pTabViewScrollPanel->AddControl(*pTabViewTextLabel);
	pTabViewScrollPanel->SetShowState(false);
	this->AddControl(*pTabViewScrollPanel);

	return r;
}

result
FHelpForm::OnTerminating(void)
{
	result r = E_SUCCESS;



	return r;
}


void
FHelpForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
	if (actionId == TABHOWTO) {
		pTabViewScrollPanel->SetShowState(false);
		pTabHowToScrollPanel->SetShowState(true);
		pTabHowToScrollPanel->ScrollToTop();
		RequestRedraw(true);
	}
	if (actionId == TABVIEW) {
		pTabHowToScrollPanel->SetShowState(false);
		pTabViewScrollPanel->SetShowState(true);
		pTabViewScrollPanel->ScrollToTop();
		RequestRedraw(true);
	}
}

