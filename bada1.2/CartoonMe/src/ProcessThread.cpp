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
 * ProcessThread.cpp
 *
 *  Created on: 7.11.2010
 *      Author: mrak
 */

#include "ProcessThread.h"
#include "FCartoonMe.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Base::Runtime;

void ProcessThread::StopMe(void) {
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCartoonMe") {
	FCartoonMe * pMainForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());
	AppLog("stop me start");
	if (pMainForm->imagefilter_ != null) {
		pMainForm->imagefilter_->stopprocess = true;
	}
	}
	AppLog("stop me");
}

void ProcessThread::RunProcess(void) {
	long long benchmarktime, benchmarktimeend;
	Osp::System::SystemTime::GetTicks(benchmarktime);
	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	if (pFrame->GetCurrentForm()->GetName() == L"FCartoonMe") {
	FCartoonMe * pMainForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());
	if (pMainForm->imagefilter_ != null) {
	isstoped = false;
	pMainForm->ProcessThreadCallback(1, 0);
	
	//AppLog("proc start");

	int window_w = 5;
	double sigma_d = 3.0f;
	double sigma_r = 0.1f;
	double max_gradient = 0.2f;
	int sharpness_levelsa = 3;
	int sharpness_levelsb = 14;
	int quant_levels = 8;
	double min_edge_strength = 0.2f;
	double origimgoverlay = 0.1f;
	double overlaytexture = 0.3f;

	pMainForm->ProcessThreadCallback(2, 0);
	pMainForm->imagefilter_->CartoonfyF(pMainForm->srcbmp, pMainForm->destbmp, window_w, sigma_d, sigma_r, max_gradient, sharpness_levelsa, sharpness_levelsb, quant_levels, min_edge_strength, origimgoverlay, pMainForm->papertexture, overlaytexture);

	Osp::System::SystemTime::GetTicks(benchmarktimeend);
	AppLog("time %d", (int)(benchmarktimeend - benchmarktime));

	isstoped = true;
	if (pMainForm->imagefilter_->stopprocess == true) {
		pMainForm->ProcessThreadCallback(4, 100);
	} else {
		pMainForm->ProcessThreadCallback(3, 100);
	}
	}
	}
}

bool
ProcessThread::OnStart(void){
	AppLog("ponstart");
	return true;
}

result
ProcessThread::Construct(void){
	Thread::Construct();
	return E_SUCCESS;
}

void
ProcessThread::OnStop(void){
	AppLog("ponstop");
}

Object * ProcessThread::Run(void)
{
	RunProcess();
	return null;
}
