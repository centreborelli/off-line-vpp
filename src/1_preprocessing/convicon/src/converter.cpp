/*
 * Copyright (c) 2017, Pablo Arias <pablo.arias@cmla.ens-cachan.fr>
 * All rights reserved.
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later
 * version. You should have received a copy of this license along
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file converter.cpp
 * @brief Image converter between standard types (iio) and ad-hoc types.
 *
 * @author Pablo Arias <pablo.arias@cmla.ens-cachan.fr>
 **/

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <stdexcept>

#include "cmd_option.h"
extern "C" {
#include "iio.h"
}

using namespace std;

bool verbose = false;

/**
 * @brief Structure containing size informations of an image.
 *
 * @param width     : width of the image;
 * @param height    : height of the image;
 * @param channels : number of channels in the image;
 * @param wh        : equal to width * height. Provided for convenience;
 * @param whc       : equal to width * height * channels. Provided for convenience.
 **/
struct ImageSize
{
	unsigned width;
	unsigned height;
	unsigned channels;
	unsigned wh;
	unsigned whc;
};

/**
 * @brief Load image, check the number of channels.
 *
 * @param name : name of the image to read;
 * @param im : vector which will contain the image : R, G and B concatenated;
 * @param sz : will contain the size of the image;
 *
 * @return EXIT_SUCCESS if the image has been loaded, EXIT_FAILURE otherwise.
 **/
int load_image_iio(
	char * name
,	std::vector<float> &im
,	ImageSize &sz
){
	// read input image
	if (verbose) cout << endl << "Read input image...";

	float *imTmp = NULL;
	int w, h, c;
	imTmp = iio_read_image_float_split(name, &w, &h, &c);
	if (!imTmp) {
		cout << "error :: " << name << " not found or not a correct png image" << endl;
		return EXIT_FAILURE;
	}

	if (verbose) cout << "done." << endl;

	/* test if image is really a color image and exclude the alpha channel
	if (c > 2)
	{
		unsigned k = 0;
		while (k < w * h && imTmp[k] == imTmp[w * h + k] && imTmp[k] == imTmp[2 * w * h + k])
			k++;

		c = (k == w * h ? 1 : 3);
	}*/

	// Some image informations
	if (verbose) {
		cout << "image size :" << endl;
		cout << " - width          = " << w << endl;
		cout << " - height         = " << h << endl;
		cout << " - nb of channels = " << c << endl;
	}

	// Initializations
	sz.width      = w;
	sz.height     = h;
	sz.channels   = c;
	sz.wh         = w * h;
	sz.whc        = w * h * c;
	im.resize(w * h * c);
	for (unsigned k = 0; k < w * h * c; k++)
		im[k] = imTmp[k];

	free(imTmp);

	return EXIT_SUCCESS;
}

/**
 * @brief write image.
 *
 * @param name : path+name+extension of the image;
 * @param im : vector which contains the image;
 * @param sz : size of the image;
 *
 * @return EXIT_SUCCESS if the image has been saved, EXIT_FAILURE otherwise
 **/
int save_image_iio(
	char * name
,	std::vector<float> const& im
,	const ImageSize &sz
){
	// Allocate Memory
	float* imTmp = new float[sz.whc];

	for (unsigned k = 0; k < sz.whc; k++)
	{
		imTmp[k] = im[k];
	}

	iio_save_image_float_split(name, imTmp,
			sz.width,
			sz.height,
			sz.channels);

	// Free Memory
	delete[] imTmp;

	return EXIT_SUCCESS;
}

/**
 * @brief Transform the color space of an image, from RGB to YUV, or vice-versa.
 *
 * @param im: image on which the transform will be applied;
 * @param sz: size of im;
 * @param isForward: if true, go from RGB to YUV, otherwise go from YUV to RGB.
 *
 * @return none.
 **/
