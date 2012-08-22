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
#include "FPregled.h"
#include "CarConClass.h"
#include "FormMgr.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"
#include "FGrafZoom.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Base::Utility;
using namespace Osp::Locales;

GrafItemData::GrafItemData(void) {

}

GrafItemData::~GrafItemData(void) {

}

FPregled::FPregled(DateTime fromdate, DateTime todate, int formtype):__pOptionMenu(null)
{
	this->filterfromdate = fromdate;
	this->filtertodate = todate;
	this->formtype = formtype;
}

FPregled::~FPregled(void)
{
}

bool
FPregled::Initialize()
{
	Form::Construct(L"IDF_FPREGLED");
	SetName(L"FPregled");
	return true;
}

result
FPregled::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));

	pFGrafZoom_ = new FGrafZoom();
	pFGrafZoom_->Initialize();
	Application::GetInstance()->GetAppFrame()->GetFrame()->AddControl(*pFGrafZoom_);

	SetSoftkeyActionId(SOFTKEY_1, ID_BACKBTN);
	AddSoftkeyActionListener(SOFTKEY_1, *this);
	SetSoftkeyActionId(SOFTKEY_0, ID_FILTERBTN);
	AddSoftkeyActionListener(SOFTKEY_0, *this);

	SetOptionkeyActionId(ID_OPTIONKEY);
	AddOptionkeyActionListener(*this);
	__pOptionMenu = new OptionMenu();
	__pOptionMenu->Construct();
	if (this->formtype != REPORTCON)
	__pOptionMenu->AddItem("Consumption analytics",REPORTCON);
	if (this->formtype != REPORTFP)
	__pOptionMenu->AddItem("Fuel prices analytics",REPORTFP);
	if (this->formtype != REPORTFIN)
	__pOptionMenu->AddItem("Expenses analytics",REPORTFIN);
	__pOptionMenu->AddActionEventListener(*this);

	this->AddTouchEventListener(*this);

	drawingclass_ = pFormMgr->drawingclass_;
	tablisticonbmp = drawingclass_->FDrawSpriteToBmpN(Rectangle(500,1012,32,32));
	tabgraficonbmp = drawingclass_->FDrawSpriteToBmpN(Rectangle(500,1012+32,32,32));
	tabsumiconbmp = drawingclass_->FDrawSpriteToBmpN(Rectangle(500,1012+64,32,32));

	Tab* pTab = GetTab();
	pTab->AddItem(*tablisticonbmp, L"List", TABBROWSE);
	pTab->AddItem(*tabgraficonbmp, L"Graph", TABGRAPH);
	pTab->AddItem(*tabsumiconbmp, L"Overview", TABOVERVIEW);
	pTab->AddActionEventListener(*this);


	if (this->formtype == REPORTCON) {
		Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(13);
		this->SetTitleIcon(titleiconbmp);
		delete titleiconbmp;
	} else if (this->formtype == REPORTFP) {
		Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(15);
		this->SetTitleIcon(titleiconbmp);
		delete titleiconbmp;
	} else if (this->formtype == REPORTFIN) {
		Bitmap * titleiconbmp = drawingclass_->FDrawFormTitleIconN(14);
		this->SetTitleIcon(titleiconbmp);
		delete titleiconbmp;
	}

	controlhandler_ = pFormMgr->controlhandler_;
	controlhandler_->Reconstruct();

	pExList = new CustomList();
	pExList->Construct(Rectangle(0, 60, 480, GetClientAreaBounds().height-60-70-60), CUSTOM_LIST_STYLE_RADIO, false);
	pExList->SetTextOfEmptyList(L"No data");
	this->AddControl(*pExList);
	pExList->AddCustomItemEventListener(*this);

	pCustomListItemFormat = new CustomListItemFormat();
	pCustomListItemFormat->Construct();
	pCustomListItemFormat->AddElement(LIST_ELEMENT_COSTUM, Rectangle(0, 0, 480, 100));
	pCustomListItemFormat->SetElementEventEnabled(LIST_ELEMENT_COSTUM,true);

	pPopupFilter_ = new Popup();
	pPopupFilter_->Construct(L"IDP_POPUP5");
	Osp::Ui::Controls::Button *pPopupFilterButton_;
	Osp::Ui::Controls::EditField *pPopupFilterEF_;
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"cancelbtn"));
	pPopupFilterButton_->SetActionId(FILTERCANCEL);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"applybtn"));
	pPopupFilterButton_->SetActionId(FILTERAPPLY);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"monthbtn"));
	buttonmonthbmpn_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), false, L"One month", drawingclass_->BUTTONSPECIALGROUP_LEFT);
	buttonmonthbmps_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), true, L"One month", drawingclass_->BUTTONSPECIALGROUP_LEFT);
	pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonmonthbmpn_);
	pPopupFilterButton_->SetPressedBackgroundBitmap(*buttonmonthbmps_);
	pPopupFilterButton_->SetText(L"");
	pPopupFilterButton_->SetActionId(FILTERMONTH);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"quaterbtn"));
	buttonquaterbmpn_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), false, L"Quarter year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	buttonquaterbmps_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), true, L"Quarter year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonquaterbmpn_);
	pPopupFilterButton_->SetPressedBackgroundBitmap(*buttonquaterbmps_);
	pPopupFilterButton_->SetText(L"");
	pPopupFilterButton_->SetActionId(FILTERQUATER);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"halfbtn"));
	buttonhalfbmpn_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), false, L"Half year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	buttonhalfbmps_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), true, L"Half year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonhalfbmpn_);
	pPopupFilterButton_->SetPressedBackgroundBitmap(*buttonhalfbmps_);
	pPopupFilterButton_->SetText(L"");
	pPopupFilterButton_->SetActionId(FILTERHALF);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"yearbtn"));
	buttonyearbmpn_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), false, L"Year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	buttonyearbmps_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,90,100), true, L"Year", drawingclass_->BUTTONSPECIALGROUP_MIDDLE);
	pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonyearbmps_);
	pPopupFilterButton_->SetPressedBackgroundBitmap(*buttonyearbmps_);
	pPopupFilterButton_->SetText(L"");
	pPopupFilterButton_->SetActionId(FILTERYEAR);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"clearbtn"));
	buttonclearbmpn_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,81,100), false, L"All", drawingclass_->BUTTONSPECIALGROUP_RIGHT);
	buttonclearbmps_ = drawingclass_->FDrawButtonSpecialToBmpN(Rectangle(0,0,81,100), true, L"All", drawingclass_->BUTTONSPECIALGROUP_RIGHT);
	pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonclearbmpn_);
	pPopupFilterButton_->SetPressedBackgroundBitmap(*buttonclearbmps_);
	pPopupFilterButton_->SetText(L"");
	pPopupFilterButton_->SetActionId(FILTERCLEAR);
	pPopupFilterButton_->AddActionEventListener(*this);
	pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"fromdate"));
	pPopupFilterEF_->SetText(controlhandler_->DateFormater(this->filterfromdate));
	pPopupFilterEF_->SetKeypadEnabled(false);
	pPopupFilterEF_->AddTouchEventListener(*this);
	pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"todate"));
	pPopupFilterEF_->SetText(controlhandler_->DateFormater(this->filtertodate));
	pPopupFilterEF_->SetKeypadEnabled(false);
	pPopupFilterEF_->AddTouchEventListener(*this);

	filterbuttonselcur = 4;
	filterbuttonselold = filterbuttonselcur;

	pDatePicker_ = new DatePicker();
    pDatePicker_->Construct(L"Date");
    pDatePicker_->AddDateChangeEventListener(*this);
    pDatePicker_->SetCurrentDate();
    pDatePicker_->SetYearRange(pDatePicker_->GetYear()-10, pDatePicker_->GetYear());

    carconclass_ = pFormMgr->carconclass_;
	carconclass_->GetSettingsData(settingsdata_);

	grafdata.Construct();

	pGrafBmp = null;

	LoadData();

	return r;
}

