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
#include "FBrowseForm.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"
#include "FMainForm.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;

FavoritesListItem::FavoritesListItem(Osp::Base::String path, Osp::Base::String displaypath, int id) {
	this->path = path;
	this->displaypath = displaypath;
	this->id = id;
}

FavoritesListItem::~FavoritesListItem(void) {

}

FBrowseForm::FBrowseForm(bool fromplaylist)
{
	this->fromplaylist = fromplaylist;
}

FBrowseForm::~FBrowseForm(void)
{
}

bool
FBrowseForm::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL|FORM_STYLE_INDICATOR);
	this->SetName(L"FBrowseForm");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FBrowseForm::OnInitializing(void)
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
    __pCustomListItemFormat->AddElement(ID_LIST_TEXT, Osp::Graphics::Rectangle(65, 5, 360, 64));
    __pCustomListItemFormat->AddElement(ID_LIST_ICON, Osp::Graphics::Rectangle(0, 5, 64, 64));
    __pCustomListItemFormat->SetElementEventEnabled(ID_LIST_TEXT, true);
    __pCustomListItemFormat->SetElementEventEnabled(ID_LIST_ICON, true);

    __pCustomListItemFormatD = new CustomListItemFormat();
    __pCustomListItemFormatD->Construct();
    __pCustomListItemFormatD->AddElement(ID_LIST_TEXT, Osp::Graphics::Rectangle(5, 5, 470, 64));
    __pCustomListItemFormatD->SetElementEventEnabled(ID_LIST_TEXT, true);

    UListContextmenu_ = new ContextMenu;
	UListContextmenu_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	UListContextmenu_->AddItem(L"Play", APLAY);
	UListContextmenu_->AddItem(L"Enqueue", AENQUENE);
	UListContextmenu_->AddItem(L"Add folder to fav.", AADDTOFAV);
	UListContextmenu_->AddActionEventListener(*this);

	UListContextmenuFav_ = new ContextMenu;
	UListContextmenuFav_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	UListContextmenuFav_->AddItem(L"Play folder", APLAYFROMFAVORITES);
	UListContextmenuFav_->AddItem(L"Enqueue folder", AENQUENEFROMFAVORITES);
	UListContextmenuFav_->AddItem(L"Remove", AREMOVEFROMFAV);
	UListContextmenuFav_->AddActionEventListener(*this);

	UList_->AddTouchEventListener(*this);

	curselitemid = 0;

    AddControl(*UList_);

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(12,667,128,124), ABACK, 10, BOXBTN, L"Back");
	buttons_->Add(*backbtn);
	ButtonCostumEl * favoritesbtn = new ButtonCostumEl(Rectangle(140,667,128,124), AFAVORITES, 101, BOXBTN, AFAVORITESCLOSE, 101, L"Favorites");
	buttons_->Add(*favoritesbtn);
	ButtonCostumEl * homebtn = new ButtonCostumEl(Rectangle(268,667,128,124), AHOME, 100, BOXBTN, L"Home");
	buttons_->Add(*homebtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	curdir = publicfunc_->lastdir;
	prevdir = publicfunc_->lastdir;

    Image* image_ = new Image();
    image_->Construct();
    foldericon = image_->DecodeN(L"/Res/folderico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    folderupicon = image_->DecodeN(L"/Res/folderupico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    filedefico = image_->DecodeN(L"/Res/filedefico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    filemusicico = image_->DecodeN(L"/Res/filemusicico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    filevideoico = image_->DecodeN(L"/Res/filevideoico.png", BITMAP_PIXEL_FORMAT_ARGB8888);

    delete image_;

    favoritesmode = false;

    favoriteslist = new Osp::Base::Collection::ArrayList;
    favoriteslist->Construct();

    Osp::Io::Database * db_ = new Osp::Io::Database();
    db_->Construct((L"/Home/data.db"), true);
    Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT ID, DIR FROM FAVORITES WHERE CLIENTID=" + Osp::Base::Integer::ToString(publicfunc_->selectedclientid) + L" ORDER BY ID");
    if(!pEnum_) {
    	//no records
    } else {
    	pEnum_->Reset();
    	while (pEnum_->MoveNext() ==  E_SUCCESS) {
    		FavoritesListItem * favoriteslistitem_ = new FavoritesListItem(L"", L"", 0);
    		pEnum_->GetIntAt(0, favoriteslistitem_->id);
    		if (pEnum_->GetColumnSize(1) > 0) {
    			pEnum_->GetStringAt(1, favoriteslistitem_->path);
    			favoriteslistitem_->displaypath = GetFolderDisplayName(favoriteslistitem_->path);
    		}

    		favoriteslist->Add(*favoriteslistitem_);
    	}
    }
    delete pEnum_;
    delete db_;

    if (publicfunc_->iswificonnected) {
    	LoadList(curdir);
    }

	return r;
}

Osp::Base::String FBrowseForm::GetFolderDisplayName(Osp::Base::String dir) {
	Osp::Base::String displayname = dir;
	if (dir.GetLength() > 3) {
		int extpos1 = 0;
		int extpos2 = 0;
		if (dir.GetLength() > 5) {
		dir.LastIndexOf(L"%252F",dir.GetLength()-1,extpos1);
		}
		dir.LastIndexOf(L"%2F",dir.GetLength()-1,extpos2);
		if (extpos1 > extpos2) {
			dir.SubString(extpos1+5,displayname);
		} else {
			dir.SubString(extpos2+3,displayname);
		}
	}
	return displayname;
}

class CustomListElement : public ICustomListElement
 {
public:
 Osp::Base::String text;
 bool center;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     Font textfont;
     textfont.Construct(Osp::Graphics::FONT_STYLE_PLAIN, 28);

     EnrichedText texteel;
     texteel.Construct(Dimension(rect.width, rect.height));
     if (center == true) {
    	 texteel.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_CENTER);
     } else {
    	 texteel.SetHorizontalAlignment(Osp::Graphics::TEXT_ALIGNMENT_LEFT);
     }
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

result FBrowseForm::AddListItem(int itemid, String itemText, Bitmap* pBitmapNormal) {
    CustomListItem* pItem = new CustomListItem();
    CustomListElement * custom_element = new CustomListElement();
    custom_element->text = itemText;
    custom_element->center = false;

    pItem->Construct(74);
    if (pBitmapNormal != null) {
    	pItem->SetItemFormat(*__pCustomListItemFormat);
    	pItem->SetElement(ID_LIST_TEXT, *(static_cast<ICustomListElement *>(custom_element)));
    	pItem->SetElement(ID_LIST_ICON, *pBitmapNormal, pBitmapNormal);
    } else {
    	custom_element->center = true;
    	pItem->SetItemFormat(*__pCustomListItemFormatD);
    	pItem->SetElement(ID_LIST_TEXT, *(static_cast<ICustomListElement *>(custom_element)));
    }

    UList_->AddItem(*pItem, itemid);

    return E_SUCCESS;
}

void FBrowseForm::FillListItems() {
	Osp::Base::Collection::IEnumerator * pEnum = publicfunc_->filelist->GetEnumeratorN();
	FileListItem * filelistitem_ = null;
	int listitemindex = 1;
	int mediafiletype = 0;
	while (pEnum->MoveNext() == E_SUCCESS) {
		filelistitem_ = static_cast<FileListItem *> (pEnum->GetCurrent());
		if (filelistitem_->isfolder) {
			if (filelistitem_->name == L"..") {
				AddListItem(listitemindex, filelistitem_->name, folderupicon);
			} else {
				AddListItem(listitemindex, filelistitem_->name, foldericon);
			}
		} else {
			mediafiletype = publicfunc_->GetMediaFileType(filelistitem_->fileext);
			if (mediafiletype != 0) {
				if (mediafiletype == publicfunc_->MEDIAFILETYPE_VIDEO) {
					AddListItem(listitemindex, filelistitem_->name, filevideoico);
				} else if (mediafiletype == publicfunc_->MEDIAFILETYPE_AUDIO) {
					AddListItem(listitemindex, filelistitem_->name, filemusicico);
				} else {
					AddListItem(listitemindex, filelistitem_->name, filedefico);
				}
			} else {
				//AppLog("invalid file %S   %S", filelistitem_->name.GetPointer(), filelistitem_->fileext.GetPointer());
			}
		}
		listitemindex++;
	}
	delete pEnum;
	if (UList_->GetItemCount() < 1) {
		AddListItem(-2, L"..", folderupicon);
		AddListItem(-1, L"Error: Goto \"Documents\" folder", null);
	} else {
		prevdir = curdir;
	}
	UList_->ScrollToTop();
	this->RequestRedraw();
}

void FBrowseForm::LoadList(Osp::Base::String dir) {
	curdir = dir;
	UList_->SetTextOfEmptyList(L"Refreshing...");
	UList_->RemoveAllItems();
	UList_->ScrollToTop();
	this->RequestRedraw();
	publicfunc_->LoadBrowseList(dir);
}

void FBrowseForm::LoadFavoritesList() {
	UList_->SetTextOfEmptyList(L"Loading...");
	UList_->RemoveAllItems();
	UList_->ScrollToTop();
	this->RequestRedraw();
	AddListItem(-1, L"[browse all]", folderupicon);
	Osp::Base::Collection::IEnumerator * pEnum = favoriteslist->GetEnumeratorN();
	FavoritesListItem * FavoritesListItem_ = null;
	int listitemindex = 1;
	while (pEnum->MoveNext() == E_SUCCESS) {
		FavoritesListItem_ = static_cast<FavoritesListItem *> (pEnum->GetCurrent());
		AddListItem(listitemindex, FavoritesListItem_->displaypath, foldericon);
		listitemindex++;
	}
	delete pEnum;
	UList_->ScrollToTop();
	this->RequestRedraw();
}

result
FBrowseForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		pCanvas_->FillRectangle(Color(0,0,0,50),Rectangle(0,660,480,140));
		//draw title
		pCanvas_->FillRectangle(Color(0,0,0,100),Rectangle(0,38,480,50));
		pCanvas_->FillRectangle(Color(255,255,255,100),Rectangle(0,38+50-2,480,2));
		if (favoritesmode == true) {
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(0,38,480,50),L"Browse favorites",28,FONT_STYLE_BOLD,false,Color::COLOR_WHITE,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		} else {
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(0,38,480,50),L"Browse",28,FONT_STYLE_BOLD,false,Color::COLOR_WHITE,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		}
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
					if (button_->toogle) {
						btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
					} else {
						btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
					}
					if (button_->icoindex >= 100) {
						btnicorect.x = 200;
						btnicorect.y = 800;
						btnicorect.y = btnicorect.y + ((button_->icoindex-100) * btnicorect.height);
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
FBrowseForm::OnTerminating(void)
{
	result r = E_SUCCESS;


	publicfunc_->SetCurClientLastDir(curdir);

	buttons_->RemoveAll(true);
	delete buttons_;

	delete foldericon;
	delete folderupicon;
	delete filedefico;
	delete filemusicico;
	delete filevideoico;

	delete UListContextmenu_;
	delete UListContextmenuFav_;

	delete __pCustomListItemFormat;
	delete __pCustomListItemFormatD;

	favoriteslist->RemoveAll(true);
	delete favoriteslist;

	return r;
}

void
FBrowseForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == APLAY) {
		if ((curselitemid > 0) && (curselitemid <= publicfunc_->filelist->GetCount())) {
			if (publicfunc_->sendingcommand == false) {
				FileListItem * filelistitem_ = static_cast<FileListItem *> (publicfunc_->filelist->GetAt(curselitemid-1));
				FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
				if (pMainForm->isstreaming) {
					if (!(pMainForm->StreamingBrowseAddMedia(filelistitem_->filename, true))) {
						MessageBox msgbox;
						int modalResult = 0;
						msgbox.Construct("Unsupported file type!", "This file type is not supported for streaming!", MSGBOX_STYLE_OK, 10000);
						msgbox.ShowAndWait(modalResult);
					}
				} else {
					publicfunc_->SendCommand(L"in_play&input=" + publicfunc_->prepareurl(filelistitem_->filename));
				}
			}
		}
	} else if (actionId == AENQUENE) {
		if ((curselitemid > 0) && (curselitemid <= publicfunc_->filelist->GetCount())) {
			if (publicfunc_->sendingcommand == false) {
				FileListItem * filelistitem_ = static_cast<FileListItem *> (publicfunc_->filelist->GetAt(curselitemid-1));
				FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
				if (pMainForm->isstreaming) {
					if (!(pMainForm->StreamingBrowseAddMedia(filelistitem_->filename, false))) {
						MessageBox msgbox;
						int modalResult = 0;
						msgbox.Construct("Unsupported file type!", "This file type is not supported for streaming!", MSGBOX_STYLE_OK, 10000);
						msgbox.ShowAndWait(modalResult);
					}
				} else {
					publicfunc_->SendCommand(L"in_enqueue&input=" + publicfunc_->prepareurl(filelistitem_->filename));
				}
			}
		}
	} else if (actionId == APLAYFROMFAVORITES) {
		if ((curselitemid > 0) && (curselitemid <= favoriteslist->GetCount())) {
			if (publicfunc_->sendingcommand == false) {
				FavoritesListItem * favoriteslistitem_ = static_cast<FavoritesListItem *> (favoriteslist->GetAt(curselitemid-1));
				FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
				if (pMainForm->isstreaming) {
					if (!(pMainForm->StreamingBrowseAddMedia(favoriteslistitem_->path, true))) {
						MessageBox msgbox;
						int modalResult = 0;
						msgbox.Construct("Unsupported file type!", "This file type is not supported for streaming!", MSGBOX_STYLE_OK, 10000);
						msgbox.ShowAndWait(modalResult);
					}
				} else {
					publicfunc_->SendCommand(L"in_play&input=" + publicfunc_->prepareurl(favoriteslistitem_->path));
				}
			}
		}
	} else if (actionId == AENQUENEFROMFAVORITES) {
		if ((curselitemid > 0) && (curselitemid <= favoriteslist->GetCount())) {
			if (publicfunc_->sendingcommand == false) {
				FavoritesListItem * favoriteslistitem_ = static_cast<FavoritesListItem *> (favoriteslist->GetAt(curselitemid-1));
				FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
				if (pMainForm->isstreaming) {
					if (!(pMainForm->StreamingBrowseAddMedia(favoriteslistitem_->path, false))) {
						MessageBox msgbox;
						int modalResult = 0;
						msgbox.Construct("Unsupported file type!", "This file type is not supported for streaming!", MSGBOX_STYLE_OK, 10000);
						msgbox.ShowAndWait(modalResult);
					}
				} else {
					publicfunc_->SendCommand(L"in_enqueue&input=" + publicfunc_->prepareurl(favoriteslistitem_->path));
				}
			}
		}
	} else if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		if (fromplaylist) {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PLAYLISTFORM, null);
		} else {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
		}
	} else if (actionId == AFAVORITES) {
		favoritesmode = true;
		LoadFavoritesList();
	} else if (actionId == AFAVORITESCLOSE) {
		favoritesmode = false;
		LoadList(curdir);
	} else if (actionId == AFAVORITESCLOSEFROMLIST) {
		if (favoritesmode == true) {
			ButtonCostumEl* button_;
			for (int n=0; n<buttons_->GetCount(); n++) {
				button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
				if (button_->action == AFAVORITES) {
					if (button_->toogle) {
						button_->toogle = false;
						buttons_->SetAt(*button_,n,false);
						favoritesmode = false;
						RequestRedraw();
					}
					break;
				}
			}
		}
		favoritesmode = false;
		LoadList(curdir);
	} else if (actionId == AHOME) {
		if (favoritesmode == true) {
			ButtonCostumEl* button_;
			for (int n=0; n<buttons_->GetCount(); n++) {
				button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
				if (button_->action == AFAVORITES) {
					if (button_->toogle) {
						button_->toogle = false;
						buttons_->SetAt(*button_,n,false);
						favoritesmode = false;
						RequestRedraw();
					}
					break;
				}
			}
		}
		curdir = L"";
		LoadList(curdir);
	} else if (actionId == AADDTOFAV) {
		if ((curselitemid > 0) && (curselitemid <= publicfunc_->filelist->GetCount())) {
			FileListItem * filelistitem_ = static_cast<FileListItem *> (publicfunc_->filelist->GetAt(curselitemid-1));
			Osp::Base::String foldername = filelistitem_->filename;
			if (filelistitem_->isfolder != true) {
				foldername = curdir;
			}
			Osp::Io::Database * db_ = new Osp::Io::Database();
			db_->Construct((L"/Home/data.db"), true);
			bool exists = false;

			Osp::Io::DbStatement * pStmt_ = db_->CreateStatementN(L"SELECT * FROM FAVORITES WHERE CLIENTID=? AND DIR=?");
			pStmt_->BindInt(0, publicfunc_->selectedclientid);
			pStmt_->BindString(1, foldername);
			Osp::Io::DbEnumerator * pEnum_ = db_->ExecuteStatementN(*pStmt_);
			if(!(!pEnum_)) {
				exists = true;
			}
			delete pEnum_;
			delete pStmt_;
			if (exists == true) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Already exists!", "Folder already exists under favorites!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
			} else {
				int favoritesfoldernewid;
				db_->BeginTransaction();
				Osp::Io::DbStatement * pStmt_ = db_->CreateStatementN(L"INSERT INTO FAVORITES (ID, CLIENTID, DIR) VALUES (NULL, ?, ?)");
				pStmt_->BindInt(0, publicfunc_->selectedclientid);
				pStmt_->BindString(1, foldername);
				Osp::Io::DbEnumerator * pEnum2_ = db_->ExecuteStatementN(*pStmt_);
				db_->CommitTransaction();
				delete pEnum2_;
				Osp::Io::DbEnumerator * pEnumgetid_ = db_->QueryN(L"SELECT last_insert_rowid()");
				if(!(!pEnumgetid_)) {
					pEnumgetid_->Reset();
					pEnumgetid_->MoveNext();
					pEnumgetid_->GetIntAt(0, favoritesfoldernewid);
				}
				delete pEnumgetid_;
				delete pStmt_;

				FavoritesListItem * favoriteslistitem_ = new FavoritesListItem(foldername, GetFolderDisplayName(foldername), favoritesfoldernewid);
				favoriteslist->Add(*favoriteslistitem_);
			}

			delete db_;
		}
	} else if (actionId == AREMOVEFROMFAV) {
		if ((curselitemid > 0) && (curselitemid <= favoriteslist->GetCount())) {
			FavoritesListItem * favoriteslistitem_ = static_cast<FavoritesListItem *> (favoriteslist->GetAt(curselitemid-1));
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Remove?", "Do you want to remove folder from favorites?", MSGBOX_STYLE_YESNO, 0);
			msgbox.ShowAndWait(modalResult);
			if (modalResult == MSGBOX_RESULT_YES) {
				Osp::Io::Database * db_ = new Osp::Io::Database();
				db_->Construct((L"/Home/data.db"), true);
				Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT * FROM FAVORITES WHERE ID=" + Osp::Base::Integer::ToString(favoriteslistitem_->id));
				if (!(!pEnum_)) {
					db_->ExecuteSql(L"DELETE FROM FAVORITES WHERE ID=" + Osp::Base::Integer::ToString(favoriteslistitem_->id), true);
					favoriteslist->RemoveAt(curselitemid-1,true);
					UList_->RemoveAllItems();
					LoadFavoritesList();
				}
				delete pEnum_;
				delete db_;
			}
		}
	}
}

