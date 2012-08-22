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
 * ImageFilters.cpp
 *
 *  Created on: 25.1.2011
 *      Author: mrak
 */

#include "ImageFilters.h"
#include <math.h>
#include <stdlib.h>
#include "ColorConv.h"
#include "FCartoonMe.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Graphics;

ImageFilters::ImageFilters() {

}

ImageFilters::~ImageFilters() {
	delete colorconv_;
	if (tmpimgbuffer_ != null) {
		delete tmpimgbuffer_;
		tmpimgbuffer_ = null;
	}
	if (pixdatainlab_ != null) {
		delete pixdatainlab_;
		pixdatainlab_ = null;
	}
}

void ImageFilters::Construct() {
	colorconv_ = new ColorConv();
	colorconv_->Construct();
	tmpimgbuffer_ = null;
	pixdatainlab_ = null;
}

void ImageFilters::BilateralFilterGray(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r) {
	BufferInfo inbmpbuff;
	BufferInfo outbmpbuff;

	inputbmp->Lock(inbmpbuff);
	outputbmp->Lock(outbmpbuff);

	int width = inbmpbuff.width;
	int height = inbmpbuff.height;
	int bitsPerPixel = inbmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}

	//Pre-compute Gaussian distance weights.
	double Gaussian_d[(window_w*2)+1][(window_w*2)+1];
	memset(Gaussian_d, 0, ((window_w*2)+1)*((window_w*2)+1));
	for (int y=-window_w;y<=window_w;y++) {
		for (int x=-window_w;x<=window_w;x++) {
			Gaussian_d[x+window_w][y+window_w] = exp(-(pow(x,2)+pow(y,2))/(2*pow(sigma_d,2)));
		}
	}

	//filter loop
	int xMin, xMax, yMin, yMax, windowx, windowy;
	double gaussian_w; //Gaussian intensity weight
	double curpix, centerpix;
	double sumF, sumI, filterresponse;

	int * colorin;
	byte * coloringray;
	int * colortmp;
	byte * colortmpgray;
	int * colorout;
	byte * coloroutr;
	byte * coloroutg;
	byte * coloroutb;
	int dgray;

	for (int y=1;y<=height;y++) {
		for (int x=1;x<=width;x++) {
			xMin = x-window_w;
			if (xMin < 1) xMin = 1;
			xMax = x+window_w;
			if (xMax > width) xMax = width;
			yMin = y-window_w;
			if (yMin < 1) yMin = 1;
			yMax = y+window_w;
			if (yMax > height) yMax = height;

			colorin = ((int *)((byte *) inbmpbuff.pPixels + (y-1) * inbmpbuff.pitch + (x-1) *bytesPerPixel));
			coloringray = (byte*) colorin;
			colorout = ((int *)((byte *) outbmpbuff.pPixels + (y-1) * outbmpbuff.pitch + (x-1) *bytesPerPixel));
			coloroutb = (byte*) colorout;
			coloroutg = coloroutb + 1;
			coloroutr = coloroutg + 1;

			centerpix = (*coloringray) / 255.0f;

			sumF = 0;
			sumI = 0;
			windowy = 1;
			for (int px_y=yMin;px_y<=yMax;px_y++) {
				windowx = 1;
				for (int px_x=xMin;px_x<=xMax;px_x++) {
					colortmp = ((int *)((byte *) inbmpbuff.pPixels + (px_y-1) * inbmpbuff.pitch + (px_x-1) *bytesPerPixel));
					colortmpgray = (byte*) colortmp;

					curpix = (*colortmpgray) / 255.0f;
					//Calculate gaussian intensity weight
					gaussian_w = exp((-pow((curpix - centerpix),2))/(2.0f*pow(sigma_r,2)));
					//Calculate bilateral filter response
					filterresponse = gaussian_w*Gaussian_d[px_y-y+window_w+1-1][px_x-x+window_w+1-1];
					//Sum values
					sumF = sumF + filterresponse;
					sumI = sumI + (curpix * filterresponse);
					windowx++;
				}
				windowy++;
			}

			//save pix
			dgray = (int)((double)((sumI / sumF) * 255.0f));
			if (dgray < 0) dgray = 0;
			if (dgray > 255) dgray = 255;
			*coloroutr = (byte)dgray;
			*coloroutg = (byte)dgray;
			*coloroutb = (byte)dgray;
		}
	}

	inputbmp->Unlock();
	outputbmp->Unlock();
}

