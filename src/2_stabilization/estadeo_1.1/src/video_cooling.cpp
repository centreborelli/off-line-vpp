// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD Lcense. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.


#include "video_cooling.h"
#include "direct_method/transformation.h"
#include "sutherland_hodgman.h"


#include <stdio.h>
#include <stack>

/**
  *
  * Computes the inscribed rectangle by updating the corners
  * according to the projection of the image corners
  *
**/
void fast_rectangle(
  float *H,    //set of transformations to be cropped
  int nx,      //number of columns
  int ny,      //number of rows
  int nframes, //number of frames
  int nparams, //type of transformation
  float &x1,   //x coordinate of top-left corner
  float &y1,   //y coordinate of top-left corner
  float &x2,   //x coordinate of bottom-right corner
  float &y2    //y coordinate of bottom-right corner
)
{  
  float *H_1=new float[nparams];
  
  x1=y1=0; x2=nx; y2=ny;
  
  //find the inside common rectangle
  for(int i=0;i<nframes;i++)
  {
    inverse_transform(&(H[i*nparams]), H_1, nparams);
    float xp, yp;
    
    //project the image corners and store the limits
    project(0,0,H_1,xp,yp,nparams);
    if(xp>x1) x1=xp;
    if(yp>y1) y1=yp;

    project(nx,0,H_1,xp,yp,nparams);
    if(xp<x2) x2=xp;
    if(yp>y1) y1=yp;
    
    project(0,ny,H_1,xp,yp,nparams);
    if(xp>x1) x1=xp;
    if(yp<y2) y2=yp;
    
    project(nx,ny,H_1,xp,yp,nparams);
    if(xp<x2) x2=xp;
    if(yp<y2) y2=yp;
  }
  
  delete []H_1;
}



/**
  *
  * Find the intersecting polygon to all the homographies 
  *
**/
poly find_intersecting_polygon(
  float *H,    //set of transformations to be cropped
  int nx,      //number of columns
  int ny,      //number of rows
  int nframes, //number of frames
  int nparams  //type of transformation
)
{
  float *H_1=new float[nparams];
  
  poly clipper=poly_new();
  clipper->len=4;
  clipper->alloc=0;
  clipper->v=new vec_t[4];
  clipper->v[0].x=0 ; clipper->v[0].y=0; 
  clipper->v[1].x=nx; clipper->v[1].y=0;
  clipper->v[2].x=nx; clipper->v[2].y=ny; 
  clipper->v[3].x=0 ; clipper->v[3].y=ny;

  //find the inside common rectangle
  for(int i=0;i<nframes;i++)
  {
    inverse_transform(&(H[i*nparams]), H_1, nparams);
    
    //project the image corners and store the limits
    vec_t s[4]; 
    project(0, 0, H_1,s[0].x,s[0].y,nparams);
    project(nx,0, H_1,s[1].x,s[1].y,nparams);
    project(nx, ny,H_1,s[2].x,s[2].y,nparams);
    project(0,ny,H_1,s[3].x,s[3].y,nparams);
    
    poly_t subject = {4, 0, s};
    
    //compute the intersection of both polygons using sutherland-hodgman method
    poly res=poly_clip(&subject, clipper);
    
    poly_free(clipper);
    clipper=res;
  }

  delete []H_1;
  
  return clipper;
}


