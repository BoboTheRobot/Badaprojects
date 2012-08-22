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
#include "FCarSelect.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Media;

FCarSelect::FCarSelect(int autoselectid, int formtype)
{
	this->autoselectid = autoselectid;
	this->formtype = formtype;
}

FCarSelect::~FCarSelect(void)
{
}

bool
FCarSelect::Initialize()
{
	Form::Construct(L"IDF_FCARSELECT");

	return true;
}

result
FCarSelect::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(1);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	SetSoftkeyActionId(SOFTKEY_1, ID_CANCELSELECT);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	SetSoftkeyActionId(SOFTKEY_0, ID_ADD);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	if (this->formtype == 2) {
		SetSoftkeyText(SOFTKEY_1, L"Next");
	}

	pExList = static_cast<CustomList *>(GetControl(L"IDC_CUSTOMLIST1"));
	pExList->AddCustomItemEventListener(*this);
	pExListmenu_ = new ContextMenu;
	pExListmenu_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	pExListmenu_->AddItem(L"Detail", ID_DETAIL);
	pExListmenu_->AddItem(L"Edit", ID_EDIT);
	pExListmenu_->AddItem(L"Delete", ID_DELETE);
	pExListmenu_->AddActionEventListener(*this);
	pExList->AddTouchEventListener(*this);
	selectedctxmenuitemindex = 0;

	checkboximg = drawingclass_->FDrawSpriteToBmpN(Rectangle(60,600,52,49));
	pCustomListItemFormat = new CustomListItemFormat();
	pCustomListItemFormat->Construct();
	pCustomListItemFormat->AddElement(LEL_BG, Rectangle(0, 0, 355, 100));
	pCustomListItemFormat->AddElement(LEL_COL1, Rectangle(20, 15, 335, 50));
	pCustomListItemFormat->AddElement(LEL_COL2, Rectangle(20, 55, 335, 50), 32);
	pCustomListItemFormat->AddElement(LEL_CBBG, Rectangle(355,0,125,100));
	pCustomListItemFormat->AddElement(LEL_CB, Rectangle(394, 22, 52, 49));
	pCustomListItemFormat->SetElementEventEnabled(LEL_BG,true);
	pCustomListItemFormat->SetElementEventEnabled(LEL_COL1,true);
	pCustomListItemFormat->SetElementEventEnabled(LEL_COL2,true);
	pCustomListItemFormat->SetElementEventEnabled(LEL_CBBG,true);
	pCustomListItemFormat->SetElementEventEnabled(LEL_CB,true);

	carconclass_ = pFormMgr->carconclass_;
	CarData cardata_;
	this->selectedcarid = carconclass_->GetLastSelectedID();
	if (carconclass_->GetCarDataListStart()) {
		while (carconclass_->GetCarDataListGetData(cardata_)) {
			AddListItem(cardata_.Brand, cardata_.Model, cardata_.ID);
		}
		carconclass_->GetCarDataListEnd();
	}
	int selecteditemindex;
	if (this->autoselectid > 0) {
		selecteditemindex = pExList->GetItemIndexFromItemId(this->autoselectid);
	} else {
		selecteditemindex = pExList->GetItemIndexFromItemId(this->selectedcarid);
	}
	if (selecteditemindex > -1) {
		pExList->ScrollToTop(selecteditemindex);
	} else {
		this->selectedcarid = 0;
	}

	return r;
}

class CustomListElement : public ICustomListElement
 {
public:
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     pCanvas->SetLineWidth(1);
     if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    	 pCanvas->FillRectangle(Color(4,58,99), rect);
     }
     pCanvas->SetForegroundColor(Color::COLOR_WHITE);
     pCanvas->DrawLine(Point(rect.x, rect.height - 1), Point(rect.width, rect.height - 1));
     if (rect.width == 125) {
    	 //pCanvas->SetForegroundColor(Color(26,67,99));
    	 pCanvas->DrawLine(Point(rect.x, 0), Point(rect.x, rect.height - 1));
     }

     return r;
 }
};

void
FCarSelect::AddListItem(Osp::Base::String col1, Osp::Base::String col2, int itemId)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElement * custom_element = new CustomListElement();

 newItem->Construct(100);
 newItem->SetItemFormat(*pCustomListItemFormat);

 newItem->SetElement(LEL_COL1, col1);
 newItem->SetElement(LEL_COL2, col2);
 newItem->SetElement(LEL_BG, *(static_cast<ICustomListElement *>(custom_element)));
 newItem->SetElement(LEL_CBBG, *(static_cast<ICustomListElement *>(custom_element)));
 if (itemId == this->selectedcarid) {
	 newItem->SetElement(LEL_CB, *checkboximg, checkboximg);
 }

 pExList->AddItem(*newItem, itemId);

}

result
FCarSelect::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
		pCanvas_->FillRectangle(Color(0,0,0,50), Rectangle(355,0,125,800));
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FCarSelect::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete checkboximg;
	delete pCustomListItemFormat;
	pExListmenu_->RemoveAllItems();
	delete pExListmenu_;

	return r;
}


void
FCarSelect::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if ((actionId == ID_SELECT) || (actionId == ID_CANCELSELECT)) {
		if (actionId == ID_SELECT)
		carconclass_->SetLastSelectedID(this->selectedcarid);
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		if (this->selectedcarid > 0) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->selectedcarid)));
			if (actionId == ID_EDIT) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITCAR, list);
			} else {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_DETAILCAR, list);
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Car not selected!", "Please select car you want to edit.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	} else if (actionId == ID_DELETE) {
		if (this->selectedcarid > 0) {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Delete car?", "If you delete car, you loose all data of that car! Do you really want to delete car?", MSGBOX_STYLE_YESNO, 10000);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				Osp::Ui::Controls::Popup* pPopup_;
				pPopup_ = new Popup();
				Dimension dim(300, 50);
				pPopup_->Construct(true, dim);
				pPopup_->SetTitleText(L"Deleting please wait...");
				pPopup_->SetShowState(true);
				pPopup_->Show();
				carconclass_->DeleteCarData(this->selectedcarid);
				int selecteditemindex = pExList->GetItemIndexFromItemId(this->selectedcarid);
				if (selecteditemindex > -1) {
					pExList->RemoveItemAt(selecteditemindex);
				}
				this->selectedcarid = 0;
				pPopup_->SetShowState(false);
				delete pPopup_;
				RequestRedraw(true);
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Car not selected!", "Please select car you want to delete.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	} else if (actionId == ID_ADD) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITCAR, null);
	}
}

void FCarSelect::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, Osp::Ui::ItemStatus status) {
	//
}

void FCarSelect::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status) {
	if (!pExListmenu_->IsVisible()) {
		if ((elementId == LEL_CBBG) || (elementId == LEL_CB)) {
			this->selectedcarid = itemId;
			OnActionPerformed(source, ID_SELECT);
		}
		if ((elementId == LEL_BG) || (elementId == LEL_COL1) || (elementId == LEL_COL2)) {
			this->selectedcarid = itemId;
			OnActionPerformed(source, ID_DETAIL);
		}
	}
}

void
FCarSelect::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCarSelect::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarSelect::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarSelect::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if(source.GetName() == L"IDC_CUSTOMLIST1") {
		if ((currentPosition.x < 355) && (pExList->GetItemIndexFromPosition(currentPosition) >= 0)) {
			pExListmenu_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y);
			pExListmenu_->SetShowState(true);
			pExListmenu_->Show();
			this->selectedcarid = pExList->GetItemIdAt(pExList->GetItemIndexFromPosition(currentPosition));
		}
	}
}

void
FCarSelect::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarSelect::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCarSelect::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}