void ImageFilters::ReadImageRGBToLAB(Osp::Graphics::Bitmap * inputbmp, Osp::Base::DoubleBuffer * outputbmpbuffer) {
	BufferInfo inbmpbuff;
	inputbmp->Lock(inbmpbuff);
	int width = inbmpbuff.width;
	int height = inbmpbuff.height;
	int bitsPerPixel = inbmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}
	int * colorin;
	byte * colorinr;
	byte * coloring;
	byte * colorinb;

	double labl, laba, labb;

	outputbmpbuffer->Rewind();
	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			colorin = ((int *)((byte *) inbmpbuff.pPixels + (y-1) * inbmpbuff.pitch + (x-1) *bytesPerPixel));
			colorinb = (byte*) colorin;
			coloring = colorinb + 1;
			colorinr = coloring + 1;

			colorconv_->Rgb2Lab(&labl, &laba, &labb, ((*colorinr) / 255.0f), ((*coloring) / 255.0f), ((*colorinb) / 255.0f));
			outputbmpbuffer->Set(labl);
			outputbmpbuffer->Set(laba);
			outputbmpbuffer->Set(labb);
		}
	}
	inputbmp->Unlock();
}

void ImageFilters::SaveImageLABToRGB(Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Graphics::Bitmap * outputbmp) {
	BufferInfo outbmpbuff;
	outputbmp->Lock(outbmpbuff);
	int width = outbmpbuff.width;
	int height = outbmpbuff.height;
	int bitsPerPixel = outbmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}
	inputbmpbuffer->Rewind();

	int * colorout;
	byte * coloroutr;
	byte * coloroutg;
	byte * coloroutb;
	double dr,dg,db, labl, laba, labb;

	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			colorout = ((int *)((byte *) outbmpbuff.pPixels + (y-1) * outbmpbuff.pitch + (x-1) *bytesPerPixel));
			coloroutb = (byte*) colorout;
			coloroutg = coloroutb + 1;
			coloroutr = coloroutg + 1;

			inputbmpbuffer->Get(labl);
			inputbmpbuffer->Get(laba);
			inputbmpbuffer->Get(labb);

			colorconv_->Lab2Rgb(&dr, &dg, &db, labl, laba, labb);
			dr = dr * 255.0f;
			dg = dg * 255.0f;
			db = db * 255.0f;
			//save pix
			if (dr < 0) dr = 0;
			if (dr > 255) dr = 255;
			if (dg < 0) dg = 0;
			if (dg > 255) dg = 255;
			if (db < 0) db = 0;
			if (db > 255) db = 255;
			*coloroutr = (byte)dr;
			*coloroutg = (byte)dg;
			*coloroutb = (byte)db;
		}
	}

	outputbmp->Unlock();
}

void ImageFilters::BilateralFilterColor(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r) {
	int width = inputbmp->GetWidth();
	int height = inputbmp->GetHeight();

	DoubleBuffer * pixdataoutlab_ = new DoubleBuffer();
	pixdataoutlab_->Construct(width*height*3);

	DoubleBuffer * pixdatainlab_ = new DoubleBuffer();
	pixdatainlab_->Construct(width*height*3);

	ReadImageRGBToLAB(inputbmp, pixdatainlab_);

	BilateralFilterColor(width, height, pixdatainlab_, pixdataoutlab_, window_w, sigma_d, sigma_r);

	delete pixdatainlab_;

	SaveImageLABToRGB(pixdataoutlab_, outputbmp);

}

void ImageFilters::BilateralFilterColor(Osp::Graphics::Bitmap * inputbmp, Osp::Base::DoubleBuffer * outputbmpbuffer, int window_w, double sigma_d, double sigma_r) {
	int width = inputbmp->GetWidth();
	int height = inputbmp->GetHeight();

	if (pixdatainlab_ == null) {
	pixdatainlab_ = new DoubleBuffer();
	pixdatainlab_->Construct(width*height*3);
	}

	ReadImageRGBToLAB(inputbmp, pixdatainlab_);

	BilateralFilterColor(width, height, pixdatainlab_, outputbmpbuffer, window_w, sigma_d, sigma_r);

	if (pixdatainlab_ != null) {
	delete pixdatainlab_;
	pixdatainlab_ = null;
	}

}