void transform_color_space(
	std::vector<float> &im
,	const ImageSize sz
,	const bool isForward
){
	// If the image as only one channel, do nothing
	if (sz.channels == 1) return;

	// Initialization
	const unsigned width  = sz.width;
	const unsigned height = sz.height;
	const unsigned chnls  = sz.channels;
	const unsigned wh     = width * height;
	vector<float> imTmp(wh * chnls);

	// RGB to YUV
	if (isForward) {
		if (chnls == 3) {
			const unsigned red   = 0;
			const unsigned green = wh;
			const unsigned blue  = wh * 2;
			const float a = 1.f / sqrtf(3.f);
			const float b = 1.f / sqrtf(2.f);
			const float c = 2.f * a * sqrtf(2.f);

			for (unsigned k = 0; k < wh; k++) {
				// Y channel
				imTmp[k + red  ] = a * (im[k + red] + im[k + green] + im[k + blue]);

				// U channel
				imTmp[k + green] = b * (im[k + red] - im[k + blue]);

				// V channel
				imTmp[k + blue ] = c * (0.25f * im[k + red ] - 0.5f * im[k + green]
				                      + 0.25f * im[k + blue]);
			}
		}
		else { // chnls == 4
			const unsigned Gr = 0;
			const unsigned R  = wh;
			const unsigned B  = wh * 2;
			const unsigned Gb = wh * 3;
			const float a = 0.5f;
			const float b = 1.f / sqrtf(2.f);

			for (unsigned k = 0; k < wh; k++) {
				imTmp[k + Gr] = a * ( im[k + Gr] + im[k + R ] +
				                      im[k + B ] + im[k + Gb]);
				imTmp[k + R ] = b * ( im[k + R ] - im[k + B ]);
				imTmp[k + B ] = a * (-im[k + Gr] + im[k + R ] +
				                      im[k + B ] - im[k + Gb]);
				imTmp[k + Gb] = b * (-im[k + Gr] + im[k + Gb]);
			}
		}
	}
	// YUV to RGB
	else {
		if (chnls == 3) {
			const unsigned red   = 0;
			const unsigned green = wh;
			const unsigned blue  = wh * 2;
			const float a = 1.f / sqrtf(3.f);
			const float b = 1.f / sqrtf(2.f);
			const float c = a / b;

			for (unsigned k = 0; k < wh; k++) {
				// R channel
				imTmp[k + red  ] = a * im[k + red] + b * im[k + green]
				                               + c * 0.5f * im[k + blue];
				// G channel
				imTmp[k + green] = a * im[k + red] - c * im[k + blue];

				// B channel
				imTmp[k + blue ] = a * im[k + red] - b * im[k + green]
				                               + c * 0.5f * im[k + blue];
			}
		}
		else {	// chnls == 4
			const unsigned Gr = 0;
			const unsigned R  = wh;
			const unsigned B  = wh * 2;
			const unsigned Gb = wh * 3;
			const float a = 0.5f;
			const float b = 1.f / sqrtf(2.f);
			for (unsigned k = 0; k < wh; k++) {
				imTmp[k + Gr] = a * im[k + Gr] - a * im[k + B] - b * im[k + Gb];
				imTmp[k + R ] = a * im[k + Gr] + b * im[k + R] + a * im[k + B];
				imTmp[k + B ] = a * im[k + Gr] - b * im[k + R] + a * im[k + B];
				imTmp[k + Gb] = a * im[k + Gr] - a * im[k + B] + b * im[k + Gb];
			}
		}
	}

	im = imTmp;
}

void save_float_RGB(
	std::vector<float> const& im
,	const ImageSize &sz
,	char* filename)
{
	size_t dummy;

	FILE *file = fopen(filename, "wb");
	if (!file)
	{
		fprintf(stderr, "Error creating float RGB file %s\n!", filename);
		exit(EXIT_FAILURE);
	}

	dummy  = fwrite(&sz.width   , sizeof(sz.width   ), 1, file);
	dummy += fwrite(&sz.height  , sizeof(sz.height  ), 1, file);
	dummy += fwrite(&sz.channels, sizeof(sz.channels), 1, file);
	dummy = dummy; // prevents a warning

	if (verbose)
		printf("save RGB (%d, %d), %d channels\n", sz.width, sz.height, sz.channels);

	float* tmp = new float[sz.whc];
	for (unsigned k = 0; k < sz.whc; k++) tmp[k] = im[k];

	for (int ch = 0; ch < sz.channels; ch++)
		dummy = fwrite(tmp + sz.wh * ch, sizeof(float), sz.wh, file);

	delete[] tmp;
	fclose(file);
}

