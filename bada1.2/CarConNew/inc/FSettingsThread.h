/*
 * FSettingsThread.h
 *
 *  Created on: 26.10.2010
 *      Author: mrak
 */

#ifndef FSETTINGSTHREAD_H_
#define FSETTINGSTHREAD_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>

class FSettingsThread: public Osp::Base::Runtime::Thread {
public:
	FSettingsThread();
	virtual ~FSettingsThread();
	result Construct();
	bool OnStart(void);
	void OnStop(void);
	Osp::Base::String selectedcc;
};

#endif /* FSETTINGSTHREAD_H_ */