void FPregled::FilterButtonsSel(int selindex) {
	Osp::Ui::Controls::Button *pPopupFilterButton_;
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"monthbtn"));
	if (selindex == 1) {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonmonthbmps_);
	} else {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonmonthbmpn_);
	}
	pPopupFilterButton_->RequestRedraw();
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"quaterbtn"));
	if (selindex == 2) {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonquaterbmps_);
	} else {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonquaterbmpn_);
	}
	pPopupFilterButton_->RequestRedraw();
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"halfbtn"));
	if (selindex == 3) {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonhalfbmps_);
	} else {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonhalfbmpn_);
	}
	pPopupFilterButton_->RequestRedraw();
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"yearbtn"));
	if (selindex == 4) {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonyearbmps_);
	} else {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonyearbmpn_);
	}
	pPopupFilterButton_->RequestRedraw();
	pPopupFilterButton_ = static_cast<Button *>(pPopupFilter_->GetControl(L"clearbtn"));
	if (selindex == 5) {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonclearbmps_);
	} else {
		pPopupFilterButton_->SetNormalBackgroundBitmap(*buttonclearbmpn_);
	}
	pPopupFilterButton_->RequestRedraw();
	filterbuttonselcur = selindex;
}

void FPregled::LoadData(void)
{
	minprice = Double::GetMaxValue();
	maxprice = Double::GetMinValue();
	minpricetime = minpricetime.GetMaxValue();
	maxpricetime = maxpricetime.GetMinValue();

	pExList->RemoveAllItems();
	grafdata.RemoveAll(true);

	if (pGrafBmp != null)
		delete pGrafBmp;

	pGrafBmp = new Bitmap;

	if (this->formtype == REPORTCON) {
		CarConData data_;
		this->SetTitleText(controlhandler_->DateFormater(this->filterfromdate, true) + L" to " + controlhandler_->DateFormater(this->filtertodate, true));
		suml = 0;
		sumprice = 0;
		mincon = Double::GetMaxValue();
		maxcon = Double::GetMinValue();
		mincontime = mincontime.GetMaxValue();
		maxcontime = maxcontime.GetMinValue();
		avgcon = 0;
		avgconreal = 0;

		int concount = 0;
		double sumcont = 0;
		int sumtripcon = 0;
		double sumstlcon = 0;

		int grafdatagroupycount(0);
		long long grafdatagroupx(0);
		double grafdatagroupysum(0);
		long long grafdatax(0);
		double grafdatay(0);

		String stlstr, constr;
		if (carconclass_->GetCarConDataListStart(carconclass_->SelectedCar.ID, this->filterfromdate, this->filtertodate)) {
			while (carconclass_->GetCarConDataListGetData(data_)) {
				stlstr = L"";
				stlstr.Format(25,L"%4.2f",data_.StL);
				stlstr = stlstr + " " + settingsdata_.volumeunitstr;
				if ((data_.Price * data_.StL) < minprice) {
					minprice = (data_.Price * data_.StL);
					minpricetime = data_.time;
				}
				if ((data_.Price * data_.StL) > maxprice) {
					maxprice = (data_.Price * data_.StL);
					maxpricetime = data_.time;
				}
				if ((data_.Con > 0) && (data_.Type == 1)) {
					if (data_.Con < mincon) {
						mincon = data_.Con;
						mincontime = data_.time;
					}
					if (data_.Con > maxcon) {
						maxcon = data_.Con;
						maxcontime = data_.time;
					}
					constr = L"";
					constr.Format(25,L"%4.2f",data_.Con);
					sumstlcon = sumstlcon + data_.StL;
					sumtripcon = sumtripcon + data_.Trip;
					sumcont = sumcont + data_.Con;
					concount++;
					grafdatax = (long long)(data_.time.GetTime().GetTicks() / 100000);
					grafdatay = data_.Con;
					if ((grafdatax == grafdatagroupx) || (grafdatagroupycount == 0)) {
						grafdatagroupysum = grafdatagroupysum + grafdatay;
						grafdatagroupycount = grafdatagroupycount + 1;
					} else {
						GrafItemData * gid = new GrafItemData();
						gid->xval = grafdatagroupx;
						gid->yval = grafdatagroupysum / grafdatagroupycount;
						grafdata.Add(*gid);
						grafdatagroupysum = grafdatay;
						grafdatagroupycount = 1;
					}
					grafdatagroupx = grafdatax;
				} else {
					constr = L"N/A";
				}
				suml = suml + data_.StL;
				sumprice = sumprice + (data_.Price * data_.StL);
				AddListItem(controlhandler_->DateFormater(data_.time,true), stlstr, controlhandler_->CurrencyFormater((data_.Price * data_.StL)), constr, data_.ID);
			}
			if (grafdatagroupycount != 0) {
				GrafItemData * gid = new GrafItemData();
				gid->xval = grafdatagroupx;
				gid->yval = grafdatagroupysum / grafdatagroupycount;
				grafdata.Add(*gid);
			}
			carconclass_->GetCarConDataListEnd();
		}
		if (concount > 0) {
			avgcon = sumcont / concount;
			if (sumtripcon > 0) {
				avgconreal = (sumstlcon / sumtripcon) * 100;
			}
		}
	} else if (this->formtype == REPORTFIN) {
		CarExpenseData expensedata_;
		this->SetTitleText(controlhandler_->DateFormater(this->filterfromdate, true) + L" to " + controlhandler_->DateFormater(this->filtertodate, true));
		sumprice = 0;
		int grafdatagroupycount(0);
		long long grafdatagroupx(0);
		double grafdatagroupysum(0);
		long long grafdatax(0);
		double grafdatay(0);
		if (carconclass_->GetExpenseDataFilterListStart(carconclass_->SelectedCar.ID, this->filterfromdate, this->filtertodate)) {
			while (carconclass_->GetExpenseDataListGetData(expensedata_)) {
				sumprice = sumprice + expensedata_.Price;
				AddListItemFIN(controlhandler_->DateFormater(expensedata_.time,true), expensedata_.Caption, controlhandler_->CurrencyFormater(expensedata_.Price), expensedata_.ID);
				grafdatax = (long long)(expensedata_.time.GetTime().GetTicks() / 100000);
				grafdatay = expensedata_.Price;
				if ((grafdatax == grafdatagroupx) || (grafdatagroupycount == 0)) {
					grafdatagroupysum = grafdatagroupysum + grafdatay;
					grafdatagroupycount = grafdatagroupycount + 1;
				} else {
					GrafItemData * gid = new GrafItemData();
					gid->xval = grafdatagroupx;
					gid->yval = grafdatagroupysum / grafdatagroupycount;
					grafdata.Add(*gid);
					grafdatagroupysum = grafdatay;
					grafdatagroupycount = 1;
				}
				grafdatagroupx = grafdatax;
			}
			if (grafdatagroupycount != 0) {
				GrafItemData * gid = new GrafItemData();
				gid->xval = grafdatagroupx;
				gid->yval = grafdatagroupysum / grafdatagroupycount;
				grafdata.Add(*gid);
			}
			carconclass_->GetExpenseDataListEnd();
		}
	} else if (this->formtype == REPORTFP) {
		CarConData data_;
		this->SetTitleText(controlhandler_->DateFormater(this->filterfromdate, true) + L" to " + controlhandler_->DateFormater(this->filtertodate, true));
		int grafdatagroupycount(0);
		long long grafdatagroupx(0);
		double grafdatagroupysum(0);
		long long grafdatax(0);
		double grafdatay(0);
		double oldfuelprice(-1);
		if (carconclass_->GetCarConDataListStart(carconclass_->SelectedCar.ID, this->filterfromdate, this->filtertodate)) {
			while (carconclass_->GetCarConDataListGetData(data_)) {
				if ((data_.Price != oldfuelprice) || (oldfuelprice == -1)) {
					if ((data_.Price) < minprice) {
						minprice = (data_.Price);
						minpricetime = data_.time;
					}
					if ((data_.Price) > maxprice) {
						maxprice = (data_.Price);
						maxpricetime = data_.time;
					}
					AddListItemFP(controlhandler_->DateFormater(data_.time,true), data_.FuelTypeCaption, controlhandler_->Currency4Formater(data_.Price), data_.ID);
					grafdatax = (long long)(data_.time.GetTime().GetTicks() / 100000);
					grafdatay = data_.Price;
					if ((grafdatax == grafdatagroupx) || (grafdatagroupycount == 0)) {
						grafdatagroupysum = grafdatagroupysum + grafdatay;
						grafdatagroupycount = grafdatagroupycount + 1;
					} else {
						GrafItemData * gid = new GrafItemData();
						gid->xval = grafdatagroupx;
						gid->yval = grafdatagroupysum / grafdatagroupycount;
						grafdata.Add(*gid);
						grafdatagroupysum = grafdatay;
						grafdatagroupycount = 1;
					}
					grafdatagroupx = grafdatax;
				}
				oldfuelprice = data_.Price;
			}
			if (grafdatagroupycount != 0) {
				GrafItemData * gid = new GrafItemData();
				gid->xval = grafdatagroupx;
				gid->yval = grafdatagroupysum / grafdatagroupycount;
				grafdata.Add(*gid);
			}
			carconclass_->GetCarConDataListEnd();
		}
	}

	GetGraph(pGrafBmp, Rectangle(0,0,GetClientAreaBounds().width-40,320));
}