void ImageFilters::BilateralFilterColor(int width, int height, Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Base::DoubleBuffer * outputbmpbuffer, int window_w, double sigma_d, double sigma_r) {

	outputbmpbuffer->Rewind();
	inputbmpbuffer->Rewind();

	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FCartoonMe * pMainForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());

	int curposupdatec=0;
	int stepi = 0;
	float progressfactor1 = 1.0f / (float)(height);
	float progressfactor2 = progressfactor1 * 0.8f;

	//Pre-compute Gaussian distance weights.
	double sigma_dpowtwo = (2*(sigma_d*sigma_d));
	double Gaussian_d[(window_w*2)+1][(window_w*2)+1];
	memset(Gaussian_d, 0, ((window_w*2)+1)*((window_w*2)+1));
	for (int y=-window_w;y<=window_w;y++) {
		for (int x=-window_w;x<=window_w;x++) {
			Gaussian_d[x+window_w][y+window_w] = exp(-((x*x)+(y*y))/sigma_dpowtwo);
		}
	}

	//Rescale range variance (using maximum luminance)
	sigma_r = 100*sigma_r;

	//filter loop
	int xMin, xMax, yMin, yMax, windowx, windowy;
	double gaussian_w; //Gaussian intensity weight
	double curpixll, curpixla, curpixlb, centerpixll, centerpixla, centerpixlb;
	double sumF, sumIll, sumIla, sumIlb, filterresponse;

	double sigma_rpowtwo = (2*(sigma_r*sigma_r));
	double sigma_rpowtwoinv = 1/sigma_rpowtwo;

	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		curposupdatec++;
		stepi++;
		if (curposupdatec > 5) {
			pMainForm->ProcessThreadCallback(2,(stepi*progressfactor2));
			curposupdatec = 0;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			xMin = x-window_w;
			if (xMin < 1) xMin = 1;
			xMax = x+window_w;
			if (xMax > width) xMax = width;
			yMin = y-window_w;
			if (yMin < 1) yMin = 1;
			yMax = y+window_w;
			if (yMax > height) yMax = height;

			inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1)*3));
			inputbmpbuffer->Get(centerpixll);
			inputbmpbuffer->Get(centerpixla);
			inputbmpbuffer->Get(centerpixlb);

			sumF = 0;
			sumIll = 0;
			sumIla = 0;
			sumIlb = 0;
			windowy = 1;
			for (int px_y=yMin;px_y<=yMax;px_y++) {
				windowx = 1;
				inputbmpbuffer->SetPosition(((px_y-1)*width*3)+((xMin-1)*3));
				for (int px_x=xMin;px_x<=xMax;px_x++) {
					//inputbmpbuffer->SetPosition(((px_y-1)*width*3)+((px_x-1)*3));
					inputbmpbuffer->Get(curpixll);
					inputbmpbuffer->Get(curpixla);
					inputbmpbuffer->Get(curpixlb);
					//Calculate gaussian intensity weight
					//gaussian_w = exp((-(pow((curpixll - centerpixll),2)+pow((curpixla - centerpixla),2)+pow((curpixlb - centerpixlb),2)))/sigma_rpowtwo);
					gaussian_w = exp((-(((curpixll - centerpixll)*(curpixll - centerpixll))+((curpixla - centerpixla)*(curpixla - centerpixla))+((curpixlb - centerpixlb)*(curpixlb - centerpixlb))))*sigma_rpowtwoinv);
					//Calculate bilateral filter response
					filterresponse = gaussian_w*Gaussian_d[px_y-y+window_w+1-1][px_x-x+window_w+1-1];
					//Sum values
					sumF = sumF + filterresponse;
					sumIll = sumIll + (curpixll * filterresponse);
					sumIla = sumIla + (curpixla * filterresponse);
					sumIlb = sumIlb + (curpixlb * filterresponse);
					windowx++;
				}
				windowy++;
			}

			outputbmpbuffer->Set((sumIll / sumF));
			outputbmpbuffer->Set((sumIla / sumF));
			outputbmpbuffer->Set((sumIlb / sumF));

		}
	}

}

