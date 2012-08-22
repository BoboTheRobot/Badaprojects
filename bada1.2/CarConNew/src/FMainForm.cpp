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
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"

using namespace Osp::App;
using namespace Osp::Base::Utility;
using namespace Osp::Locales;
using namespace Osp::Base;
using namespace Osp::Base::Collection;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Graphics;

void ButtonCostumEl::SetStartValues(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip) {
	this->action = setaction;
	this->position = setposition;
	this->pressposition = Rectangle(setposition.x+12, setposition.y+12, setposition.width-21, setposition.height-21);
	this->pressed = false;
	this->toogle = false;
	this->group = setgroup;
	this->icoindex = seticoindex;
	this->type = settype;
	this->tooltip = settooltip;
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, 0, settooltip);
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, setgroup, settooltip);
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setactiontoogle, int seticoindextoogle, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, 0, settooltip);
	this->actiontoogle = setactiontoogle;
	this->icoindextoogle = seticoindextoogle;
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, int setactiontoogle, int seticoindextoogle, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, setgroup, settooltip);
	this->actiontoogle = setactiontoogle;
	this->icoindextoogle = seticoindextoogle;
}

ButtonCostumEl::~ButtonCostumEl(void) {

}


FMainForm::FMainForm(void)
{
}

FMainForm::~FMainForm(void)
{
}

bool
FMainForm::Initialize()
{
	Construct(L"IDF_FMAINFORM");

	AddTouchEventListener(*this);

	return true;
}

result
FMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	drawingclass_ = pFormMgr->drawingclass_;
	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	buttons_ = new Osp::Base::Collection::ArrayList;

	buttons_->Add(*(new ButtonCostumEl(Rectangle(10,90,115,115), ACAR, ACAR, ICONBUTTON, L"Select, edit or add a car")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(125,90,115,115), ACON, ACON, ICONBUTTON, L"Manage consumption")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(240,90,115,115), AMONEY, AMONEY, ICONBUTTON, L"Manage expenses")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(355,90,115,115), AMORE, AMORE, ICONBUTTONTOOGLE, ALESS, ALESS, L"Open/Close more menu")));

	currpressedbtn = -1;
	moremeniopened = false;
	moremenipos = Rectangle(5,200,470,240);

	buttons_->Add(*(new ButtonCostumEl(Rectangle(10,205,115,115), APREGLEDFUELPRICE, APREGLEDFUELPRICE, ICONBUTTON, BUTTONGROUPMOREMENI, L"Fuel price reports, graphs and analytics")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(125,205,115,115), APREGLEDCON, APREGLEDCON, ICONBUTTON, BUTTONGROUPMOREMENI, L"Consumption reports, graphs and analytics")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(240,205,115,115), APREGLEDSTROSKI, APREGLEDSTROSKI, ICONBUTTON, BUTTONGROUPMOREMENI, L"Expenses reports, graphs and analytics")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(355,205,115,115), AHELP, AHELP, ICONBUTTON, BUTTONGROUPMOREMENI, L"Help")));
	//buttons_->Add(*(new ButtonCostumEl(Rectangle(10,320,115,115), ASERVICE, ASERVICE, ICONBUTTON, BUTTONGROUPMOREMENI, L"Manage services")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(125,320,115,115), ATRIP, ATRIP, ICONBUTTON, BUTTONGROUPMOREMENI, L"Trip expenses calculation")));
	//buttons_->Add(*(new ButtonCostumEl(Rectangle(240,320,115,115), APARK, APARK, ICONBUTTON, BUTTONGROUPMOREMENI, L"Manage parking timer")));
	buttons_->Add(*(new ButtonCostumEl(Rectangle(355,320,115,115), ASETTINGS, ASETTINGS, ICONBUTTON, BUTTONGROUPMOREMENI, L"Settings")));

	ButtonCostumEl * selviewecobtn = new ButtonCostumEl(Rectangle(11,380,235,120), ASELVIEWECO, ASELVIEWECOICO, BIGBUTTONGREENTOOGLE, BUTTONGROUPVIEW, ASELVIEWECO, ASELVIEWECOICO, L"Show consumption view");
	selviewecobtn->toogle = true;
	buttons_->Add(*selviewecobtn);
	buttons_->Add(*(new ButtonCostumEl(Rectangle(234,380,235,120), ASELVIEWMONEY, ASELVIEWMONEYICO, BIGBUTTONBLUETOOGLE, BUTTONGROUPVIEW, ASELVIEWMONEY, ASELVIEWMONEYICO, L"Show expenses view")));

	buttons_->Add(*(new ButtonCostumEl(Rectangle(10,652,227,140), ANEWFILLUP, 0, FILLUPBUTTON, BUTTONGROUPDONTDRAW, L"Add new fill up")));

	carconclass_ = pFormMgr->carconclass_;
	carconclass_->GetSettingsData(settingsdata_);
	carconclass_->GetCarConGetLastData(carconclass_->SelectedCar.ID, 0, lastcarcondata_, true);

	if (settingsdata_.avgcalctype == 2) {
		lastcarcondata_.AvgCon = carconclass_->GetCarConAvgSum(carconclass_->SelectedCar.ID);
	}

	currentselview = VIEWECO;

	BgCacheNumbersBmpN = null;

	CreateBgCacheNumbersBmpN();

	return r;
}

