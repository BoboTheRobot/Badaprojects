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
#include "FGallery.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::Io;
using namespace Osp::System;
using namespace Osp::App;
using namespace Osp::Base::Utility;

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

FGallery::FGallery(void)
{
}

FGallery::~FGallery(void)
{
}

bool
FGallery::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FGallery");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FGallery::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	loadingpopup_ = new Popup();
	loadingpopup_->Construct(L"IDP_POPUP1");

	buttons_ = new Osp::Base::Collection::ArrayList;
	buttons_->Add(*new ButtonCostumEl(Rectangle(295,702,165,72), ABACK, 0, REDBTN, 0, L"Back"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(21,702,85,72), APREVPAGE, 0, PAGERBTN, 0, L"Prev"));
	buttons_->Add(*new ButtonCostumEl(Rectangle(191,702,85,72), ANEXTPAGE, 0, PAGERBTN, 0, L"Next"));

	currpressedbtn = -1;

	this->AddTouchEventListener(*this);

	//kreiraj icon list
	__pIconList = new IconList();
    __pIconList->Construct(Rectangle(16+28,88+13,400,560), ICON_LIST_STYLE_RADIO, 98, 130);
    __pIconList->SetTextColorOfEmptyList(Color::COLOR_RED);
    __pIconList->SetTextOfEmptyList(L"No images in gallery!");
    __pIconList->SetMargin(0,0);
    __pIconList->AddItemEventListener(*this);

    thumbslist = new Osp::Base::Collection::ArrayList;
    thumbslist->Construct();

    actioninprogress = false;

    AddControl(*__pIconList);

	return r;
}

