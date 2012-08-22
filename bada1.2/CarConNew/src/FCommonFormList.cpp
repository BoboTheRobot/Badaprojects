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
#include "FCommonFormList.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

FCommonFormList::FCommonFormList(int autoselectid, int FormTypeID)
{
	this->autoselectid = autoselectid;
	this->formtypeid = FormTypeID;
}

FCommonFormList::~FCommonFormList(void)
{
}

bool
FCommonFormList::Initialize()
{
	Form::Construct(L"IDF_FCOMMONFORMLIST");

	return true;
}

result
FCommonFormList::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_1, ID_BACK);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	SetSoftkeyActionId(SOFTKEY_0, ID_ADD);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	this->AddTouchEventListener(*this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	pKeypad = new Keypad();
    pKeypad->Construct(KEYPAD_STYLE_NORMAL, KEYPAD_MODE_ALPHA);
    pKeypad->AddTextEventListener(*this);

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

	pCustomListItemFormat = new CustomListItemFormat();
	pCustomListItemFormat->Construct();
	pCustomListItemFormat->AddElement(LEL_BG, Rectangle(0, 0, 480, 80));
	pCustomListItemFormat->SetElementEventEnabled(LEL_BG,true);

	pCustomListItemSearchFormat = new CustomListItemFormat();
	pCustomListItemSearchFormat->Construct();
	pCustomListItemSearchFormat->AddElement(LEL_BG, Rectangle(0, 0, 480, 80));
	pCustomListItemSearchFormat->SetElementEventEnabled(LEL_BG,true);

	carconclass_ = pFormMgr->carconclass_;
	if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
		this->SetTitleText(L"Fuel Types");
		FuelTypeData fueltypedata_;
		if (carconclass_->GetFuelTypeDataListStart()) {
			while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
				AddListItem(fueltypedata_.Caption, fueltypedata_.ID);
			}
			carconclass_->GetFuelTypeDataListEnd();
		}
	}
	if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
		this->SetTitleText(L"Locations");
		LocationData locationdata_;
		if (carconclass_->GetLocationDataListStart()) {
			while (carconclass_->GetLocationDataListGetData(locationdata_)) {
				AddListItem(locationdata_.Caption, locationdata_.ID);
			}
			carconclass_->GetLocationDataListEnd();
		}
	}
	if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
		this->SetTitleText(L"Expense categories");
		CarExpenseKatData expensekatdata_;
		if (carconclass_->GetExpenseKatDataListStart()) {
			while (carconclass_->GetExpenseKatDataListGetData(expensekatdata_)) {
				AddListItem(expensekatdata_.Caption, expensekatdata_.ID);
			}
			carconclass_->GetExpenseKatDataListEnd();
		}
	}
	if (this->autoselectid > 0) {
		this->selecteditemid = this->autoselectid;
	} else {
		this->selecteditemid = 0;
	}
	int selecteditemindex = pExList->GetItemIndexFromItemId(this->selecteditemid);
	if (selecteditemindex > -1) {
		pExList->ScrollToTop(selecteditemindex);
	} else {
		this->selecteditemid = 0;
	}

	return r;
}