GrafItemDataMountains::GrafItemDataMountains(void) {

}

GrafItemDataMountains::~GrafItemDataMountains(void) {

}

void FMainForm::CreateBgCacheNumbersBmpN() {
	BgCacheNumbersBmpN = new Bitmap;
	Canvas * pCanvas_ = new Canvas;
	pCanvas_->Construct(Rectangle(0,0,960,800));

		//draw sky
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,960,800), Rectangle(0,0,5,800));
		//draw mountains con

		CarConData condata_;
		Osp::Base::Collection::IList* conmountains;

		Rectangle mountainsrect;
		ArrayList * grafdata = new ArrayList();
		grafdata->Construct();
		int datacount(0);
		int grafdatagroupycount(0);
		long long grafdatagroupx(0);
		double grafdatagroupysum(0);
		long long grafdatax(0);
		double grafdatay(0);
		int xpos(0);
		int ypos(0);
		int xposold(-1);
		int yposold(0);
		double yvalsum(0);
		double yvalcount(0);
		double yval;
		long long xval;
		double miny, maxy;
		miny = 0;
		maxy = 0;
		long long minx, maxx;
		minx = 0;
		maxx = 0;

		int peakpointindex(-1);
		Osp::Graphics::Point peakpoint;
		String peakpointvalue;

		GrafItemDataMountains* pid;

		mountainsrect = Rectangle(0,640-160,480,160);

		if (carconclass_->GetCarConDataListStart(carconclass_->SelectedCar.ID)) {
			while (carconclass_->GetCarConDataListGetData(condata_)) {
				if (condata_.Type == 1) {
				grafdatax = (long long)(condata_.time.GetTime().GetTicks() / 100000)+datacount;
				grafdatay = condata_.Con;
				if ((grafdatax == grafdatagroupx) || (grafdatagroupycount == 0)) {
					grafdatagroupysum = grafdatagroupysum + grafdatay;
					grafdatagroupycount = grafdatagroupycount + 1;
				} else {
					GrafItemDataMountains * gid = new GrafItemDataMountains();
					gid->xval = grafdatagroupx;
					gid->yval = grafdatagroupysum / grafdatagroupycount;
					grafdata->Add(*gid);
					grafdatagroupysum = grafdatay;
					grafdatagroupycount = 1;
				}
				grafdatagroupx = grafdatax;
				if (datacount > 5)
					break;
				datacount++;
				}
			}
			carconclass_->GetCarConDataListEnd();
		}
		if (datacount > 3) {
			conmountains = new Osp::Base::Collection::ArrayList;

			for (int n=0; n<grafdata->GetCount(); n++) {
			pid = static_cast<GrafItemDataMountains *> (grafdata->GetAt(n));
			if (pid->yval > maxy) {
				maxy = pid->yval;
				peakpointindex = n;
			}
			if ((pid->yval < miny) || (miny == 0))
				miny = pid->yval;
			if (pid->xval > maxx)
				maxx = pid->xval;
			if ((pid->xval < minx) || (minx == 0))
				minx = pid->xval;
			}
			if (maxy == miny) {
				maxy = maxy + 1;
				miny = miny - 1;
			}
			conmountains->Add(*(new Point(mountainsrect.x,mountainsrect.y+(int)Math::Round(mountainsrect.height * 0.5f))));
			for (int n=grafdata->GetCount()-1; n>=0; n--) {
				pid = static_cast<GrafItemDataMountains *> (grafdata->GetAt(n));
				yval = pid->yval;
				xval = pid->xval;
				yvalcount = yvalcount + 1;
				yvalsum = yvalsum + yval;
				yval = (yvalsum / yvalcount);
				double tmpxv(xval - minx);
				double tmpxv2(maxx - minx);
				xpos = (int)Math::Round((((mountainsrect.width-40) / tmpxv2) * tmpxv))+1;
				if ((maxy - miny) == 0)  {
				  ypos = (int)Math::Round(((mountainsrect.height)-(((mountainsrect.height) / 1) * (yval-miny))));
				} else {
				  ypos = (int)Math::Round(((mountainsrect.height)-(((mountainsrect.height) / (maxy - miny)) * (yval-miny))));
				}
				if (ypos > (mountainsrect.height)) {
				  ypos = (mountainsrect.height);
				}
				if (ypos < (0)) {
				  ypos = (0);
				}
				if ((xpos > xposold)) {
					if (peakpointindex == n) {
						peakpoint.SetPosition(mountainsrect.x+xpos+20, mountainsrect.y+ypos);
						peakpointvalue = L"";
						peakpointvalue.Format(25,L"%4.2f",yval);
					}
					if (xposold == -1) {
						conmountains->Add(*(new Point(mountainsrect.x+20,mountainsrect.y+ypos)));
					}
					conmountains->Add(*(new Point(mountainsrect.x+xpos+20,mountainsrect.y+ypos)));
					yvalcount = 0;
					yvalsum = 0;
				}
				xposold = xpos;
				yposold = ypos;
			}

			conmountains->Add(*(new Point(mountainsrect.x+mountainsrect.width,mountainsrect.y+(int)Math::Round(mountainsrect.height * 0.5f))));
		} else {
			conmountains = drawingclass_->CreateBlankMountains(mountainsrect);
		}
		grafdata->RemoveAll(true);

		drawingclass_->FDrawMountainsToCanvas(pCanvas_, mountainsrect, conmountains, peakpoint, peakpointvalue, settingsdata_.contypestr);
		conmountains->RemoveAll(true);
		delete conmountains;
		//draw mountains fin

		CarExpenseData findata_;
		Osp::Base::Collection::IList* finmountains;

		datacount = 0;
		grafdatagroupycount = 0;
		grafdatagroupx = 0;
		grafdatagroupysum = 0;
		grafdatax = 0;
		grafdatay = 0;
		xpos = 0;
		ypos = 0;
		xposold = -1;
		yposold = 0;
		yvalsum = 0;
		yvalcount = 0;
		yval = 0;
		xval = 0;
		miny = 0;
		maxy = 0;
		minx = 0;
		maxx = 0;
		peakpointindex = -1;
		peakpoint.SetPosition(0,0);
		peakpointvalue = L"";

		mountainsrect = Rectangle(480,640-160,480,160);

		if (carconclass_->GetExpenseDataListStart(carconclass_->SelectedCar.ID)) {
			while (carconclass_->GetExpenseDataListGetData(findata_)) {
				grafdatax = (long long)(findata_.time.GetTime().GetTicks() / 100000)+datacount;
				grafdatay = findata_.Price;
				if ((grafdatax == grafdatagroupx) || (grafdatagroupycount == 0)) {
					grafdatagroupysum = grafdatagroupysum + grafdatay;
					grafdatagroupycount = grafdatagroupycount + 1;
				} else {
					GrafItemDataMountains * gid = new GrafItemDataMountains();
					gid->xval = grafdatagroupx;
					gid->yval = grafdatagroupysum / grafdatagroupycount;
					grafdata->Add(*gid);
					grafdatagroupysum = grafdatay;
					grafdatagroupycount = 1;
				}
				grafdatagroupx = grafdatax;
				if (datacount > 5)
					break;
				datacount++;
			}
			carconclass_->GetExpenseDataListEnd();
		}
		if (datacount > 3) {
			finmountains = new Osp::Base::Collection::ArrayList;
			for (int n=0; n<grafdata->GetCount(); n++) {
			pid = static_cast<GrafItemDataMountains *> (grafdata->GetAt(n));
			if (pid->yval > maxy) {
				maxy = pid->yval;
				peakpointindex = n;
			}
			if ((pid->yval < miny) || (miny == 0))
				miny = pid->yval;
			if (pid->xval > maxx)
				maxx = pid->xval;
			if ((pid->xval < minx) || (minx == 0))
				minx = pid->xval;
			}
			if (maxy == miny) {
				maxy = maxy + 1;
				miny = miny - 1;
			}
			finmountains->Add(*(new Point(mountainsrect.x,mountainsrect.y+(int)Math::Round(mountainsrect.height * 0.5f))));
			for (int n=grafdata->GetCount()-1; n>=0; n--) {
				pid = static_cast<GrafItemDataMountains *> (grafdata->GetAt(n));
				yval = pid->yval;
				xval = pid->xval;
				yvalcount = yvalcount + 1;
				yvalsum = yvalsum + yval;
				yval = (yvalsum / yvalcount);
				double tmpxv(xval - minx);
				double tmpxv2(maxx - minx);
				xpos = (int)Math::Round((((mountainsrect.width-40) / tmpxv2) * tmpxv))+1;
				if ((maxy - miny) == 0)  {
				  ypos = (int)Math::Round(((mountainsrect.height)-(((mountainsrect.height) / 1) * (yval-miny))));
				} else {
				  ypos = (int)Math::Round(((mountainsrect.height)-(((mountainsrect.height) / (maxy - miny)) * (yval-miny))));
				}
				if (ypos > (mountainsrect.height)) {
				  ypos = (mountainsrect.height);
				}
				if (ypos < (0)) {
				  ypos = (0);
				}
				if ((xpos > xposold)) {
					if (xposold == -1) {
						finmountains->Add(*(new Point(mountainsrect.x+20,mountainsrect.y+ypos)));
					}
					finmountains->Add(*(new Point(mountainsrect.x+xpos+20,mountainsrect.y+ypos)));
					yvalcount = 0;
					yvalsum = 0;
				}
				if (peakpointindex == n) {
					peakpoint.SetPosition(mountainsrect.x+xpos+20, mountainsrect.y+ypos);
					peakpointvalue = L"";
					peakpointvalue.Format(25,L"%4.2f",yval);
				}
				xposold = xpos;
				yposold = ypos;
			}
			finmountains->Add(*(new Point(mountainsrect.x+mountainsrect.width,mountainsrect.y+(int)Math::Round(mountainsrect.height * 0.5f))));
		} else {
			finmountains = drawingclass_->CreateBlankMountains(mountainsrect);
		}

		drawingclass_->FDrawMountainsToCanvas(pCanvas_, mountainsrect, finmountains, peakpoint, peakpointvalue, controlhandler_->GetCurrencyStr());
		finmountains->RemoveAll(true);
		delete finmountains;

		grafdata->RemoveAll(true);
		delete grafdata;

		//draw lastfillup text
		if (lastcarcondata_.time == lastcarcondata_.time.GetMinValue()) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,210,440,26), L"No fill up records", 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(480+20,210,440,26), L"No fill up records", 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		} else {
			//con
			String lastfilluptext = L"Last fill up: " + controlhandler_->DateFormater(lastcarcondata_.time, true);
			if (lastcarcondata_.Type == 1) {
			String lastcon;
			if ((settingsdata_.contype == SETTINGSCONTYPE_LKM) || (settingsdata_.contype == SETTINGSCONTYPE_GMI) || (settingsdata_.contype == SETTINGSCONTYPE_KMG) || (settingsdata_.contype == SETTINGSCONTYPE_MIL)) {
				lastcon.Format(25,L"%4.2f",lastcarcondata_.Con);
			} else {
				if (lastcarcondata_.AvgCon > 0) {
					lastcon.Format(25,L"%4.2f",(100 / lastcarcondata_.Con));
				} else {
					lastcon.Format(25,L"%4.2f",0);
				}
			}
			lastfilluptext.Append(L" (" + lastcon + L" " + settingsdata_.contypestr + L")");
			}
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,210,440,26), lastfilluptext, 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
			//fin
			String lastfilluptextfin = lastcarcondata_.FuelTypeCaption + L" price: " + controlhandler_->CurrencyFormater(lastcarcondata_.Price) + L" on " + controlhandler_->DateFormater(lastcarcondata_.time, true);
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(480+20,210,440,26), lastfilluptextfin, 26, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		}
		//draw icon con
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(20,250,77,117), Rectangle(5,0,77,117));
		int colorc = 3;
		if ((lastcarcondata_.PreviusCon > 0) && (lastcarcondata_.Con > 0)) {
			if (lastcarcondata_.Con > (lastcarcondata_.PreviusCon+(lastcarcondata_.PreviusCon * lastcarcondata_.ConTolerance)))
			colorc = 2;
			if (lastcarcondata_.Con < (lastcarcondata_.PreviusCon-(lastcarcondata_.PreviusCon * lastcarcondata_.ConTolerance)))
			colorc = 1;
		}
		//if (colorc == 3)
		//drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(395,260,41,34), Rectangle(10,600,41,34));
		if (colorc == 2)
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(395,260,41,34), Rectangle(10,668,41,34));
		if (colorc == 1)
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(395,260,41,34), Rectangle(10,634,41,34));
		//draw icon fin
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(480+20,250,80,110), Rectangle(5,124,80,110));
		//draw numbers eco
		String value = L"";
		if ((settingsdata_.contype == SETTINGSCONTYPE_LKM) || (settingsdata_.contype == SETTINGSCONTYPE_GMI) || (settingsdata_.contype == SETTINGSCONTYPE_KMG) || (settingsdata_.contype == SETTINGSCONTYPE_MIL)) {
			value.Format(25,L"%4.2f",lastcarcondata_.AvgCon);
		} else {
			if (lastcarcondata_.AvgCon > 0) {
				value.Format(25,L"%4.2f",(100 / lastcarcondata_.AvgCon));
			} else {
				value.Format(25,L"%4.2f",0);
			}
		}
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(90,254,270,66), value, 2);
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(375,300,100,26), settingsdata_.contypestr, 24, FONT_STYLE_PLAIN, false, Color(230,230,230), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_BOTTOM);
		if (settingsdata_.contype2 > 0) {
		String altconvalue = L"";
		if ((settingsdata_.contype == SETTINGSCONTYPE_LKM) || (settingsdata_.contype == SETTINGSCONTYPE_GMI)) {
			if (lastcarcondata_.AvgCon > 0) {
				altconvalue.Format(25,L"%4.2f",(100 / lastcarcondata_.AvgCon));
			} else {
				altconvalue.Format(25,L"%4.2f",0);
			}
		} else {
		altconvalue.Format(25,L"%4.2f",lastcarcondata_.AvgCon);
		}
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(90,335,270,26), altconvalue, 1);
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(375,339,100,26), settingsdata_.contype2str, 24, FONT_STYLE_PLAIN, false, Color(230,230,230), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_BOTTOM);
		}
		//draw numbers economy view
		double value1, value2;
		if ((settingsdata_.contype == SETTINGSCONTYPE_LKM) || (settingsdata_.contype == SETTINGSCONTYPE_GMI) || (settingsdata_.contype == SETTINGSCONTYPE_KMG) || (settingsdata_.contype == SETTINGSCONTYPE_MIL)) {
			value1 = lastcarcondata_.AvgCon * lastcarcondata_.Price;
		} else {
			if (lastcarcondata_.AvgCon > 0) {
				value1 = (100 / lastcarcondata_.AvgCon) * lastcarcondata_.Price;
			} else {
				value1 = 0;
			}
		}
		if (value1 > 0) {
			value2 = 100 / value1;
		} else {
			value2 = 0;
		}
		String valuefin = L"";
		valuefin.Format(25,L"%4.2f",value1);
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(480+90,254,270,66), valuefin, 2);
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(480+375,300,100,26), controlhandler_->GetCurrencyStr() + "/100" + settingsdata_.distanceunitstr, 24, FONT_STYLE_PLAIN, false, Color(230,230,230), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_BOTTOM);
		String altfinvalue = L"";
		altfinvalue.Format(25,L"%4.2f",value2);
		drawingclass_->FDrawNumbersToCanvas(pCanvas_, Rectangle(480+90,335,270,26), altfinvalue, 1);
		drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(480+375,339,100,26), settingsdata_.distanceunitstr + "/1 " + controlhandler_->GetCurrencyStr(), 24, FONT_STYLE_PLAIN, false, Color(230,230,230), TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_BOTTOM);

	Canvas * pCanvasOut_ = new Canvas;
	pCanvasOut_->Construct(Rectangle(0,0,960,430));
	pCanvasOut_->Copy(Point(0,0), *pCanvas_, Rectangle(0,210,960,640));

	BgCacheNumbersBmpN->Construct(*pCanvasOut_, pCanvasOut_->GetBounds());

	delete pCanvas_;
	delete pCanvasOut_;
}

