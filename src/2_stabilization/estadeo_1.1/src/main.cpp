// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.

#include "estadeo.h"
#include "motion_smoothing.h"
#include "video_cooling.h"
#include "utils.h"
#include "direct_method/transformation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAR_DEFAULT_OUTVIDEO "output_video.raw"
#define PAR_DEFAULT_MOTION DIRECT_METHOD
#define PAR_DEFAULT_TRANSFORM SIMILARITY_TRANSFORM
#define PAR_DEFAULT_SMOOTHING LOCAL_MATRIX_BASED_SMOOTHING
#define PAR_DEFAULT_SIGMA 30
#define PAR_DEFAULT_BC NEUMANN_BC
#define PAR_DEFAULT_POSTPROCESS NO_POSTPROCESS
#define PAR_DEFAULT_OUTTRANSFORM "transform.mat"
#define PAR_DEFAULT_INTRANSFORM "in_transform.mat"
#define PAR_DEFAULT_VERBOSE 0


/**
 *
 *  Print a help message 
 *
 */
void print_help(char *name)
{
  printf("\n  Usage: %s raw_input_video width height nframes [OPTIONS] \n\n",
          name);
  printf("  Video stabilization program:\n");
  printf("  'raw_input_video' is a video file in raw format (rgb24).\n");
  printf("  'width' is the width of the images in pixels.\n");
  printf("  'height' is the height of the images in pixels.\n");
  printf("  'nframes' is the number of frames in the video.\n");
  printf("  -----------------------------------------------\n");
  printf("  Converting to raw data:\n");
  printf("  'avconv -i video.mp4 -f rawvideo -pix_fmt rgb24 -y "
         "raw_video.raw'\n");
  printf("  to convert an mp4 video to raw format.\n");
  printf("  'avconv -f rawvideo -pix_fmt rgb24 -video_size 640x360 "
         "-framerate\n");
  printf("  30 -i output_video.raw -pix_fmt yuv420p output_video.mp4'\n");
  printf("  to convert a raw video to mp4 format.\n");
  printf("  -----------------------------------------------\n");
  printf("  More information in http://www.ipol.im \n\n");
  printf("  OPTIONS:\n"); 
  printf("  --------\n");
  printf("   -o name  output video name to write the computed raw video\n");
  printf("              default value '%s'\n", PAR_DEFAULT_OUTVIDEO);
  printf("   -m N     motion estimation method\n");
  printf("              0-direct method; 1-feature based method\n");
  printf("              default value %d\n", PAR_DEFAULT_MOTION);
  printf("   -t N     transformation type to be computed:\n");
  printf("              2-traslation; 3-Euclidean transform;\n");
  printf("              4-similarity; 6-affinity; 8-homography\n"); 
  printf("              default value %d\n", PAR_DEFAULT_TRANSFORM);
  printf("   -s N     motion smoothing strategy:\n");
  printf("              0-pure composition;\n");
  printf("              1-compositional smoothing;\n");
  printf("              2-compositional local smoothing; \n");
  printf("              3-local matrix-based smoothing;\n");
  printf("              4-local point-based smoothing\n");
  printf("              5-local linear matrix-based smoothing\n");
  printf("              6-local linear point-based smoothing\n");
  printf("              default value %d\n", PAR_DEFAULT_SMOOTHING);
  printf("   -r N     Gaussian standard deviation for smoothing\n");
  printf("              default value %d\n", PAR_DEFAULT_SIGMA);
  printf("   -b N     type of boundary condition: \n");
  printf("              0-constant; 1-neumann; 2-dirichlet\n");
  printf("              default value %d\n", PAR_DEFAULT_BC);
  printf("   -p N     video postprocessing \n");
  printf("              0-no postprocessing; 1-fast crop&zoom; 2-crop&zoom\n");
  printf("              default value %d\n", PAR_DEFAULT_POSTPROCESS);
  printf("   -w name  write transformations to file\n");
  printf("   -l name  load transformations from file\n");
  printf("   -f name  write stabilizing transformations to file\n");
  printf("   -v       switch on verbose mode \n\n\n");
}

/**
 *
 *  Read command line parameters 
 *
 */
