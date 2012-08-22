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
#include "FGalleryForm.h"
#include "FormMgr.h"
#include "Cam3Dclass.h"
#include "ButtonCostumEl.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Io;
using namespace Osp::Media;
using namespace Osp::Base::Utility;

ThumbsListItemThumb::ThumbsListItemThumb(Osp::Base::String filename, Osp::Graphics::Rectangle pos, Cam3Dclass * cam3dclass_, Osp::Media::Image * pimagedecoder_) {
	CAM3D_FILEINFO fileinfo_;
	this->filename = filename;
	this->pos = pos;
	this->thumbimg = null;
	Osp::Base::ByteBuffer * tmpimgbuffer = new Osp::Base::ByteBuffer();
	cam3dclass_->ReadCam3Dfileinfo(this->filename, fileinfo_);
	tmpimgbuffer->Construct(fileinfo_.leftimgsize);
	cam3dclass_->ReadCam3Dfileimg(this->filename, cam3dclass_->LEFTIMG, *tmpimgbuffer);
	this->thumbimg = pimagedecoder_->DecodeN(*tmpimgbuffer, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565, 220, 132);
	delete tmpimgbuffer;
}

ThumbsListItemThumb::~ThumbsListItemThumb(void) {
	if (this->thumbimg != null) {
		delete this->thumbimg;
	}
}

ThumbsListItem::ThumbsListItem(Osp::Base::String filename, Osp::Base::DateTime filetime) {
	this->filename = filename;
	this->filetime = filetime;
}

ThumbsListItem::~ThumbsListItem(void) {

}

class MySort : public Osp::Base::Collection::IComparer
{
public:
result Compare(const Osp::Base::Object& obj1, const Osp::Base::Object& obj2, int& cmp) const
{
const ThumbsListItem* objitem1 = dynamic_cast<const ThumbsListItem*>(&obj1);
const ThumbsListItem* objitem2 = dynamic_cast<const ThumbsListItem*>(&obj2);
cmp = (objitem1->filetime > objitem2->filetime ? -1 : (objitem1->filetime==objitem2->filetime ? 0 : 1));
return E_SUCCESS;
}
};

FGalleryForm::FGalleryForm(Osp::Base::String returnfilename)
{
	this->returnfilename = returnfilename;
	AppLog("return file name %S", this->returnfilename.GetPointer());
}

FGalleryForm::~FGalleryForm(void)
{
}

bool
FGalleryForm::Initialize()
{

	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"GalleryForm");
	this->SetOrientation(ORIENTATION_LANDSCAPE);

	return true;
}

