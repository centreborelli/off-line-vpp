#ifndef ORSA_H
#define ORSA_H

int orsa_homography(
  float *I1, 
  float *I2, 
  float *Hout,
  int   nx,
  int   ny,
  double precision=0, 
  float fSiftRatio=0.6f
);

#endif