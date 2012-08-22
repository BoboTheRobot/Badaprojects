/*
 * ButtonCostumEl.h
 *
 *  Created on: 29.12.2010
 *      Author: mrak
 */

#ifndef ButtonCostumEl_H_
#define ButtonCostumEl_H_

#include <FBase.h>
#include <FUi.h>

class ButtonCostumEl : public Osp::Base::Object {
public:
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, Osp::Base::String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, Osp::Base::String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setactiontoogle, int seticoindextoogle, Osp::Base::String settooltip);
	ButtonCostumEl(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, int setactiontoogle, int seticoindextoogle, Osp::Base::String settooltip);
	virtual ~ButtonCostumEl(void);
private:
	void SetStartValues(Osp::Graphics::Rectangle setposition, int setaction, int seticoindex, int settype, int setgroup, Osp::Base::String settooltip);

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
	Osp::Base::String tooltip;
};

#endif /* ButtonCostumEl_H_ */
