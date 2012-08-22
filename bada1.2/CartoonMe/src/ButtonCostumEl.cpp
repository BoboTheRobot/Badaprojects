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
 * ButtonCostumEl.cpp
 *
 *  Created on: 29.12.2010
 *      Author: mrak
 */

#include "ButtonCostumEl.h"

using namespace Osp::Base;
using namespace Osp::Graphics;

void ButtonCostumEl::SetStartValues(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip) {
	this->action = setaction;
	this->position = setposition;
	this->pressposition = Rectangle(setposition.x, setposition.y, setposition.width, setposition.height);
	this->pressed = false;
	this->toogle = false;
	this->group = setgroup;
	this->icoindex = seticoindex;
	this->type = settype;
	this->tooltip = settooltip;
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, 0, settooltip);
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, setgroup, settooltip);
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setactiontoogle, int seticoindextoogle, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, 0, settooltip);
	this->actiontoogle = setactiontoogle;
	this->icoindextoogle = seticoindextoogle;
}

ButtonCostumEl::ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, int setactiontoogle, int seticoindextoogle, String settooltip) {
	this->SetStartValues(setposition, setaction, seticoindex, settype, setgroup, settooltip);
	this->actiontoogle = setactiontoogle;
	this->icoindextoogle = seticoindextoogle;
}

ButtonCostumEl::~ButtonCostumEl(void) {

}