void FGallery::LoadThumbsPage(int page) {

	int thumbsperpage = 16;

	Osp::Ui::Controls::Progress * progressbar_ = static_cast<Progress *>(loadingpopup_->GetControl(L"IDC_PROGRESS1"));
	Osp::Ui::Controls::Label * statlabel_ = static_cast<Label *>(loadingpopup_->GetControl(L"IDC_LABELSTAT"));
	statlabel_->SetText(L"");
	statlabel_->Draw();
	statlabel_->Show();
	progressbar_->SetRange(1,thumbsperpage);
	progressbar_->SetValue(1);
	loadingpopup_->SetShowState(true);
	loadingpopup_->Show();

	int showpageindex = page;
	if (showpageindex == 0) {
		statlabel_->SetText(L"Init");
		statlabel_->Draw();
		statlabel_->Show();
		Osp::Base::Collection::ArrayList * dirlist = new Osp::Base::Collection::ArrayList;
		dirlist->Construct();
		if (Osp::Io::File::IsFileExist(L"/Media/Images")) {
			dirlist->Add(*(new String(L"/Media/Images")));
		}
		if (Osp::Io::File::IsFileExist(L"/Storagecard/Media/Images")) {
			dirlist->Add(*(new String(L"/Storagecard/Media/Images")));
		}
		int curdirindex = 0;
		String * curdirp;
		String curdir;
		while (curdirindex < dirlist->GetCount()) {
			curdirp = static_cast<String *> (dirlist->GetAt(curdirindex));
			curdir = L"";
			curdir.Append(*curdirp);
			Directory* pDir = new Directory;
			DirEnumerator* pDirEnum;
			pDir->Construct(curdir);
			pDirEnum = pDir->ReadN();
			while(pDirEnum->MoveNext() == E_SUCCESS) {
				DirEntry entry = pDirEnum->GetCurrentDirEntry();
				String filename = entry.GetName();
				statlabel_->SetText(L"Scan:" + filename);
				statlabel_->Draw();
				statlabel_->Show();
				if (entry.IsDirectory()) {
					if ((filename != L".") && (filename != L"..")) {
						dirlist->Add(*(new String(curdir + L"/" + filename)));
					}
				} else if (!entry.IsHidden()) {
					if ((Osp::Io::File::GetFileExtension(filename) == L"jpg") || (Osp::Io::File::GetFileExtension(filename) == L"png")) {
						ThumbsListItem * thumslistitem_ = new ThumbsListItem(curdir + L"/" + filename, entry.GetDateTime());
						thumbslist->Add(*thumslistitem_);
					}
				}
			}
			delete pDir;
			delete pDirEnum;
			curdirindex++;
		}
		dirlist->RemoveAll(true);
		delete dirlist;
		statlabel_->SetText(L"Sorting images");
		statlabel_->Draw();
		statlabel_->Show();
		Osp::Base::Collection::IComparer* pComp = new MySort;
		thumbslist->Sort(*pComp);
		delete pComp;
		showpageindex = 1;
		this->pagescount = (int)Math::Ceiling(thumbslist->GetCount() / (double)thumbsperpage);
	}
	if (showpageindex < 1) showpageindex = 1;
	if (showpageindex > this->pagescount) showpageindex = this->pagescount;
	this->curpage = showpageindex;

	statlabel_->SetText(L"Loading thumbs");
	statlabel_->Draw();
	statlabel_->Show();
	Osp::Base::Collection::IEnumerator * pEnum = thumbslist->GetEnumeratorN();
    ThumbsListItem * thumslistitem_ = null;
    int imgindex = 1;
    int progressimgi = 0;
    Osp::Media::Image * pImage = new Image();
    int imgh, imgw, cropt;
    double ratiow, ratioh, ratio;
    pImage->Construct();
    Osp::Graphics::Canvas * tmpcanvas;
	tmpcanvas = new Canvas();
	tmpcanvas->Construct(Rectangle(0,0,98,1000));
	Bitmap *pThumbBitmap = new Bitmap();
	pThumbBitmap->Construct(Dimension(98,130), BITMAP_PIXEL_FORMAT_RGB565);
	String thumbname = L"";
    while (pEnum->MoveNext() == E_SUCCESS) {
    	if ((imgindex > ((showpageindex-1) * thumbsperpage)) && (imgindex <= ((showpageindex) * thumbsperpage))) {
			thumslistitem_ = static_cast<ThumbsListItem *> (pEnum->GetCurrent());
    		progressbar_->SetValue(progressimgi);
    		progressbar_->Draw();
    		progressbar_->Show();

    		statlabel_->SetText(L"Thumb:" + Osp::Io::File::GetFileName(thumslistitem_->filename));
			statlabel_->Draw();
			statlabel_->Show();

			Osp::Content::ImageContentInfo * imageContentInfo = new Osp::Content::ImageContentInfo();
			if (imageContentInfo->Construct(thumslistitem_->filename, L"", false) == E_SUCCESS) {
			imgh = imageContentInfo->GetHeight();
			imgw = imageContentInfo->GetWidth();
			if ((imgh >= 130) && (imgw >= 130)) {
			if (imgw > imgh) {
				ratiow = ((double)pThumbBitmap->GetHeight() / (double)imgw);
				ratioh = ((double)pThumbBitmap->GetWidth() / (double)imgh);
				if (ratiow > ratioh) {
					ratio = ratiow;
				} else {
					ratio = ratioh;
				}
				imgw = imgw * ratio;
				imgh = imgh * ratio;
				Bitmap *pThumbBitmapO = pImage->DecodeN(thumslistitem_->filename, BITMAP_PIXEL_FORMAT_RGB565, imgw, imgh);
				tmpcanvas->DrawBitmap(Point(imgh,imgw),*pThumbBitmapO,Point(imgw,0),-90);
				Osp::Graphics::Bitmap * rotatedimage = new Bitmap();
				rotatedimage->Construct(*tmpcanvas,Rectangle(0,0,imgh,imgw));
				cropt = (int)((double)(rotatedimage->GetHeight() - pThumbBitmap->GetHeight()) * 0.5f);
				pThumbBitmap->Merge(Point(0,0), *rotatedimage, Rectangle(0,cropt,rotatedimage->GetWidth(),rotatedimage->GetHeight()));
				delete rotatedimage;
				delete pThumbBitmapO;
			} else {
				ratiow = ((double)pThumbBitmap->GetWidth() / (double)imgw);
				ratioh = ((double)pThumbBitmap->GetHeight() / (double)imgh);
				if (ratiow > ratioh) {
					ratio = ratiow;
				} else {
					ratio = ratioh;
				}
				imgw = imgw * ratio;
				imgh = imgh * ratio;
				Bitmap *pThumbBitmapO = pImage->DecodeN(thumslistitem_->filename, BITMAP_PIXEL_FORMAT_RGB565, imgw, imgh);
				cropt = (int)((double)(pThumbBitmapO->GetHeight() - pThumbBitmap->GetHeight()) * 0.5f);
				pThumbBitmap->Merge(Point(0,0), *pThumbBitmapO, Rectangle(0,cropt,pThumbBitmap->GetWidth(),pThumbBitmap->GetHeight()));
				delete pThumbBitmapO;
			}
			__pIconList->AddItem(&thumbname, pThumbBitmap, pThumbBitmap, imgindex);
			}
			}
			delete imageContentInfo;
			progressimgi++;
    	} else if (imgindex > ((showpageindex) * thumbsperpage)) {
    		break;
    	}
    	imgindex++;
    }
    progressbar_->SetValue(thumbsperpage);
    progressbar_->Draw();
    progressbar_->Show();
    delete pImage;
    delete tmpcanvas;
    delete pThumbBitmap;
    delete pEnum;

    loadingpopup_->SetShowState(false);
    this->RequestRedraw();
}

