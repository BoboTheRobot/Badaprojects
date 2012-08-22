
#ifndef _FHELP_H_
#define _FHELP_H_

#include <FBase.h>
#include <FUi.h>
#include "DrawingClass.h"

class FHelp :
	public Osp::Ui::Controls::Form
    ,public Osp::Ui::IActionEventListener
{

// Construction
public:
	FHelp(void);
	virtual ~FHelp(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_BACKBTN = 101;
	static const int ID_ABOUTBTN = 100;
	static const int ID_CLOSEABOUT = 102;
	static const int TABHOME = 111;
	static const int TABBROWSE = 112;
	static const int TABEDIT = 113;
	static const int TABFILLUP = 114;
	static const int TABTRIP = 115;
	static const int TABREPORTS = 116;
	Osp::Ui::Controls::Popup* pPopup_;
	DrawingClass * drawingclass_;

// Generated call-back functions
public:
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
};

#endif