void
FBrowseForm::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FBrowseForm::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FBrowseForm::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FBrowseForm::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if(&source==UList_) {
			if (UList_->GetItemIndexFromPosition(currentPosition) >= 0) {
				curselitemid = UList_->GetItemIdAt(UList_->GetItemIndexFromPosition(currentPosition));
				if (curselitemid > 0) {
					bool canopen = false;
					if (favoritesmode == true) {
						UListContextmenuFav_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y+UList_->GetBounds().y);
						UListContextmenuFav_->SetShowState(true);
						UListContextmenuFav_->Show();
					} else {
						FileListItem * filelistitem_ = static_cast<FileListItem *> (publicfunc_->filelist->GetAt(curselitemid-1));
						if (filelistitem_->isfolder) {
							canopen = (filelistitem_->name != L"..");
						} else {
							canopen = true;
						}
						if (canopen) {
							UListContextmenu_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y+UList_->GetBounds().y);
							UListContextmenu_->SetShowState(true);
							UListContextmenu_->Show();
						}
					}
				}
			}
	}
}

void
FBrowseForm::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
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
FBrowseForm::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FBrowseForm::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	bool istoogle = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			istoogle = (button_->action == AFAVORITES);
			button_->pressed = false;
			if (istoogle) {
				button_->toogle = !button_->toogle;
			}
			buttons_->SetAt(*button_,currpressedbtn,false);
			if (istoogle) {
				if (!button_->toogle) {
					this->OnActionPerformed(source, button_->actiontoogle);
				} else {
					this->OnActionPerformed(source, button_->action);
				}
			} else {
				this->OnActionPerformed(source, button_->action);
			}
			needsredraw = true;
		}
	}
	currpressedbtn = -1;
	if (needsredraw) {
		RequestRedraw();
	}
}

