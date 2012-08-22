#ifndef _FMAINFORM_H_
#define _FMAINFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FApp.h>
#include <FLocales.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "ControlHandlerItem.h"

class ButtonCostumEl : public Osp::Base::Object {
public:
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setactiontoogle, int seticoindextoogle, String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, int setactiontoogle, int seticoindextoogle, String settooltip);
	virtual ~ButtonCostumEl(void);
private:
	void SetStartValues(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip);

public:
	Osp::Graphics::Rectangle position;
	Osp::Graphics::Rectangle pressposition;
	int action;
	bool pressed;
	bool toogle;
	int group;
	int icoindex;
	int type;
	int actiontoogle;
	int icoindextoogle;
	String tooltip;
};

class GrafItemDataMountains : public Osp::Base::Object {
public:
	GrafItemDataMountains(void);
	virtual ~GrafItemDataMountains(void);
private:
	//
public:
	long long xval;
	double yval;
};

class FMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FMainForm(void);
	virtual ~FMainForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int ICONBUTTON = 1;
	static const int ICONBUTTONTOOGLE = 2;
	static const int BIGBUTTONGREEN = 3;
	static const int BIGBUTTONGREENTOOGLE = 4;
	static const int BIGBUTTONBLUE = 5;
	static const int BIGBUTTONBLUETOOGLE = 6;
	static const int FILLUPBUTTON = 7;

	static const int ACAR = 1;
	static const int ACON = 2;
	static const int AHELP = 3;
	static const int ALESS = 4;
	static const int AMONEY = 5;
	static const int AMORE = 6;
	static const int APARK = 7;
	static const int APREGLED = 8;
	static const int ASERVICE = 9;
	static const int ASETTINGS = 10;
	static const int ATRIP = 11;
	static const int AWHEEL = 12;
	static const int ASELVIEWECOICO = 1;
	static const int ASELVIEWMONEYICO = 2;
	static const int ASELVIEWECO = 31;
	static const int ASELVIEWMONEY = 32;
	static const int ANEWFILLUP = 33;
	static const int APREGLEDCON = 13;
	static const int APREGLEDSTROSKI = 14;
	static const int APREGLEDFUELPRICE = 15;

	static const int BUTTONGROUPMOREMENI = 2;
	static const int BUTTONGROUPVIEW = 3;
	static const int BUTTONGROUPDONTDRAW = 4;

	static const int VIEWECO = 1;
	static const int VIEWFIN = 2;

	int currentselview;

	DrawingClass * drawingclass_;
	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	bool moremeniopened;
	Osp::Graphics::Rectangle moremenipos;
	ControlHandler * controlhandler_;

	CarConClass * carconclass_;
	SettingsData settingsdata_;
	CarConData lastcarcondata_;

	Osp::Graphics::Bitmap* BgCacheNumbersBmpN;

	void CreateBgCacheNumbersBmpN();

public:
	virtual result OnDraw();
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	virtual void OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
};

#endif	//_FMAINFORM_H_
