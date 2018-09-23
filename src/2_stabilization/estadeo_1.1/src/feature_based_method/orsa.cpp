/**
 * @file orsa_homography.cpp
 * @brief Homographic image registration
 * @author Pascal Monasse, Pierre Moulon
 * 
 * Copyright (c) 2011-2012 Pascal Monasse, Pierre Moulon
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <ctime>  

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "orsa.h"
#include "libImage/image_io.hpp"
#include "libImage/image_drawing.hpp"
#include "libImage/image_crop.hpp"
#include "libNumerics/matrix.h"
#include "libNumerics/numerics.h"
#include "libOrsa/homography_model.hpp"
#include "sift/library.h"
#include "siftMatch.hpp"
#include "warping.hpp"

/// Number of random samples in ORSA
static const int ITER_ORSA=10000;

/// ORSA homography estimation
bool ORSA(
  const std::vector<Match>& vec_matchings, 
  int w1,
  int h1, 
  int w2,
  int h2,
  double precision,
  libNumerics::matrix<double>& H, 
  std::vector<int>& vec_inliers)
{
  const int n = static_cast<int>( vec_matchings.size() );
  if(n < 5)
    return false;
  
  libNumerics::matrix<double> xA(2,n), xB(2,n);

  for (int i=0; i < n; ++i)
  {
    xA(0,i) = vec_matchings[i].x1;
    xA(1,i) = vec_matchings[i].y1;
    xB(0,i) = vec_matchings[i].x2;
    xB(1,i) = vec_matchings[i].y2;
  }

  orsa::HomographyModel model(xA, w1, h1, xB, w2, h2, true);

  if(model.orsa(vec_inliers, ITER_ORSA, &precision, &H, false)>0.0)
    return false;

  if(model.ComputeModel(vec_inliers,&H)) // Re-estimate with all inliers
    return true;
  else
    return false;
}


int orsa_homography(
  float *I1, 
  float *I2, 
  float *Hout,
  int   nx,
  int   ny,
  double precision, 
  float fSiftRatio
)
{
  for(int i=0;i<8;i++) Hout[i]=0;

  // Init random seed
  time_t seed = time(0);     //Replace by a fixed value to debug a reproducible run
  srand((unsigned int)seed);


  Image<unsigned char> image1Gray, image2Gray;
  libs::rgb2gray(I1, &image1Gray, nx, ny);
  libs::rgb2gray(I2, &image2Gray, nx, ny);

  std::vector<Match> vec_matchings;
  SIFT(image1Gray, image2Gray, vec_matchings, fSiftRatio, 0);

  // Remove duplicates (frequent with SIFT)
  rm_duplicates(vec_matchings);

  const int w1=int(image1Gray.Width()), h1=int(image1Gray.Height());
  const int w2=int(image2Gray.Width()), h2=int(image2Gray.Height());

  // Estimation of homography with ORSA
  libNumerics::matrix<double> H(3,3);
  std::vector<int> vec_inliers;
  bool ok = ORSA(vec_matchings, w1, h1, w2, h2, precision, H, vec_inliers);
  if(ok)
  {
    H /= H(2,2);
    for(int i=0;i<8;i++) Hout[i]=H(i);
    Hout[0]-=1;
    Hout[4]-=1;
    return 0;
  }
  else
  {
    return 1;
  }
}
