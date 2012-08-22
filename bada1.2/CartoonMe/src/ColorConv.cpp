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
 * ColorConv.cpp
 *
 *  Created on: 25.1.2011
 *      Author: mrak
 */

#include "ColorConv.h"
#include <math.h>

ColorConv::ColorConv() {


}

ColorConv::~ColorConv() {

}

void ColorConv::Construct(void) {
	 cbrt_table[0] = pow(float(1)/float(ROOT_TAB_SIZE*2), 0.3333);
	 qn_table[0]   = pow(float(1)/float(ROOT_TAB_SIZE*2), 0.2);
	 for(int i = 1; i < ROOT_TAB_SIZE +1; i++) {
		 cbrt_table[i] = pow(float(i)/float(ROOT_TAB_SIZE), 0.3333);
		 qn_table[i] = pow(float(i)/float(ROOT_TAB_SIZE), 0.2);
	 }
}

double ColorConv::cbrt(double x) {
	double y = cbrt_table[int(x*ROOT_TAB_SIZE )]; // assuming x \in [0, 1]
	y = (2.0 * y + x/(y*y))/3.0;
	y = (2.0 * y + x/(y*y))/3.0; // polish twice
	return y;
}


double ColorConv::qnrt(double x) {
	double y = qn_table[int(x*ROOT_TAB_SIZE)]; // assuming x \in [0, 1]
	double Y = y*y;
	y = (4.0*y + x/(Y*Y))/5.0;
	Y = y*y;
	y = (4.0*y + x/(Y*Y))/5.0; // polish twice
	return y;
}

double ColorConv::pow24(double x) {
	double onetwo = x*qnrt(x);
	return onetwo*onetwo;
}

void ColorConv::Rgb2Lab(double  *L, double  *a, double  *b, double  R, double  G, double  B) {
	if (R > 0.04045)
		//R = (float) pow((R + 0.055) / 1.055, 2.4);
		R = (float) pow24((R + 0.055) / 1.055);
	else
		R = R / 12.92;
	if (G > 0.04045)
		//G = (float) pow((G + 0.055) / 1.055, 2.4);
		G = (float) pow24((G + 0.055) / 1.055);
	else
		G = G / 12.92;
	if (B > 0.04045)
		//B = (float) pow((B + 0.055) / 1.055, 2.4);
		B = (float) pow24((B + 0.055) / 1.055);
	else
		B = B / 12.92;
	// Use white = D65
	float x = R * 0.4124 + G * 0.3576 + B * 0.1805;
	float y = R * 0.2126 + G * 0.7152 + B * 0.0722;
	float z = R * 0.0193 + G * 0.1192 + B * 0.9505;
	float vx = x / 0.95047;
	float vy = y;
	float vz = z / 1.08883;
	if (vx > 0.008856)
		//vx = (float) pow(vx, 0.3333);
		vx = (float) cbrt(vx);
	else
		vx = (7.787 * vx) + (16.0 / 116.0);
	if (vy > 0.008856)
		//vy = (float) pow(vy, 0.3333);
		vy = (float) cbrt(vy);
	else
		vy = (7.787 * vy) + (16.0 / 116.0);
	if (vz > 0.008856)
		//vz = (float) pow(vz, 0.3333);
		vz = (float) cbrt(vz);
	else
		vz = (7.787 * vz) + (16.0 / 116.0);
	*L = (116.0 * vy - 16.0);
	*a = (500.0 * (vx - vy));
	*b = (200.0 * (vy - vz));
}

void ColorConv::Lab2Rgb(double  *R, double  *G, double  *B, double  L, double  a, double  b) {
	float vy = (L + 16.0) / 116.0;
	float vx = a / 500.0 + vy;
	float vz = vy - b / 200.0;

	float vx3 = vx * vx * vx;
	float vy3 = vy * vy * vy;
	float vz3 = vz * vz * vz;

	if (vy3 > 0.008856)
		vy = vy3;
	else
		vy = (vy - 16.0 / 116.0) / 7.787;

	if (vx3 > 0.008856)
		vx = vx3;
	else
		vx = (vx - 16.0 / 116.0) / 7.787;

	if (vz3 > 0.008856)
		vz = vz3;
	else
		vz = (vz - 16.0 / 116.0) / 7.787;

	vx *= 0.95047; //use white = D65
	vz *= 1.08883;

	float vr =(float)(vx *  3.2406 + vy * -1.5372 + vz * -0.4986);
	float vg =(float)(vx * -0.9689 + vy *  1.8758 + vz *  0.0415);
	float vb =(float)(vx *  0.0557 + vy * -0.2040 + vz *  1.0570);

	if (vr > 0.0031308)
		vr = (float)(1.055 * pow(vr, (1.0 / 2.4)) - 0.055);
	else
		vr = 12.92 * vr;

	if (vg > 0.0031308)
		vg = (float)(1.055 * pow(vg, (1.0 / 2.4)) - 0.055);
	else
		vg = 12.92 * vg;

	if (vb > 0.0031308)
		vb = (float)(1.055 * pow(vb, (1.0 / 2.4)) - 0.055);
	else
		vb = 12.92 * vb;
	*R = vr;
	*G = vg;
	*B = vb;
}
