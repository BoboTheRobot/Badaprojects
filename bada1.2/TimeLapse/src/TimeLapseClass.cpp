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
 * TimeLapseClass.cpp
 *
 *  Created on: 13.6.2011
 *      Author: mrak
 */

#include "TimeLapseClass.h"

using namespace Osp::Base;
using namespace Osp::Io;
using namespace Osp::Base::Utility;
using namespace Osp::Base::Collection;
using namespace Osp::Media;
using namespace Osp::Graphics;

Avi_idx1_item::Avi_idx1_item(unsigned long offset, unsigned long size) {
	this->offset = offset;
	this->size = size;
}

Avi_idx1_item::~Avi_idx1_item(void) {

}

TimeLapseClass::TimeLapseClass() {
	// TODO Auto-generated constructor stub

}

TimeLapseClass::~TimeLapseClass() {
	// TODO Auto-generated destructor stub
}

bool TimeLapseClass::ReadTLVfileinfo(Osp::Base::String filename, TIMELAPSE_FILEINFO & data) {
	TIMELAPSE_FILEFORMAT fileformatdata;
	TIMELAPSE_FRAMEHEADER frameheader;
	String frameheadersignature = L"TLFJ";

	FileAttributes attr;
	long long size;
	File::GetAttributes(filename, attr);
	size = attr.GetFileSize();

	data.filesize = size;
	data.fileframessize = size - sizeof(fileformatdata);
	data.firstframesize = 0;
	data.noofframes = 0;

	File file;
	file.Construct(filename, L"r");
	file.Read(&fileformatdata, sizeof(fileformatdata));
	if (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		data.firstframesize = frameheader.size;
		data.datetimeend.SetValue(frameheader.year + 2000,frameheader.month,frameheader.day,frameheader.hour,frameheader.min,frameheader.sec);
		data.noofframes = frameheader.frameno;
	}
	data.authorid = fileformatdata.authorid;
	data.interval = fileformatdata.interval;
	data.playframerate = fileformatdata.playframerate;
	data.datetime.SetValue(fileformatdata.year + 2000,fileformatdata.month,fileformatdata.day,fileformatdata.hour,fileformatdata.min,fileformatdata.sec);
	data.datetimeend.SetValue(fileformatdata.year + 2000,fileformatdata.month,fileformatdata.day,fileformatdata.hour,fileformatdata.min,fileformatdata.sec);
	data.hasindex = (fileformatdata.flag == 1);
	data.innerframecodecs = L"JPEG";
	if (fileformatdata.innerframecodecs == 2) {
		data.innerframecodecs = L"PNG";
	}
	data.resolution.SetSize(fileformatdata.width, fileformatdata.height);
	if (fileformatdata.noofframes > 0) {
		data.noofframes = (int)fileformatdata.noofframes;
	}
	if ((fileformatdata.year >= 1) && (fileformatdata.month >= 1) && (fileformatdata.month <= 12) && (fileformatdata.day >= 1) && (fileformatdata.day <= 31) && (fileformatdata.hour >= 0) && (fileformatdata.hour <= 24) && (fileformatdata.min >= 0) && (fileformatdata.min <= 60) && (fileformatdata.sec >= 0) && (fileformatdata.sec <= 60)) {
		data.datetimeend.SetValue(fileformatdata.year + 2000,fileformatdata.month,fileformatdata.day,fileformatdata.hour,fileformatdata.min,fileformatdata.sec);
	}
	if (fileformatdata.noofframes <= 0) {
		//v headerju ni podatkov o framih
		//dobi zadnji frame tako da premaknes pointer na konec-1mb ali na zacetek fila
		long backpos = 512000;
		if (((size-1)-sizeof(fileformatdata)) < backpos) {
			backpos = ((size-1)-sizeof(fileformatdata));
		}
		file.Seek(FILESEEKPOSITION_END, -backpos);
		long framefilepos = sizeof(fileformatdata);
		bool framefound = false;
		int validatedsignature = 0;
		int tmpbuffersize = 4096;
		int checkbuffsize = 0;
		char tmpbuffer[tmpbuffersize];
		while (file.Read(&tmpbuffer, tmpbuffersize) == tmpbuffersize) {
			checkbuffsize += tmpbuffersize;
			for (int bufferpos=0;bufferpos < tmpbuffersize;bufferpos++) {
				if (tmpbuffer[bufferpos] == (char)frameheadersignature[validatedsignature]) {
					validatedsignature++;
				} else {
					validatedsignature = 0;
					if (tmpbuffer[bufferpos] == (char)frameheadersignature[0]) {
						validatedsignature++;
					}
				}
				if (validatedsignature == 3) {
					framefilepos = file.Tell()-(tmpbuffersize-1)+(bufferpos-3);
					framefound = true;
					break;
				}
			}
			if (framefound == true) {
				break;
			}
		}
		file.Seek(FILESEEKPOSITION_BEGIN, framefilepos);
		while (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
			data.datetimeend.SetValue(frameheader.year + 2000,frameheader.month,frameheader.day,frameheader.hour,frameheader.min,frameheader.sec);
			file.Seek(FILESEEKPOSITION_CURRENT, frameheader.size);
			data.noofframes = frameheader.frameno;
		}
	}

	data.fileframessize -= (sizeof(frameheader) * data.noofframes);

	return true;
}