/**
  *
  * Create a binary image with 1 inside the common
  * region and 0 in the empty regions 
  *
**/
void create_binary_image(
  poly polygon, //intersecting polygon
  int  *I,      //output binary image with polygon mask
  int  nx,      //number of columns
  int  ny       //number of rows
)
{
  for(int i=0; i<nx*ny; i++) I[i]=0;
  
  float ymin=ny, ymax=0;
  
  //find the y-limits of the polygon
  for(int i=0; i<polygon->len; i++)
  {
    if(ymin>polygon->v[i].y) ymin=polygon->v[i].y;
    if(ymax<polygon->v[i].y) ymax=polygon->v[i].y;
  }
  
  //intersect with the polygon all the rows in between and fill
  for(int y=(int)(ymin+0.5); y<(int)(ymax+0.5); y++)
  {
    float xmin=-1, xmax=-1;

    int i=0;
    int j=polygon->len-1;
    int intersections=0;
    
    //find the intersection of the current row with the polygon
    //since the polygon is convex we need to find two contacts
    while(i<polygon->len && intersections<2)
    {
      //intersection test
      if((polygon->v[j].y>=y && polygon->v[i].y<=y)||
         (polygon->v[j].y<=y && polygon->v[i].y>=y))
      {
        float x1=polygon->v[j].x;
        float y1=polygon->v[j].y;
        float x2=polygon->v[i].x;
        float y2=polygon->v[i].y;
        
        //if line is coincident, fill it completely
        if((int)(y1+0.5)==(int)(y2+0.5))
        {
          xmin=x1; xmax=x2;
          intersections=2;
        }
        else if(intersections==0)
          xmin=(y-y1)*(x2-x1)/(y2-y1)+x1;
        else
          xmax=(y-y1)*(x2-x1)/(y2-y1)+x1;

        intersections++;
      }
      
      j=i;
      i++;
    }
        
    //swap x limits if not ordered
    if(xmin>xmax)
    {
      float tmp=xmin;
      xmin=xmax;
      xmax=tmp;
    }
    
    //fill the row 
    if(xmin>=0 && xmin<nx && xmax>=0 && xmax<nx)
      for(int x=(int)xmin; x<=(int)xmax; x++) I[y*nx+x]=255;
  }
}


/**
  *
  * Find the rectangle from the binary image
  * Method of maximum rectangle beneath a histogram
  *
**/
void find_rectangle(
  int *I,    //binary image with polygon mask
  int nx,    //number of columns of the image
  int ny,    //number of rows of the image
  float &x1, //computed x coordinate of top-left corner of the rectangle
  float &y1, //computed y coordinate of top-left corner of the rectangle
  float &x2, //computed x coordinate of bottom-right corner of the rectangle
  float &y2  //computed y coordinate of bottom-right corner of the rectangle
)
{
  //initialize histogram array to 0
  int *H=new int[nx*ny]();

  //create the histogram array
  for(int i=1; i<ny; i++)
    for(int j=0; j<nx; j++)
      if(I[i*nx+j])
        H[i*nx+j]=1+H[(i-1)*nx+j];
      
  long int max_area = 0;

  //find the maximum area axis-parallel rectangle 
  for(int i=1; i<ny; i++)
  { 
    int *L=new int[nx]();
    int *R=new int[nx]();
    std::stack<int> stack; 

    //find the left limit for this position of the histogram
    for(int j=0; j<nx; j++)
    {
      while (!stack.empty() && H[i*nx+j]<=H[i*nx+stack.top()])
        stack.pop();
      
      int t;
      if(stack.empty())
        t=0;
      else
        t=stack.top();
      
      L[j]=t;
      stack.push(j);
    }

    //empty stack 
    std::stack<int> empty;
    std::swap(stack, empty);

    //find the right limit for this position of the histogram
    for (int j=nx-1; j>=0; j--)
    {
      while(!stack.empty() && H[i*nx+j]<=H[i*nx+stack.top()])
        stack.pop();

      int t;
      if(stack.empty())
        t=nx-1;
      else
        t=stack.top();
      
      R[j]=t;
      stack.push(j);
    }

    //calculate final area in the row and find the maximum
    for (int j=0; j<nx; j++)
    {
      long int A=H[i*nx+j]*(R[j]-L[j]);
      if (A>max_area)
      {
        max_area=A;
        x1=L[j]+1;
        y1=i-H[i*nx+j];
        x2=R[j];
        y2=i;
      }
    }
    delete []L;
    delete []R;
  }

  delete []H;
}


