/*
 * FCameraThread.h
 *
 *  Created on: 15.6.2011
 *      Author: mrak
 */

#ifndef FCAMERATHREAD_H_
#define FCAMERATHREAD_H_

#include <FBase.h>
#include <FMedia.h>
#include <FSystem.h>
#include <FIo.h>
#include <FGraphics.h>
#include "TimeLapseClass.h"

class FCameraThread:
	public Osp::Base::Runtime::Thread
{
public:
	FCameraThread();
	virtual ~FCameraThread();
	result Construct();
	bool OnStart(void);
	void OnStop(void);
	void SaveFrame();
	void ConvertYCbCr420p2RGB565(Osp::Base::ByteBuffer * pB);
	void ConvertYCbCr420p2RGB888(Osp::Base::ByteBuffer * pB);

	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	static const int THREAD_SAVEFRAME = 100;
	static const int THREAD_FINISH = 101;

protected:
	Osp::Media::Image * pimageencoder_;
	TimeLapseClass * TimeLapseClass_;

	Osp::Graphics::Bitmap * framebmp;

	TIMELAPSE_FILEFORMAT fileheader;
	TIMELAPSE_FRAMEHEADER frameheader;
	Osp::Io::File file;

public:
	Osp::Base::ByteBuffer * framepreview;

	Osp::Base::String filename;
	Osp::Graphics::Dimension resolution;

	int frameindex;
	int playframerate;
	int interval;
	long long filesize;
	bool isconstructed;
	bool issaving;

};

#endif /* FCAMERATHREAD_H_ */