result
FGalleryForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	cam3dclass_ = new Cam3Dclass();

	pimagedecoder_ = new Image();
	pimagedecoder_->Construct();

	thumbslist = new Osp::Base::Collection::ArrayList;
	thumbslistthumbs = new Osp::Base::Collection::ArrayList;

	if (Osp::Io::File::IsFileExist(L"/Home/Gallery")) {
		Directory* pDir = new Directory;
		DirEnumerator* pDirEnum;
		pDir->Construct(L"/Home/Gallery");
		pDirEnum = pDir->ReadN();
		while(pDirEnum->MoveNext() == E_SUCCESS) {
			DirEntry entry = pDirEnum->GetCurrentDirEntry();
			if (Osp::Io::File::GetFileExtension(entry.GetName()) == L"c3d") {
				ThumbsListItem * thumslistitem_ = new ThumbsListItem(L"/Home/Gallery/" + entry.GetName(), entry.GetDateTime());
				thumbslist->Add(*thumslistitem_);
			}
		}
		delete pDir;
		delete pDirEnum;
	}

	Osp::Base::Collection::IComparer* pComp = new MySort;
	thumbslist->Sort(*pComp);
	delete pComp;

	Osp::Base::Collection::IEnumerator * pEnum = thumbslist->GetEnumeratorN();
	ThumbsListItem * thumslistitem_ = null;
	int imgindex = 1;
	int foundimgindex = 1;
	if (this->returnfilename != L"") {
	while (pEnum->MoveNext() == E_SUCCESS) {
		thumslistitem_ = static_cast<ThumbsListItem *> (pEnum->GetCurrent());
		if (thumslistitem_->filename == this->returnfilename) {
			foundimgindex = imgindex;
			break;
		}
		imgindex++;
	}
	}
	delete pEnum;

	curpage = Math::Ceiling(foundimgindex / 6.0f);

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(30,395,64,64), ABACK, 0, ROUNDBTNBLUE, L"Back");
	backbtn->pressposition = backbtn->position;
	buttons_->Add(*backbtn);
	ButtonCostumEl * prevbtn = new ButtonCostumEl(Rectangle(282,395,64,64), APREVPAGE, 0, ROUNDBTNGREEN, L"Settings");
	prevbtn->pressposition = prevbtn->position;
	buttons_->Add(*prevbtn);
	ButtonCostumEl * nextbtn = new ButtonCostumEl(Rectangle(454,395,64,64), ANEXTPAGE, 0, ROUNDBTNGREEN, L"Help");
	nextbtn->pressposition = nextbtn->position;
	buttons_->Add(*nextbtn);

	currpressedbtn = -1;

	pagescount = Math::Ceiling(thumbslist->GetCount() / 6.0f);
	LoadThumbsPage(curpage);

	this->AddTouchEventListener(*this);
	this->AddKeyEventListener(*this);

	return r;
}

void FGalleryForm::LoadThumbsPage(int page) {
	thumbslistthumbs->RemoveAll(true);
	Osp::Base::Collection::IEnumerator * pEnum = thumbslist->GetEnumeratorN();
    ThumbsListItem * thumslistitem_ = null;
    int imgindex = 1;
    int imgindexout = 1;
    Osp::Graphics::Rectangle poss;
    int xsep = 266;
    int ysep = 170;

    ButtonCostumEl* button_;

    int n = 0;
   	while (n < buttons_->GetCount()) {
   		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
   		if (button_->type == THUMBBTN) {
   			buttons_->RemoveAt(n, true);
   			n--;
   		} else {
   			n++;
   		}
    }

    while (pEnum->MoveNext() == E_SUCCESS) {
    	if ((imgindex > ((page-1) * 6)) && (imgindex <= ((page) * 6))) {
    		thumslistitem_ = static_cast<ThumbsListItem *> (pEnum->GetCurrent());
    		poss = Rectangle(16,20,236,151);
    		if (imgindexout >= 4) {
    			poss.SetPosition(poss.x, poss.y + ysep);
    		}
    		if ((imgindexout == 2) || (imgindexout == 5)) {
    			poss.SetPosition(poss.x + xsep, poss.y);
    		} else if ((imgindexout == 3) || (imgindexout == 6)) {
    			poss.SetPosition(poss.x + (xsep * 2), poss.y);
    		}
    		ThumbsListItemThumb * thumslistitemthumb_ = new ThumbsListItemThumb(thumslistitem_->filename, poss, cam3dclass_, pimagedecoder_);
    		thumbslistthumbs->Add(*thumslistitemthumb_);

    		button_ = new ButtonCostumEl(poss, 1000 + imgindex, 0, THUMBBTN, L"");
    		button_->pressposition = button_->position;
    		buttons_->Add(*button_);
    		imgindexout++;
    	}
   		imgindex++;
    }
    delete pEnum;
}