void load_float_RGB(
	std::vector<float> & im
,	ImageSize &sz
,	char* filename)
{
	size_t dummy;

	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr,"error opening float RGB file %s\n!", filename);
		exit(EXIT_FAILURE);
	}

	dummy  = fread(&sz.width   , sizeof(sz.width),    1, file);
	dummy += fread(&sz.height  , sizeof(sz.height),   1, file);
	dummy += fread(&sz.channels, sizeof(sz.channels), 1, file);
	dummy = dummy; // prevent a warning

	sz.wh = sz.width * sz.height;
	sz.whc = sz.wh * sz.channels;

	if (verbose)
		printf("load RGB (%dx%d), %d channels\n", sz.width, sz.height, sz.channels);

	float* tmp = new float[sz.whc];
	dummy = fread(tmp, sizeof(float), sz.whc, file);
	fclose(file);

	im.resize(sz.whc);
	for (int i = 0; i < sz.whc; i++) im[i] = tmp[i];

	delete [] tmp;
}


int main(int argc, char **argv)
{
	clo_usage("Image converter between standard types (IIO) and some ad-hoc types");
	clo_help("Supported ad-hoc types:\n"
			"\t.RGB\t\t: Miguel Colom's floating point type\n");

	// paths to input/output sequences
	using std::string;
	string  ipath  = clo_option("-i", "", "< input");
	string  opath  = clo_option("-o", "", "> output");
	bool    bayer4 = clo_option("-b4", false, "< transform Bayer pattern into 4 channel image");
	bool    bayer3 = clo_option("-b3", false, "< transform Bayer pattern into RGB image (no debayering)");

	// check inputs
	if (ipath == "")
	{
		fprintf(stderr, "%s: no input image.\nTry `%s --help' for more information.\n",
				argv[0], argv[0]);
		return EXIT_FAILURE;
	}

	if (bayer4 && bayer3)
		throw std::runtime_error("b4 and b3 options are incompatible");

	// declarations
	vector<float> im;
	ImageSize sz;

	char ipath_c[1024], opath_c[1024];
	sprintf(ipath_c, "%s", ipath.c_str());
	sprintf(opath_c, "%s", opath.c_str());

	// file extensions
	string itype = ipath.substr(ipath.find_last_of(".") + 1);
	string otype = opath.substr(opath.find_last_of(".") + 1);

	// load
	if (itype == "RGB")
		load_float_RGB(im, sz, ipath_c);
	else
		if (load_image_iio(ipath_c, im, sz) == EXIT_FAILURE)
			throw std::runtime_error("loading of " + ipath + " failed");

	if (sz.channels == 3)
	{
		fprintf(stderr, "Warning: 3 channel input image. Channels will be averaged!\n"); 

		// average all channels
		vector<float> im1(sz.wh);

		for (int y = 0, i = 0; y < sz.height; ++y)
		for (int x = 0       ; x < sz.width ; ++x, ++i)
			im1[i] = (im[i + 0*sz.wh]
					 +  im[i + 1*sz.wh]
					 +  im[i + 2*sz.wh])/3.f;
		im = im1;
		sz.channels = 1;
		sz.whc = sz.wh;
	}

	// process
	if (bayer4)
	{
		if ((sz.channels != 1) && (sz.channels != 3) && (sz.channels != 4))
			throw std::runtime_error("b4 option only works for 1 or 4 channel images");

		if (sz.channels == 1)
		{
			vector<float> im4(sz.whc);

			ImageSize sz4;
			sz4.width    = sz.width/2;
			sz4.height   = sz.height/2;
			sz4.channels = 4;
			sz4.wh       = sz.wh/4;
			sz4.whc      = sz.whc;

			for (int y = 0, i4 = 0; y < sz4.height; ++y)
			for (int x = 0        ; x < sz4.width ; ++x, ++i4)
			{
				im4[i4 + 0*sz4.wh] = im[(2*y + 0)*sz.width + 2*x + 1];//Gr
				im4[i4 + 1*sz4.wh] = im[(2*y + 0)*sz.width + 2*x + 0];//R
				im4[i4 + 2*sz4.wh] = im[(2*y + 1)*sz.width + 2*x + 1];//B
				im4[i4 + 3*sz4.wh] = im[(2*y + 1)*sz.width + 2*x + 0];//Gb
			}

			im = im4;
			sz.width    = sz4.width;
			sz.height   = sz4.height;
			sz.channels = sz4.channels;
			sz.wh       = sz4.wh;
			sz.whc      = sz4.whc;
		}
		else
		{
			vector<float> im4(sz.whc);

			ImageSize sz4;
			sz4.width    = 2*sz.width;
			sz4.height   = 2*sz.height;
			sz4.channels = 1;
			sz4.wh       = 4*sz.wh;
			sz4.whc      = sz.whc;

			for (int y = 0, i4 = 0; y < sz.height; ++y)
			for (int x = 0        ; x < sz.width ; ++x, ++i4)
			{
				im4[(2*y + 0)*sz4.width + 2*x + 1] = im[i4 + 0*sz.wh];//Gr
				im4[(2*y + 0)*sz4.width + 2*x + 0] = im[i4 + 1*sz.wh];//R
				im4[(2*y + 1)*sz4.width + 2*x + 1] = im[i4 + 2*sz.wh];//B
				im4[(2*y + 1)*sz4.width + 2*x + 0] = im[i4 + 3*sz.wh];//Gb
			}

			im = im4;
			sz.width    = sz4.width;
			sz.height   = sz4.height;
			sz.channels = sz4.channels;
			sz.wh       = sz4.wh;
			sz.whc      = sz4.whc;
		}
	}
	else if(bayer3)
	{
		if ((sz.channels != 1) && (sz.channels != 4))
			throw std::runtime_error("b3 option only works for 1 or 4 channel images");

		if (sz.channels == 1)
		{
			vector<float> im3(sz.wh*3);

			ImageSize sz3;
			sz3.width    = sz.width/2;
			sz3.height   = sz.height/2;
			sz3.channels = 3;
			sz3.wh       = sz.wh/4;
			sz3.whc      = sz.wh*3;

			for (int y = 0, i3 = 0; y < sz3.height; ++y)
			for (int x = 0        ; x < sz3.width ; ++x, ++i3)
			{
				im3[i3 + 0*sz3.wh] = im[(2*y + 0)*sz.width + 2*x + 0];//R
				im3[i3 + 1*sz3.wh] = im[(2*y + 0)*sz.width + 2*x + 1]*0.5 //.5(Gr+Gb)
				                   + im[(2*y + 1)*sz.width + 2*x + 0]*0.5;//.5(Gr+Gb)
				im3[i3 + 2*sz3.wh] = im[(2*y + 1)*sz.width + 2*x + 1];//B
			}

			im = im3;
			sz.width    = sz3.width;
			sz.height   = sz3.height;
			sz.channels = sz3.channels;
			sz.wh       = sz3.wh;
			sz.whc      = sz3.whc;
		}
		else
		{
			vector<float> im3(sz.wh*3);

			for (int y = 0, i = 0; y < sz.height; ++y)
			for (int x = 0        ; x < sz.width ; ++x, ++i)
			{
				im3[i + 0*sz.wh] = im[i + 1*sz.wh];//R
				im3[i + 1*sz.wh] = im[i + 0*sz.wh]*0.5//0.5(Gb + Gr)
				                 + im[i + 3*sz.wh]*0.5;
				im3[i + 2*sz.wh] = im[i + 2*sz.wh];//B
			}

			im = im3;
			sz.channels = 3;
			sz.whc      = sz.wh*3;
		}
	}

	// save
	if (otype == "RGB")
		save_float_RGB(im, sz, opath_c);
	else
		if (save_image_iio(opath_c, im, sz) == EXIT_FAILURE)
			throw std::runtime_error("saving of " + ipath + " failed");

	return EXIT_SUCCESS;
}