result
FMainForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw sky
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,5,800));
		//draw hills
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,640,480,160), Rectangle(0,800,480,160));
		//draw car name
		if (carconclass_->SelectedCar.ID > 0) {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,38,440,62), carconclass_->SelectedCar.Brand + L" - " + carconclass_->SelectedCar.Model, 30, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		} else {
			drawingclass_->FDrawTextToCanvas(pCanvas_, Rectangle(20,38,440,62), L"Car not selected", 30, FONT_STYLE_BOLD, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
		}
		//draw numbers
		if (BgCacheNumbersBmpN != null) {
		if (currentselview == VIEWECO) {
			pCanvas_->DrawBitmap(Rectangle(0,210,480,430),*BgCacheNumbersBmpN,Rectangle(0,0,480,430));
		} else if (currentselview == VIEWFIN) {
			pCanvas_->DrawBitmap(Rectangle(0,210,480,430),*BgCacheNumbersBmpN,Rectangle(480,0,480,430));
		}
		}
		//draw mainmeni buttons
		ButtonCostumEl* button_;
		bool isvisible = true;
		for (int ilayer=0; ilayer<2;ilayer++) {
		if ((moremeniopened) && (ilayer == 1)) {
			pCanvas_->SetForegroundColor(Color(255, 255, 255));
			pCanvas_->SetLineWidth(2);
			pCanvas_->FillRoundRectangle(Color(71, 154, 192, 220), moremenipos, Dimension(10,10));
			pCanvas_->DrawRoundRectangle(moremenipos, Dimension(10,10));
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(10+13,320+13,90,90), Rectangle(0,970,90,90));
			drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(240+13,320+13,90,90), Rectangle(0,970,90,90));
		}
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->group == BUTTONGROUPDONTDRAW) {
				isvisible = false;
			} else if (button_->group == BUTTONGROUPMOREMENI) {
				isvisible = (moremeniopened && (ilayer == 1));
			} else {
				isvisible = (ilayer == 0);
			}
			if (isvisible) {
				if (((button_->type == ICONBUTTONTOOGLE) || (button_->type == BIGBUTTONGREENTOOGLE) || (button_->type == BIGBUTTONBLUETOOGLE)) && (button_->toogle)) {
					drawingclass_->FDrawButtonIcoToCanvas(pCanvas_, Point(button_->position.x,button_->position.y), button_->type, button_->icoindextoogle, button_->toogle);
				} else {
					drawingclass_->FDrawButtonIcoToCanvas(pCanvas_, Point(button_->position.x,button_->position.y), button_->type, button_->icoindex, button_->pressed);
				}
				if ((button_->pressed) && (button_->tooltip != L"")) {
					Rectangle tooltippos = Rectangle(10,button_->position.y-50,460,50);
					int tooltipcarroth = 12;
					if ((button_->type == BIGBUTTONGREEN) || (button_->type == BIGBUTTONGREENTOOGLE) || (button_->type == BIGBUTTONBLUE) || (button_->type == BIGBUTTONBLUETOOGLE)) {
						tooltippos = Rectangle(10,button_->position.y-50,460,50);
					}
					pCanvas_->SetForegroundColor(Color(255, 255, 255));
					pCanvas_->SetLineWidth(2);
					pCanvas_->FillRoundRectangle(Color(0, 172, 1, 180), tooltippos, Dimension(10,10));
					pCanvas_->DrawRoundRectangle(tooltippos, Dimension(10,10));
					pCanvas_->FillTriangle(Color(255,255,255), Point(button_->position.x + (button_->position.width / 2)-8, tooltippos.y + tooltippos.height), Point(button_->position.x + (button_->position.width / 2)+8, tooltippos.y + tooltippos.height), Point(button_->position.x+(button_->position.width / 2),tooltippos.y + tooltippos.height+tooltipcarroth));
					drawingclass_->FDrawTextToCanvas(pCanvas_, tooltippos, button_->tooltip, 28, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
				}
			}
			if (button_->type == FILLUPBUTTON) {
				if ((button_->pressed) && (button_->tooltip != L"")) {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+20,button_->position.y,187,95), Rectangle(10+187,702,187,95));
					Rectangle tooltippos = Rectangle(10,button_->position.y-52,460,50);
					int tooltipcarroth = 12;
					pCanvas_->SetForegroundColor(Color(255, 255, 255));
					pCanvas_->SetLineWidth(2);
					pCanvas_->FillRoundRectangle(Color(0, 172, 1, 180), tooltippos, Dimension(10,10));
					pCanvas_->DrawRoundRectangle(tooltippos, Dimension(10,10));
					pCanvas_->FillTriangle(Color(255,255,255), Point(button_->position.x + 50-8, tooltippos.y + tooltippos.height), Point(button_->position.x + 50+8, tooltippos.y + tooltippos.height), Point(button_->position.x+50,tooltippos.y + tooltippos.height+tooltipcarroth));
					drawingclass_->FDrawTextToCanvas(pCanvas_, tooltippos, button_->tooltip, 28, FONT_STYLE_PLAIN, false, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
				} else {
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+20,button_->position.y,187,95), Rectangle(10,702,187,95));
				}
			}
		}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