result
FGallery::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(75,16,330,55), Rectangle(950,400,330,55));
		//draw form frame
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(16,88,448,598), Rectangle(480,0,448,598));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		int leftpos = 0;
		Osp::Graphics::Rectangle btntextpos;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			isvisible = true;
			if (button_->type == REDBTN) {
				leftpos = 1008;
				if (button_->action == ABACK) {
					btntextpos.SetBounds(221,800,102,34);
				} else {
					btntextpos.SetBounds(0,0,0,0);
				}
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598+button_->position.height,button_->position.width,button_->position.height));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598,button_->position.width,button_->position.height));
				}
				drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x + (int)((button_->position.width / 2.0f) - (btntextpos.width / 2.0f)),button_->position.y + (int)((button_->position.height / 2.0f) - (btntextpos.height / 2.0f)),btntextpos.width,btntextpos.height), btntextpos);
				}
			}
			if (button_->type == PAGERBTN) {
				leftpos = 759;
				if (button_->action == ANEXTPAGE) {
					leftpos = 759+249-button_->position.width;
					btntextpos.SetBounds(34,888,34,17);
				} else if (button_->action == APREVPAGE) {
					btntextpos.SetBounds(0,888,34,17);
				}
				isvisible = (pagescount > 1);
				if (isvisible) {
				if (button_->pressed) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598+button_->position.height,button_->position.width,button_->position.height));
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(leftpos,598,button_->position.width,button_->position.height));
				}
				drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x + (int)((button_->position.width / 2.0f) - (btntextpos.width / 2.0f)),button_->position.y + (int)((button_->position.height / 2.0f) - (btntextpos.height / 2.0f)),btntextpos.width,btntextpos.height), btntextpos);
				}
			}
		}
		//draw pager
		if (pagescount > 1) {
			pCanvas_->FillRectangle(Color(9,0,193),Rectangle(106,703,85,71));
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(106,703,85,71), Osp::Base::Integer::ToString(curpage) + "/" + Osp::Base::Integer::ToString(pagescount), 30, Osp::Graphics::FONT_STYLE_BOLD, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FGallery::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete loadingpopup_;

	thumbslist->RemoveAll(true);
	delete thumbslist;

	buttons_->RemoveAll(true);
	delete buttons_;

	return r;
}


