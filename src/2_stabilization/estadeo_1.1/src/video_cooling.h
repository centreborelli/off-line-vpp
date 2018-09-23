#ifndef VIDEO_COOLING
#define VIDEO_COOLING

#define NO_POSTPROCESS 0
#define FAST_CROP_ZOOM 1
#define CROP_ZOOM 2
#define NEIGHBOR_INPAINT 3

void video_cooling(
  float *H,    //set of transformations to be cropped
  int nx,      //number of columns
  int ny,      //number of rows
  int nframes, //number of frames
  int nparams, //type of transformation
  int type,    //type of filling strategy
  int verbose  //verbose mode
);

#endif
