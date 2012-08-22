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
#include "FTrip.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Base::Utility;
using namespace Osp::Locales;

FTrip::FTrip(int autoselectid)
{
	this->autoselectid = autoselectid;
}

FTrip::~FTrip(void)
{
}

bool
FTrip::Initialize()
{
	Form::Construct(L"IDF_FTRIP");

	return true;
}

result
FTrip::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(11);
	this->SetTitleIcon(titleiconbmp);
	delete titleiconbmp;

	SetSoftkeyActionId(SOFTKEY_1, ID_BACK);
	AddSoftkeyActionListener(SOFTKEY_1, *this);
	SetSoftkeyActionId(SOFTKEY_0, ID_ADD);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	this->AddTouchEventListener(*this);

	pKeypad = new Keypad();
    pKeypad->Construct(KEYPAD_STYLE_NORMAL, KEYPAD_MODE_ALPHA);
    pKeypad->AddTextEventListener(*this);

	pExList = static_cast<CustomList *>(GetControl(L"IDC_CUSTOMLIST1", true));
	pExList->AddCustomItemEventListener(*this);
	pExListmenu_ = new ContextMenu;
	pExListmenu_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	pExListmenu_->AddItem(L"Detail", ID_DETAIL);
	pExListmenu_->AddItem(L"Edit", ID_EDIT);
	pExListmenu_->AddItem(L"Delete", ID_DELETE);
	pExListmenu_->AddActionEventListener(*this);
	pExList->AddTouchEventListener(*this);
	selectedctxmenuitemindex = 0;

	carconclass_ = pFormMgr->carconclass_;

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	pCustomListItemFormat = new CustomListItemFormat();
	pCustomListItemFormat->Construct();
	pCustomListItemFormat->AddElement(LEL_BG, Rectangle(0, 0, 480, 100));
	pCustomListItemFormat->SetElementEventEnabled(LEL_BG,true);

	pCustomListItemSearchFormat = new CustomListItemFormat();
	pCustomListItemSearchFormat->Construct();
	pCustomListItemSearchFormat->AddElement(LEL_BG, Rectangle(0, 0, 480, 150));
	pCustomListItemSearchFormat->SetElementEventEnabled(LEL_BG,true);

	TripData data_;
	String expenses, tripdate, triplocation;
	if (carconclass_->GetTripDataListStart(carconclass_->SelectedCar.ID)) {
		while (carconclass_->GetTripDataListGetData(data_)) {
			expenses = L"";
			tripdate = L"";
			triplocation = L"";
			if (data_.RealExpenses > 0) {
				expenses = controlhandler_->CurrencyFormater(data_.RealExpenses);
			} else {
				expenses = L"*" + controlhandler_->CurrencyFormater(data_.CalcExpenses);
			}
			tripdate = controlhandler_->DateFormater(data_.StartTime, true);
			if ((data_.EndTime != data_.StartTime) && (data_.EndTime > data_.EndTime.GetMinValue())) {
				tripdate = tripdate + L" - " + controlhandler_->DateFormater(data_.EndTime, true);
			}
			triplocation = data_.StartLocationCaption;
			if ((data_.EndLocation != data_.StartLocation) && (data_.EndLocationCaption != L"")) {
				triplocation = triplocation + L" - " + data_.EndLocationCaption;
			}
			AddListItem(triplocation, tripdate, expenses, data_.ID);
		}
		carconclass_->GetTripDataListEnd();
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

void FTrip::SearchAction(Osp::Base::String searchqstr) {
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
		TripData data_;
		String expenses, tripdate, triplocation;
		int searchwfoundi1(-1),searchwfoundi2(-1);
		Osp::Base::String lowercasedsearchq, lowercased1, lowercased2, tmps;
		bool isresult;
		this->searchq.ToLower(lowercasedsearchq);
		if (carconclass_->GetTripDataListStart(carconclass_->SelectedCar.ID)) {
			while (carconclass_->GetTripDataListGetData(data_)) {
				expenses = L"";
				tripdate = L"";
				triplocation = L"";
				if (data_.RealExpenses > 0) {
					expenses = controlhandler_->CurrencyFormater(data_.RealExpenses);
				} else {
					expenses = L"*" + controlhandler_->CurrencyFormater(data_.CalcExpenses);
				}
				tripdate = controlhandler_->DateFormater(data_.StartTime, true);
				if ((data_.EndTime != data_.StartTime) && (data_.EndTime > data_.EndTime.GetMinValue())) {
					tripdate = tripdate + L" - " + controlhandler_->DateFormater(data_.EndTime, true);
				}
				triplocation = data_.StartLocationCaption;
				if ((data_.EndLocation != data_.StartLocation) && (data_.EndLocationCaption != L"")) {
					triplocation = triplocation + L" - " + data_.EndLocationCaption;
				}
				if (this->searchq.GetLength() > 0) {
					lowercased1 = L"";
					lowercased2 = L"";
					triplocation.ToLower(lowercased1);
					data_.Remark.ToLower(lowercased2);
					if (lowercased1 == L"") lowercased1.Append(L" ");
					if (lowercased2 == L"") lowercased2.Append(L" ");
					if ((lowercased1.IndexOf(lowercasedsearchq,0,searchwfoundi1) == E_SUCCESS) || (lowercased2.IndexOf(lowercasedsearchq,0,searchwfoundi2) == E_SUCCESS)) {
						isresult = true;
						if (searchwfoundi1 > 0) {
							tmps = L"";
							lowercased1.SubString(searchwfoundi1-1,1,tmps);
							isresult = (tmps == " ");
						}
						if (searchwfoundi2 > 0) {
							tmps = L"";
							lowercased2.SubString(searchwfoundi2-1,1,tmps);
							isresult = (tmps == " ");
						}
						if (isresult)
						AddListItemSearch(triplocation, tripdate, expenses, data_.Remark, data_.ID, searchwfoundi1, searchwfoundi2);
					}
				} else {
					AddListItem(triplocation, tripdate, expenses, data_.ID);
				}
			}
			carconclass_->GetTripDataListEnd();
		}
		pExList->RequestRedraw();
		if (this->searchq.GetLength() > 0) {
			SetSoftkeyActionId(SOFTKEY_1, ID_CLEARSEARCH);
			pExList->SetTextOfEmptyList(L"No search result");
		} else {
			SetSoftkeyActionId(SOFTKEY_1, ID_BACK);
			pExList->SetTextOfEmptyList(L"No items in list.");
		}
		this->RequestRedraw(true);
		}
	}
}

