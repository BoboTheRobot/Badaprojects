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
#include "FPlaylistForm.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"
#include "FMainForm.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

FPlaylistForm::FPlaylistForm(void)
{
}

FPlaylistForm::~FPlaylistForm(void)
{
}

bool
FPlaylistForm::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL|FORM_STYLE_INDICATOR);
	this->SetName(L"FPlaylistForm");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FPlaylistForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;
	publicfunc_ = pFormMgr->publicfunc_;

	UList_ = new CustomList();
	UList_->Construct(Rectangle(0,50,480,572), CUSTOM_LIST_STYLE_NORMAL, true);
    UList_->SetTextColorOfEmptyList(Color::COLOR_WHITE);
    UList_->SetTextOfEmptyList(L"Refreshing...");
    UList_->AddCustomItemEventListener(*this);

    __pCustomListItemFormat = new CustomListItemFormat();
    __pCustomListItemFormat->Construct();
    __pCustomListItemFormat->AddElement(ID_LIST_TEXTARTIST, Osp::Graphics::Rectangle(55, 5, 420, 35));
    __pCustomListItemFormat->AddElement(ID_LIST_TEXTTITLE, Osp::Graphics::Rectangle(55, 40, 420, 35));
    __pCustomListItemFormat->AddElement(ID_LIST_ICON, Osp::Graphics::Rectangle(0, 15, 50, 50));
    __pCustomListItemFormat->SetElementEventEnabled(ID_LIST_TEXTARTIST, true);
    __pCustomListItemFormat->SetElementEventEnabled(ID_LIST_TEXTTITLE, true);
    __pCustomListItemFormat->SetElementEventEnabled(ID_LIST_ICON, true);

    UListContextmenu_ = new ContextMenu;
	UListContextmenu_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	UListContextmenu_->AddItem(L"Play", APLAY);
	UListContextmenu_->AddItem(L"Remove", AREMOVE);
	UListContextmenu_->AddActionEventListener(*this);

	UList_->AddTouchEventListener(*this);

	curselitemid = 0;
	curplayingitemid = -1;

    AddControl(*UList_);

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * addbtn = new ButtonCostumEl(Rectangle(12,667,128,124), ABACK, 10, BOXBTN, L"Back");
	buttons_->Add(*addbtn);
	ButtonCostumEl * emptybtn = new ButtonCostumEl(Rectangle(140,667,128,124), AADD, 14, BOXBTN, L"Add");
	buttons_->Add(*emptybtn);
	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(268,667,128,124), ACLEAR, 11, BOXBTN, L"Clear");
	buttons_->Add(*backbtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

    Image* image_ = new Image();
    image_->Construct();
    currenticon = image_->DecodeN(L"/Res/playlist_cur.png", BITMAP_PIXEL_FORMAT_ARGB8888);

    delete image_;

    if (publicfunc_->iswificonnected) {
    	LoadPlaylist();
    }

	return r;
}

class CustomListElement : public ICustomListElement
 {
public:
 Osp::Base::String text;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     Font textfont;
     textfont.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 28);

     EnrichedText texteel;
     texteel.Construct(Dimension(rect.width, rect.height));
   	 texteel.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     texteel.SetVerticalAlignment(Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
     texteel.SetTextWrapStyle(TEXT_WRAP_CHARACTER_WRAP);
     texteel.SetTextAbbreviationEnabled(true);
     TextElement textelcol;
     textelcol.Construct(text);
     textelcol.SetTextColor(Color::COLOR_WHITE);
     textelcol.SetFont(textfont);
     texteel.Add(textelcol);
     pCanvas->DrawText(Point(0,0), texteel);

     return r;
 }
};

result FPlaylistForm::AddListItem(int itemid, String itemTextArtist, String itemTextTitle, Bitmap* pBitmapNormal) {
    CustomListItem* pItem = new CustomListItem();
    CustomListElement * custom_element = new CustomListElement();
    CustomListElement * custom_element2 = new CustomListElement();
    custom_element->text = itemTextArtist;
    custom_element2->text = itemTextTitle;

    pItem->Construct(80);
    pItem->SetItemFormat(*__pCustomListItemFormat);
    if (pBitmapNormal != null) {
    	pItem->SetElement(ID_LIST_TEXTARTIST, *(static_cast<ICustomListElement *>(custom_element)));
    	pItem->SetElement(ID_LIST_TEXTTITLE, *(static_cast<ICustomListElement *>(custom_element2)));
    	pItem->SetElement(ID_LIST_ICON, *pBitmapNormal, pBitmapNormal);
    } else {
    	pItem->SetElement(ID_LIST_TEXTARTIST, *(static_cast<ICustomListElement *>(custom_element)));
    	pItem->SetElement(ID_LIST_TEXTTITLE, *(static_cast<ICustomListElement *>(custom_element2)));
    }

    UList_->AddItem(*pItem, itemid);

    return E_SUCCESS;
}