class CustomListElementPregled : public ICustomListElement
 {
public:
 String col1, col2, col3, col4;
 int datatype;
 result DrawElement(const Osp::Graphics::Canvas& canvas, const Osp::Graphics::Rectangle& rect, CustomListItemStatus itemStatus)
 {
     result r = E_SUCCESS;

     Canvas* pCanvas = const_cast<Canvas*>(&canvas);

     pCanvas->SetLineWidth(1);
     if ((itemStatus == CUSTOM_LIST_ITEM_STATUS_SELECTED) || (itemStatus == CUSTOM_LIST_ITEM_STATUS_FOCUSED)) {
    	 pCanvas->FillRectangle(Color(4,58,99), rect);
     }
   	 pCanvas->SetForegroundColor(Color::COLOR_WHITE);
     pCanvas->DrawLine(Point(162, 0), Point(162, rect.height-1));
     pCanvas->DrawLine(Point(316, 0), Point(316, rect.height-1));

     Font font1;
     font1.Construct(FONT_STYLE_PLAIN, 28);
     Font font2;
     font2.Construct(FONT_STYLE_PLAIN, 32);

     EnrichedText col1text;
     col1text.Construct(Dimension(140, 30));
     col1text.SetHorizontalAlignment(TEXT_ALIGNMENT_CENTER);
     col1text.SetVerticalAlignment(TEXT_ALIGNMENT_MIDDLE);
     TextElement col1textel;
     col1textel.Construct(col1);
     col1textel.SetTextColor(Color::COLOR_WHITE);
     col1textel.SetFont(font1);
     col1text.Add(col1textel);
     pCanvas->DrawText(Point(10,15), col1text);
     EnrichedText col2text;
     if (datatype == 1) {
     col2text.Construct(Dimension(130, 80));
     col2text.SetHorizontalAlignment(TEXT_ALIGNMENT_RIGHT);
     col2text.SetVerticalAlignment(TEXT_ALIGNMENT_TOP);
     col2text.SetTextWrapStyle(TEXT_WRAP_CHARACTER_WRAP);
     col2text.SetTextAbbreviationEnabled(true);
     col2text.SetLineSpace(10);
     } else if ((datatype == 2) || (datatype == 3)) {
     col2text.Construct(Dimension(140, 30));
     col2text.SetHorizontalAlignment(TEXT_ALIGNMENT_LEFT);
     col2text.SetVerticalAlignment(TEXT_ALIGNMENT_MIDDLE);
     col2text.SetTextWrapStyle(TEXT_WRAP_CHARACTER_WRAP);
     col2text.SetTextAbbreviationEnabled(true);
     }
     TextElement col2textel;
     if (datatype == 1) {
     col2textel.Construct(col2 + "\n" + col3);
     } else if ((datatype == 2) || (datatype == 3)) {
     col2textel.Construct(col2);
     }
     col2textel.SetTextColor(Color::COLOR_WHITE);
     col2textel.SetFont(font1);
     col2text.Add(col2textel);
     if (datatype == 1) {
     pCanvas->DrawText(Point(172,5), col2text);
     } else if ((datatype == 2) || (datatype == 3)) {
     pCanvas->DrawText(Point(172,15), col2text);
     }

     EnrichedText col4text;
     col4text.Construct(Dimension(120, 100));
     col4text.SetHorizontalAlignment(TEXT_ALIGNMENT_RIGHT);
     col4text.SetVerticalAlignment(TEXT_ALIGNMENT_MIDDLE);
     TextElement col4textel;
     col4textel.Construct(col4);
     col4textel.SetTextColor(Color::COLOR_WHITE);
     col4textel.SetFont(font2);
     col4text.Add(col4textel);
     pCanvas->DrawText(Point(326,0), col4text);

     pCanvas->DrawLine(Point(rect.x, rect.height - 1), Point(rect.width, rect.height - 1));

     return r;
 }
};

