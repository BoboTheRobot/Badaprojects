/*
 * FPlayerThread.h
 *
 *  Created on: 14.6.2011
 *      Author: mrak
 */

#ifndef FPLAYERTHREAD_H_
#define FPLAYERTHREAD_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "TimeLapseClass.h"

class FPlayerThread:
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Base::Runtime::Thread
{
public:
	FPlayerThread();
	virtual ~FPlayerThread();
	result Construct();
	bool OnStart(void);
	void OnStop(void);

protected:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	bool GetNextFrame();
	bool SkipFrame();

	Osp::Media::Image * pimagedecoder_;
	TimeLapseClass * TimeLapseClass_;

	TIMELAPSE_FILEFORMAT fileformatdata;
	TIMELAPSE_FRAMEHEADER frameheader;
	Osp::Io::File file;

	Osp::Base::ByteBuffer * tmpimgbuffer;
	Osp::Base::ByteBuffer * tmpframebuffer;

	Osp::Base::Runtime::Timer * pTimer_;

	long long curfilepos;
	double curposprocfactor;

	bool timerhasstarted;

	long long fpscontrol_lasttime;
	int frameinterval;

	Osp::Base::Collection::ArrayList * frameindexlist;

public:
	static const int THREAD_FRAMEDRAWED = 100;
	static const int THREAD_PLAYPAUSE = 101;
	static const int THREAD_PREVFRAME = 102;
	static const int THREAD_NEXTFRAME = 103;

	bool SetFrameRate(int framerate);

	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);

	Osp::Base::String filename;
	Osp::Base::DateTime curframedatetime;

	TIMELAPSE_FILEINFO fileinfo_;

	int curframe;
	double curposproc;

	long long curframefilepos;

	long long filesize;
	long long framescount;
	Osp::Graphics::Dimension resolution;
	Osp::Graphics::Dimension vresolution;
	Osp::Base::DateTime startframetime;
	Osp::Base::DateTime endframetime;
	long long curframesize;

	Osp::Graphics::Canvas * framecanvas;
	Osp::Graphics::Bitmap * framebmp;

	int skipframes;

	bool playing;

};

#endif /* FPLAYERTHREAD_H_ */
