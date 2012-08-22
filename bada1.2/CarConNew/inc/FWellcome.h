
#ifndef _FWELLCOME_H_
#define _FWELLCOME_H_

#include <FBase.h>
#include <FUi.h>
#include <FApp.h>
#include <FMedia.h>
#include "DrawingClass.h"

class FWellcome :
	public Osp::Ui::Controls::Form
	,public Osp::Ui::IActionEventListener

{

// Construction
public:
	FWellcome(void);
	virtual ~FWellcome(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ID_NEXT = 100;
	static const int ID_EXITAPP = 101;

	DrawingClass * drawingclass_;

// Generated call-back functions
public:
	virtual result OnDraw();
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
};

#endif