result
FMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	controlhandler_->Clear();
	buttons_->RemoveAll(true);
	delete buttons_;
	delete BgCacheNumbersBmpN;

	return r;
}

void OpenForm(int formid) {
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
	pFormMgr->SendUserEvent(formid, null);
}

void
FMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{

	switch(actionId) {
	case ACAR: {
			OpenForm(FormMgr::REQUEST_CARSELECTFORM);
			break;
		}
	case ACON: {
			OpenForm(FormMgr::REQUEST_CONFORM);
			break;
		}
	case AHELP: {
			OpenForm(FormMgr::REQUEST_HELPFORM);
			break;
		}
	case AMONEY: {
			OpenForm(FormMgr::REQUEST_EXPENSESFORM);
			break;
		}
	case APARK: {
			AppLog("APARK");
			break;
		}
	case APREGLED: {
			OpenForm(FormMgr::REQUEST_PREGLEDFORM);
			break;
		}
	case ASERVICE: {
			AppLog("ASERVICE");
			break;
		}
	case ASETTINGS: {
			OpenForm(FormMgr::REQUEST_SETTINGSFORM);
			break;
		}
	case ATRIP: {
			OpenForm(FormMgr::REQUEST_TRIPFORM);
			break;
		}
	case AWHEEL: {
			OpenForm(FormMgr::REQUEST_EXPENSESFORM);
			break;
		}
	case ALESS: {
			moremeniopened = false;
			break;
		}
	case AMORE: {
			moremeniopened = true;
			break;
		}
	case ASELVIEWECO: {
			currentselview = VIEWECO;
			break;
		}
	case ASELVIEWMONEY: {
			currentselview = VIEWFIN;
			break;
		}
	case ANEWFILLUP: {
			OpenForm(FormMgr::REQUEST_VNOSPORABEFORM);
			break;
		}
	case APREGLEDCON: {
			OpenForm(FormMgr::REQUEST_PREGLEDCONFORM);
			break;
		}
	case APREGLEDSTROSKI: {
			OpenForm(FormMgr::REQUEST_PREGLEDEXPENSESFORM);
			break;
		}
	case APREGLEDFUELPRICE: {
			OpenForm(FormMgr::REQUEST_PREGLEDFUELPRICEFORM);
			break;
		}
	default:
		break;
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
		if (button_->group == BUTTONGROUPMOREMENI) {
			isvisible = moremeniopened;
		} else {
			isvisible = !((button_->group == BUTTONGROUPVIEW) && moremeniopened);
		}
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
void FMainForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}
void FMainForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo) {
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if ((moremeniopened) && (!moremenipos.Contains(currentPosition))) {
		bool ismorelessbutton = false;
		if (currpressedbtn >= 0) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
			ismorelessbutton = (button_->action == AMORE);
		}
		if (!ismorelessbutton) {
		for (int n=0; n<buttons_->GetCount(); n++) {
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->action == AMORE) {
				button_->toogle = false;
				this->OnActionPerformed(source, button_->actiontoogle);
				buttons_->SetAt(*button_,n,false);
				break;
			}
		}
		needsredraw = true;
		}
	}
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			button_->pressed = false;
			if ((button_->type == ICONBUTTONTOOGLE) || (button_->type == BIGBUTTONGREENTOOGLE) || (button_->type == BIGBUTTONBLUETOOGLE)) {
				button_->toogle = !button_->toogle;
				bool hasmorethanthistoogleingroup = false;
				ButtonCostumEl* buttontmp_;
				for (int n=0; n<buttons_->GetCount(); n++) {
					buttontmp_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
					if ((buttontmp_->type == ICONBUTTONTOOGLE) || (buttontmp_->type == BIGBUTTONGREENTOOGLE) || (buttontmp_->type == BIGBUTTONBLUETOOGLE)) {
					if ((buttontmp_->group == button_->group) && (n != currpressedbtn)) {
						buttontmp_->toogle = false;
						hasmorethanthistoogleingroup = true;
						buttons_->SetAt(*buttontmp_,n,false);
					}
					}
				}
				if (hasmorethanthistoogleingroup) {
				button_->toogle = true;
				}
			}
			buttons_->SetAt(*button_,currpressedbtn,false);
			if (((button_->type == ICONBUTTONTOOGLE) || (button_->type == BIGBUTTONGREENTOOGLE) || (button_->type == BIGBUTTONBLUETOOGLE)) && (!button_->toogle)) {
				this->OnActionPerformed(source, button_->actiontoogle);
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