void FPlaylistForm::FillListItems() {
	curplayingitemid = -1;
	int curplayingitemindex = -1;
	UList_->SetTextOfEmptyList(L"Empty playlist");
	Osp::Base::Collection::IEnumerator * pEnum;
	FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
	if (pMainForm->isstreaming) {
		pEnum = pMainForm->playliststream->GetEnumeratorN();
	} else {
		pEnum = publicfunc_->playlist->GetEnumeratorN();
	}
	PlayListItem * playlistitem_ = null;
	int listitemindex = 1;
	while (pEnum->MoveNext() == E_SUCCESS) {
		playlistitem_ = static_cast<PlayListItem *> (pEnum->GetCurrent());
		if (pMainForm->isstreaming) {
			playlistitem_->iscurrent = (pMainForm->streamingcurrentitemindex == (listitemindex-1));
		}
		Osp::Base::String artist = playlistitem_->artist;
		Osp::Base::String title = playlistitem_->title;
		artist.Trim();
		title.Trim();
		if ((title == L"") && (artist == L"")) {
			title = publicfunc_->GetTitleFromUri(playlistitem_->uri);
			artist = "Unknown artist - without metadata";
		}
		if (artist == L"") {
			artist = L"Unknown artist";
		}
		if (title == L"") {
			title = L"Unknown title";
		}
		if (playlistitem_->iscurrent) {
			AddListItem(listitemindex, artist, title, currenticon);
			curplayingitemid = listitemindex;
			curplayingitemindex = listitemindex-1;
		} else {
			AddListItem(listitemindex, artist, title, null);
		}
		listitemindex++;
	}
	delete pEnum;
	if (curplayingitemindex > 1) {
		UList_->ScrollToTop(curplayingitemindex-1);
	} else {
		UList_->ScrollToTop();
	}
	this->RequestRedraw();
}

void FPlaylistForm::LoadPlaylist() {
	UList_->SetTextOfEmptyList(L"Refreshing...");
	UList_->RemoveAllItems();
	UList_->ScrollToTop();
	this->RequestRedraw();
	FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
	if (pMainForm->isstreaming) {
		FillListItems();
	} else {
		publicfunc_->LoadPlaylist();
	}
}

result
FPlaylistForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		pCanvas_->FillRectangle(Color(0,0,0,50),Rectangle(0,660,480,140));
		//draw title
		pCanvas_->FillRectangle(Color(0,0,0,100),Rectangle(0,38,480,50));
		pCanvas_->FillRectangle(Color(255,255,255,100),Rectangle(0,38+50-2,480,2));
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(0,38,480,50),L"Playlist",28,FONT_STYLE_BOLD,false,Color::COLOR_WHITE,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		//draw icon
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(419,668,61,124), Rectangle(0,800,61,124));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		Rectangle btnicorect;
		Point btnicoshift;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == BOXBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(818,256,90,44);
					btnicoshift.SetPosition(0,0);
					if ((button_->icoindex == 5) && (!button_->toogle)) {
						btnicoshift.y = 5;
					} else if (button_->icoindex == 11) {
						btnicoshift.y = 6;
					}
					if (button_->toogle) {
						btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
					} else {
						btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
					}
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+19+btnicoshift.x,button_->position.y+38+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FPlaylistForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	publicfunc_->playlist->RemoveAll(true);

	buttons_->RemoveAll(true);
	delete buttons_;

	delete currenticon;

	delete UListContextmenu_;
	delete __pCustomListItemFormat;

	return r;
}

