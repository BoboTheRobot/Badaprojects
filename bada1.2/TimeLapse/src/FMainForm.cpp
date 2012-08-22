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
#include "FMainForm.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"
#include "TimeLapseClass.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Io;
using namespace Osp::Media;
using namespace Osp::Base::Utility;
using namespace Osp::Graphics;
using namespace Osp::App;


IntervalValueItem::IntervalValueItem(int sec, Osp::Base::String caption) {
	this->sec = sec;
	this->caption = caption;
}

IntervalValueItem::~IntervalValueItem(void) {

}

ResolutionValueItem::ResolutionValueItem(int width, int height) {
	this->width = width;
	this->height = height;
}

ResolutionValueItem::~ResolutionValueItem(void) {

}

StopAfterValueItem::StopAfterValueItem(int frames) {
	this->frames = frames;
}

StopAfterValueItem::~StopAfterValueItem(void) {

}

ThumbsListItemThumb::ThumbsListItemThumb(Osp::Base::String filename, Osp::Graphics::Rectangle pos, TimeLapseClass * TimeLapseClass_, Osp::Media::Image * pimagedecoder_) {

	TIMELAPSE_FILEINFO fileinfo_;
	this->filename = filename;
	this->pos = pos;
	this->thumbimg = null;
	TimeLapseClass_->ReadTLVfileinfo(this->filename, fileinfo_);
	this->title = fileinfo_.datetime.ToString() + L"\n" + fileinfo_.datetimeend.ToString();
	if (fileinfo_.firstframesize > 0) {
	Osp::Base::ByteBuffer * tmpimgbuffer = new Osp::Base::ByteBuffer();
	tmpimgbuffer->Construct(fileinfo_.firstframesize);
	TimeLapseClass_->ReadTLVFrame(this->filename, 1, *tmpimgbuffer);
	/*int thumbheight = 125;
	if (fileinfo_.resolution.width == 320) {
		thumbheight = 156;
	}*/
	this->thumbimg = pimagedecoder_->DecodeN(*tmpimgbuffer, IMG_FORMAT_JPG, BITMAP_PIXEL_FORMAT_RGB565, 208, 125);
	delete tmpimgbuffer;
	}

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

FMainForm::FMainForm(void)
{
}

FMainForm::~FMainForm(void)
{
}

bool
FMainForm::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL);
	this->SetName(L"FMainForm");
	this->SetOrientation(ORIENTATION_LANDSCAPE);

	return true;
}

