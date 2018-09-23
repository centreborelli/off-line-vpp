// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "estadeo.h"
#include "video_cooling.h"
#include "motion_smoothing.h"
#include "utils.h"
#include "color_bicubic_interpolation.h"
#include "direct_method/inverse_compositional_algorithm.h"
#include "direct_method/transformation.h"
#include "feature_based_method/orsa.h"


/**
  *
  *  Function to convert an rgb image to grayscale levels
  * 
**/
void rgb2gray(
  float *rgb,  //input color image
  float *gray, //output grayscale image
  int nx,       //number of pixels
  int ny, 
  int nz
)
{
  int size=nx*ny;
  if(nz>=3)
    //#pragma omp parallel for
    for(int i=0;i<size;i++)
      gray[i]=(0.2989*rgb[i*nz]+0.5870*rgb[i*nz+1]+0.1140*rgb[i*nz+2]);
  else
    //#pragma omp parallel for
    for(int i=0;i<size;i++)
      gray[i]=rgb[i];
}


/**
  *
  * Function for estimating the transformation between frames
  *
**/
void motion_estimation
(
  float *I,        //input video to estabilize
  float *H,        //output matrix transformations
  int motion_type, //use direct or feature-based method
  int nparams,     //type of matrix transformation
  int nx,          //number of columns
  int ny,          //number of rows
  int nz,          //number of channels
  int nframes,     //number of frames of the video
  int verbose      //verbose mode
)
{ 
  //introduce identity matrix in the first transform
  for(int i=0;i<nparams;i++) H[i]=0;

  int size=nx*ny*nz;

  if(motion_type==DIRECT_METHOD)
  {
    if(verbose) printf("  Direct method: ");

    //parameters for the direct method
    int nscales=10;
    float zfactor=0.5;
    float TOL=1E-3;
    float lambda=0;
    float N;
    int robust=LORENTZIAN;

    N=1+log(((nx<ny)?nx:ny)/50.)/log(1./zfactor);
    if ((int) N<nscales) nscales=(int) N;

    //motion estimation through direct methods
    #pragma omp parallel for
    for(int i=0;i<nframes-1;i++) 
    {
      if(verbose) 
      {
        printf("%d, ",i);
        fflush(stdout);
      }

      float *I1=new float[nx*ny];
      float *I2=new float[nx*ny];
      
      rgb2gray(&I[size*i], I1, nx, ny, nz);
      rgb2gray(&I[size*(i+1)], I2, nx, ny, nz);
      
      pyramidal_inverse_compositional_algorithm(
        I1, I2, &(H[(i+1)*nparams]), nparams, 
        nx, ny, nscales, zfactor, TOL, robust, lambda, 0
      );
      
      delete []I1;
      delete []I2;
    }
  }
  else    
  {
    if(verbose) printf("  Feature-base method: ");

    //motion estimation through feature based methods
    #pragma omp parallel for
    for(int i=0;i<nframes-1;i++)
    {
      if(verbose) 
      {
        printf("%d, ",i);
        fflush(stdout);
      }
    
      orsa_homography(
        &I[size*i], &I[size*(i+1)], &(H[(i+1)*nparams]), nx, ny
      );
    }
  }
  
  if(verbose) printf("\n");
}




/**
  *
  * Function for warping the frames of the input video
  *
**/
void warp_video
(
  float *I,    //video to be warped
  float *H,    //matrix transformations
  int nparams, //type of matrix transformation
  int nx,      //number of columns   
  int ny,      //number of rows
  int nz,      //number of channels
  int nframes, //number of frames of the video
  int border,  //do not fill the border
  int verbose  //verbose mode
)
{
  int size=nx*ny*nz;
  
  if(verbose)
    printf("  ");
  
  //variable used to allow empty borders
  bool empty_border;
  if(border) empty_border=false;
  else empty_border=true;
    
  //warp the frames
  #pragma omp parallel for
  for(int i=0;i<nframes;i++)
  {
    if(verbose)
    {
      printf("%d, ", i);
      fflush(stdout);
    }
    
    float *I2=new float[nx*ny*nz];
    
    //warp the image
    bicubic_interpolation(
      &(I[size*i]), I2, &(H[i*nparams]), nparams, nx, ny, nz, empty_border
    );
    
    //convert image from float to unsigned char
    for(int j=0; j<size; j++)
      I[size*i+j]=I2[j];

    delete []I2;
  }

  if(verbose) printf("\n");
}



/**
  *
  * Function for video estabilization
  *
**/
void estadeo(
  float *I,            //input video to estabilize
  int nx,              //number of columns 
  int ny,              //number of rows
  int nz,              //number of channels
  int nframes,         //number of frames of the video
  int motion_type,     //use direct or feature-based method to estimate motion
  int nparams,         //type of matrix transformation
  int smooth_strategy, //motion smoothing strategy
  float sigma,         //radius for smoothing the motion
  int bc,              //boundary condition for smoothing
  int postprocessing,  //method for dealing with empty regions
  char *in_transform,  //input file with the computed transformations
  char *out_transform, //output file to write the computed transformations
  char *out_stransform,//output file to write the stabilizing transformations
  int verbose          //verbose mode
)
{
  float *Hmotion=new float[nframes*nparams];
  float *Hsmooth=new float[nframes*nparams];
  
  //--------------------------------
  //motion estimation between frames
  //--------------------------------
  if(in_transform==NULL)
  {
    if(verbose)
      printf("\n 1-Motion estimation:\n");
    
    //compute the motion between consecutive frames
    motion_estimation(
      I, Hmotion, motion_type, nparams, 
      nx, ny, nz, nframes, verbose
    );
    
    //save the original transformation
    if(out_transform!=NULL)
    {
      if(verbose)
        printf("\n  Write transformations to file '%s'\n", out_transform);

      save_transforms(out_transform, Hmotion, nparams, nframes, nx, ny);
    }

  }
  else
  {
    //load transformations from the input file
    if(verbose)
      printf(" 1-Loading motion parameters from '%s':\n", in_transform);
    
    read_transforms(in_transform, Hmotion, nparams, nframes, nx ,ny);
  }
  
  //----------------------------------------------------------------  
  //smooth the transformations and obtain the stabilizing parameters
  //----------------------------------------------------------------
  if(verbose)
    printf("\n 2-Motion smoothing:\n");
  
  motion_smoothing(
    Hmotion, Hsmooth, nparams, nframes, 
    sigma, smooth_strategy, bc, verbose
  );
  
  //save the stabilizing transformation 
  if(out_stransform!=NULL)
  {
    if(verbose)
      printf("\n  Write smoothed transformations to file '%s'\n", 
             out_stransform);
    
    float *Ho=new float[nframes*nparams];
    compute_smooth_transforms(Hmotion, Hsmooth, Ho, nparams, nframes);
    save_transforms(out_stransform, Ho, nparams, nframes, nx, ny);
    delete []Ho;
  }
   
  
  
  //---------------------------------------------
  //postprocessing step for removing empty spaces
  //---------------------------------------------
  if(verbose)
    printf("\n 3-Postprocessing:\n");
  
  video_cooling(
    Hsmooth, nx, ny, nframes, nparams, 
    postprocessing, verbose
  );
  
  
  //-------------------------------------------------------
  //warp the input video to produce the stabilized sequence
  //-------------------------------------------------------
  if(verbose)
    printf("\n 4-Video warping:\n");
  
  warp_video(
    I, Hsmooth, nparams, nx, ny, nz, 
    nframes, postprocessing, verbose
  );
  
  delete []Hmotion;
  delete []Hsmooth;
}