class CustomListElementTrip : public ICustomListElement
 {
public:
 Osp::Base::String col1, col2, col3, col4, searchq, col1b, col1s, col1a, col4b, col4s, col4a;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     pCanvas->SetLineWidth(1);
     if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
    	 pCanvas->FillRectangle(Color(4,58,99), rect);
     }
     pCanvas->SetForegroundColor(Color::COLOR_WHITE);

     Font fontcol1;
     fontcol1.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 36);
     Font fontcol2;
     fontcol2.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 28);
     Font fontcol3;
     fontcol3.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 36);
     Font fontcol4;
     fontcol4.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 28);

     EnrichedText texteelcol1;
     texteelcol1.Construct(Dimension(280, 50));
     texteelcol1.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     texteelcol1.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_TOP);
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
     pCanvas->DrawText(Point(20,15), texteelcol1);
     delete textelcol1b;
     delete textelcol1s;
     delete textelcol1a;
     } else {
     TextElement textelcol1;
     textelcol1.Construct(col1);
     textelcol1.SetTextColor(Color::COLOR_WHITE);
     textelcol1.SetFont(fontcol1);
     texteelcol1.Add(textelcol1);
     pCanvas->DrawText(Point(20,15), texteelcol1);
     }

	 EnrichedText texteelcol2;
     texteelcol2.Construct(Dimension(280, 40));
     texteelcol2.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     texteelcol2.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_TOP);
     texteelcol2.SetTextAbbreviationEnabled(true);
     TextElement textelcol2;
     textelcol2.Construct(col2);
     if (itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) {
     textelcol2.SetTextColor(Color::COLOR_WHITE);
     } else {
     textelcol2.SetTextColor(Color(10, 73, 136));
     }
     textelcol2.SetFont(fontcol2);
     texteelcol2.Add(textelcol2);
     if ((searchq.GetLength() > 0) && (col4.GetLength() > 0)) {
    	 pCanvas->DrawText(Point(20,53), texteelcol2);
     } else {
    	 pCanvas->DrawText(Point(20,60), texteelcol2);
     }

     EnrichedText texteelcol3;
     texteelcol3.Construct(Dimension(160, 70));
     texteelcol3.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_RIGHT);
     texteelcol3.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
     TextElement textelcol3;
     textelcol3.Construct(col3);
     textelcol3.SetTextColor(Color::COLOR_WHITE);
     textelcol3.SetFont(fontcol3);
     texteelcol3.Add(textelcol3);
	 pCanvas->DrawText(Point(300,15), texteelcol3);

	 if ((searchq.GetLength() > 0) && (col4.GetLength() > 0)) {
	 EnrichedText texteelcol4;
     texteelcol4.Construct(Dimension(430, 60));
     texteelcol4.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     texteelcol4.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_TOP);
     texteelcol4.SetTextAbbreviationEnabled(true);
     texteelcol4.SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
     if ((searchq.GetLength() > 0) && (col4s.GetLength() > 0)) {
     TextElement * textelcol4b = new TextElement();
     textelcol4b->Construct(L" ");
     if (col4b.GetLength() > 0) {
     textelcol4b->SetText(col4b);
     textelcol4b->SetTextColor(Color(220, 220, 220));
     textelcol4b->SetFont(fontcol4);
     texteelcol4.Add(*textelcol4b);
     }
     TextElement * textelcol4s = new TextElement();
     textelcol4s->Construct(col4s);
     textelcol4s->SetTextColor(Color(237,255,0));
     textelcol4s->SetFont(fontcol4);
     texteelcol4.Add(*textelcol4s);
     TextElement * textelcol4a = new TextElement();
     textelcol4a->Construct(L" ");
     if (col4a.GetLength() > 0) {
     textelcol4a->SetText(col4a);
     textelcol4a->SetTextColor(Color(220, 220, 220));
     textelcol4a->SetFont(fontcol4);
     texteelcol4.Add(*textelcol4a);
     }
     pCanvas->DrawText(Point(20,85), texteelcol4);
     delete textelcol4b;
     delete textelcol4s;
     delete textelcol4a;
     } else {
     TextElement textelcol4;
     textelcol4.Construct(col4);
     textelcol4.SetTextColor(Color(220, 220, 220));
     textelcol4.SetFont(fontcol4);
     texteelcol4.Add(textelcol4);
	 pCanvas->DrawText(Point(20,85), texteelcol4);
     }
	 }

     pCanvas->DrawLine(Point(rect.x, rect.height - 1), Point(rect.width, rect.height - 1));
     return r;
 }
};