result
FMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;
	
	if (File::IsFileExist(L"/Home/lastcapture.tmp")) {
		File::Remove(L"/Home/lastcapture.tmp");
	}

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * helpbtn = new ButtonCostumEl(Rectangle(10,20,101,98), AHELP, 0, BOXBTN, FORMTYPE_NORMAL, L"Help");
	buttons_->Add(*helpbtn);
	ButtonCostumEl * infobtn = new ButtonCostumEl(Rectangle(10,140,101,98), AINFO, 1, BOXBTN, FORMTYPE_NORMAL, L"Info");
	buttons_->Add(*infobtn);
	ButtonCostumEl * settingsbtn = new ButtonCostumEl(Rectangle(206,120,101,98), ASETTINGS, 2, BOXBTN, FORMTYPE_NORMAL, L"Settings");
	buttons_->Add(*settingsbtn);
	ButtonCostumEl * startbtn = new ButtonCostumEl(Rectangle(606,87,183,132), ASTART, 0, BIGBTN, FORMTYPE_NORMAL, L"Start");
	buttons_->Add(*startbtn);

	ButtonCostumEl * savesettingsbtn = new ButtonCostumEl(Rectangle(10,20,101,98), ASAVESETTINGS, 3, BOXBTN, FORMTYPE_SETTINGS, L"Save settings");
	buttons_->Add(*savesettingsbtn);

	ButtonCostumEl * closeinfobtn = new ButtonCostumEl(Rectangle(10,20,101,98), ACLOSEINFO, 3, BOXBTN, FORMTYPE_INFO, L"Close info");
	buttons_->Add(*closeinfobtn);

	ButtonCostumEl * closehelpbtn = new ButtonCostumEl(Rectangle(10,20,101,98), ACLOSEHELP, 3, BOXBTN, FORMTYPE_HELP, L"Close help");
	buttons_->Add(*closehelpbtn);
	
	formtype = FORMTYPE_NORMAL;

	TimeLapseClass_ = new TimeLapseClass();

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
			if (Osp::Io::File::GetFileExtension(entry.GetName()) == L"tlv") {
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

	curpage = 1;
	pagescount = Math::Ceiling(thumbslist->GetCount() / 2.0f);
	LoadThumbsPage(curpage);

	if (pagescount > 1) {
	ButtonCostumEl * prevbtn = new ButtonCostumEl(Rectangle(717,269,64,64), AGALLERYPREV, 0, ROUNDBTN, FORMTYPE_NORMAL, L"Pager");
	buttons_->Add(*prevbtn);
	ButtonCostumEl * nextbtn = new ButtonCostumEl(Rectangle(717,396,64,64), AGALLERYNEXT, 1, ROUNDBTN, FORMTYPE_NORMAL, L"Pager");
	buttons_->Add(*nextbtn);
	}

	currpressedbtn = -1;
	currselslider = -1;

	intervalselected = 0;
	intervalprocvalue = 0;

	intervalvalueslist = new Osp::Base::Collection::ArrayList;
    intervalvalueslist->Construct();
	intervalvalueslist->Add(*(new IntervalValueItem(1, L"1 sec")));
	intervalvalueslist->Add(*(new IntervalValueItem(2, L"2 sec")));
	intervalvalueslist->Add(*(new IntervalValueItem(5, L"5 sec")));
	intervalvalueslist->Add(*(new IntervalValueItem(10, L"10 sec")));
	intervalvalueslist->Add(*(new IntervalValueItem(30, L"30 sec")));
	intervalvalueslist->Add(*(new IntervalValueItem(1*60, L"1 min")));
	intervalvalueslist->Add(*(new IntervalValueItem(2*60, L"2 min")));
	intervalvalueslist->Add(*(new IntervalValueItem(5*60, L"5 min")));
	intervalvalueslist->Add(*(new IntervalValueItem(10*60, L"10 min")));
	intervalvalueslist->Add(*(new IntervalValueItem(30*60, L"30 min")));

	resolutionselected = 1;
	resolutionprocvalue = 1;
	resolutionvalueslist = new Osp::Base::Collection::ArrayList;
	resolutionvalueslist->Construct();
	resolutionvalueslist->Add(*(new ResolutionValueItem(320,240)));
	resolutionvalueslist->Add(*(new ResolutionValueItem(800,480)));

	stopafterselected = 0;
	stopafterprocvalue = 0;
	stopaftervalueslist = new Osp::Base::Collection::ArrayList;
	stopaftervalueslist->Construct();
	stopaftervalueslist->Add(*(new StopAfterValueItem(0)));

	for (int fillvali=1;fillvali <= 20;fillvali++) {
		stopaftervalueslist->Add(*(new StopAfterValueItem((fillvali * 10) * fillvali)));
	}

	settingstext = L"";

	LoadSettings();

	this->AddTouchEventListener(*this);
	
	displaybuffer_ = new Canvas();
	displaybuffer_->Construct(Rectangle(0,0,800,480));
	RedrawDisplayBuffer(displaybuffer_->GetBounds());
	
	
	return r;
}

void FMainForm::SaveSettings() {
	Osp::Base::String value;
	int intvalue;

	intvalue = intervalselected;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"interval", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"interval", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"interval", intvalue);
	}
	intvalue = resolutionselected;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"resolution", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"resolution", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"resolution", intvalue);
	}
	intvalue = stopafterselected;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"stopafter", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"stopafter", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"stopafter", intvalue);
	}

}

