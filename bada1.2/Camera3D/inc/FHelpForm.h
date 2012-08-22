
#ifndef _FHELPFORM_H_
#define _FHELPFORM_H_

#include <FBase.h>
#include <FUi.h>
#include "TextViewLabel.h"

class FHelpForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	FHelpForm(void);
	virtual ~FHelpForm(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ABACK = 101;
	static const int TABHOWTO = 111;
	static const int TABVIEW = 112;
	static const int TABABOUT = 113;

	Osp::Ui::Controls::ScrollPanel * pTabHowToScrollPanel;
	Osp::Ui::Controls::ScrollPanel * pTabViewScrollPanel;

public:
	void OnActionPerformed(const Osp::Ui::Control&, int);
};

#endif