void
FTrip::AddListItem(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId)
{

 CustomListItem * newItem = new CustomListItem();
 CustomListElementTrip * custom_element = new CustomListElementTrip();

 newItem->Construct(100);
 newItem->SetItemFormat(*pCustomListItemFormat);
 custom_element->col1 = col1;
 custom_element->col2 = col2;
 custom_element->col3 = col3;
 newItem->SetElement(LEL_BG, *(static_cast<ICustomListElement *>(custom_element)));

 pExList->AddItem(*newItem, itemId);

}

void
FTrip::AddListItemSearch(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, Osp::Base::String col4, int itemId, int searchposcol1, int searchposcol2)
{

 CustomListItem * newItem = new CustomListItem();
 CustomListElementTrip * custom_element = new CustomListElementTrip();

 newItem->Construct(150);
 newItem->SetItemFormat(*pCustomListItemSearchFormat);

 custom_element->col1 = col1;
 custom_element->col2 = col2;
 custom_element->col3 = col3;
 custom_element->col4 = col4;
 if (custom_element->col4.GetLength() > 0)
 custom_element->col4.Replace("\n"," ");
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
 if (searchposcol2 > -1) {
 if (searchposcol2 > 10) {
 custom_element->col4.SubString(searchposcol2-3,custom_element->col4);
 custom_element->col4.Insert("...",0);
 searchposcol2 = 6;
 }
 custom_element->col4.SubString(0,searchposcol2,custom_element->col4b);
 custom_element->col4.SubString(searchposcol2,this->searchq.GetLength(),custom_element->col4s);
 if (searchposcol2+searchq.GetLength() < custom_element->col4.GetLength())
 custom_element->col4.SubString(searchposcol2+searchq.GetLength(), custom_element->col4a);
 }
 Osp::Base::String lowercasedsearchq;
 this->searchq.ToLower(lowercasedsearchq);
 custom_element->searchq = lowercasedsearchq;
 newItem->SetElement(LEL_BG, *(static_cast<ICustomListElement *>(custom_element)));

 pExList->AddItem(*newItem, itemId);

}

