/*
 * ProcessThread.h
 *
 *  Created on: 7.11.2010
 *      Author: mrak
 */

#ifndef PROCESSTHREAD_H_
#define PROCESSTHREAD_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>

class ProcessThread:
	public Osp::Base::Runtime::Thread
{
public:
	result Construct();
	virtual Osp::Base::Object *  Run(void);
	virtual bool OnStart(void);
	virtual void OnStop(void);
	void StopMe(void);
	bool isstoped;
private:
	void RunProcess(void);
};

#endif /* PROCESSTHREAD_H_ */