void
FPlaylistForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == APLAY) {
		FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
		int playlistitemscount = 0;
		if (pMainForm->isstreaming) {
			playlistitemscount = pMainForm->playliststream->GetCount();
		} else {
			playlistitemscount = publicfunc_->playlist->GetCount();
		}
		if ((curselitemid > 0) && (curselitemid <= playlistitemscount)) {
			if (publicfunc_->sendingcommand == false) {
				//if (curplayingitemid != curselitemid) {
					PlayListItem * playlistitem_;
					if (pMainForm->isstreaming) {
						playlistitem_ = static_cast<PlayListItem *> (pMainForm->playliststream->GetAt(curselitemid-1));
						pMainForm->StreamingPlayListPlayItem(curselitemid-1);
					} else {
						playlistitem_ = static_cast<PlayListItem *> (publicfunc_->playlist->GetAt(curselitemid-1));
						publicfunc_->SendCommand(L"pl_play&id=" + Osp::Base::Integer::ToString(playlistitem_->id));
					}
					int listitemindex = -1;
					if (curplayingitemid >= 0) {
						PlayListItem * playlistitemold_;
						if (pMainForm->isstreaming) {
							playlistitemold_ = static_cast<PlayListItem *> (pMainForm->playliststream->GetAt(curplayingitemid-1));
						} else {
							playlistitemold_ = static_cast<PlayListItem *> (publicfunc_->playlist->GetAt(curplayingitemid-1));
						}
						listitemindex = UList_->GetItemIndexFromItemId(curplayingitemid);
						if (listitemindex >= 0) {
							CustomListItem* pItem = const_cast <CustomListItem*>(UList_->GetItemAt(listitemindex));
							pItem->SetElement(ID_LIST_ICON, L"");
							UList_->RefreshItem(listitemindex);
						}
					}
					listitemindex = UList_->GetItemIndexFromItemId(curselitemid);
					if (listitemindex >= 0) {
						CustomListItem* pItem = const_cast <CustomListItem*>(UList_->GetItemAt(listitemindex));
						pItem->SetElement(ID_LIST_ICON, *currenticon, currenticon);
						UList_->RefreshItem(listitemindex);
					}
					this->RequestRedraw();
					curplayingitemid = curselitemid;
				//}
			}
		}
	} else if (actionId == AREMOVE) {
		FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
		int playlistitemscount = 0;
		if (pMainForm->isstreaming) {
			playlistitemscount = pMainForm->playliststream->GetCount();
		} else {
			playlistitemscount = publicfunc_->playlist->GetCount();
		}
		if ((curselitemid > 0) && (curselitemid <= playlistitemscount)) {
			if (publicfunc_->sendingcommand == false) {
				if (curplayingitemid == curselitemid) {
					curplayingitemid = -1;
				}
				PlayListItem * playlistitem_;
				if (pMainForm->isstreaming) {
					playlistitem_ = static_cast<PlayListItem *> (pMainForm->playliststream->GetAt(curselitemid-1));
					pMainForm->StreamingPlayListRemoveItem(curselitemid-1);

					UList_->RemoveAllItems();
					FillListItems();
				} else {
					playlistitem_ = static_cast<PlayListItem *> (publicfunc_->playlist->GetAt(curselitemid-1));
					publicfunc_->SendCommand(L"pl_delete&id=" + Osp::Base::Integer::ToString(playlistitem_->id));
					publicfunc_->playlist->RemoveAt(curselitemid-1,true);

					UList_->RemoveAllItems();
					FillListItems();

				}
			}
		}
	} else if (actionId == ACLEAR) {
		MessageBox msgbox;
		int modalResult = 0;
		msgbox.Construct("Clear playlist?", "Do you want to clear playlist?", MSGBOX_STYLE_YESNO, 0);
		msgbox.ShowAndWait(modalResult);
		if (modalResult == MSGBOX_RESULT_YES) {
			FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
			if (pMainForm->isstreaming) {
				pMainForm->StreamingPlayListClear();
			}
			UList_->RemoveAllItems();
			publicfunc_->ClearPlayList();
			FillListItems();
		}
	} else if (actionId == AADD) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_BROWSEFORMADDTOPLAYLIST, null);
	} else if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
}

void
FPlaylistForm::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FPlaylistForm::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FPlaylistForm::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FPlaylistForm::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if(&source==UList_) {
			if (UList_->GetItemIndexFromPosition(currentPosition) >= 0) {
				curselitemid = UList_->GetItemIndexFromPosition(currentPosition)+1;
				if (curselitemid > 0) {
					UListContextmenu_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y+UList_->GetBounds().y);
					UListContextmenu_->SetShowState(true);
					UListContextmenu_->Show();
				}
			}
	}
}

void
FPlaylistForm::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (!button_->pressposition.Contains(currentPosition)) {
			if (button_->pressed) {
				button_->pressed = false;
				buttons_->SetAt(*button_,currpressedbtn,false);
				currpressedbtn = -1;
				needsredraw = true;
			}
		}
	}
	bool isvisible = true;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = true;
		if ((isvisible) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				needsredraw = true;
			}
			break;
		}
	}
	if (needsredraw) {
		RequestRedraw();
	}
}

void
FPlaylistForm::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FPlaylistForm::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			button_->pressed = false;
			buttons_->SetAt(*button_,currpressedbtn,false);
			this->OnActionPerformed(source, button_->action);
			needsredraw = true;
		}
	}
	currpressedbtn = -1;
	if (needsredraw) {
		RequestRedraw();
	}
}

void
FPlaylistForm::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (status == ITEM_SELECTED) {
		curselitemid = index+1;
		OnActionPerformed(source, APLAY);
	}
}

void
FPlaylistForm::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status)
{
	OnItemStateChanged(source, index, itemId, status);
}

void FPlaylistForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == publicfunc_->EVENTWIFICONNECTED) {
		LoadPlaylist();
	} else if (requestId == publicfunc_->EVENTWIFIDISCONNECTED) {
		UList_->SetTextOfEmptyList(L"Connection lost...");
		UList_->RemoveAllItems();
		UList_->ScrollToTop();
		this->RequestRedraw();
	} else if (requestId == publicfunc_->EVENTCONNECTIONLOST) {
		UList_->SetTextOfEmptyList(L"Connection lost...");
		UList_->RemoveAllItems();
		UList_->ScrollToTop();
		this->RequestRedraw();
	} else if (requestId == publicfunc_->EVENTREFRESHDATAUPDATEPLAYLIST) {
		FillListItems();
	} else if ((requestId == publicfunc_->EVENTREFRESHDATAUPDATE) || (requestId == publicfunc_->EVENTCOMMANDRETURN)) {
		FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
		if (pMainForm->isstreaming) {
			pMainForm->SendUserEvent(requestId,null);
		}
	}
}
