#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
//#include "LibVideoT.hpp"

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
);

size_t read_video(
  char *name,       //file name
  int first,
  int last,
  float *I, //video to read
  int size          //size of the video
);

size_t write_video(
  char *name,       //file name
  int first,
  float *I, //video to write
  int width,
  int height,
  int frames
);


void save_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int nx,          //image width
  int ny           //image height
);

void read_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int &nx,         //image width
  int &ny          //image height
);


#endif