int read_parameters(
  int   argc, 
  char  *argv[], 
  char  **video_in,
  int   &first,
  int   &last,
  char  *video_out,
  char  **out_transform,
  char  **in_transform,
  char  **out_smooth_transform,
  int   &width,
  int   &height,
  int   &nframes,
  int   &motion_type,
  int   &nparams,
  int   &smooth_strategy,
  float &sigma,
  int   &boundary_condition, 
  int   &postprocessing,
  int   &verbose
)
{
  if (argc < 5){
    print_help(argv[0]); 
    return 0;
  }
  else{
    int i=1;
    *video_in=argv[i++];
    first=atoi(argv[i++]);
    last=atoi(argv[i++]);
    *in_transform=NULL;
    *out_transform=NULL;
    *out_smooth_transform=NULL;
    width=atoi(argv[i++]);
    height=atoi(argv[i++]);
    nframes=atoi(argv[i++]);

    //assign default values to the parameters
    strcpy(video_out,PAR_DEFAULT_OUTVIDEO);
    motion_type=PAR_DEFAULT_MOTION;
    nparams=PAR_DEFAULT_TRANSFORM;
    smooth_strategy=PAR_DEFAULT_SMOOTHING;
    sigma=PAR_DEFAULT_SIGMA;
    boundary_condition=PAR_DEFAULT_BC;
    postprocessing=PAR_DEFAULT_POSTPROCESS;
    verbose=PAR_DEFAULT_VERBOSE;
    
    //read each parameter from the command line
    while(i<argc)
    {
      if(strcmp(argv[i],"-o")==0)
        if(i<argc-1)
          strcpy(video_out,argv[++i]);
      
      if(strcmp(argv[i],"-m")==0)
        if(i<argc-1)
          motion_type=atoi(argv[++i]);

      if(strcmp(argv[i],"-t")==0)
        if(i<argc-1)
          nparams=atof(argv[++i]);

      if(strcmp(argv[i],"-s")==0)
        if(i<argc-1)
          smooth_strategy=atoi(argv[++i]);

      if(strcmp(argv[i],"-r")==0)
        if(i<argc-1)
          sigma=atof(argv[++i]);
        
      if(strcmp(argv[i],"-b")==0)
        if(i<argc-1)
          boundary_condition=atoi(argv[++i]);

      if(strcmp(argv[i],"-p")==0)
        if(i<argc-1)
          postprocessing=atoi(argv[++i]);

      if(strcmp(argv[i],"-w")==0)
        if(i<argc-1)
          *out_transform=argv[++i];

      if(strcmp(argv[i],"-l")==0)
        if(i<argc-1)
          *in_transform=argv[++i];

      if(strcmp(argv[i],"-f")==0)
        if(i<argc-1)
          *out_smooth_transform=argv[++i];

      if(strcmp(argv[i],"-v")==0)
        verbose=1;
      
      i++;
    }

    //check parameter values
    if(motion_type<0 || motion_type>1)
      motion_type=PAR_DEFAULT_MOTION;
    if(nparams!=2 && nparams!=3 && nparams!=4 && 
      nparams!=6 && nparams!=8) nparams=PAR_DEFAULT_TRANSFORM;
    if(smooth_strategy<0 || smooth_strategy>N_SMOOTH_METHODS)
      smooth_strategy=PAR_DEFAULT_SMOOTHING;
    if(sigma<1)
      sigma=PAR_DEFAULT_SIGMA;
    if(boundary_condition<0 || boundary_condition>2)
      boundary_condition=PAR_DEFAULT_BC;
    if(postprocessing<0 || postprocessing>2)
      postprocessing=PAR_DEFAULT_POSTPROCESS;
    if(motion_type==1) nparams=HOMOGRAPHY_TRANSFORM;
  }

  return 1;
}



/**
 *
 *  Main program:
 *   This program reads the parameters from the console and
 *   then call the video stabilization method called 'estadeo'
 *
 */
int main (int argc, char *argv[])
{
  //parameters of the method
  char *video_in, video_out[300];
  char *out_transform, *in_transform, *out_smooth_transform;
  int  width, height, nchannels=3, nframes;
  int  motion_type, nparams, smooth_strategy; 
  int  boundary_condition, postprocessing;
  int  verbose;
  float sigma;
  int first, last;
  
  //read the parameters from the console
  int result=read_parameters(
        argc, argv, &video_in, first, last, video_out, &out_transform, &in_transform, 
        &out_smooth_transform, width, height, nframes, motion_type, nparams,
        smooth_strategy, sigma, boundary_condition, postprocessing, verbose
      );

  if(result)
  {

    // default values for output size (equal to input size)
    if (nframes < 0) nframes = last - first + 1;
    if (width < 0 || height < 0 || nchannels < 0)
    {
      size_t r=get_frame_size(video_in, first, width, height, nchannels);
      nchannels = 3; // FIXME very ugly
    }

    if(verbose)
      printf(
        " Input video: '%s'\n Output video: '%s'\n Width: %d, Height: %d, "
        " Number of frames: %d\n Motion type: %d, Transformation: %d \n" 
        " Smooth strategy: %d, Sigma: %f, BC: %d \n Postprocess: %d\n",
        video_in, video_out, width, height, nframes, motion_type, nparams,
        smooth_strategy, sigma, boundary_condition, postprocessing
      );

    // allocate video
    int size=width*height*nchannels*nframes;
    //unsigned char *I=new unsigned char[size];
    float *If=new float[size];

    size_t r=read_video(video_in, first, last, If, size);

    if(r<=0)
    {
      fprintf(stderr, "Error: Cannot read the input video '%s'.\n", video_in);
      return EXIT_FAILURE;
    }

    if(verbose) 
      printf(" Size of video in bytes %d\n", (int) r);

    //convert the input video to float
    //float *If=new float[size];
    //for(int i=0; i<size; i++)
    //  If[i]=(float)I[i];

    if(verbose)
      printf("\n Starting the stabilization\n");

    //call the method for video stabilization
    estadeo(
      If, width, height, nchannels, nframes, motion_type, nparams, 
      smooth_strategy, sigma, boundary_condition, postprocessing, 
      in_transform, out_transform, out_smooth_transform, verbose
    );
    
    //convert the stabilized video to unsigned char
    //for(int i=0; i<size; i++)
    //{
    //  if(If[i]<0)
    //    I[i]=0;
    //  else if(If[i]>255)
    //    I[i]=255;
    //  else I[i]=(unsigned char)If[i];
    //}
    
    if(verbose)
      printf("\n Writing the output video to '%s'\n", video_out);

    write_video(video_out, first, If, width, height, nframes);
    
    //delete []I;
    delete []If;
  }

  return EXIT_SUCCESS;
}