void FCommonFormList::SearchAction(Osp::Base::String searchqstr) {
	if (searchqstr != this->searchq) {
		if ((searchqstr.GetLength() < 2) && (searchqstr != "")) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Search", "For search you need to input at least 2 characters!", MSGBOX_STYLE_OK, 10000);
		msgbox.ShowAndWait(modalResult);
		} else {
		this->searchq = searchqstr;
		if (this->searchq.GetLength() > 0)
		this->searchq.Replace("\n"," ");
		pExList->RemoveAllItems();
		int searchwfoundi1(-1);
		Osp::Base::String lowercasedsearchq, lowercased1, tmps;
		bool isresult;
		this->searchq.ToLower(lowercasedsearchq);
		if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
			FuelTypeData fueltypedata_;
			if (carconclass_->GetFuelTypeDataListStart()) {
				while (carconclass_->GetFuelTypeDataListGetData(fueltypedata_)) {
					if (this->searchq.GetLength() > 0) {
						lowercased1 = L"";
						fueltypedata_.Caption.ToLower(lowercased1);
						if (lowercased1 == L"") lowercased1.Append(L" ");
						if ((lowercased1.IndexOf(lowercasedsearchq,0,searchwfoundi1) == E_SUCCESS)) {
							isresult = true;
							if (searchwfoundi1 > 0) {
								tmps = L"";
								lowercased1.SubString(searchwfoundi1-1,1,tmps);
								isresult = (tmps == " ");
							}
							if (isresult)
							AddListItemSearch(fueltypedata_.Caption, fueltypedata_.ID, searchwfoundi1);
						}
					} else {
						AddListItem(fueltypedata_.Caption, fueltypedata_.ID);
					}
				}
				carconclass_->GetFuelTypeDataListEnd();
			}
		}
		if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
			LocationData locationdata_;
			if (carconclass_->GetLocationDataListStart()) {
				while (carconclass_->GetLocationDataListGetData(locationdata_)) {
					if (this->searchq.GetLength() > 0) {
						lowercased1 = L"";
						locationdata_.Caption.ToLower(lowercased1);
						if (lowercased1 == L"") lowercased1.Append(L" ");
						if ((lowercased1.IndexOf(lowercasedsearchq,0,searchwfoundi1) == E_SUCCESS)) {
							isresult = true;
							if (searchwfoundi1 > 0) {
								tmps = L"";
								lowercased1.SubString(searchwfoundi1-1,1,tmps);
								isresult = (tmps == " ");
							}
							if (isresult)
							AddListItemSearch(locationdata_.Caption, locationdata_.ID, searchwfoundi1);
						}
					} else {
						AddListItem(locationdata_.Caption, locationdata_.ID);
					}
				}
				carconclass_->GetLocationDataListEnd();
			}
		}
		if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
			CarExpenseKatData expensekatdata_;
			if (carconclass_->GetExpenseKatDataListStart()) {
				while (carconclass_->GetExpenseKatDataListGetData(expensekatdata_)) {
					if (this->searchq.GetLength() > 0) {
						lowercased1 = L"";
						expensekatdata_.Caption.ToLower(lowercased1);
						if (lowercased1 == L"") lowercased1.Append(L" ");
						if ((lowercased1.IndexOf(lowercasedsearchq,0,searchwfoundi1) == E_SUCCESS)) {
							AddListItemSearch(expensekatdata_.Caption, expensekatdata_.ID, searchwfoundi1);
						}
					} else {
						AddListItem(expensekatdata_.Caption, expensekatdata_.ID);
					}
				}
				carconclass_->GetExpenseKatDataListEnd();
			}
		}
		if (this->searchq.GetLength() > 0) {
			SetSoftkeyActionId(SOFTKEY_1, ID_CLEARSEARCH);
			pExList->SetTextOfEmptyList(L"No search result");
		} else {
			SetSoftkeyActionId(SOFTKEY_1, ID_BACK);
			pExList->SetTextOfEmptyList(L"No items in list.");
		}
		pExList->RequestRedraw();
		this->RequestRedraw(true);
		}
	}
}

class CustomListElementCommon : public ICustomListElement
 {
public:
 Osp::Base::String col1, searchq, col1b, col1s, col1a;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     pCanvas->SetLineWidth(1);
     if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    	 pCanvas->FillRectangle(Color(4,58,99), rect);
     }
     Font fontcol1;
     fontcol1.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 36);

     EnrichedText texteelcol1;
     texteelcol1.Construct(Dimension(460, 40));
     texteelcol1.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     texteelcol1.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
     texteelcol1.SetTextAbbreviationEnabled(true);
     if ((searchq.GetLength() > 0) && (col1s.GetLength() > 0)) {
     TextElement * textelcol1b = new TextElement();
     textelcol1b->Construct(L" ");
     if (col1b.GetLength() > 0) {
     textelcol1b->SetText(col1b);
     textelcol1b->SetTextColor(Color::COLOR_WHITE);
     textelcol1b->SetFont(fontcol1);
     texteelcol1.Add(*textelcol1b);
     }
     TextElement * textelcol1s = new TextElement();
     textelcol1s->Construct(col1s);
     textelcol1s->SetTextColor(Color(237,255,0));
     textelcol1s->SetFont(fontcol1);
     texteelcol1.Add(*textelcol1s);
     TextElement * textelcol1a = new TextElement();
     textelcol1a->Construct(L" ");
     if (col1a.GetLength() > 0) {
     textelcol1a->SetText(col1a);
     textelcol1a->SetTextColor(Color::COLOR_WHITE);
     textelcol1a->SetFont(fontcol1);
     texteelcol1.Add(*textelcol1a);
     }
     pCanvas->DrawText(Point(20,20), texteelcol1);
     delete textelcol1b;
     delete textelcol1s;
     delete textelcol1a;
     } else {
     TextElement textelcol1;
     textelcol1.Construct(col1);
     textelcol1.SetTextColor(Color::COLOR_WHITE);
     textelcol1.SetFont(fontcol1);
     texteelcol1.Add(textelcol1);
     pCanvas->DrawText(Point(20,20), texteelcol1);
     }

     pCanvas->SetForegroundColor(Color::COLOR_WHITE);
     pCanvas->DrawLine(Point(rect.x, rect.height - 1), Point(rect.width, rect.height - 1));
     return r;
 }
};