/**
  *
  * Find the optimal inscribed rectangle 
  *
**/
void accurate_rectangle(
  float *H,    //set of transformations to be cropped
  int nx,      //number of columns
  int ny,      //number of rows
  int nframes, //number of frames
  int nparams, //type of transformation
  float &x1,   //x coordinate of top-left corner
  float &y1,   //y coordinate of top-left corner
  float &x2,   //x coordinate of bottom-right corner
  float &y2,   //y coordinate of bottom-right corner
  int verbose  //verbose mode
)
{
  if(verbose)
    printf("   Calculating intersection polygon\n");
  poly polygon=find_intersecting_polygon(H,nx,ny,nframes,nparams);
  
  int *I=new int[nx*ny];
  
  if(verbose)
    printf("   Creating binary image with polygon mask\n");
  create_binary_image(polygon, I, nx, ny);
  
  if(verbose)
    printf("   Computing the inscribed rectangle\n");
  find_rectangle(I, nx, ny, x1, y1, x2, y2);  
  
  poly_free(polygon);
  delete []I;
}

/**
  *
  * Computes the transformation neccessary to remove the 
  * empty regions (similarity: translate + scale)
  *
**/
void apply_crop_zoom_transform(
  float *H,
  int nx,
  int ny,
  int nframes,
  int nparams,
  float x1,
  float y1,
  float x2, 
  float y2,
  int verbose
)
{
  //compute necessary zoom and translation to apply to the transformations
  float s, s1=(y2-y1)/ny;
  float s2=(x2-x1)/nx;

  if(s1<s2)
    s=s1;
  else
    s=s2;

  //translate the center of the rectangle to the origin
  float dx=(x1+x2)/2;
  float dy=(y1+y2)/2;
  
  if(verbose)
  {
    printf("   -Crop rectangle: (%f, %f)-(%f, %f)\n", x1, y1, x2, y2);
    printf("   -Transformation: zoom=%f, translation=(%f, %f)\n", s, dx, dy);
  }
  
  //create an Euclidean transform for composition
  float matrix[9]={s,0,dx-s*dx,0,s,dy-s*dy,0,0,1};
  float *T=new float[nparams];
  
  matrix2params(matrix,T,nparams);
  
  //modify the smoothing transformations
  for(int i=0;i<nframes;i++) 
    compose_transform(&(H[i*nparams]), T, &(H[i*nparams]),nparams);
  
  delete []T;

}


/**
  *
  * Function for dealing with empty regions after stabilization
  *
**/
void video_cooling(
  float *H,    //set of transformations to be cropped
  int nx,      //number of columns
  int ny,      //number of rows
  int nframes, //number of frames
  int nparams, //type of transformation
  int type,    //type of filling strategy
  int verbose  //verbose mode
)
{
  //test if the transformations has enough degrees of freedom
  if(nparams<=3)
  {
    if(verbose)
    {
      printf("   Cannot apply Crop&Zoom to translation "
             "or Euclidean transforms\n");
    }
    
    return;
  }

  //coordinates of the cropping rectangle
  float x1, y1, x2, y2;
  
  //calculating the cropping rectangle following one strategy
  switch(type)
  {
    case NO_POSTPROCESS:
      if(verbose)
        printf("  No post-processing:\n");
      break;
    case FAST_CROP_ZOOM:
      if(verbose)
        printf("  Fast Crop and Zoom:\n");
      fast_rectangle(H, nx, ny, nframes, nparams, x1, y1, x2, y2);
      //compute the new transformation for cropping and zooming
      apply_crop_zoom_transform(H, nx, ny, nframes, nparams, x1, y1, x2, y2, verbose);
      break;
    case CROP_ZOOM:
      if(verbose)
        printf("  Crop and Zoom:\n");
      accurate_rectangle(H, nx, ny, nframes, nparams, x1, y1, x2, y2, verbose);
      //compute the new transformation for cropping and zooming
      apply_crop_zoom_transform(H, nx, ny, nframes, nparams, x1, y1, x2, y2, verbose);
      break;
    case NEIGHBOR_INPAINT:
      if(verbose)
        printf("  Neighbor inpainting (not implemented yet):\n");
      break;
  }
}
