/*
 * FExportThread.h
 *
 *  Created on: 18.6.2011
 *      Author: mrak
 */

#ifndef FEXPORTTHREAD_H_
#define FEXPORTTHREAD_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "TimeLapseClass.h"

class FExportThread:
	public Osp::Base::Runtime::Thread
{
public:
	FExportThread();
	virtual ~FExportThread();
	result Construct();
	bool OnStart(void);
	void OnStop(void);

protected:


public:
	TimeLapseClass * TimeLapseClass_;

	static const int THREAD_EXPORTMJPEG = 200;
	static const int THREAD_EXPORTFRAME = 201;

	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);

	Osp::Base::String filename;
	Osp::Base::String filenameout;
	long long framefilepos;
};

#endif /* FEXPORTTHREAD_H_ */
