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
#include "FHelp.h"
#include "FormMgr.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;


FHelp::FHelp(void)
{
}

FHelp::~FHelp(void)
{
}

bool
FHelp::Initialize()
{
	Form::Construct(L"IDF_FHELP");

	return true;
}

result
FHelp::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_0, ID_ABOUTBTN);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	SetSoftkeyActionId(SOFTKEY_1, ID_BACKBTN);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	Tab* pTab = GetTab();
	pTab->AddItem(L"Home", TABHOME);
	pTab->AddItem(L"Browse", TABBROWSE);
	pTab->AddItem(L"Add/edit", TABEDIT);
	pTab->AddItem(L"Fill up", TABFILLUP);
	pTab->AddItem(L"Trip", TABTRIP);
	pTab->AddItem(L"Reports", TABREPORTS);
	pTab->AddActionEventListener(*this);
	this->SetTitleText(L"Help about home screen",ALIGNMENT_LEFT);
	pTab->SetSelectedItem(0);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(3);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	pPopup_ = new Popup();
	pPopup_->Construct(L"IDP_POPUP4");
	Osp::Ui::Controls::Button *pPopupCloseButton_;
	pPopupCloseButton_ = static_cast<Button *>(pPopup_->GetControl(L"IDC_BUTTON1"));
	pPopupCloseButton_->SetActionId(ID_CLOSEABOUT);
	pPopupCloseButton_->AddActionEventListener(*this);

	return r;
}

result
FHelp::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
		Tab* pTab = GetTab();
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABHOME) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,146), L"All main menu buttons have tooltip that shows when you touch the button. You can move over all buttons for tooltip change, action of button is executed only when you release the button.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+146,440,35), L"Ecology view", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+146+35,400,120), L"Shows you avg.consumption and last fill up data. Depending of inc./dec. of last con. data shows green or red arrow on right side of number.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+146+155,440,35), L"Economy view", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+146+155+35,400,148), L"Shows you last fuel price and calculated assumption expenses vs. distance. This calculation depends of your avg.consumption and fuel price.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABBROWSE) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,146), L"All browse forms are basically the same, showing only most important data in list. If you tap on item in list it will open detail view of that record. You can open context menu for more actions on record.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+146,440,35), L"Search", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+146+35,400,120), L"Tap above the list on field marked with search icon. Input search keyword and press done. Search needs at least two characters to input.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+146+155,440,35), L"Car select", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+146+155+35,400,148), L"List for cars are little different. On right you have darker blue background. Tap on this area will select car. Selected car is indicated with white checked icon.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABEDIT) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,117), L"All input fields are self formatting when not in focus, so you don't need to worry about valid input. You input decimal point with * or # characters on keypad.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+117,440,35), L"Detail/Edit", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+117+35,400,148), L"When you open record from list is opened in detail view (greener background). You can't edit fields in that view. For edit you press Edit softkey.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+117+183,440,35), L"Delete", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+117+183+35,400,170), L"You can delete records from list (browse form) by opening context menu (long press on list item). Not perfect but more safe, because you don't want to delete valuable record by mistake.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABFILLUP) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,102), L"You input your data after you fill up your tank. For accurate calculation is required to fill up your tank to full!", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+102,440,35), L"What is partial fill up?", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+102+35,400,63), L"You select this option when you didn't fill up your tank to full.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+102+108,440,35), L"What is missed fill up?", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+102+108+35,400,83), L"You select this option when you forgot or missed to input previous record in application.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABTRIP) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,800-GetClientAreaBounds().y-40), L"Trip helps you when you need to calculate expenses on a trip.\nYou can use this tool for shared expenses of trip when more passengers share a ride, or for your own evidence of business or personal trips.\nCalculated expenses are based on fuel price and your avg. consumption.\n\nFor record you need to input only departure and arrival odometer, and number of passenger.\n\nYou need to have at least one valid fill up record for calculation to work!", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABREPORTS) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20,440,35), L"Graphs", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+35,400,90), L"Shows you graphs of records. On double tap opens full screen view. You close full screen view with double tap.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,GetClientAreaBounds().y+20+125,440,35), L"Filter", 28, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(40,GetClientAreaBounds().y+20+125+35,400,300), L"Default filter is one year history from today. You can change range of data to view by selecting 'To date' and 'From date'. The buttons between date selectors helps you select range by setting 'From date' to one month, quarter of year, half year or year in history from 'To date'.", 28, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);

		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FHelp::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete pPopup_;

	return r;
}


void
FHelp::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (actionId == ID_BACKBTN) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		if (pFormMgr != null)
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ID_CLOSEABOUT) {
		pPopup_->SetShowState(false);
		RequestRedraw(true);
	} else if (actionId == ID_ABOUTBTN) {
		pPopup_->SetShowState(true);
		pPopup_->Show();
	} else if (actionId == TABHOME) {
		this->SetTitleText(L"Help about home screen",ALIGNMENT_LEFT);
		RequestRedraw(true);
	} else if (actionId == TABBROWSE) {
		this->SetTitleText(L"Help about browse forms",ALIGNMENT_LEFT);
		RequestRedraw(true);
	} else if (actionId == TABEDIT) {
		this->SetTitleText(L"Help about record add/edit forms",ALIGNMENT_LEFT);
		RequestRedraw(true);
	} else if (actionId == TABFILLUP) {
		this->SetTitleText(L"Help about fill up's",ALIGNMENT_LEFT);
		RequestRedraw(true);
	} else if (actionId == TABTRIP) {
		this->SetTitleText(L"Help about trip managing",ALIGNMENT_LEFT);
		RequestRedraw(true);
	} else if (actionId == TABREPORTS) {
		this->SetTitleText(L"Help about report forms",ALIGNMENT_LEFT);
		RequestRedraw(true);
	}
}

