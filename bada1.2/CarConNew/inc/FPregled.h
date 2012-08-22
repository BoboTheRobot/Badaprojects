
#ifndef _FPREGLED_H_
#define _FPREGLED_H_

#include <FBase.h>
#include <FUi.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"
#include "FGrafZoom.h"

class GrafItemData : public Osp::Base::Object {
public:
	GrafItemData(void);
	virtual ~GrafItemData(void);
private:
	//
public:
	long long xval;
	double yval;
};

class FPregled :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener
	,public Osp::Ui::ITouchEventListener
	,public Osp::Ui::ICustomItemEventListener
	,public Osp::Ui::IDateChangeEventListener
{

// Construction
public:
	FPregled(Osp::Base::DateTime fromdate, Osp::Base::DateTime todate, int formtype);
	virtual ~FPregled(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_BACKBTN = 101;
	static const int ID_FILTERBTN = 100;
	static const int TABBROWSE = 111;
	static const int TABGRAPH = 112;
	static const int TABOVERVIEW = 113;
	static const int FILTERCANCEL = 114;
	static const int FILTERAPPLY = 115;
	static const int FILTERMONTH = 116;
	static const int FILTERQUATER = 117;
	static const int FILTERHALF = 118;
	static const int FILTERYEAR = 119;
	static const int FILTERCLEAR = 120;
	static const int LIST_ELEMENT_COSTUM = 205;
	static const int ID_OPTIONKEY = 99;
	static const int REPORTCON = 1;
	static const int REPORTFP = 2;
	static const int REPORTFIN = 3;

	FGrafZoom *pFGrafZoom_;

	Osp::Graphics::Bitmap * pGrafBmp;

	Osp::Ui::Controls::Popup* pPopupFilter_;
	Osp::Ui::Controls::DatePicker* pDatePicker_;
	Osp::Base::String datepickersource;

	Osp::Ui::Controls::OptionMenu* __pOptionMenu;

	Osp::Ui::Controls::CustomList * pExList;
	Osp::Ui::Controls::CustomListItemFormat * pCustomListItemFormat;

	DrawingClass * drawingclass_;
	ControlHandler * controlhandler_;
	CarConClass * carconclass_;
	SettingsData settingsdata_;
	double suml;
	double sumprice;
	double avgcon;
	double avgconreal;
	double mincon;
	double maxcon;
	double minprice;
	double maxprice;
	Osp::Base::DateTime mincontime;
	Osp::Base::DateTime maxcontime;
	Osp::Base::DateTime minpricetime;
	Osp::Base::DateTime maxpricetime;

	ArrayList grafdata;

	Osp::Base::DateTime filterfromdate;
	Osp::Base::DateTime filtertodate;
	int formtype;
	Osp::Graphics::Rectangle graphzoomtouchregion;

	Osp::Graphics::Bitmap * tablisticonbmp;
	Osp::Graphics::Bitmap * tabgraficonbmp;
	Osp::Graphics::Bitmap * tabsumiconbmp;

	Osp::Graphics::Bitmap * buttonmonthbmpn_;
	Osp::Graphics::Bitmap * buttonmonthbmps_;
	Osp::Graphics::Bitmap * buttonquaterbmpn_;
	Osp::Graphics::Bitmap * buttonquaterbmps_;
	Osp::Graphics::Bitmap * buttonhalfbmpn_;
	Osp::Graphics::Bitmap * buttonhalfbmps_;
	Osp::Graphics::Bitmap * buttonyearbmpn_;
	Osp::Graphics::Bitmap * buttonyearbmps_;
	Osp::Graphics::Bitmap * buttonclearbmpn_;
	Osp::Graphics::Bitmap * buttonclearbmps_;
	int filterbuttonselcur, filterbuttonselold;

	void AddListItem(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, Osp::Base::String col4, int itemId);
	void AddListItemFP(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId);
	void AddListItemFIN(Osp::Base::String col1, Osp::Base::String col2, Osp::Base::String col3, int itemId);
	void LoadData(void);
	void FilterButtonsSel(int selindex);
// Generated call-back functions
public:
	void GetGraph(Osp::Graphics::Bitmap * pGrafBmp, Osp::Graphics::Rectangle grafimgrect, bool fullzoom=false);
	virtual result OnDraw();
	virtual void OnDateChanged(const Osp::Ui::Control& source, int year, int month, int day);
    virtual void OnDateChangeCanceled(const Osp::Ui::Control& source);
	void OnActionPerformed(const Osp::Ui::Control&, int);
    virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, Osp::Ui::ItemStatus status);
    virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
    virtual void OnTouchDoublePressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusIn(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchFocusOut(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchLongPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchMoved(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
    virtual void OnTouchReleased(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
};

#endif