void FPregled::GetGraph(Osp::Graphics::Bitmap * pGrafBmp, Osp::Graphics::Rectangle grafimgrect, bool fullzoom) {
	Canvas * pGrafCanvas_ = new Canvas;
	pGrafCanvas_->Construct(grafimgrect);
	int graftoppos = 10;
	int grafheight = grafimgrect.height-40;
	if (fullzoom) {
		pGrafCanvas_->FillRectangle(Color::COLOR_WHITE, Rectangle(0, 0, grafimgrect.width, grafimgrect.height - 10));
		pGrafCanvas_->FillRectangle(Color(4,58,99), Rectangle(0, grafheight, grafimgrect.width, grafimgrect.height - grafheight));
	} else {
		pGrafCanvas_->FillRoundRectangle(Color::COLOR_WHITE, Rectangle(0, 0, grafimgrect.width, grafimgrect.height - 10), Dimension(10,10));
		pGrafCanvas_->FillRoundRectangle(Color(4,58,99), Rectangle(0, grafheight, grafimgrect.width, grafimgrect.height - grafheight), Dimension(10,10));
		pGrafCanvas_->FillRectangle(Color(4,58,99), Rectangle(0, grafheight, grafimgrect.width, 10));
	}
	pGrafCanvas_->SetLineWidth(1);
	if (grafdata.GetCount() < 1) {
		drawingclass_->FDrawTextToCanvas(pGrafCanvas_,Rectangle(20, 20, grafimgrect.width-40, grafheight-40), L"Not enough data!", 24, FONT_STYLE_BOLD, true, Color::COLOR_RED, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
	} else {
		Font fontgrid;
		fontgrid.Construct(FONT_STYLE_PLAIN, 20);
		pGrafCanvas_->SetFont(fontgrid);
		GrafItemData* pid;
		double miny, maxy;
		miny = 0;
		maxy = 0;
		long long minx, maxx;
		minx = 0;
		maxx = 0;
		for (int n=0; n<grafdata.GetCount(); n++) {
			pid = static_cast<GrafItemData *> (grafdata.GetAt(n));
			if (pid->yval > maxy)
				maxy = pid->yval;
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
		int gridxcount = 4;
		int gridycount = 5;
		int gridxstep = int(grafimgrect.width / gridxcount);
		int gridystep = int(grafheight / gridycount);
		double gridystepval = ((maxy-miny) / gridycount);
		double gridxstepval = ((maxx-minx) / gridxcount);
		double yval;
		long long xval;
		DateTime xvald;
		String tmps, tmpspart1, tmpspart2;
		Dimension tmpd;
		for (int n=1; n<=gridxcount; n++) {
			pGrafCanvas_->SetForegroundColor(Color(200, 200, 200));
			if (n<gridxcount)
			pGrafCanvas_->DrawLine(Point((n * gridxstep), 0), Point((n * gridxstep), grafimgrect.height));
			pGrafCanvas_->SetForegroundColor(Color(150, 150, 150));
			xval = minx+(n*gridxstepval);
			TimeSpan xvalc(xval * 100000);
			xvald.SetValue(xvalc);
			tmps = controlhandler_->DateFormater(xvald,false);
			tmps.SubString(0,2,tmpspart1);
			tmps.SubString(2,tmpspart2);
			tmpspart1.Trim();
			tmpspart2.Trim();
			tmpspart2.SubString(0,3,tmpspart2);
			tmps = tmpspart2 + L" " + tmpspart1;
			fontgrid.GetTextExtent(tmps, tmps.GetLength(), tmpd);
			pGrafCanvas_->SetForegroundColor(Color(255, 255, 255));
			pGrafCanvas_->DrawText(Point((n * gridxstep)-tmpd.width-5, grafimgrect.height-tmpd.height-9), tmps);
		}
		for (int n=0; n<gridycount; n++) {
			pGrafCanvas_->SetForegroundColor(Color(200, 200, 200));
			if (n > 0)
			pGrafCanvas_->DrawLine(Point(0, graftoppos+(n * gridystep)), Point(grafimgrect.width, graftoppos+(n * gridystep)));
			tmps = L"";
			yval = (miny+((gridycount-n) * gridystepval));
			tmps.Format(25,L"%4.2f",yval);
			fontgrid.GetTextExtent(tmps, tmps.GetLength(), tmpd);
			pGrafCanvas_->SetForegroundColor(Color(150, 150, 150));
			pGrafCanvas_->DrawText(Point(grafimgrect.width-tmpd.width-10, graftoppos+(n * gridystep)+5), tmps);
		}
		pGrafCanvas_->SetForegroundColor(Color(4,58,99));
		pGrafCanvas_->SetLineWidth(2);
		int xpos(0);
		int ypos(0);
		int xposold(-1);
		int yposold(0);
		double yvalsum(0);
		double yvalcount(0);
		Osp::Base::Collection::IList* grafpolyline =  new Osp::Base::Collection::ArrayList;
		if (grafdata.GetCount() < 2) {
			grafpolyline->Add(*(new Point(0,graftoppos+(int)Math::Round(grafheight / 2))));
			grafpolyline->Add(*(new Point(grafimgrect.width,graftoppos+(int)Math::Round(grafheight / 2))));
		} else {
			for (int n=grafdata.GetCount()-1; n>=0; n--) {
				pid = static_cast<GrafItemData *> (grafdata.GetAt(n));
				yval = pid->yval;
				xval = pid->xval;
				yvalcount = yvalcount + 1;
				yvalsum = yvalsum + yval;
				yval = (yvalsum / yvalcount);
				double tmpxv(xval - minx);
				double tmpxv2(maxx - minx);
				xpos = (int)Math::Round((((grafimgrect.width) / tmpxv2) * tmpxv))+1;
				if ((maxy - miny) == 0)  {
				  ypos = (int)Math::Round(((grafheight-10)-(((grafheight-10) / 1) * (yval-miny))));
				} else {
				  ypos = (int)Math::Round(((grafheight-10)-(((grafheight-10) / (maxy - miny)) * (yval-miny))));
				}
				if (ypos > (grafheight)) {
				  ypos = (grafheight);
				}
				if (ypos < (0)) {
				  ypos = (0);
				}
				if ((xpos > xposold)) {
					if (xposold == -1) {
						grafpolyline->Add(*(new Point(0,graftoppos+ypos)));
					}
					grafpolyline->Add(*(new Point(xpos,graftoppos+ypos)));
					yvalcount = 0;
					yvalsum = 0;
				}
				xposold = xpos;
				yposold = ypos;
			}
		}
		grafpolyline->Add(*(new Point(grafimgrect.width,grafheight+1)));
		grafpolyline->Add(*(new Point(0,grafheight+1)));
		pGrafCanvas_->FillPolygon(Color(4,58,99,50), *grafpolyline);
		grafpolyline->RemoveItems(grafpolyline->GetCount()-2,2,true);
		pGrafCanvas_->DrawPolyline(*grafpolyline);
		grafpolyline->RemoveAll(true);
		delete grafpolyline;
	}
	pGrafBmp->Construct(*pGrafCanvas_, pGrafCanvas_->GetBounds());
	delete pGrafCanvas_;
}

result
FPregled::OnDraw()
{
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(750,0,5,800));
		Tab* pTab = GetTab();
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABGRAPH) {
			if (pGrafBmp != null) {
				pCanvas_->DrawBitmap(Point(20,GetClientAreaBounds().y + 20), *pGrafBmp);
				graphzoomtouchregion.SetBounds(20,GetClientAreaBounds().y + 20,pGrafBmp->GetWidth(),pGrafBmp->GetHeight());
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(20,GetClientAreaBounds().y+pGrafBmp->GetHeight()+10,pGrafBmp->GetWidth(), 70), L"Double touch graph for zoom in/out.", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
			}
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABBROWSE) {
			int footertoppos = (GetClientAreaBounds().height-60-70+GetClientAreaBounds().y);
			pCanvas_->FillRectangle(Color(0,0,0,50), Rectangle(0,0,480,GetClientAreaBounds().y+60));
			pCanvas_->FillRectangle(Color(0,0,0,50), Rectangle(0,footertoppos,480,60+70));
			pCanvas_->SetLineWidth(1);
			pCanvas_->SetForegroundColor(Color::COLOR_WHITE);
			pCanvas_->DrawLine(Point(0, GetClientAreaBounds().y+60), Point(480, GetClientAreaBounds().y+60));
			pCanvas_->DrawLine(Point(0, footertoppos), Point(480, footertoppos));
			pCanvas_->DrawLine(Point(162, GetClientAreaBounds().y+5), Point(162, GetClientAreaBounds().y+60));
			pCanvas_->DrawLine(Point(316, GetClientAreaBounds().y+5), Point(316, GetClientAreaBounds().y+60));
			drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(10,GetClientAreaBounds().y,140, 60), L"Date", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
			if (this->formtype == REPORTCON) {
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(172,GetClientAreaBounds().y,130, 60), settingsdata_.volumeunitfullstr + L"\nPrice", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(326,GetClientAreaBounds().y,120, 60), settingsdata_.contypestr, 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
			} else if (this->formtype == REPORTFIN) {
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(172,GetClientAreaBounds().y,140, 60), L"Caption", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(326,GetClientAreaBounds().y,120, 60), L"Price", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
			} else if (this->formtype == REPORTFP) {
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(172,GetClientAreaBounds().y,140, 60), L"Fuel type", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_MIDDLE);
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(326,GetClientAreaBounds().y,120, 60), L"Price", 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
			}
			if (this->formtype == REPORTCON) {
				String avgconstr, sumlstr;
				sumlstr.Format(25,L"%4.2f",suml);
				sumlstr = sumlstr + " " + settingsdata_.volumeunitstr;
				if (settingsdata_.avgcalctype == 2) {
				avgconstr.Format(25,L"%4.2f",avgconreal);
				} else {
				avgconstr.Format(25,L"%4.2f",avgcon);
				}
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(152,footertoppos,150, 60), sumlstr + L"\n" + controlhandler_->CurrencyFormater(sumprice), 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(326,footertoppos,120, 60), avgconstr, 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
			} else if (this->formtype == REPORTFIN) {
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(326,footertoppos,120, 60), controlhandler_->CurrencyFormater(sumprice), 24, FONT_STYLE_PLAIN, true, Color::COLOR_WHITE, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_MIDDLE);
			}
		}
		if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABOVERVIEW) {
			String textstr, tmps;
			if (this->formtype == REPORTCON) {
				textstr = L"";
				textstr.Append(L"Min consumption:\n");
				if (mincontime < mincontime.GetMaxValue()) {
					tmps = L"";
					tmps.Format(25,L"%4.2f",mincon);
					textstr.Append(L"   " + tmps + " " + settingsdata_.contypestr + "\n");
					textstr.Append(L"   " + controlhandler_->DateFormater(mincontime,true) + "\n \n");
				} else {
					textstr.Append(L"   N/A \n \n");
				}
				textstr.Append(L"Max consumption:\n");
				if (maxcontime > maxcontime.GetMinValue()) {
					tmps = L"";
					tmps.Format(25,L"%4.2f",maxcon);
					textstr.Append(L"   " + tmps + " " + settingsdata_.contypestr + "\n");
					textstr.Append(L"   " + controlhandler_->DateFormater(maxcontime,true) + "\n \n");
				} else {
					textstr.Append(L"   N/A \n \n");
				}
				textstr.Append(L"Avg consumption:\n");
				tmps = L"";
				tmps.Format(25,L"%4.2f",avgcon);
				textstr.Append(L"   " + tmps + " " + settingsdata_.contypestr + "\n");
				textstr.Append(L"Avg real consumption:\n");
				tmps = L"";
				tmps.Format(25,L"%4.2f",avgconreal);
				textstr.Append(L"   " + tmps + " " + settingsdata_.contypestr + "\n \n");
				textstr.Append(L"Sum price:\n");
				textstr.Append(L"   " + controlhandler_->CurrencyFormater(sumprice) + "\n");
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(20,GetClientAreaBounds().y+20,GetClientAreaBounds().width-40, GetClientAreaBounds().height-70-40), textstr, 32, FONT_STYLE_PLAIN, true, Osp::Graphics::Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			} else if (this->formtype == REPORTFIN) {
				textstr = L"";
				textstr.Append(L"Sum price:\n");
				textstr.Append(L"   " + controlhandler_->CurrencyFormater(sumprice) + "\n");
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(20,GetClientAreaBounds().y+20,GetClientAreaBounds().width-40, GetClientAreaBounds().height-70-40), textstr, 32, FONT_STYLE_PLAIN, true, Osp::Graphics::Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			} else if (this->formtype == REPORTFP) {
				textstr = L"";
				textstr.Append(L"Min price:\n");
				if (minpricetime != minpricetime.GetMaxValue()) {
					textstr.Append(L"   " + controlhandler_->Currency4Formater(minprice) + "\n");
					textstr.Append(L"   " + controlhandler_->DateFormater(minpricetime,true) + "\n \n");
				} else {
					textstr.Append(L"   N/A \n \n");
				}
				textstr.Append(L"Max price:\n");
				if (maxpricetime != maxpricetime.GetMinValue()) {
					textstr.Append(L"   " + controlhandler_->Currency4Formater(maxprice) + "\n");
					textstr.Append(L"   " + controlhandler_->DateFormater(maxpricetime,true) + "\n \n");
				} else {
					textstr.Append(L"   N/A \n \n");
				}
				drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(20,GetClientAreaBounds().y+20,GetClientAreaBounds().width-40, GetClientAreaBounds().height-70-40), textstr, 32, FONT_STYLE_PLAIN, true, Osp::Graphics::Color::COLOR_WHITE, TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void
FPregled::AddListItem(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, Osp::Base::String col4, int itemId)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementPregled * custom_element = new CustomListElementPregled();
 custom_element->col1 = col1;
 custom_element->col2 = col2;
 custom_element->col3 = col3;
 custom_element->col4 = col4;
 custom_element->datatype = 1;

 newItem->Construct(100);
 newItem->SetItemFormat(*pCustomListItemFormat);

 newItem->SetElement(LIST_ELEMENT_COSTUM, *(static_cast<ICustomListElement *>(custom_element)));

 pExList->AddItem(*newItem, itemId);
}

void
FPregled::AddListItemFP(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementPregled * custom_element = new CustomListElementPregled();
 custom_element->col1 = col1;
 if (col2 == L"") {
	 col2 = "-";
 }
 custom_element->col2 = col2;
 custom_element->col3 = col3;
 custom_element->col4 = col3;
 custom_element->datatype = 2;

 newItem->Construct(100);
 newItem->SetItemFormat(*pCustomListItemFormat);

 newItem->SetElement(LIST_ELEMENT_COSTUM, *(static_cast<ICustomListElement *>(custom_element)));
 newItem->SetCheckBox(LIST_ELEMENT_COSTUM);

 pExList->AddItem(*newItem, itemId);
}

void
FPregled::AddListItemFIN(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId)
{
 CustomListItem * newItem = new CustomListItem();
 CustomListElementPregled * custom_element = new CustomListElementPregled();
 custom_element->col1 = col1;
 if (col2 == L"") {
	 col2 = "-";
 }
 custom_element->col2 = col2;
 custom_element->col3 = col3;
 custom_element->col4 = col3;
 custom_element->datatype = 3;

 newItem->Construct(100);
 newItem->SetItemFormat(*pCustomListItemFormat);

 newItem->SetElement(LIST_ELEMENT_COSTUM, *(static_cast<ICustomListElement *>(custom_element)));
 newItem->SetCheckBox(LIST_ELEMENT_COSTUM);

 pExList->AddItem(*newItem, itemId);
}

result
FPregled::OnTerminating(void)
{
	result r = E_SUCCESS;

	delete __pOptionMenu;
	delete pCustomListItemFormat;
	grafdata.RemoveAll(true);
	controlhandler_->Clear();
	delete tablisticonbmp;
	delete tabgraficonbmp;
	delete tabsumiconbmp;
	delete pPopupFilter_;

	delete buttonmonthbmpn_;
	delete buttonmonthbmps_;
	delete buttonquaterbmpn_;
	delete buttonquaterbmps_;
	delete buttonhalfbmpn_;
	delete buttonhalfbmps_;
	delete buttonyearbmpn_;
	delete buttonyearbmps_;
	delete buttonclearbmpn_;
	delete buttonclearbmps_;

	delete pGrafBmp;

	return r;
}

void
FPregled::OnActionPerformed(const Control& source, int actionId)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (actionId == ID_OPTIONKEY) {
			if (__pOptionMenu != null) {
				__pOptionMenu->SetShowState(true);
				__pOptionMenu->Show();
			}
	} else if (actionId == ID_BACKBTN) {
		Application::GetInstance()->GetAppFrame()->GetFrame()->RemoveControl(*pFGrafZoom_);
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == REPORTCON) {
		Application::GetInstance()->GetAppFrame()->GetFrame()->RemoveControl(*pFGrafZoom_);
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PREGLEDCONFORM, null);
	} else if (actionId == REPORTFP) {
		Application::GetInstance()->GetAppFrame()->GetFrame()->RemoveControl(*pFGrafZoom_);
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PREGLEDFUELPRICEFORM, null);
	} else if (actionId == REPORTFIN) {
		Application::GetInstance()->GetAppFrame()->GetFrame()->RemoveControl(*pFGrafZoom_);
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_PREGLEDEXPENSESFORM, null);
	} else if (actionId == ID_FILTERBTN) {
		filterbuttonselold = filterbuttonselcur;
		Osp::Ui::Controls::EditField * pPopupFilterEF_;
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"fromdate"));
		pPopupFilterEF_->SetText(controlhandler_->DateFormater(this->filterfromdate,false));
		pPopupFilterEF_->RequestRedraw();
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"todate"));
		pPopupFilterEF_->SetText(controlhandler_->DateFormater(this->filtertodate,false));
		pPopupFilterEF_->RequestRedraw();
		pPopupFilter_->SetShowState(true);
		pPopupFilter_->Show();
	} else if (actionId == TABBROWSE) {
		pExList->SetShowState(true);
		RequestRedraw(true);
	} else if (actionId == TABGRAPH) {
		pExList->SetShowState(false);
		RequestRedraw(true);
	} else if (actionId == TABOVERVIEW) {
		pExList->SetShowState(false);
		RequestRedraw(true);
	} else if (actionId == FILTERCANCEL) {
		FilterButtonsSel(filterbuttonselold);
		pPopupFilter_->SetShowState(false);
		RequestRedraw(true);
	} else if (actionId == FILTERAPPLY) {
		Osp::Ui::Controls::EditField * pPopupFilterEF_;
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"fromdate"));
		this->filterfromdate = controlhandler_->ParseDateTime(pPopupFilterEF_->GetText());
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"todate"));
		this->filtertodate = controlhandler_->ParseDateTime(pPopupFilterEF_->GetText());
		LoadData();
		pPopupFilter_->SetShowState(false);
		RequestRedraw(true);
	} else if ((actionId == FILTERMONTH) || (actionId == FILTERQUATER) || (actionId == FILTERHALF) || (actionId == FILTERYEAR)) {
		Osp::Ui::Controls::EditField * pPopupFilterEF_;
		Osp::Base::DateTime setdate;
		setdate = controlhandler_->GetValueDate(this, L"todate");
		setdate = pDatePicker_->GetDate();
		if (actionId == FILTERMONTH) {
			setdate.AddMonths(-1);
			FilterButtonsSel(1);
		} else if (actionId == FILTERQUATER) {
			setdate.AddMonths(-3);
			FilterButtonsSel(2);
		} else if (actionId == FILTERHALF) {
			setdate.AddMonths(-6);
			FilterButtonsSel(3);
		} else if (actionId == FILTERYEAR) {
			setdate.AddYears(-1);
			FilterButtonsSel(4);
		}
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"fromdate"));
		pPopupFilterEF_->SetText(controlhandler_->DateFormater(setdate,false));
		pPopupFilterEF_->RequestRedraw();
	} else if (actionId == FILTERCLEAR) {
		FilterButtonsSel(5);
		Osp::Ui::Controls::EditField * pPopupFilterEF_;
		pDatePicker_->SetCurrentDate();
		Osp::Base::DateTime mindate;
		int minyear,maxyear;
		pDatePicker_->GetYearRange(minyear, maxyear);
		mindate.SetValue(minyear,1,1,0,0,0);
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"fromdate"));
		pPopupFilterEF_->SetText(controlhandler_->DateFormater(mindate,false));
		pPopupFilterEF_->RequestRedraw();
		pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(L"todate"));
		pPopupFilterEF_->SetText(controlhandler_->DateFormater(pDatePicker_->GetDate(),false));
		pPopupFilterEF_->RequestRedraw();
	}
}

