// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2015, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// Copyright (C) 2014, Nelson Monzón López <nmonzon@ctim.es>
// All rights reserved.


#include "color_bicubic_interpolation.h"
#include "direct_method/bicubic_interpolation.h"
#include "direct_method/transformation.h"

/**
  *
  * Compute the bicubic interpolation of a point in an image. 
  * Detects if the point goes outside the image domain
  *
**/
float
bicubic_interpolation(
  float *input, //image to be interpolated
  float uu,     //x component of the vector field
  float vv,     //y component of the vector field
  int nx,       //width of the image
  int ny,       //height of the image
  int nz,       //number of channels of the image
  int k,        //actual channel
  bool border_out //if true, put zeros outside the region
)
{
  int sx = (uu < 0) ? -1 : 1;
  int sy = (vv < 0) ? -1 : 1;

  int x, y, mx, my, dx, dy, ddx, ddy;
  bool out = false;

  x = neumann_bc ((int) uu, nx, out);
  y = neumann_bc ((int) vv, ny, out);
  mx = neumann_bc ((int) uu - sx, nx, out);
  my = neumann_bc ((int) vv - sx, ny, out);
  dx = neumann_bc ((int) uu + sx, nx, out);
  dy = neumann_bc ((int) vv + sy, ny, out);
  ddx = neumann_bc ((int) uu + 2 * sx, nx, out);
  ddy = neumann_bc ((int) vv + 2 * sy, ny, out);

  if (out && border_out) 
    return 0;
  else
    {
      //obtain the interpolation points of the image
      float p11 = input[(mx  + nx * my) * nz + k];
      float p12 = input[(x   + nx * my) * nz + k];
      float p13 = input[(dx  + nx * my) * nz + k];
      float p14 = input[(ddx + nx * my) * nz + k];

      float p21 = input[(mx  + nx * y) * nz + k];
      float p22 = input[(x   + nx * y) * nz + k];
      float p23 = input[(dx  + nx * y) * nz + k];
      float p24 = input[(ddx + nx * y) * nz + k];

      float p31 = input[(mx  + nx * dy) * nz + k];
      float p32 = input[(x   + nx * dy) * nz + k];
      float p33 = input[(dx  + nx * dy) * nz + k];
      float p34 = input[(ddx + nx * dy) * nz + k];

      float p41 = input[(mx  + nx * ddy) * nz + k];
      float p42 = input[(x   + nx * ddy) * nz + k];
      float p43 = input[(dx  + nx * ddy) * nz + k];
      float p44 = input[(ddx + nx * ddy) * nz + k];

      //create array
      float pol[4][4] = { 
        {p11, p21, p31, p41}, {p12, p22, p32, p42},
        {p13, p23, p33, p43}, {p14, p24, p34, p44}
      };

      //return interpolation
      return bicubic_interpolation (pol, (float) uu - x, (float) vv - y);
    }
}



/**
  *
  * Compute the bicubic interpolation of an image from a parametric trasform
  *
**/
void bicubic_interpolation(
  float *input,   //image to be warped
  float *output,  //warped output image with bicubic interpolation
  float *params,  //x component of the vector field
  int nparams,     //number of parameters of the transform
  int nx,          //width of the image
  int ny,          //height of the image
  int nz,          //number of channels of the image       
  bool border_out  //if true, put zeros outside the region
)
{
  for (int i=0; i<ny; i++)
    for (int j=0; j<nx; j++)
    {
      int p=i*nx+j;
      float x, y;

      //transform coordinates using the parametric model
      project(j, i, params, x, y, nparams);
      
      //obtain the bicubic interpolation at position (uu, vv)
      for(int k=0; k<nz; k++)
        output[p*nz+k]=bicubic_interpolation(
          input, x, y, nx, ny, nz, k, border_out
        );
    }
}
