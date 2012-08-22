/*
 * Cam3Dclass.h
 *
 *  Created on: 29.12.2010
 *      Author: mrak
 */

#ifndef CAM3DCLASS_H_
#define CAM3DCLASS_H_

#include <FBase.h>
#include <FIo.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>

typedef struct {
	char signature[5];
	byte ver;
	byte year;
	byte day;
	byte month;
	byte hour;
	byte min;
	byte sec;
	unsigned long authorid;
	unsigned long leftimgsize;
	unsigned long rightimgsize;
	int aligmentx;
	int aligmenty;
} CAM3D_FILEFORMAT;

typedef struct {
	Osp::Base::DateTime time;
	unsigned long authorid;
	unsigned long leftimgsize;
	unsigned long rightimgsize;
	int aligmentx;
	int aligmenty;
} CAM3D_FILEINFO;

class Cam3Dclass {
public:
	Cam3Dclass();
	virtual ~Cam3Dclass();

private:

public:
	static const int LEFTIMG = 0;
	static const int RIGHTIMG = 1;

	Osp::Graphics::Rectangle MakeAnaglyphRC(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp);
	Osp::Graphics::Rectangle MakeAnaglyphRCGray(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp);
	Osp::Graphics::Rectangle MakeAnaglyphParallel(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp);
	Osp::Graphics::Rectangle MakeAnaglyphCrossEyed(Osp::Graphics::Bitmap * leftbmp, Osp::Graphics::Bitmap * rightbmp, Osp::Graphics::Bitmap * outbitmap, Osp::Graphics::Point aligmentrp);

	result SaveCam3Dfile(Osp::Base::String leftimgfn, Osp::Base::String rightimgfn, Osp::Base::DateTime time, unsigned long authorid, int aligmentx, int aligmenty, Osp::Base::String filename);
	bool ReadCam3Dfileinfo(Osp::Base::String filename, CAM3D_FILEINFO & data);
	bool ReadCam3Dfileimg(Osp::Base::String filename, int imgtype, Osp::Base::ByteBuffer & imagebuf);
};

#endif /* CAM3DCLASS_H_ */