float ImageFilters::squash_tanh(float y)
{
    union
    {
     float f;
          int i;
    }x;

    int s=(1<<31);
    x.f=y;
    int temp=x.i;
    x.i+=(24<<23);
    x.i|=s;
    s&=temp;
    x.f+=float(127<<23);
    x.i=x.f;
    x.f+=1.0f;
    x.i=(255<<23)-x.i;
    x.f-=1.0f;
    x.i|=s;
    return x.f;
}

float ImageFilters::squash_sqrt(float number) {
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}


void ImageFilters::Cartoonfy(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength) {
	BufferInfo inbmpbuff;
	inputbmp->Lock(inbmpbuff);

	int width = inbmpbuff.width;
	int height = inbmpbuff.height;
	int bitsPerPixel = inbmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}

	int * colorin;
	byte * colorinr;
	byte * coloring;
	byte * colorinb;
	double colorinll, colorinla, colorinlb;

	DoubleBuffer * pixdatainlab_ = new DoubleBuffer();
	pixdatainlab_->Construct(width*height*3);
	pixdatainlab_->Rewind();
	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			colorin = ((int *)((byte *) inbmpbuff.pPixels + (y-1) * inbmpbuff.pitch + (x-1) *bytesPerPixel));
			colorinb = (byte*) colorin;
			coloring = colorinb + 1;
			colorinr = coloring + 1;

			colorconv_->Rgb2Lab(&colorinll, &colorinla, &colorinlb, ((*colorinr) / 255.0f), ((*coloring) / 255.0f), ((*colorinb) / 255.0f));
			pixdatainlab_->Set(colorinll);
			pixdatainlab_->Set(colorinla);
			pixdatainlab_->Set(colorinlb);
		}
	}
	inputbmp->Unlock();

	Cartoonfy(pixdatainlab_, outputbmp, max_gradient, sharpness_levelsa, sharpness_levelsb, quant_levels, min_edge_strength);

	delete pixdatainlab_;
}

