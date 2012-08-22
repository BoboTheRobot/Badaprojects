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
#include "FSettingsForm.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::App;

FSettingsForm::FSettingsForm(void)
{
}

FSettingsForm::~FSettingsForm(void)
{
}

bool
FSettingsForm::Initialize()
{
	Form::Construct(L"IDF_FSETTINGS");

	return true;
}

void FSettingsForm::LoadSettings(int &isolevel, int &exposure, int &brightness, int &contrast, bool &flashenable, bool &shuttersound) {
	isolevel = 200;
	exposure = 5;
	brightness = 5;
	contrast = 5;
	flashenable = false;
	shuttersound = true;
	Osp::Base::String value;
	int intvalue;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"isolevel", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		isolevel = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"exposure", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		exposure = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"brightness", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		brightness = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"contrast", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		contrast = intvalue;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"flash", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		flashenable = (intvalue == 1);
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"shuttersound", value) == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		if (IsFailed(Integer::Parse(value, intvalue))) {
			intvalue = -1;
		}
	}
	if (intvalue >= 0) {
		shuttersound = (intvalue == 1);
	}
}

void FSettingsForm::SaveSettings(int isolevel, int exposure, int brightness, int contrast, bool flashenable, bool shuttersound) {
	Osp::Base::String value;
	int intvalue;

	intvalue = isolevel;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"isolevel", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"isolevel", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"isolevel", intvalue);
	}

	intvalue = exposure;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"exposure", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"exposure", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"exposure", intvalue);
	}

	intvalue = brightness;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"brightness", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"brightness", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"brightness", intvalue);
	}

	intvalue = contrast;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"contrast", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"contrast", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"contrast", intvalue);
	}

	if (flashenable == true) {
		intvalue = 1;
	} else {
		intvalue = 0;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"flash", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"flash", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"flash", intvalue);
	}
	if (shuttersound == true) {
		intvalue = 1;
	} else {
		intvalue = 0;
	}
	if (Application::GetInstance()->GetAppRegistry()->Get(L"shuttersound", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"shuttersound", intvalue);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"shuttersound", intvalue);
	}

	Application::GetInstance()->GetAppRegistry()->Save();
}

result
FSettingsForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	SetSoftkeyActionId(SOFTKEY_0, ABACK);
	AddSoftkeyActionListener(SOFTKEY_0, *this);
	SetSoftkeyActionId(SOFTKEY_1, ASAVE);
	AddSoftkeyActionListener(SOFTKEY_1, *this);



	//load settings
	LoadSettings(isolevel, exposure, brightness, contrast, flashenable, shuttersound);
	if (isolevel == 50) {
		isoleveln = 1;
	} else if (isolevel == 100) {
		isoleveln = 2;
	} else if (isolevel == 200) {
		isoleveln = 3;
	} else if (isolevel == 400) {
		isoleveln = 4;
	} else if (isolevel == 800) {
		isoleveln = 5;
	} else if (isolevel == 1600) {
		isoleveln = 6;
	} else {
		isoleveln = 1;
	}

	Osp::Ui::Controls::Slider * sliderel;
	Osp::Ui::Controls::Label * labelel;
	Osp::Ui::Controls::CheckButton * chkel;
	sliderel = static_cast<Slider *>(GetControl(L"SLIDERISO", true));
	sliderel->SetValue(isoleveln);
	sliderel->AddAdjustmentEventListener(*this);
	labelel = static_cast<Label *>(GetControl(L"LABELISO", true));
	labelel->SetText(Osp::Base::Integer::ToString(isolevel));
	sliderel = static_cast<Slider *>(GetControl(L"SLIDEREXP", true));
	sliderel->SetValue(exposure);
	sliderel->AddAdjustmentEventListener(*this);
	labelel = static_cast<Label *>(GetControl(L"LABELEXP", true));
	labelel->SetText(Osp::Base::Integer::ToString(exposure));
	sliderel = static_cast<Slider *>(GetControl(L"SLIDERBRI", true));
	sliderel->SetValue(brightness);
	sliderel->AddAdjustmentEventListener(*this);
	labelel = static_cast<Label *>(GetControl(L"LABELBRI", true));
	labelel->SetText(Osp::Base::Integer::ToString(brightness));
	sliderel = static_cast<Slider *>(GetControl(L"SLIDERCON", true));
	sliderel->SetValue(contrast);
	sliderel->AddAdjustmentEventListener(*this);
	labelel = static_cast<Label *>(GetControl(L"LABELCON", true));
	labelel->SetText(Osp::Base::Integer::ToString(contrast));
	chkel = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON1", true));
	chkel->SetSelected(flashenable);
	chkel = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON2", true));
	chkel->SetSelected(shuttersound);

	return r;
}

result
FSettingsForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	return r;
}


void
FSettingsForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == ABACK) {
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
	if (actionId == ASAVE) {
		Osp::Ui::Controls::CheckButton * chkel;
		chkel = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON1", true));
		flashenable = chkel->IsSelected();
		chkel = static_cast<CheckButton *>(GetControl(L"IDPC_CHECKBUTTON2", true));
		shuttersound = chkel->IsSelected();
		SaveSettings(isolevel, exposure, brightness, contrast, flashenable, shuttersound);
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	}
}

void FSettingsForm::OnAdjustmentValueChanged (const Osp::Ui::Control &source, int adjustment) {
	Osp::Ui::Controls::Label * labelel;
	int showvalue = 0;
	if (source.GetName() == L"SLIDERISO") {
		if (adjustment == 1) {
			showvalue = 50;
		} else if (adjustment == 2) {
			showvalue = 100;
		} else if (adjustment == 3) {
			showvalue = 200;
		} else if (adjustment == 4) {
			showvalue = 400;
		} else if (adjustment == 5) {
			showvalue = 800;
		} else if (adjustment == 6) {
			showvalue = 1600;
		} else {
			showvalue = 50;
		}
		isolevel = showvalue;
		labelel = static_cast<Label *>(GetControl(L"LABELISO", true));
		labelel->SetText(Osp::Base::Integer::ToString(showvalue));
		this->RequestRedraw();
	}
	if (source.GetName() == L"SLIDEREXP") {
		showvalue = adjustment;
		exposure = showvalue;
		labelel = static_cast<Label *>(GetControl(L"LABELEXP", true));
		labelel->SetText(Osp::Base::Integer::ToString(showvalue));
		this->RequestRedraw();
	}
	if (source.GetName() == L"SLIDERBRI") {
		showvalue = adjustment;
		brightness = showvalue;
		labelel = static_cast<Label *>(GetControl(L"LABELBRI", true));
		labelel->SetText(Osp::Base::Integer::ToString(showvalue));
		this->RequestRedraw();
	}
	if (source.GetName() == L"SLIDERCON") {
		showvalue = adjustment;
		contrast = showvalue;
		labelel = static_cast<Label *>(GetControl(L"LABELCON", true));
		labelel->SetText(Osp::Base::Integer::ToString(showvalue));
		this->RequestRedraw();
	}
}