void
FCommonFormList::AddListItem(Osp::Base::String col1, int itemId)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementCommon * custom_element = new CustomListElementCommon();

 newItem->Construct(80);
 newItem->SetItemFormat(*pCustomListItemFormat);

 custom_element->col1 = col1;
 newItem->SetElement(LEL_BG, *(static_cast<ICustomListElement *>(custom_element)));

 pExList->AddItem(*newItem, itemId);

}

void
FCommonFormList::AddListItemSearch(Osp::Base::String col1, int itemId, int searchposcol1)
{

 CustomListItem * newItem = new CustomListItem();
 CustomListElementCommon * custom_element = new CustomListElementCommon();

 newItem->Construct(80);
 newItem->SetItemFormat(*pCustomListItemSearchFormat);

 custom_element->col1 = col1;
 if (searchposcol1 > -1) {
 if (searchposcol1 > 10) {
 custom_element->col1.SubString(searchposcol1-3,custom_element->col1);
 custom_element->col1.Insert("...",0);
 searchposcol1 = 6;
 }
 custom_element->col1.SubString(0,searchposcol1,custom_element->col1b);
 custom_element->col1.SubString(searchposcol1,this->searchq.GetLength(),custom_element->col1s);
 if (searchposcol1+searchq.GetLength() < custom_element->col1.GetLength())
 custom_element->col1.SubString(searchposcol1+searchq.GetLength(), custom_element->col1a);
 }
 Osp::Base::String lowercasedsearchq;
 this->searchq.ToLower(lowercasedsearchq);
 custom_element->searchq = lowercasedsearchq;
 newItem->SetElement(LEL_BG, *(static_cast<ICustomListElement *>(custom_element)));

 pExList->AddItem(*newItem, itemId);

}

result
FCommonFormList::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete pCustomListItemFormat;
	delete pCustomListItemSearchFormat;
	delete pKeypad;
	delete pExListmenu_;

	return r;
}

result
FCommonFormList::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
		pCanvas_->FillRectangle(Color(0,0,0,50), Rectangle(0,0,480,170));
		pCanvas_->SetForegroundColor(Color::COLOR_WHITE);
		pCanvas_->DrawLine(Point(0, 170), Point(480, 170));
		pCanvas_->FillRoundRectangle(Color::COLOR_WHITE, Rectangle(20,105,440,50), Dimension(10,10));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(30,115,32,32), Rectangle(500,980,32,32));
		if (this->searchq == "") {
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(70,105,380,50),L"Tab here to search", 32, FONT_STYLE_PLAIN, false, Color(200,200,200), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_MIDDLE);
		} else {
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(70,105,380,50),this->searchq, 32, FONT_STYLE_PLAIN, false, Color(20,62,91), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_MIDDLE);
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}