void FMainForm::LoadSettings() {
	int intvalue;
	Osp::Base::String value;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"interval", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue > -1) {
		intervalselected = intvalue;
		intervalprocvalue = ((double)(intvalue) / (double)(intervalvalueslist->GetCount()-1));
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"resolution", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue > -1) {
		resolutionselected = intvalue;
		resolutionprocvalue = ((double)(intvalue) / (double)(resolutionvalueslist->GetCount()-1));
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"stopafter", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue > -1) {
		stopafterselected = intvalue;
		stopafterprocvalue = ((double)(intvalue) / (double)(stopaftervalueslist->GetCount()-1));
	}

	settingstext = L"";
	ResolutionValueItem * resolutionvalitem_ = static_cast<ResolutionValueItem *> (resolutionvalueslist->GetAt(resolutionselected));
	settingstext.Append(L"Resolution " + Osp::Base::Integer::ToString(resolutionvalitem_->width) + L"x" + Osp::Base::Integer::ToString(resolutionvalitem_->height));
	StopAfterValueItem * stopaftervalitem_ = static_cast<StopAfterValueItem *> (stopaftervalueslist->GetAt(stopafterselected));
	if (stopaftervalitem_->frames > 0) {
		if (settingstext != L"") settingstext.Append(L"\n");
		settingstext.Append(L"Stop after " + Osp::Base::Integer::ToString(stopaftervalitem_->frames) + L" frames");
	}

}

void FMainForm::LoadThumbsPage(int page) {
	thumbslistthumbs->RemoveAll(true);
	Osp::Base::Collection::IEnumerator * pEnum = thumbslist->GetEnumeratorN();
    ThumbsListItem * thumslistitem_ = null;
    int imgindex = 1;
    int imgindexout = 1;
    Osp::Graphics::Rectangle poss;
    int xsep = 232;

    ButtonCostumEl* button_;

    int n = 0;
   	while (n < buttons_->GetCount()) {
   		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
   		if (button_->type == THUMB) {
   			buttons_->RemoveAt(n, true);
   			n--;
   		} else {
   			n++;
   		}
    }

   	int thumbindex = 0;
    while (pEnum->MoveNext() == E_SUCCESS) {
    	if ((imgindex > ((page-1) * 2)) && (imgindex <= ((page) * 2))) {
    		thumslistitem_ = static_cast<ThumbsListItem *> (pEnum->GetCurrent());
    		poss = Rectangle(213,270,208,188);
    		if (imgindexout == 2) {
    			poss.SetPosition(poss.x + xsep, poss.y);
    		}
    		thumbindex++;
    		ThumbsListItemThumb * thumslistitemthumb_ = new ThumbsListItemThumb(thumslistitem_->filename, poss, TimeLapseClass_, pimagedecoder_);
    		thumbslistthumbs->Add(*thumslistitemthumb_);

    		button_ = new ButtonCostumEl(poss, 1000 + imgindex, thumbindex, THUMB, FORMTYPE_NORMAL, L"");
    		button_->pressposition = button_->position;
    		buttons_->Add(*button_);
    		imgindexout++;
    	}
   		imgindex++;
    }
    delete pEnum;
}