void
FGallery::OnActionPerformed(const Control& source, int actionId)
{
	if (actioninprogress == true) {
		return;
	}
	if (actionId == AIMGSEL) {
		int imgh, imgw, cropt;
		double ratiow, ratioh, ratio;

		bool isok = false;
		bool imgtoosmall = false;
		Osp::Content::ImageContentInfo * pImgMeta = new Osp::Content::ImageContentInfo();
		Osp::Media::Image * pImage = new Image();
		pImage->Construct();
		Bitmap *pThumbBitmap = new Bitmap();
		pThumbBitmap->Construct(Dimension(480,640), BITMAP_PIXEL_FORMAT_RGB565);
		if (pImgMeta->Construct(selectedthumb, L"", false) == E_SUCCESS) {
		imgh = pImgMeta->GetHeight();
		imgw = pImgMeta->GetWidth();
		if (imgw > imgh) {
			if ((imgh >= 480) && (imgw >= 640)) {
			ratiow = ((double)pThumbBitmap->GetHeight() / (double)imgw);
			ratioh = ((double)pThumbBitmap->GetWidth() / (double)imgh);
			if (ratiow > ratioh) {
				ratio = ratiow;
			} else {
				ratio = ratioh;
			}
			imgw = imgw * ratio;
			imgh = imgh * ratio;
			if ((imgh >= 480) && (imgw >= 640)) {
			Bitmap *pThumbBitmapO = pImage->DecodeN(selectedthumb, BITMAP_PIXEL_FORMAT_RGB565, imgw, imgh);
			Osp::Graphics::Canvas * tmpcanvas;
			tmpcanvas = new Canvas();
			tmpcanvas->Construct(Rectangle(0,0,imgh,imgw));
			tmpcanvas->DrawBitmap(Point(imgh,imgw),*pThumbBitmapO,Point(imgw,0),-90);
			Osp::Graphics::Bitmap * rotatedimage = new Bitmap();
			rotatedimage->Construct(*tmpcanvas,tmpcanvas->GetBounds());
			cropt = (int)((double)(rotatedimage->GetHeight() - pThumbBitmap->GetHeight()) * 0.5f);
			if (cropt < 0) cropt = 0;
			pThumbBitmap->Merge(Point(0,0), *rotatedimage, Rectangle(0,cropt,rotatedimage->GetWidth(),rotatedimage->GetHeight()));
			delete rotatedimage;
			delete tmpcanvas;
			delete pThumbBitmapO;
			} else {
			imgtoosmall = true;
			}
			} else {
			imgtoosmall = true;
			}
		} else {
			if ((imgh >= 640) && (imgw >= 480)) {
			ratiow = ((double)pThumbBitmap->GetWidth() / (double)imgw);
			ratioh = ((double)pThumbBitmap->GetHeight() / (double)imgh);
			if (ratiow > ratioh) {
				ratio = ratiow;
			} else {
				ratio = ratioh;
			}
			imgw = imgw * ratio;
			imgh = imgh * ratio;
			if ((imgh >= 640) && (imgw >= 480)) {
			Bitmap *pThumbBitmapO = pImage->DecodeN(selectedthumb, BITMAP_PIXEL_FORMAT_RGB565, imgw, imgh);
			cropt = (int)((double)(pThumbBitmapO->GetHeight() - pThumbBitmap->GetHeight()) * 0.5f);
			if (cropt < 0) cropt = 0;
			pThumbBitmap->Merge(Point(0,0), *pThumbBitmapO, Rectangle(0,cropt,pThumbBitmap->GetWidth(),pThumbBitmap->GetHeight()));
			delete pThumbBitmapO;
			} else {
			imgtoosmall = true;
			}
			} else {
			imgtoosmall = true;
			}
		}
		if (Osp::Io::File::IsFileExist(L"/Home/capturedimg.jpg")) {
			Osp::Io::File::Remove(L"/Home/capturedimg.jpg");
		}
		if (imgtoosmall) {
			MessageBox msgboxsmall;
			int modalResultsmall = 0;
			msgboxsmall.Construct("Image too small", "Image is too small, only 640x480 or 480x640 or above is supported!", MSGBOX_STYLE_OK, 10000);
			msgboxsmall.ShowAndWait(modalResultsmall);
		} else {
			pImage->EncodeToFile(*pThumbBitmap,IMG_FORMAT_JPG,L"/Home/capturedimg.jpg", true);
			isok = true;
		}
		}
		delete pImgMeta;
		delete pThumbBitmap;
		delete pImage;
		if (isok) {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->previewbackgotoform = 2;
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PREVIEWFORM, null);
		} else {
			if (!imgtoosmall) {
				MessageBox msgbox;
				int modalResult = 0;
				msgbox.Construct("Error", "Error opening image!", MSGBOX_STYLE_OK, 10000);
				msgbox.ShowAndWait(modalResult);
			}
		}
	} else if (actionId == APREVPAGE) {
		int showpage = this->curpage;
		if ((this->curpage-1) >= 1) {
			showpage = this->curpage - 1;
		} else {
			showpage = this->pagescount;
		}
		if (this->curpage != showpage) {
			__pIconList->RemoveAllItems();
			LoadThumbsPage(showpage);
			__pIconList->ScrollToTop();
		}
	} else if (actionId == ANEXTPAGE) {
		int showpage = this->curpage;
		if ((this->curpage+1) <= this->pagescount) {
			showpage = this->curpage + 1;
		} else {
			showpage = 1;
		}
		if (this->curpage != showpage) {
			__pIconList->RemoveAllItems();
			LoadThumbsPage(showpage);
			__pIconList->ScrollToTop();
		}
	} else if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_SELECTFORM, null);
	}
	actioninprogress = false;
}

void
FGallery::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGallery::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGallery::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGallery::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FGallery::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
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
FGallery::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FGallery::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			button_->pressed = false;
			buttons_->SetAt(*button_,currpressedbtn,false);
			currpressedbtn = -1;
			RequestRedraw();
			this->OnActionPerformed(source, button_->action);
		}
	}
}

void
FGallery::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	ThumbsListItem * thumslistitem_ = static_cast<ThumbsListItem *> (thumbslist->GetAt(itemId-1));
	selectedthumb = thumslistitem_->filename;
    this->OnActionPerformed(source, AIMGSEL);
}
