
#ifndef _FSETTINGS_H_
#define _FSETTINGS_H_

#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FSystem.h>
#include <FContent.h>
#include "CarConClass.h"
#include "DrawingClass.h"
#include "FSettingsThread.h"

class CurrencyListItem : public Osp::Base::Object {
public:
	CurrencyListItem(Osp::Base::String name, Osp::Base::String value, int itemid);
	virtual ~CurrencyListItem(void);
private:
	//
public:
	Osp::Base::String name;
	Osp::Base::String value;
	int itemid;
};

class RestoreFileListItem : public Osp::Base::Object {
public:
	RestoreFileListItem(Osp::Base::String filename, int itemid);
	virtual ~RestoreFileListItem(void);
private:
	//
public:
	Osp::Base::String filename;
	int itemid;
};

class FSettings :
	public Osp::Ui::Controls::Form
	, public Osp::Ui::IActionEventListener
	, public Osp::Ui::IItemEventListener
	, public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FSettings(int formtype=0);
	virtual ~FSettings(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_BACKBTN = 101;
	static const int ID_SAVEBTN = 100;
	static const int ID_FUELTYPESBTN = 102;
	static const int ID_LOCATIONSBTN = 103;
	static const int ID_POPUPCANCEL = 110;
	static const int ABACKUP = 121;
	static const int AEXPORT = 122;
	static const int ARESTORE = 123;
	static const int ABACKUP_PHONE = 124;
	static const int ABACKUP_SD = 125;
	static const int ABACKUP_CANCEL = 126;
	static const int ARESTORE_SEL = 127;
	static const int ARESTORE_CANCEL = 128;
	static const int ACLEARCOUNTRYLANGCACHE = 130;

	static const int SELECTTYPE_VOLUME = 1;
	static const int SELECTTYPE_DISTANCE = 2;
	static const int SELECTTYPE_CON = 3;
	static const int SELECTTYPE_CURRENCY = 4;
	CarConClass * carconclass_;
	SettingsData settingsdata_;
	DrawingClass * drawingclass_;
	Osp::Ui::Controls::Popup* pPopupSelectVolumeUnit_;
	Osp::Ui::Controls::List* pPopupSelectVolumeUnitList_;
	Osp::Ui::Controls::Popup* pPopupSelectDistanceUnit_;
	Osp::Ui::Controls::List* pPopupSelectDistanceUnitList_;
	Osp::Ui::Controls::Popup* pPopupSelectConUnit_;
	Osp::Ui::Controls::List* pPopupSelectConUnitList_;
	Osp::Ui::Controls::Popup* pPopupSelectCurrency_;
	Osp::Ui::Controls::List* pPopupSelectCurrencyList_;
	Osp::Ui::Controls::Popup* pPopupProgress_;
	FSettingsThread * pSettingsThread_;
	Osp::Base::Collection::IList* currencylist_;
	int formtype;
	int firstdraw;

	Osp::Ui::Controls::Popup* BackupExportpopup_;
	Osp::Ui::Controls::Popup* Restorepopup_;
	Osp::Ui::Controls::List* RestorepopupFilelist_;
	Osp::Base::Collection::IList* RestorepopupFilelistList_;

	int restorepopupselecteditemid_;


// Generated call-back functions
private:
	void PopupSelect(int selecttype, int value);
	void fillupselectconunit();
	bool Save();

public:
	void ThreadCallbackAddItem(Osp::Base::String col1, Osp::Base::String itemValue, int itemid);
	void ThreadCallbackEnd(int selectedcurrencyid, int systemdefaultlocaleid);
	void ThreadCallbackSetB(int min, int max);
	void ThreadCallbackProgress(int progress);
	bool LoadOnShow();
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
    virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
};

#endif