void FMainForm::RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect) {
	displaybufferisindrawingstate = true;
	Osp::Graphics::Rectangle thisrect;
	//bg
	drawingclass_->FDrawSpriteToCanvas(displaybuffer_, redrawrect, redrawrect);
	//bg separator
	if (formtype == FORMTYPE_NORMAL) {
		thisrect = Rectangle(186,230,614,20);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			displaybuffer_->FillRectangle(Color(0,0,0,80), thisrect);
		}
	}
	//toolbar 1 bg
	thisrect = Rectangle(0,0,120,480);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		displaybuffer_->FillRectangle(Color(0,0,0,125), Rectangle(0,0,120,480));
	}
	//logo
	thisrect = Rectangle(28,290,66,156);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		drawingclass_->FDrawSpriteToCanvas(displaybuffer_, thisrect, Rectangle(800,0,thisrect.width,thisrect.height));
	}
	//toolbar 2 bg
	thisrect = Rectangle(120,0,66,480);
	if (redrawrect.IsIntersected(thisrect)) {
		redrawrect = redrawrect.GetUnion(thisrect);
		displaybuffer_->FillRectangle(Color(0,0,0,80), Rectangle(120,0,66,480));
		if (formtype == FORMTYPE_NORMAL) {
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(142,33,22,165), Rectangle(822,156,22,165));
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(142,283,22,165), Rectangle(800,156,22,165));
		} else if (formtype == FORMTYPE_SETTINGS) {
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(142,146,22,188), Rectangle(800,321,22,188));
		} else if (formtype == FORMTYPE_INFO) {
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(142,193,22,94), Rectangle(822,321,22,94));
		} else if (formtype == FORMTYPE_HELP) {
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(142,193,22,94), Rectangle(822,415,22,94));
		}
	}
	int sliderposx = 0;
	int tmpvalindex = 0;
	double cursliderval = 0;
	if (formtype == FORMTYPE_NORMAL) {
		//interval slider
		thisrect = Rectangle(203,10,383,100);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x,thisrect.y+42,thisrect.width,15), Rectangle(0,480,383,15));
			cursliderval = intervalprocvalue;
			if (currselslider == 1) {
				cursliderval = cursliderintervalvalue;
			}
			sliderposx = (thisrect.x + 35) + (int)(cursliderval * 313.0f) - 31;
			tmpvalindex = Math::Round(cursliderval * (double)(intervalvalueslist->GetCount()-1));
			IntervalValueItem * tmpvalitem_ = static_cast<IntervalValueItem *> (intervalvalueslist->GetAt(tmpvalindex));
			if (cursliderval > 0.5f) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+14,142,20), Rectangle(0,495,142,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+62,100,23), tmpvalitem_->caption, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-142-20,thisrect.y+14,142,20), Rectangle(0,495,142,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-100-20,thisrect.y+62,100,23), tmpvalitem_->caption, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_RIGHT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			}
			if (currselslider == 1) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(445,480,62,100));
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(383,480,62,100));
			}
		}
	} else if (formtype == FORMTYPE_SETTINGS) {
		//resolution slider
		thisrect = Rectangle(203,10,383,100);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x,thisrect.y+42,thisrect.width,15), Rectangle(0,480,383,15));
			cursliderval = resolutionprocvalue;
			if (currselslider == 2) {
				cursliderval = cursliderresolutinvalue;
			}
			sliderposx = (thisrect.x + 35) + (int)(cursliderval * 313.0f) - 31;
			tmpvalindex = Math::Round(cursliderval * (double)(resolutionvalueslist->GetCount()-1));
			ResolutionValueItem * tmpvalitemres_ = static_cast<ResolutionValueItem *> (resolutionvalueslist->GetAt(tmpvalindex));
			if (cursliderval > 0.5f) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+14,176,20), Rectangle(142,495,176,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+62,100,23), Osp::Base::Integer::ToString(tmpvalitemres_->width) + L"x" + Osp::Base::Integer::ToString(tmpvalitemres_->height), 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-176-20,thisrect.y+14,176,20), Rectangle(142,495,176,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-100-20,thisrect.y+62,100,23), Osp::Base::Integer::ToString(tmpvalitemres_->width) + L"x" + Osp::Base::Integer::ToString(tmpvalitemres_->height), 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_RIGHT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			}
			if (currselslider == 2) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(445,480,62,100));
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(383,480,62,100));
			}
		}
		//stopafter slider
		thisrect = Rectangle(203,160,383,100);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x,thisrect.y+42,thisrect.width,15), Rectangle(0,480,383,15));
			cursliderval = stopafterprocvalue;
			if (currselslider == 3) {
				cursliderval = cursliderstopaftervalue;
			}
			sliderposx = (thisrect.x + 35) + (int)(cursliderval * 313.0f) - 31;
			tmpvalindex = Math::Round(cursliderval * (double)(stopaftervalueslist->GetCount()-1));
			StopAfterValueItem * tmpvalitemstopafter_ = static_cast<StopAfterValueItem *> (stopaftervalueslist->GetAt(tmpvalindex));
			String stopaftervalcaption;
			if (tmpvalitemstopafter_->frames > 0) {
				stopaftervalcaption = Osp::Base::Integer::ToString(tmpvalitemstopafter_->frames) + L" frames";
			} else {
				stopaftervalcaption = L"Manual stop";
			}
			if (cursliderval > 0.5f) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+14,170,20), Rectangle(142,515,170,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+20,thisrect.y+59,150,28), stopaftervalcaption, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-170-20,thisrect.y+14,170,20), Rectangle(142,515,170,20));
				drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(thisrect.x+thisrect.width-150-20,thisrect.y+59,150,28), stopaftervalcaption, 28, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_RIGHT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			}
			if (currselslider == 3) {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(445,480,62,100));
			} else {
				drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(sliderposx,thisrect.y,62,100), Rectangle(383,480,62,100));
			}
		}
	}
	if (formtype == FORMTYPE_NORMAL) {
		//settings text
		thisrect = Rectangle(327,120,250,98);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			drawingclass_->FDrawTextToCanvas(displaybuffer_, thisrect, settingstext, 24, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
		//pager
		if (pagescount > 1) {
			thisrect = Rectangle(717,333,64,63);
			if (redrawrect.IsIntersected(thisrect)) {
				redrawrect = redrawrect.GetUnion(thisrect);
				drawingclass_->FDrawTextToCanvas(displaybuffer_, thisrect, Osp::Base::Integer::ToString(curpage) + L"/" + Osp::Base::Integer::ToString(pagescount), 24, Osp::Graphics::FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			}
		}
		//gallery empty
		if (thumbslist->GetCount() < 1) {
			thisrect = Rectangle(213,270,504,200);
			if (redrawrect.IsIntersected(thisrect)) {
				redrawrect = redrawrect.GetUnion(thisrect);
				drawingclass_->FDrawTextToCanvas(displaybuffer_, thisrect, L"You have no videos in gallery.\nStart recording ;)", 24, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
			}
		}
	} else if (formtype == FORMTYPE_SETTINGS) {
		//
	} else if (formtype == FORMTYPE_INFO) {
		thisrect = Rectangle(203,10,587,460);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			String infotext = L"ABOUT\n";
			infotext.Append(L"Time-lapse photography is a cinematography technique whereby the frequency at which film frames are captured (the frame rate) is much lower than that which will be used to play the sequence back. When replayed at normal speed, time appears to be moving faster and thus lapsing. (source: Wikipedia)\n\n");
			infotext.Append(L"Programming & Design:\nBostjan Mrak\n\n");
			infotext.Append(L"Business partner:\nPagein,d.o.o.");
			drawingclass_->FDrawTextToCanvas(displaybuffer_, thisrect, infotext, 24, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
	} else if (formtype == FORMTYPE_HELP) {
		thisrect = Rectangle(203,10,587,460);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			String helptext = L"WHEN TO USE\n";
			helptext.Append(L"For recording your road trip, clouds in the sky, sun from dawn to dusk, flowers opening in the morning, flowers growing...etc.\nFor fun just record person with different face expressions :)\n\n");
			helptext.Append(L"HOW TO USE\n");
			helptext.Append(L"1. Choose speed, resolution, stop after counter and press START.\n");
			helptext.Append(L"2. Put phone in fixed position and press camera button.\n");
			helptext.Append(L"3. For stop recording press camera button.\n\n");
			helptext.Append(L"NOTICE\n");
			helptext.Append(L"For best performance use lower resolution.\nWhen recording long footage it's recommended that you have full battery or device plugged to charger.\nYou need at least 50mb of free space on active memory, when memory is low the recording will stop.\nWe recommend you to activate airplane mode, when interruption occurred (incoming call) the recording will stop.");
			drawingclass_->FDrawTextToCanvas(displaybuffer_, thisrect, helptext, 24, Osp::Graphics::FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, Osp::Graphics::TEXT_ALIGNMENT_LEFT, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
		}
	}
	//buttons
	ButtonCostumEl* button_;
	ThumbsListItemThumb * thumslistitem_ = null;
	bool isvisible;
	Rectangle btnicorect;
	Point btnicoshift;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = (button_->group == formtype);
		if (isvisible) {
		thisrect = Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height);
		if (redrawrect.IsIntersected(thisrect)) {
			redrawrect = redrawrect.GetUnion(thisrect);
			if (button_->type == BOXBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,0+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,0,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(1049,(button_->icoindex * 60),60,60);
					btnicoshift.SetPosition(0,0);
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+20+btnicoshift.x,button_->position.y+19+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
			if (button_->type == BIGBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,324+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,324,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(0,515+(button_->icoindex * 25),131,25);
					btnicoshift.SetPosition(0,0);
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+30+btnicoshift.x,button_->position.y+54+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
			if (button_->type == ROUNDBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,196+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(866,196,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(1109,0+(button_->icoindex * 40),40,40);
					btnicoshift.SetPosition(0,0);
					drawingclass_->FDrawSpriteToCanvas(displaybuffer_, Rectangle(button_->position.x+12+btnicoshift.x,button_->position.y+12+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
			if (button_->type == THUMB) {
				if (isvisible) {
					Osp::Graphics::Color fcolor = Color(255, 255, 255);
					if ((button_->pressed) || (button_->toogle)) {
						fcolor = Color(215, 167, 72);
					}
					thumslistitem_ = static_cast<ThumbsListItemThumb *> (thumbslistthumbs->GetAt(button_->icoindex-1));
					if (thumslistitem_->thumbimg != null) {
						/*Rectangle cropthumbrect;
						cropthumbrect.SetBounds(0,0,thumslistitem_->thumbimg->GetWidth(), thumslistitem_->thumbimg->GetHeight());
						if (thumslistitem_->thumbimg->GetHeight() == 156) {
							cropthumbrect.y = 15;
						}*/
						displaybuffer_->DrawBitmap(Rectangle(button_->position.x,button_->position.y,button_->position.width,125), *thumslistitem_->thumbimg);
						//displaybuffer_->DrawBitmap(Point(button_->position.x, button_->position.y), *thumslistitem_->thumbimg);
					}
					drawingclass_->FDrawTextToCanvas(displaybuffer_, Rectangle(button_->position.x, button_->position.y + 140,button_->position.width,48), thumslistitem_->title, 24, Osp::Graphics::FONT_STYLE_PLAIN, true, fcolor, Osp::Graphics::TEXT_ALIGNMENT_CENTER, Osp::Graphics::TEXT_ALIGNMENT_MIDDLE);
					displaybuffer_->SetForegroundColor(fcolor);
					displaybuffer_->DrawRectangle(Rectangle(button_->position.x, button_->position.y, button_->position.width, 125));
				}
			}
		}
		}
	}
	canvasredrawrect_ = redrawrect;
	displaybufferisindrawingstate = false;
}

result
FMainForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		pCanvas_->Copy(Point(0,0),*displaybuffer_,Rectangle(0,0,800,480));
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	buttons_->RemoveAll(true);
	delete buttons_;

	intervalvalueslist->RemoveAll(true);
	delete intervalvalueslist;

	resolutionvalueslist->RemoveAll(true);
	delete resolutionvalueslist;

	stopaftervalueslist->RemoveAll(true);
	delete stopaftervalueslist;

	thumbslist->RemoveAll(true);
	delete thumbslist;

	thumbslistthumbs->RemoveAll(true);
	delete thumbslistthumbs;

	delete pimagedecoder_;

	delete displaybuffer_;

	return r;
}

void
FMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId) {
	if (actionId == AHELP) {
		formtype = FORMTYPE_HELP;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == AINFO) {
		formtype = FORMTYPE_INFO;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == ASETTINGS) {
		formtype = FORMTYPE_SETTINGS;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == ASAVESETTINGS) {
		SaveSettings();
		LoadSettings();
		formtype = FORMTYPE_NORMAL;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == ACLOSEINFO) {
		formtype = FORMTYPE_NORMAL;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == ACLOSEHELP) {
		formtype = FORMTYPE_NORMAL;
		RedrawDisplayBuffer(displaybuffer_->GetBounds());
		RequestRedraw();
	} else if (actionId == AGALLERYNEXT) {
		if (curpage < pagescount) {
			curpage++;
			LoadThumbsPage(curpage);
			RedrawDisplayBuffer(displaybuffer_->GetBounds());
			this->RequestRedraw();
		}
	} else if (actionId == AGALLERYPREV) {
		if (curpage > 1) {
			curpage--;
			LoadThumbsPage(curpage);
			RedrawDisplayBuffer(displaybuffer_->GetBounds());
			this->RequestRedraw();
		}
	} else if (actionId == ASTART) {
		SaveSettings();
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		Osp::Base::Collection::ArrayList * list = new Osp::Base::Collection::ArrayList;
		IntervalValueItem * tmpvalitem_ = static_cast<IntervalValueItem *> (intervalvalueslist->GetAt(intervalselected));
		ResolutionValueItem * tmpvalresitem_ = static_cast<ResolutionValueItem *> (resolutionvalueslist->GetAt(resolutionselected));
		StopAfterValueItem * tmpvalstopafteritem_ = static_cast<StopAfterValueItem *> (stopaftervalueslist->GetAt(stopafterselected));
		list->Construct();
		list->Add(*(new Integer(tmpvalitem_->sec)));
		list->Add(*(new Osp::Graphics::Dimension(tmpvalresitem_->width,tmpvalresitem_->height)));
		list->Add(*(new Integer(tmpvalstopafteritem_->frames)));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_CAMERAFORM, list);
	} else if (actionId >= 1001) {
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
			AppLog("open filename %S", imgfilename.GetPointer());
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			Osp::Base::Collection::ArrayList * list = new Osp::Base::Collection::ArrayList;
			list->Construct();
			list->Add(*(new Osp::Base::String(imgfilename)));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PLAYERFORM, list);
		}
	}
}

void FMainForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo){

}
void FMainForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {

}
void FMainForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	if ((Osp::Base::Utility::Math::Abs(oldpresspos.x - currentPosition.x) < 15) && (Osp::Base::Utility::Math::Abs(oldpresspos.y - currentPosition.y) < 15)) {
		return;
	}
	bool needsredraw = false;
	Osp::Graphics::Rectangle redrawrect(0,0,-1,-1);
	if (currselslider == -1) {
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (!button_->pressposition.Contains(currentPosition)) {
			if (button_->pressed) {
				button_->pressed = false;
				buttons_->SetAt(*button_,currpressedbtn,false);
				currpressedbtn = -1;
				if (needsredraw == true) {
					redrawrect = redrawrect.GetUnion(button_->position);
				} else {
					redrawrect = button_->position;
				}
				needsredraw = true;
			}
		}
	}
	bool isvisible = true;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = (button_->group == formtype);
		if ((isvisible) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				if (needsredraw == true) {
					redrawrect = redrawrect.GetUnion(button_->position);
				} else {
					redrawrect = button_->position;
				}
				needsredraw = true;
			}
			break;
		}
	}
	}
	if ((currpressedbtn == -1) && (formtype == FORMTYPE_NORMAL)) {
		Osp::Graphics::Rectangle sliderpos;
		sliderpos.SetBounds(203,10,383,100);
		if (((currentPosition.y >= sliderpos.y) && (currentPosition.y <= (sliderpos.y+sliderpos.height)) && (currselslider == -1)) || (currselslider == 1)) {
			if (((currentPosition.x >= sliderpos.x+31) && (currentPosition.x <= sliderpos.x+31+313) && (currselslider == -1)) || (currselslider == 1)) {
				if (currselslider != 1) {
					cursliderintervalvalue = intervalprocvalue;
				}
				double sliderp = ((double)(currentPosition.x - 31 - (sliderpos.x)) / 313.0f);
				if (sliderp > 1) sliderp = 1;
				if (sliderp < 0) sliderp = 0;
				if (sliderp != cursliderintervalvalue) {
					cursliderintervalvalue = sliderp;
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(sliderpos);
					} else {
						redrawrect = sliderpos;
					}
					needsredraw = true;
				}
				currselslider = 1;
			}
		}
	} else if ((currpressedbtn == -1) && (formtype == FORMTYPE_SETTINGS)) {
		Osp::Graphics::Rectangle sliderpos;
		sliderpos.SetBounds(203,10,383,100);
		if (((currentPosition.y >= sliderpos.y) && (currentPosition.y <= (sliderpos.y+sliderpos.height)) && (currselslider == -1)) || (currselslider == 2)) {
			if (((currentPosition.x >= sliderpos.x+31) && (currentPosition.x <= sliderpos.x+31+313) && (currselslider == -1)) || (currselslider == 2)) {
				if (currselslider != 2) {
					cursliderresolutinvalue = resolutionprocvalue;
				}
				double sliderp = ((double)(currentPosition.x - 31 - (sliderpos.x)) / 313.0f);
				if (sliderp > 1) sliderp = 1;
				if (sliderp < 0) sliderp = 0;
				if (sliderp != cursliderresolutinvalue) {
					cursliderresolutinvalue = sliderp;
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(sliderpos);
					} else {
						redrawrect = sliderpos;
					}
					needsredraw = true;
				}
				currselslider = 2;
			}
		}
		sliderpos.SetBounds(203,160,383,100);
		if (((currentPosition.y >= sliderpos.y) && (currentPosition.y <= (sliderpos.y+sliderpos.height)) && (currselslider == -1)) || (currselslider == 3)) {
			if (((currentPosition.x >= sliderpos.x+31) && (currentPosition.x <= sliderpos.x+31+313) && (currselslider == -1)) || (currselslider == 3)) {
				if (currselslider != 3) {
					cursliderstopaftervalue = stopafterprocvalue;
				}
				double sliderp = ((double)(currentPosition.x - 31 - (sliderpos.x)) / 313.0f);
				if (sliderp > 1) sliderp = 1;
				if (sliderp < 0) sliderp = 0;
				if (sliderp != cursliderstopaftervalue) {
					cursliderstopaftervalue = sliderp;
					if (needsredraw == true) {
						redrawrect = redrawrect.GetUnion(sliderpos);
					} else {
						redrawrect = sliderpos;
					}
					needsredraw = true;
				}
				currselslider = 3;
			}
		}
	}
	oldpresspos = currentPosition;
	if (needsredraw) {
		if (displaybufferisindrawingstate == false) {
			RedrawDisplayBuffer(redrawrect); //redraw only changed region
			/*if (canvasredrawrect_.y < 100) {
				RequestRedraw();
			} else {*/
				Canvas* formCanvas;
				formCanvas = GetCanvasN();
				formCanvas->Copy(Point(canvasredrawrect_.x, canvasredrawrect_.y), *displaybuffer_, canvasredrawrect_);
				formCanvas->Show(canvasredrawrect_);
				delete formCanvas;
			/*}*/
		}
	}
}
void FMainForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	currselslider = -1;
	cursliderintervalvalue = 0;
	cursliderresolutinvalue = 0;
	cursliderstopaftervalue = 0;
	oldpresspos.SetPosition(-1,-1);
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FMainForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	bool needsredraw = false;
	Osp::Graphics::Rectangle redrawrect(0,0,-1,-1);
	bool istoogle = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			istoogle = false;
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
			if (needsredraw == true) {
				redrawrect = redrawrect.GetUnion(button_->position);
			} else {
				redrawrect = button_->position;
			}
			needsredraw = true;
		}
	}
	if ((currselslider == 1) && (formtype == FORMTYPE_NORMAL)) {
		intervalprocvalue = cursliderintervalvalue;
		double sliderlockfactor = (1.0f/(double)(intervalvalueslist->GetCount()-1));
		intervalprocvalue = Math::Round(intervalprocvalue / sliderlockfactor) * sliderlockfactor;
		intervalselected = (int)Math::Round(intervalprocvalue * (double)(intervalvalueslist->GetCount()-1));
		if (needsredraw == true) {
			redrawrect = redrawrect.GetUnion(Rectangle(203,10,383,100));
		} else {
			redrawrect = Rectangle(203,10,383,100);
		}
		needsredraw = true;
	} else if ((currselslider == 2) && (formtype == FORMTYPE_SETTINGS)) {
		resolutionprocvalue = cursliderresolutinvalue;
		double sliderlockfactor = (1.0f/(double)(resolutionvalueslist->GetCount()-1));
		resolutionprocvalue = Math::Round(resolutionprocvalue / sliderlockfactor) * sliderlockfactor;
		resolutionselected = (int)Math::Round(resolutionprocvalue * (double)(resolutionvalueslist->GetCount()-1));
		if (needsredraw == true) {
			redrawrect = redrawrect.GetUnion(Rectangle(203,10,383,100));
		} else {
			redrawrect = Rectangle(203,10,383,100);
		}
		needsredraw = true;
	} else if ((currselslider == 3) && (formtype == FORMTYPE_SETTINGS)) {
		stopafterprocvalue = cursliderstopaftervalue;
		double sliderlockfactor = (1.0f/(double)(stopaftervalueslist->GetCount()-1));
		stopafterprocvalue = Math::Round(stopafterprocvalue / sliderlockfactor) * sliderlockfactor;
		stopafterselected = (int)Math::Round(stopafterprocvalue * (double)(stopaftervalueslist->GetCount()-1));
		if (needsredraw == true) {
			redrawrect = redrawrect.GetUnion(Rectangle(203,160,383,100));
		} else {
			redrawrect = Rectangle(203,160,383,100);
		}
		needsredraw = true;
	}
	currselslider = -1;
	currpressedbtn = -1;
	cursliderintervalvalue = 0;
	if (needsredraw) {
		RedrawDisplayBuffer(redrawrect);
		RequestRedraw();
	}
}