result
FGalleryForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,800,480), Rectangle(0,0,800,480));
		//draw overlay
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,350,800,130), Rectangle(0,480,800,130));
		//draw form title
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(580,437,180,23), Rectangle(0,948,180,23));
		//draw thumbs
		Osp::Base::Collection::IEnumerator * pEnum = thumbslistthumbs->GetEnumeratorN();
		ThumbsListItemThumb * thumslistitem_ = null;
		//int imgindex = 1;
		Osp::Graphics::Rectangle poss;
		while (pEnum->MoveNext() == E_SUCCESS) {
			thumslistitem_ = static_cast<ThumbsListItemThumb *> (pEnum->GetCurrent());
			poss = thumslistitem_->pos;
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, poss, Rectangle(0,797,236,151));
			pCanvas_->DrawBitmap(Rectangle(poss.x+8,poss.y+10,220,132), *thumslistitem_->thumbimg);
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(poss.x+8,poss.y+10,220,132), Rectangle(236,797,220,132));
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(poss.x+1,poss.y,233,73), Rectangle(239,929,233,73));
		}
		delete pEnum;
		//draw pager
		if (pagescount > 1) {
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(282+64,395,454-(282+64),64), Osp::Base::Integer::ToString(curpage) + "/" + Osp::Base::Integer::ToString(pagescount), 30, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == ROUNDBTNBLUE) {
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(472,670+button_->position.height,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(472,798+40,40,40));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(472,670,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(472,798,40,40));
				}
				}
			}
			if (button_->type == ROUNDBTNGREEN) {
				int leftpos = 0;
				if (button_->action == ANEXTPAGE) {
					leftpos = 592;
					isvisible = (curpage < pagescount);
				} else if (button_->action == APREVPAGE) {
					leftpos = 552;
					isvisible = (curpage > 1);
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(536,670+button_->position.height,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,798+40,40,40));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(536,670,button_->position.width,button_->position.height));
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+12,button_->position.y+12,40,40), Rectangle(leftpos,798,40,40));
				}
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FGalleryForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete cam3dclass_;
	delete pimagedecoder_;

	thumbslist->RemoveAll(true);
	delete thumbslist;

	thumbslistthumbs->RemoveAll(true);
	delete thumbslistthumbs;

	buttons_->RemoveAll(true);
	delete buttons_;

	return r;
}

void
FGalleryForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
	if (actionId == ANEXTPAGE) {
		if (curpage < pagescount) {
			curpage++;
			LoadThumbsPage(curpage);
			this->RequestRedraw();
		}
	}
	if (actionId == APREVPAGE) {
		if (curpage > 1) {
			curpage--;
			LoadThumbsPage(curpage);
			this->RequestRedraw();
		}
	}
	if ((actionId >= 1001)) {
		AppLog("open img index %d", actionId-1000);
		Osp::Base::String imgfilename = L"";
		Osp::Base::Collection::IEnumerator * pEnum = thumbslist->GetEnumeratorN();
		ThumbsListItem * thumslistitem_ = null;
		int imgindex=1;
		while (pEnum->MoveNext() == E_SUCCESS) {
		   if (imgindex == (actionId-1000)) {
			   thumslistitem_ = static_cast<ThumbsListItem *> (pEnum->GetCurrent());
			   imgfilename = (thumslistitem_->filename);
			   break;
		   }
		   imgindex++;
		}
		if (imgfilename != L"") {
			AppLog("open img filename %S", imgfilename.GetPointer());
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			Osp::Base::Collection::ArrayList * list = new Osp::Base::Collection::ArrayList;
			list->Construct();
			list->Add(*(new Osp::Base::String(imgfilename)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_DETAILVIEW, list);
		}
	}
}

void FGalleryForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FGalleryForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FGalleryForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FGalleryForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FGalleryForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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
void FGalleryForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FGalleryForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
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

void FGalleryForm::OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FGalleryForm::OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{

}

void FGalleryForm::OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode)
{
	if ((keyCode == Osp::Ui::KEY_SIDE_UP)) {
		this->OnActionPerformed(source, APREVPAGE);
	}
	if ((keyCode == Osp::Ui::KEY_SIDE_DOWN)) {
		this->OnActionPerformed(source, ANEXTPAGE);
	}
}