bool TimeLapseClass::ReadTLVFrame(Osp::Base::String filename, int frameno, Osp::Base::ByteBuffer & imagebuf) {
	TIMELAPSE_FILEFORMAT fileformatdata;
	String frameheadersignature = L"TLFJ";
	File file;
	file.Construct(filename, L"r");
	file.Read(&fileformatdata, sizeof(fileformatdata));
	TIMELAPSE_FRAMEHEADER frameheader;
	while (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		for (int chari=0;chari < frameheadersignature.GetLength();chari++) {
			if (not(frameheader.signature[chari] == (char)frameheadersignature[chari])) {
				return false;
			}
		}
		if (frameheader.frameno == frameno) {
			file.Read(imagebuf);
			break;
		}
		file.Seek(FILESEEKPOSITION_CURRENT, frameheader.size);
	}
	return true;
}



bool TimeLapseClass::ExportToMJPEGfile(Osp::Base::String filename, Osp::Base::String outputfilename) {
	RIFF_HEADER riffheader;
	RIFFLIST_CHUNK listchunk;
	RIFF_CHUNK chunk;
	AVI_AVIH avihdata;
	AVI_STREAMH avistreamdata;
	AVI_STREAMF avistreamfdata;
	AVI_IDX1 aviidx1;
	AVI_LISTODML avilistodml;

	int avihdatapos;
	int movilistchunkpos;

	TIMELAPSE_FILEINFO fileinfodata;
	ReadTLVfileinfo(filename, fileinfodata);

	int width = fileinfodata.resolution.width;
	int height = fileinfodata.resolution.height;

	int fps = fileinfodata.playframerate;

	long frames = fileinfodata.noofframes;
	exportfileframescount = frames;
	exportfileframecurno = 0;
	long per_usec = 1000000 / fps;
	long jpg_sz = fileinfodata.fileframessize; //all frames size in bytes
	bool hasindex = true;
	long jpg_sz_wp = 0;

	String signature = L"";

	File file;
	file.Construct(outputfilename, L"w+", true);

	//riff header
	signature = L"RIFF";
	for (int chari=0;chari < signature.GetLength();chari++) {
		riffheader.signature[chari] = signature[chari];
	}
	riffheader.size = 4+sizeof(listchunk)+sizeof(avihdata)+sizeof(chunk)+sizeof(listchunk)+sizeof(avistreamdata)+sizeof(chunk)+sizeof(avistreamfdata)+sizeof(chunk)+sizeof(avilistodml)+(sizeof(listchunk)+jpg_sz+(frames*sizeof(chunk))); //chunk size = filesize - 8
	if (hasindex == true) {
		riffheader.size += sizeof(chunk)+((sizeof(chunk)+sizeof(aviidx1))*frames);
	}
	signature = L"AVI ";
	for (int chari=0;chari < signature.GetLength();chari++) {
		riffheader.filetype[chari] = signature[chari];
	}
	file.Write(&riffheader, sizeof(riffheader));
	file.Flush();

	//list header
	signature = L"LIST";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.signature[chari] = signature[chari];
	}
	listchunk.size = 4+sizeof(avihdata)+sizeof(chunk)+sizeof(listchunk)+sizeof(avistreamdata)+sizeof(chunk)+sizeof(avistreamfdata)+sizeof(chunk)+sizeof(avilistodml); //chunk size
	signature = L"hdrl";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.listtype[chari] = signature[chari];
	}
	file.Write(&listchunk, sizeof(listchunk));
	file.Flush();

	//avih chunk
	signature = L"avih";
	for (int chari=0;chari < signature.GetLength();chari++) {
		chunk.signature[chari] = signature[chari];
	}
	chunk.size = sizeof(avihdata);
	file.Write(&chunk, sizeof(chunk));
	file.Flush();

	//avih data
	avihdatapos = file.Tell();
	avihdata.dwMicroSecPerFrame = per_usec;
	avihdata.dwMaxBytesPerSec = (1000000 * (jpg_sz/frames) / per_usec);
	avihdata.dwPaddingGranularity = 0;
	if (hasindex == true) {
		avihdata.dwFlags = 0x00000010; //HASINDEX
	} else {
		avihdata.dwFlags = 0;
	}
	avihdata.dwTotalFrames = frames;
	avihdata.dwInitialFrames = 0;
	avihdata.dwStreams = 1;
	avihdata.dwSuggestedBufferSize = 0;
	avihdata.dwWidth = width;
	avihdata.dwHeight = height;
	avihdata.dwReserved1 = 0;
	avihdata.dwReserved2 = 0;
	avihdata.dwReserved3 = 0;
	avihdata.dwReserved4 = 0;
	file.Write(&avihdata, sizeof(avihdata));
	file.Flush();

	//list header
	signature = L"LIST";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.signature[chari] = signature[chari];
	}
	listchunk.size = 4+sizeof(avistreamdata)+sizeof(chunk)+sizeof(avistreamfdata)+sizeof(chunk);
	signature = L"strl";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.listtype[chari] = signature[chari];
	}
	file.Write(&listchunk, sizeof(listchunk));
	file.Flush();

	//avi stream header chunk
	signature = L"strh";
	for (int chari=0;chari < signature.GetLength();chari++) {
		chunk.signature[chari] = signature[chari];
	}
	chunk.size = sizeof(avistreamdata);
	file.Write(&chunk, sizeof(chunk));
	file.Flush();

	//avistreamdata
	signature = L"vids";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avistreamdata.type[chari] = signature[chari];
	}
	signature = L"mjpg";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avistreamdata.fccHandler[chari] = signature[chari];
	}
	avistreamdata.dwFlags = 0;
	avistreamdata.wPriority = 0;
	avistreamdata.wLanguage = 0;
	avistreamdata.dwInitialFrames = 0;
	avistreamdata.dwScale = per_usec;
	avistreamdata.dwRate = 1000000;
	avistreamdata.dwStart = 0;
	avistreamdata.dwLength = frames;
	avistreamdata.dwSuggestedBufferSize = 0;
	avistreamdata.dwQuality = 0; //0
	avistreamdata.dwSampleSize = 0;
	avistreamdata.rcFrame1 = 0;
	avistreamdata.rcFrame2 = 0;
	file.Write(&avistreamdata, sizeof(avistreamdata));
	file.Flush();

	//avi format stream header chunk
	signature = L"strf";
	for (int chari=0;chari < signature.GetLength();chari++) {
		chunk.signature[chari] = signature[chari];
	}
	chunk.size = sizeof(avistreamfdata);
	file.Write(&chunk, sizeof(chunk));
	file.Flush();

	//avistreamfdata
	avistreamfdata.biSize = 40;
	avistreamfdata.biWidth = width;
	avistreamfdata.biHeight = height;
	avistreamfdata.biPlanesbiBitCount = 1 + 24*256*256;
	signature = L"MJPG";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avistreamfdata.biCompression[chari] = signature[chari];
	}
	avistreamfdata.biSizeImage = width * height * 3;
	avistreamfdata.biXPelsPerMeter = 0;
	avistreamfdata.biYPelsPerMeter = 0;
	avistreamfdata.biClrUsed = 0;
	avistreamfdata.biClrImportant = 0;
	file.Write(&avistreamfdata, sizeof(avistreamfdata));
	file.Flush();

	//LIST odml
	signature = L"LIST";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avilistodml.list[chari] = signature[chari];
	}
	avilistodml.size = 16;
	signature = L"odml";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avilistodml.listtype[chari] = signature[chari];
	}
	signature = L"dmlh";
	for (int chari=0;chari < signature.GetLength();chari++) {
		avilistodml.chunkname[chari] = signature[chari];
	}
	avilistodml.chunksize = 4;
	avilistodml.frames = frames;
	file.Write(&avilistodml, sizeof(avilistodml));
	file.Flush();


	//LIST movi
	movilistchunkpos = file.Tell();
	signature = L"LIST";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.signature[chari] = signature[chari];
	}
	listchunk.size = 4+jpg_sz+(frames*sizeof(chunk));
	signature = L"movi";
	for (int chari=0;chari < signature.GetLength();chari++) {
		listchunk.listtype[chari] = signature[chari];
	}
	file.Write(&listchunk, sizeof(listchunk));
	file.Flush();
	//frames output
	TIMELAPSE_FILEFORMAT fileformatdata;
	TIMELAPSE_FRAMEHEADER frameheader;
	File fileinput;
	fileinput.Construct(filename, L"r");
	fileinput.Seek(FILESEEKPOSITION_CURRENT, sizeof(fileformatdata));
	signature = L"00dc";
	for (int chari=0;chari < signature.GetLength();chari++) {
		chunk.signature[chari] = signature[chari];
	}
	Osp::Base::ByteBuffer * framebuf = new Osp::Base::ByteBuffer();
	framebuf->Construct(1048576);
	int paddingsize = 0;
	char paddingzeros[50];
	for (int chari=0;chari < 50;chari++) {
		paddingzeros[chari] = 0;
	}
	Osp::Base::Collection::ArrayList * frameindexlist = new Osp::Base::Collection::ArrayList();
	frameindexlist->Construct();
	unsigned long indexposcur = 4;
	while (fileinput.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		exportfileframecurno++;
		framebuf->SetLimit(frameheader.size);
		framebuf->Rewind();
		fileinput.Read(*framebuf);
		paddingsize = (4-(frameheader.size%4)) % 4;

		chunk.size = frameheader.size + paddingsize;

		Avi_idx1_item * Avi_idx1_item_ = new Avi_idx1_item(indexposcur, chunk.size);

		frameindexlist->Add(*Avi_idx1_item_);

		indexposcur += chunk.size+8;

		jpg_sz_wp += chunk.size;
		file.Write(&chunk, sizeof(chunk));
		/*framebuf->SetByte(6, 0x41); //A
		framebuf->SetByte(7, 0x56); //V
		framebuf->SetByte(8, 0x49); //I
		framebuf->SetByte(9, 0x31); //1*/
		framebuf->Rewind();
		file.Write(*framebuf);

		if (paddingsize > 0) {
			file.Write(&paddingzeros, paddingsize);
		}

		file.Flush();
	}
	delete framebuf;

	//set real size in file
	file.Seek(FILESEEKPOSITION_BEGIN,0);
	riffheader.size = riffheader.size -jpg_sz + jpg_sz_wp;
	file.Write(&riffheader, sizeof(riffheader));
	file.Flush();

	file.Seek(FILESEEKPOSITION_BEGIN,avihdatapos);
	avihdata.dwMaxBytesPerSec = (1000000 * (jpg_sz_wp/frames) / per_usec);
	file.Write(&avihdata, sizeof(avihdata));
	file.Flush();

	file.Seek(FILESEEKPOSITION_BEGIN,movilistchunkpos);
	listchunk.size = 4+jpg_sz_wp+(frames*sizeof(chunk));
	file.Write(&listchunk, sizeof(listchunk));
	file.Flush();

	file.Seek(FILESEEKPOSITION_END,0);


	if (hasindex == true) {
		signature = L"idx1";
		for (int chari=0;chari < signature.GetLength();chari++) {
			chunk.signature[chari] = signature[chari];
		}
		chunk.size = 16 * frames;
		file.Write(&chunk, sizeof(chunk));
		file.Flush();
		signature = L"00db";
		for (int chari=0;chari < signature.GetLength();chari++) {
			chunk.signature[chari] = signature[chari];
		}
		chunk.size = sizeof(aviidx1);
		Osp::Base::Collection::IEnumerator * pEnum = frameindexlist->GetEnumeratorN();
		Avi_idx1_item * Avi_idx1_item_ = null;
		while (pEnum->MoveNext() == E_SUCCESS) {
			Avi_idx1_item_ = static_cast<Avi_idx1_item *> (pEnum->GetCurrent());
			aviidx1.offset = Avi_idx1_item_->offset;
			aviidx1.size = Avi_idx1_item_->size;
			file.Write(&chunk, sizeof(chunk));
			file.Write(&aviidx1, sizeof(aviidx1));
			file.Flush();
		}
	}

	frameindexlist->RemoveAll(true);
	delete frameindexlist;

	return true;
}

bool TimeLapseClass::ExportToJPEGframefile(Osp::Base::String filename, long long framefilepos, Osp::Base::String outputfilename) {
	File file;
	file.Construct(filename, L"r");
	file.Seek(FILESEEKPOSITION_BEGIN, framefilepos);
	TIMELAPSE_FRAMEHEADER frameheader;
	if (file.Read(&frameheader, sizeof(frameheader)) == sizeof(frameheader)) {
		Osp::Base::ByteBuffer * imagebuf = new Osp::Base::ByteBuffer();
		imagebuf->Construct(frameheader.size);
		file.Read(*imagebuf);
		File fileout;
		fileout.Construct(outputfilename, L"w+", true);
		fileout.Write(*imagebuf);
		fileout.Flush();
		delete imagebuf;
	}
	file.Seek(FILESEEKPOSITION_BEGIN, 0);
	return true;
}