void
FCommonFormList::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		if (this->selecteditemid > 0) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->selecteditemid)));
			if (actionId == ID_EDIT) {
				if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITFUELTYPE, list);
				}
				if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITLOCATION, list);
				}
				if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITEXPENSEKAT, list);
				}
			} else {
				if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_FUELTYPEDETAIL, list);
				}
				if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_LOCATIONDETAIL, list);
				}
				if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
					pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSEKATDETAIL, list);
				}
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Record not selected!", "Please select record you want to edit.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	} else if (actionId == ID_DELETE) {
		if (this->selecteditemid > 0) {
			MessageBox msgbox;
			int modalResult = 0;
			if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
				if (carconclass_->DeleteFuelTypeData(this->selecteditemid, true) == false) {
					msgbox.Construct("Can't delete fuel type!", "Fuel type is selected in reference table!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
				} else {
					msgbox.Construct("Delete fuel type?", "Do you really want to delete fuel type?", MSGBOX_STYLE_YESNO, 10000);
					msgbox.ShowAndWait(modalResult);
					if (modalResult == MSGBOX_RESULT_YES) {
						Osp::Ui::Controls::Popup* pPopup_;
						pPopup_ = new Popup();
						Dimension dim(300, 50);
						pPopup_->Construct(true, dim);
						pPopup_->SetTitleText(L"Deleting please wait...");
						pPopup_->SetShowState(true);
						pPopup_->Show();
						carconclass_->DeleteFuelTypeData(this->selecteditemid);
						int selecteditemindex = pExList->GetItemIndexFromItemId(this->selecteditemid);
						if (selecteditemindex > -1) {
							pExList->RemoveItemAt(selecteditemindex);
						}
						this->selecteditemid = 0;
						pPopup_->SetShowState(false);
						delete pPopup_;
						RequestRedraw(true);
					}
				}
			}
			if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
				if (carconclass_->DeleteLocationData(this->selecteditemid, true) == false) {
					msgbox.Construct("Can't delete location!", "Location is selected in reference table!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
				} else {
					msgbox.Construct("Delete location?", "Do you really want to delete location?", MSGBOX_STYLE_YESNO, 10000);
					msgbox.ShowAndWait(modalResult);
					if (modalResult == MSGBOX_RESULT_YES) {
						Osp::Ui::Controls::Popup* pPopup_;
						pPopup_ = new Popup();
						Dimension dim(300, 50);
						pPopup_->Construct(true, dim);
						pPopup_->SetTitleText(L"Deleting please wait...");
						pPopup_->SetShowState(true);
						pPopup_->Show();
						carconclass_->DeleteLocationData(this->selecteditemid);
						int selecteditemindex = pExList->GetItemIndexFromItemId(this->selecteditemid);
						if (selecteditemindex > -1) {
							pExList->RemoveItemAt(selecteditemindex);
						}
						this->selecteditemid = 0;
						pPopup_->SetShowState(false);
						delete pPopup_;
						RequestRedraw(true);
					}
				}
			}
			if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
				if (carconclass_->DeleteExpenseKatData(this->selecteditemid, true) == false) {
					msgbox.Construct("Can't delete category!", "Category is selected in reference table!", MSGBOX_STYLE_OK, 10000);
					msgbox.ShowAndWait(modalResult);
				} else {
					msgbox.Construct("Delete category?", "Do you really want to delete category?", MSGBOX_STYLE_YESNO, 10000);
					msgbox.ShowAndWait(modalResult);
					if (modalResult == MSGBOX_RESULT_YES) {
						Osp::Ui::Controls::Popup* pPopup_;
						pPopup_ = new Popup();
						Dimension dim(300, 50);
						pPopup_->Construct(true, dim);
						pPopup_->SetTitleText(L"Deleting please wait...");
						pPopup_->SetShowState(true);
						pPopup_->Show();
						carconclass_->DeleteExpenseKatData(this->selecteditemid);
						int selecteditemindex = pExList->GetItemIndexFromItemId(this->selecteditemid);
						if (selecteditemindex > -1) {
							pExList->RemoveItemAt(selecteditemindex);
						}
						this->selecteditemid = 0;
						pPopup_->SetShowState(false);
						delete pPopup_;
						RequestRedraw(true);
					}
				}
			}
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Record not selected!", "Please select record you want to delete.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	} else if (actionId == ID_ADD) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		if (this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITFUELTYPE, null);
		}
		if (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITLOCATION, null);
		}
		if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
		pFormMgr->SendUserEvent(FormMgr::REQUEST_ADDEDITEXPENSEKAT, null);
		}
	} else if (actionId == ID_BACK) {
		if ((this->formtypeid == FormMgr::REQUEST_FUELTYPEFORM) or (this->formtypeid == FormMgr::REQUEST_LOCATIONFORM)) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_SETTINGSFORM, null);
		} else if (this->formtypeid == FormMgr::REQUEST_EXPENSEKATFORM) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_EXPENSESFORM, null);
		} else {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
		}
	} else if (actionId == ID_CLEARSEARCH) {
		SearchAction(L"");
	}
}

void FCommonFormList::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, Osp::Ui::ItemStatus status) {
	//
}

void FCommonFormList::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status) {
	if (!pExListmenu_->IsVisible()) {
		if (elementId == LEL_BG) {
			this->selecteditemid = itemId;
			OnActionPerformed(source, ID_DETAIL);
		}
	}
}

void
FCommonFormList::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FCommonFormList::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormList::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormList::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if(&source==pExList) {
		if (pExList->GetItemIndexFromPosition(currentPosition) >= 0) {
			pExListmenu_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y+pExList->GetBounds().y);
			pExListmenu_->SetShowState(true);
			pExListmenu_->Show();
			this->selecteditemid = pExList->GetItemIdAt(pExList->GetItemIndexFromPosition(currentPosition));
		}
	}
}

void
FCommonFormList::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FCommonFormList::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    if (&source == this) {
	if (Rectangle(20,105,440,50).Contains(currentPosition)) {
		pKeypad->SetText(this->searchq);
		pKeypad->Show();
	}
	}
}

void
FCommonFormList::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void FCommonFormList::OnTextValueChanged(const Osp::Ui::Control& source) {
	SearchAction(pKeypad->GetText());
}

void FCommonFormList::OnTextValueChangeCanceled(const Osp::Ui::Control& source) {

}
