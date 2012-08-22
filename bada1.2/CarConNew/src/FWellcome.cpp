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
#include "FWellcome.h"
#include "FormMgr.h"
#include "DrawingClass.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Media;

FWellcome::FWellcome(void)
{
}

FWellcome::~FWellcome(void)
{
}

bool
FWellcome::Initialize()
{
	Form::Construct(L"IDF_FWELLCOME");

	return true;
}

result
FWellcome::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_0, ID_NEXT);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	SetSoftkeyActionId(SOFTKEY_1, ID_EXITAPP);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;

	return r;
}

result
FWellcome::OnDraw()
{
	result r = E_SUCCESS;
	// canvas
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null)
	{
		pCanvas_->SetBackgroundColor(Color::COLOR_BLACK);
		pCanvas_->SetForegroundColor(Color::COLOR_WHITE);
		pCanvas_->Clear();
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,5,800));
		Image *pImage_ = new Image();
		pImage_->Construct();
		Bitmap* pBitmap_ = pImage_->DecodeN(L"/Res/Bitmap/wellcome.png", BITMAP_PIXEL_FORMAT_ARGB8888);
		delete pImage_;
		pCanvas_->DrawBitmap(Point(10, GetClientAreaBounds().y+10), *pBitmap_);
		delete pBitmap_;
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(10+184,GetClientAreaBounds().y+10+130,256,40),L"Welcome",42,FONT_STYLE_BOLD,true,Color(36,88,22),TEXT_ALIGNMENT_LEFT,TEXT_ALIGNMENT_BOTTOM);
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(15,GetClientAreaBounds().y+10+180,450,80),L"Before you can start you must choose some settings and input your car/s.",30,FONT_STYLE_PLAIN,true,Color(36,88,22),TEXT_ALIGNMENT_LEFT,TEXT_ALIGNMENT_TOP);
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(15,GetClientAreaBounds().y+10+180+80,450,280),L"Don\'t forget\n1.) Input decimal point with keypad # or * key.\n2.) Arrow beside average consumption will show your consumption increases or consumption decreases.\n3.) Input your data correct and before you forget :)\n \n",30,FONT_STYLE_PLAIN,true,Color::COLOR_WHITE,TEXT_ALIGNMENT_LEFT,TEXT_ALIGNMENT_TOP);
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(15,GetClientAreaBounds().y+10+180+80+260,450,90),L"Most important:\nKeep environment cleaner by responsible driving and sometimes catch a bus.",28,FONT_STYLE_PLAIN,true,Color(36,88,22),TEXT_ALIGNMENT_LEFT,TEXT_ALIGNMENT_TOP);
	}
	delete pCanvas_;
	return r;
}


result
FWellcome::OnTerminating(void)
{
	result r = E_SUCCESS;


	return r;
}

void
FWellcome::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	switch(actionId){
		case ID_EXITAPP:
			Application::GetInstance()->Terminate();
			break;
		case ID_NEXT: {
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			if (pFormMgr != null) {
				ArrayList * list = new ArrayList;
				list->Construct();
				list->Add(*(new Integer(2)));
				pFormMgr->SendUserEvent(FormMgr::REQUEST_SETTINGSFORM, list);
			}
			break;
		}
		default:
			break;
	};
}

