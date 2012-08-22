/*
 * TimeLapseClass.h
 *
 *  Created on: 13.6.2011
 *      Author: mrak
 */

#ifndef TIMELAPSECLASS_H_
#define TIMELAPSECLASS_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FIo.h>
#include <FSystem.h>

typedef struct {
	char signature[9];
	byte ver;
	byte year;
	byte day;
	byte month;
	byte hour;
	byte min;
	byte sec;
	unsigned long authorid;
	unsigned long interval;
	unsigned long playframerate;
	unsigned long width;
	unsigned long height;
	unsigned long noofframes;
	byte yearend;
	byte dayend;
	byte monthend;
	byte hourend;
	byte minend;
	byte secend;
	byte flag;
	byte innerframecodecs;
	unsigned long indexdatafilepos;
	unsigned long reserved1;
	unsigned long reserved2;
	unsigned long reserved3;
	unsigned long reserved4;
} TIMELAPSE_FILEFORMAT;

typedef struct {
	Osp::Base::DateTime datetime;
	Osp::Base::DateTime datetimeend;
	int noofframes;
	unsigned long firstframesize;
	unsigned long authorid;
	unsigned long interval;
	unsigned long playframerate;
	unsigned long filesize;
	unsigned long fileframessize;
	Osp::Graphics::Dimension resolution;
	bool hasindex;
	Osp::Base::String innerframecodecs;
} TIMELAPSE_FILEINFO;

typedef struct {
	char signature[4];
	unsigned long size;
	byte year;
	byte day;
	byte month;
	byte hour;
	byte min;
	byte sec;
	bool skip;
	int frameno;
} TIMELAPSE_FRAMEHEADER;


typedef struct {
	char signature[4];
	unsigned long size;
	char filetype[4];
} RIFF_HEADER;

typedef struct {
	char signature[4];
	unsigned long size;
	char listtype[4];
} RIFFLIST_CHUNK;

typedef struct {
	char signature[4];
	unsigned long size;
} RIFF_CHUNK;

typedef struct {
	unsigned long dwMicroSecPerFrame;
	unsigned long dwMaxBytesPerSec;
	unsigned long dwPaddingGranularity;
	unsigned long dwFlags;
	unsigned long dwTotalFrames;
	unsigned long dwInitialFrames;
	unsigned long dwStreams;
	unsigned long dwSuggestedBufferSize;
	unsigned long dwWidth;
	unsigned long dwHeight;
	unsigned long dwReserved1;
	unsigned long dwReserved2;
	unsigned long dwReserved3;
	unsigned long dwReserved4;
} AVI_AVIH;

typedef struct {
	char type[4];
	char fccHandler[4];
	unsigned long dwFlags;
	unsigned short wPriority;
	unsigned short wLanguage;
	unsigned long dwInitialFrames;
	unsigned long dwScale;
	unsigned long dwRate;
	unsigned long dwStart;
	unsigned long dwLength;
	unsigned long dwSuggestedBufferSize;
	unsigned long dwQuality;
	unsigned long dwSampleSize;
	unsigned long rcFrame1;
	unsigned long rcFrame2;
} AVI_STREAMH;

typedef struct {
	unsigned long biSize;
	unsigned long biWidth;
	unsigned long biHeight;
	unsigned long biPlanesbiBitCount;
	char biCompression[4];
	unsigned long biSizeImage;
	unsigned long biXPelsPerMeter;
	unsigned long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} AVI_STREAMF;

typedef struct {
	char list[4];
	unsigned long size;
	char listtype[4];
	char chunkname[4];
	unsigned long chunksize;
	unsigned long frames;
} AVI_LISTODML;

typedef struct {
	unsigned long offset;
	unsigned long size;
} AVI_IDX1;

class Avi_idx1_item : public Osp::Base::Object {
public:
	Avi_idx1_item(unsigned long offset, unsigned long size);
	virtual ~Avi_idx1_item(void);

public:
	unsigned long offset;
	unsigned long size;
};

class TimeLapseClass {
public:
	TimeLapseClass();
	virtual ~TimeLapseClass();

	bool ReadTLVfileinfo(Osp::Base::String filename, TIMELAPSE_FILEINFO & data);
	bool ReadTLVFrame(Osp::Base::String filename, int frameno, Osp::Base::ByteBuffer & imagebuf);

	long exportfileframescount;
	long exportfileframecurno;

	bool ExportToMJPEGfile(Osp::Base::String filename, Osp::Base::String outputfilename);
	bool ExportToJPEGframefile(Osp::Base::String filename, long long framefilepos, Osp::Base::String outputfilename);

};

#endif /* TIMELAPSECLASS_H_ */
