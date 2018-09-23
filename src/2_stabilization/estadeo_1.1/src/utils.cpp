// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.


#include <stdio.h>
#include "LibVideoT.hpp"

/**
  *
  *  Function to get the frame size
  * 
**/
size_t get_frame_size(
  char *name,       //file name
  int first,        //first frame number
  int &width,       //size of the video
  int &height,      //size of the video
  int &channels     //size of the video
) 
{
	Video<float> input;
	input.loadVideo(name, first, first);

	if(input.sz.channels != 1)
	{
		printf("Requires only 1 channel for this version\n");
		return 0;
	}

	width = input.sz.width;
	height = input.sz.height;
	channels = input.sz.channels;

	return width * height * channels;
}


/**
  *
  *  Function to read a video in raw data
  * 
**/
size_t read_video(
  char *name,       //file name
  int first,
  int last,
  float *I, //video to read
  int size          //size of the video
) 
{
	Video<float> input;
	input.loadVideo(name, first, last);

	if(input.sz.channels != 1)
	{
		printf("Requires only 1 channel for this version\n");
		return 0;
	}
	int it = 0;
	for(int i = 0; i < input.sz.width*input.sz.height*input.sz.frames; ++i)
	{
		I[it]   = input(i);
		I[it+1] = input(i);
		I[it+2] = input(i);
		it+=3;
	}
	return it;
}


/**
  *
  *  Function to read a video in raw data
  * 
**/
size_t write_video(
  char *name,       //file name
  int first,
  float *I, //video to write
  int width,
  int height,
  int frames
) 
{
	Video<float> output(width, height, frames, 1);

	int it = 0;
	for(int i = 0; i < width*height*frames; ++i)
	{
		output(i) = I[it];
		it+=3;
	}

	output.saveVideo(name, first);
	return it;
}


/**
  *
  *  Function to save transformations to a file
  * 
**/
void save_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int nx,          //image width
  int ny           //image height
)
{
  FILE *fd=fopen(name,"w");
  if(fd!=NULL)
  {
    fprintf(fd,"%d %d %d %d\n", nparams, ntransforms, nx, ny);

    for(int i=0;i<ntransforms;i++)
    {
      for(int j=0;j<nparams;j++) fprintf(fd, "%.15f ", H[i*nparams+j]);
      fprintf(fd, "\n");
    }
    fclose(fd);
  }
}


/**
  *
  *  Function to read transformations from a file
  * 
**/
void read_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int &nx,         //image width
  int &ny          //image height
)
{
  FILE *fd=fopen(name,"r");
  if(fd!=NULL)
  {
    int r=fscanf(fd,"%d %d %d %d", &nparams, &ntransforms, &nx, &ny);
    if(r>0)
      for(int i=0;i<ntransforms;i++) {
	for(int j=0;j<nparams;j++) r=fscanf(fd, "%f", &(H[i*nparams+j]));
      }
    fclose(fd);
  }
}