void ImageFilters::Cartoonfy(Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Graphics::Bitmap * outputbmp, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength) {
	BufferInfo outbmpbuff;

	long long benchmarktime, benchmarktimeend;
	Osp::System::SystemTime::GetTicks(benchmarktime);

	outputbmp->Lock(outbmpbuff);
	inputbmpbuffer->Rewind();

	Osp::Ui::Controls::Frame* pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	FCartoonMe * pMainForm = static_cast<FCartoonMe*>(pFrame->GetCurrentForm());

	int width = outbmpbuff.width;
	int height = outbmpbuff.height;
	int bitsPerPixel = outbmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}

	int curposupdatec=0;
	int stepi = 0;
	float progressfactor1 = 1.0f / (float)(height);
	float progressfactor2 = progressfactor1 * 0.2f;

	int * colorout;
	byte * coloroutr;
	byte * coloroutg;
	byte * coloroutb;
	double dr,dg,db;
	double colorinll, colorinla, colorinlb;
	double colortmpll;

	double gradientleftpix, gradientrightpix, gradienttoppix, gradientbottompix, gradientstepx, gradientstepy;
	double gradientstepxinv, gradientstepyinv;
	double gx, gy, g;
	double edge, sharp, dq, qB, qBc;

	dq = 100.0f/(quant_levels-1);

	double onedivdq = (1/dq);
	double dqdiv2 = (dq/2);
	double max_gradientinv = 1/max_gradient;

	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		curposupdatec++;
		stepi++;
		if (curposupdatec > 5) {
			pMainForm->ProcessThreadCallback(2, 0.8f + (stepi*progressfactor2));
			curposupdatec = 0;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			colorout = ((int *)((byte *) outbmpbuff.pPixels + (y-1) * outbmpbuff.pitch + (x-1) *bytesPerPixel));
			coloroutb = (byte*) colorout;
			coloroutg = coloroutb + 1;
			coloroutr = coloroutg + 1;

			inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1)*3));
			inputbmpbuffer->Get(colorinll);
			inputbmpbuffer->Get(colorinla);
			inputbmpbuffer->Get(colorinlb);

			//Determine gradient magnitude of luminance
			//gradient x
			if (x == 1) {
				inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1+1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientleftpix = colorinll;
				gradientrightpix = colortmpll;
				gradientstepx = 1;
				gradientstepxinv = 0.01f; //1/100/1
			} else if (x == width) {
				inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientleftpix = colortmpll;
				gradientrightpix = colorinll;
				gradientstepx = 1;
				gradientstepxinv = 0.01f; //1/100/1
			} else {
				inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientleftpix = colortmpll;
				inputbmpbuffer->SetPosition(((y-1)*width*3)+((x-1+1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientrightpix = colortmpll;
				gradientstepx = 2;
				gradientstepxinv = 0.005f; //1/100/2
			}
			//gx = (((gradientrightpix) - (gradientleftpix)) / 100.0f) / gradientstepx;
			gx = ((gradientrightpix) - (gradientleftpix)) * gradientstepxinv;
			//gradient y
			if (y == 1) {
				inputbmpbuffer->SetPosition(((y-1+1)*width*3)+((x-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradienttoppix = colorinll;
				gradientbottompix = colortmpll;
				gradientstepy = 1;
				gradientstepyinv = 0.01f; //1/100/1
			} else if (y == height) {
				inputbmpbuffer->SetPosition(((y-1-1)*width*3)+((x-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientbottompix = colorinll;
				gradienttoppix = colortmpll;
				gradientstepy = 1;
				gradientstepyinv = 0.01f; //1/100/1
			} else {
				inputbmpbuffer->SetPosition(((y-1-1)*width*3)+((x-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradienttoppix = colortmpll;
				inputbmpbuffer->SetPosition(((y-1+1)*width*3)+((x-1)*3));
				inputbmpbuffer->Get(colortmpll);
				gradientbottompix = colortmpll;
				gradientstepy = 2;
				gradientstepyinv = 0.005f; //1/100/2
			}
			//gy = (((gradientbottompix) - (gradienttoppix)) / 100.0f) / gradientstepy;
			gy = ((gradientbottompix) - (gradienttoppix)) * gradientstepyinv;
			//gradient normalization
			g = squash_sqrt((gx*gx)+(gy*gy)); //g = sqrt(pow(gx,2)+pow(gy,2));
			if (g > max_gradient) {
				g = max_gradient;
			}
			//g = g/max_gradient;
			g = g*max_gradientinv;
			//Create a simple edge map using the gradient magnitudes
			edge = g;
			if (edge < min_edge_strength) {
				edge = 0;
			}
			//Determine per-pixel "sharpening" parameter
			sharp = (sharpness_levelsb - sharpness_levelsa)*g+sharpness_levelsa;
			//Apply soft luminance quantization
			qBc = colorinll;
			qB = qBc;
			qB = onedivdq*qB;
			qB = dq*round(qB);
			//qB = qB+dqdiv2*tanh(sharp*(qBc-qB));
			qB = qB+dqdiv2*squash_tanh(sharp*(qBc-qB));
			//Transform back to sRGB color space
			colorconv_->Lab2Rgb(&dr, &dg, &db, qB, colorinla, colorinlb);
			//Add gradient edges to quantized image
			dr = (1-edge) * dr;
			dg = (1-edge) * dg;
			db = (1-edge) * db;
			//double RGB to byte RGB
			dr = dr * 255.0f;
			dg = dg * 255.0f;
			db = db * 255.0f;
			//save pix
			if (dr < 0) dr = 0;
			if (dr > 255) dr = 255;
			if (dg < 0) dg = 0;
			if (dg > 255) dg = 255;
			if (db < 0) db = 0;
			if (db > 255) db = 255;
			*coloroutr = (byte)dr;
			*coloroutg = (byte)dg;
			*coloroutb = (byte)db;
		}
	}
	outputbmp->Unlock();
	Osp::System::SystemTime::GetTicks(benchmarktimeend);
	AppLog("cartoonfy only %d", (int)(benchmarktimeend - benchmarktime));
}

void ImageFilters::CartoonfyF(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength, double origimgoverlay, Osp::Graphics::Bitmap * overlaytexturebmp, double overlaytexture) {
	stopprocess = false;
	if (tmpimgbuffer_ == null) {
	tmpimgbuffer_ = new DoubleBuffer();
	tmpimgbuffer_->Construct(inputbmp->GetWidth()*inputbmp->GetHeight()*3);
	}
	outputbmp->Merge(Osp::Graphics::Point(0,0), *inputbmp, Rectangle(0,0,inputbmp->GetWidth(),inputbmp->GetHeight()));

	this->BilateralFilterColor(inputbmp, tmpimgbuffer_, window_w, sigma_d, sigma_r);
	this->Cartoonfy(tmpimgbuffer_, outputbmp, max_gradient, sharpness_levelsa, sharpness_levelsb, quant_levels, min_edge_strength);

	if (tmpimgbuffer_ != null) {
		delete tmpimgbuffer_;
		tmpimgbuffer_ = null;
	}

	BufferInfo bmpbuff;
	BufferInfo bmpbufforig;
	BufferInfo bmpbufftexture;
	outputbmp->Lock(bmpbuff);
	inputbmp->Lock(bmpbufforig);
	overlaytexturebmp->Lock(bmpbufftexture);
	int width = bmpbuff.width;
	int height = bmpbuff.height;
	int bitsPerPixel = bmpbuff.bitsPerPixel;
	int bytesPerPixel = 0;
	if(bitsPerPixel == 32) {
		bytesPerPixel = 4;
	} else if (bitsPerPixel == 24) {
		bytesPerPixel = 3;
	}
	int * colorout;
	byte * coloroutr;
	byte * coloroutg;
	byte * coloroutb;
	int * colororig;
	byte * colororigr;
	byte * colororigg;
	byte * colororigb;
	int * colortexture;
	byte * colortexturer;
	byte * colortextureg;
	byte * colortextureb;
	double cr, cg, cb, cr2, cg2, cb2, cr3, cg3, cb3;
	for (int y=1;y<=height;y++) {
		if (stopprocess == true) {
			AppLog("stop process");
			break;
		}
		for (int x=1;x<=width;x++) {
			if (stopprocess == true) {
				AppLog("stop process");
				break;
			}
			colorout = ((int *)((byte *) bmpbuff.pPixels + (y-1) * bmpbuff.pitch + (x-1) *bytesPerPixel));
			coloroutb = (byte*) colorout;
			coloroutg = coloroutb + 1;
			coloroutr = coloroutg + 1;
			colororig = ((int *)((byte *) bmpbufforig.pPixels + (y-1) * bmpbufforig.pitch + (x-1) *bytesPerPixel));
			colororigb = (byte*) colororig;
			colororigg = colororigb + 1;
			colororigr = colororigg + 1;
			colortexture = ((int *)((byte *) bmpbufftexture.pPixels + (y-1) * bmpbufftexture.pitch + (x-1) *bytesPerPixel));
			colortextureb = (byte*) colortexture;
			colortextureg = colortextureb + 1;
			colortexturer = colortextureg + 1;
			cr = (*coloroutr) / 255.0f;
			cg = (*coloroutg) / 255.0f;
			cb = (*coloroutb) / 255.0f;
			cr2 = (*colororigr) / 255.0f;
			cg2 = (*colororigg) / 255.0f;
			cb2 = (*colororigb) / 255.0f;
			cr3 = (*colortexturer) / 255.0f;
			cg3 = (*colortextureg) / 255.0f;
			cb3 = (*colortextureb) / 255.0f;
			cr = (cr * (1-origimgoverlay)) + (cr2 * origimgoverlay);
			cg = (cg * (1-origimgoverlay)) + (cg2 * origimgoverlay);
			cb = (cb * (1-origimgoverlay)) + (cb2 * origimgoverlay);

			cr = (cr) * ((1-overlaytexture+(cr3 * overlaytexture)));
			cg = (cg) * ((1-overlaytexture+(cg3 * overlaytexture)));
			cb = (cb) * ((1-overlaytexture+(cb3 * overlaytexture)));

			cr = round(cr * 255.0f);
			cg = round(cg * 255.0f);
			cb = round(cb * 255.0f);
			if (cr < 0) cr = 0;
			if (cr > 255) cr = 255;
			if (cg < 0) cg = 0;
			if (cg > 255) cg = 255;
			if (cb < 0) cb = 0;
			if (cb > 255) cb = 255;
			*coloroutb = (byte)cb;
			*coloroutg = (byte)cg;
			*coloroutr = (byte)cr;
		}
	}
	outputbmp->Unlock();
	inputbmp->Unlock();
	overlaytexturebmp->Unlock();
}
