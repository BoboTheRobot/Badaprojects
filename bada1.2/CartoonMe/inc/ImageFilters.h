/*
 * ImageFilters.h
 *
 *  Created on: 25.1.2011
 *      Author: mrak
 */

#ifndef IMAGEFILTERS_H_
#define IMAGEFILTERS_H_

#include <FBase.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>
#include "ColorConv.h"

class ImageFilters {
public:
	ImageFilters();
	virtual ~ImageFilters();

private:
	ColorConv * colorconv_;
	float squash_tanh(float y);
	float squash_sqrt(float number);
	void ReadImageRGBToLAB(Osp::Graphics::Bitmap * inputbmp, Osp::Base::DoubleBuffer * outputbmpbuffer);
	void SaveImageLABToRGB(Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Graphics::Bitmap * outputbmp);

	Osp::Base::DoubleBuffer * tmpimgbuffer_;
	Osp::Base::DoubleBuffer * pixdatainlab_;

public:
	bool stopprocess;
	void Construct();
	void BilateralFilterGray(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r);
	void BilateralFilterColor(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r);
	void BilateralFilterColor(Osp::Graphics::Bitmap * inputbmp, Osp::Base::DoubleBuffer * outputbmpbuffer, int window_w, double sigma_d, double sigma_r);
	void BilateralFilterColor(int width, int height, Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Base::DoubleBuffer * outputbmpbuffer, int window_w, double sigma_d, double sigma_r);
	void Cartoonfy(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength);
	void Cartoonfy(Osp::Base::DoubleBuffer * inputbmpbuffer, Osp::Graphics::Bitmap * outputbmp, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength);
	void CartoonfyF(Osp::Graphics::Bitmap * inputbmp, Osp::Graphics::Bitmap * outputbmp, int window_w, double sigma_d, double sigma_r, double max_gradient, int sharpness_levelsa, int sharpness_levelsb, int quant_levels, double min_edge_strength, double origimgoverlay, Osp::Graphics::Bitmap * overlaytexturebmp, double overlaytexture);
};

#endif /* IMAGEFILTERS_H_ */
