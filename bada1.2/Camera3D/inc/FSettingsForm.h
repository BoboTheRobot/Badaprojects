
#ifndef _FSETTINGSFORM_H_
#define _FSETTINGSFORM_H_

#include <FBase.h>
#include <FUi.h>



class FSettingsForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IAdjustmentEventListener
{

// Construction
public:
	FSettingsForm(void);
	virtual ~FSettingsForm(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int ABACK = 101;
	static const int ASAVE = 100;
	int isolevel,isoleveln,exposure,brightness,contrast;
	bool flashenable, shuttersound;

	void LoadSettings(int &isolevel, int &exposure, int &brightness, int &contrast, bool &flashenable, bool &shuttersound);
	void SaveSettings(int isolevel, int exposure, int brightness, int contrast, bool flashenable, bool shuttersound);

// Generated call-back functions
public:
    void OnAdjustmentValueChanged (const Osp::Ui::Control &source, int adjustment);
	void OnActionPerformed(const Osp::Ui::Control&, int);
};

#endif
