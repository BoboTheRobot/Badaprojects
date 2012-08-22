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
/*
 * FSettingsThread.cpp
 *
 *  Created on: 26.10.2010
 *      Author: mrak
 */

#include "FSettingsThread.h"
#include "FSettings.h"
using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Base::Runtime;
using namespace Osp::System;
using namespace Osp::Locales;

FSettingsThread::FSettingsThread() {

}

FSettingsThread::~FSettingsThread() {

}

result
FSettingsThread::Construct()
{
	Thread::Construct();
	return E_SUCCESS;
}

bool
FSettingsThread::OnStart(void)
{
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FSettings * Formcall = static_cast<FSettings*>(pFrame->GetCurrentForm());

	Osp::Locales::LocaleManager localeManager;
	localeManager.Construct();
	Osp::Base::Collection::IList * localeslist = localeManager.GetAvailableLocalesN();
	Osp::Locales::Locale curlocalesettings = localeManager.GetSystemLocale();
    String tmps2, tmps3, tmps4;
    int selectedcurrencyid(-1);
    int systemdefaultlocaleid(-1);
    bool issupp;
    Formcall->ThreadCallbackSetB(0,localeslist->GetCount());
    for(int i=0; i < localeslist->GetCount(); i++) {
    	Formcall->ThreadCallbackProgress(i);
        Osp::Locales::Locale * alocale = (Osp::Locales::Locale * )localeslist->GetAt(i);
        localeManager.IsSupportedLocale(*alocale, issupp);
        if (issupp == true) {
        alocale->GetCountryName(tmps2);
        NumberFormatter* pCurrencyFormatter = NumberFormatter::CreateCurrencyFormatterN(*alocale);
        tmps3 = L"";
        pCurrencyFormatter->Format(1.00f, tmps3);
        tmps4 = tmps3 + " (" + tmps2 + ")";
        Formcall->ThreadCallbackAddItem(tmps4, alocale->GetLanguageCodeString() + "_" + alocale->GetCountryCodeString(), i);
        delete pCurrencyFormatter;
        if (this->selectedcc == (alocale->GetLanguageCodeString() + "_" + alocale->GetCountryCodeString())) {
        	selectedcurrencyid = i;
        }
        if ((curlocalesettings.GetLanguageCodeString() + "_" + curlocalesettings.GetCountryCodeString()) == (alocale->GetLanguageCodeString() + "_" + alocale->GetCountryCodeString())) {
        	systemdefaultlocaleid = i;
        }
        }
    }
    Formcall->ThreadCallbackProgress(localeslist->GetCount());
    delete localeslist;
	Formcall->ThreadCallbackEnd(selectedcurrencyid, systemdefaultlocaleid);
	this->Stop();
	return true;
}

void
FSettingsThread::OnStop(void)
{
	//
}
