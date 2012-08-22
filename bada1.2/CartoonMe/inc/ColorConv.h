/*
 * ColorConv.h
 *
 *  Created on: 25.1.2011
 *      Author: mrak
 */

#ifndef COLORCONV_H_
#define COLORCONV_H_

class ColorConv {
public:
	ColorConv();
	virtual ~ColorConv();
private:
	static const int ROOT_TAB_SIZE = 16;
	float cbrt_table[ROOT_TAB_SIZE +1];
	float qn_table[ROOT_TAB_SIZE +1];

	double cbrt(double x);
	double pow24(double x);
	double qnrt(double x);

public:
	void Construct(void);
	void Rgb2Lab(double  *L, double  *a, double  *b, double  R, double  G, double  B);
	void Lab2Rgb(double  *R, double  *G, double  *B, double  L, double  a, double  b);
};

#endif /* COLORCONV_H_ */