result
FTrip::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete pKeypad;
	delete pCustomListItemFormat;
	delete pCustomListItemSearchFormat;
	delete pExListmenu_;
	controlhandler_->Clear();

	return r;
}

result
FTrip::OnDraw() {
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
FTrip::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if ((actionId == ID_EDIT) || (actionId == ID_DETAIL)) {
		if (this->selecteditemid > 0) {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			ArrayList * list = new ArrayList;
			list->Construct();
			list->Add(*(new Integer(this->selecteditemid)));
			if (actionId == ID_EDIT) {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_TRIPADDEDIT, list);
			} else {
				pFormMgr->SendUserEvent(FormMgr::REQUEST_TRIPDETAIL, list);
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
			if (carconclass_->DeleteTripData(this->selecteditemid, true) == false) {
				msgbox.Construct("Can't delete trip!", "Trip is selected in reference table!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
			} else {
				msgbox.Construct("Delete trip?", "Do you really want to delete trp?", MSGBOX_STYLE_YESNO, 10000);
				msgbox.ShowAndWait(modalResult);
				if (modalResult == MSGBOX_RESULT_YES) {
					Osp::Ui::Controls::Popup* pPopup_;
					pPopup_ = new Popup();
					Dimension dim(300, 50);
					pPopup_->Construct(true, dim);
					pPopup_->SetTitleText(L"Deleting please wait...");
					pPopup_->SetShowState(true);
					pPopup_->Show();
					carconclass_->DeleteTripData(this->selecteditemid);
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
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Record not selected!", "Please select record you want to delete.", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	} else if (actionId == ID_ADD) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_TRIPADDEDIT, null);
	} else if (actionId == ID_BACK) {
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ID_CLEARSEARCH) {
		SearchAction(L"");
	}
}

void FTrip::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, Osp::Ui::ItemStatus status) {
	//
}

void FTrip::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status) {
	if (!pExListmenu_->IsVisible()) {
		if (elementId == LEL_BG) {
			this->selecteditemid = itemId;
			OnActionPerformed(source, ID_DETAIL);
		}
	}
}

void
FTrip::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FTrip::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTrip::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FTrip::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
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
FTrip::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FTrip::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if (&source == this) {
	if (Rectangle(20,105,440,50).Contains(currentPosition)) {
		pKeypad->SetText(this->searchq);
		pKeypad->Show();
	}
	}
}

void
FTrip::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void FTrip::OnTextValueChanged(const Osp::Ui::Control& source) {
	SearchAction(pKeypad->GetText());
}

void FTrip::OnTextValueChangeCanceled(const Osp::Ui::Control& source) {

}