void
FBrowseForm::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (favoritesmode == true) {
		if (itemId == -1) {
			OnActionPerformed(source, AFAVORITESCLOSEFROMLIST);
		} else if (itemId > 0) {
			if (status == ITEM_SELECTED) {
				//favoritesmode = false;
				FavoritesListItem * FavoritesListItem_ = static_cast<FavoritesListItem *> (favoriteslist->GetAt(itemId-1));
				curdir = FavoritesListItem_->path;
				OnActionPerformed(source, AFAVORITESCLOSEFROMLIST);
				//LoadList(curdir);
			}
		}
	} else {
		if (itemId == -1) {
			curdir = L"~";
			LoadList(curdir);
		} else if (itemId == -2) {
			AppLog("cur dir %S", curdir.GetPointer());
			AppLog("prev dir %S", prevdir.GetPointer());
			curdir = prevdir;
			LoadList(curdir);
		} else if (itemId >= 0) {
			FileListItem * filelistitem_ = static_cast<FileListItem *> (publicfunc_->filelist->GetAt(itemId-1));
			if (filelistitem_->isfolder) {
				if (status == ITEM_SELECTED) {
					curdir = filelistitem_->filename;
					LoadList(curdir);
				}
			} else {
				if (status == ITEM_SELECTED) {
					curselitemid = itemId;
					if (fromplaylist) {
						OnActionPerformed(source, AENQUENE);
					} else {
						OnActionPerformed(source, APLAY);
					}
				}
			}
		}
	}
}

void
FBrowseForm::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status)
{
	OnItemStateChanged(source, index, itemId, status);
}

void FBrowseForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (requestId == publicfunc_->EVENTWIFICONNECTED) {
		if (favoritesmode == false) {
			LoadList(curdir);
		} else {
			LoadFavoritesList();
		}
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
	} else if (requestId == publicfunc_->EVENTREFRESHDATAUPDATEBROWSE) {
		if (favoritesmode == false) {
			FillListItems();
		}
	} else if ((requestId == publicfunc_->EVENTREFRESHDATAUPDATE) || (requestId == publicfunc_->EVENTCOMMANDRETURN)) {
		if ((requestId == publicfunc_->EVENTCOMMANDRETURN) && (fromplaylist)) {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PLAYLISTFORM, null);
		} else {
			FMainForm *pMainForm = static_cast<FMainForm *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FMainForm"));
			if (pMainForm->isstreaming) {
				pMainForm->SendUserEvent(requestId,null);
			}
			if (requestId == publicfunc_->EVENTCOMMANDRETURN) {
				Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
			}
		}
	}
}