void
FPregled::OnItemStateChanged(const Control& source, int index, int itemId, ItemStatus status)
{

}

void
FPregled::OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status)
{

}

void
FPregled::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	Tab* pTab = GetTab();
	if (pTab->GetItemActionIdAt(pTab->GetSelectedItemIndex()) == TABGRAPH) {
		if (graphzoomtouchregion.Contains(currentPosition)) {
			Application::GetInstance()->GetAppFrame()->GetFrame()->SetCurrentForm(*pFGrafZoom_);
			pFGrafZoom_->onopen();
			pFGrafZoom_->Draw();
			pFGrafZoom_->Show();
		}
	}
}

void
FPregled::OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FPregled::OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FPregled::OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
	//
}

void
FPregled::OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    //
}

void
FPregled::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{
    if (source.GetName() == L"fromdate") {
    	EditField *control = (EditField*) &source;
    	datepickersource = source.GetName();
    	pDatePicker_->SetDate(controlhandler_->ParseDateTime(control->GetText()));
		pDatePicker_->Show();
    }
    if (source.GetName() == L"todate") {
    	EditField *control = (EditField*) &source;
    	datepickersource = source.GetName();
    	pDatePicker_->SetDate(controlhandler_->ParseDateTime(control->GetText()));
		pDatePicker_->Show();
    }
}

void
FPregled::OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void FPregled::OnDateChanged(const Control& source, int year, int month, int day) {
	Osp::Ui::Controls::EditField * pPopupFilterEF_ = static_cast<EditField *>(pPopupFilter_->GetControl(datepickersource));
	pPopupFilterEF_->SetText(controlhandler_->DateFormater(pDatePicker_->GetDate(),false));
	if (datepickersource == L"todate") {
		if (filterbuttonselcur == 1) {
			OnActionPerformed(source, FILTERMONTH);
		} else if (filterbuttonselcur == 2) {
			OnActionPerformed(source, FILTERQUATER);
		} else if (filterbuttonselcur == 3) {
			OnActionPerformed(source, FILTERHALF);
		} else if (filterbuttonselcur == 4) {
			OnActionPerformed(source, FILTERYEAR);
		} else {
			FilterButtonsSel(0);
		}
	} else {
		FilterButtonsSel(0);
	}
}

void FPregled::OnDateChangeCanceled(const Control& source) {
   	//
}
