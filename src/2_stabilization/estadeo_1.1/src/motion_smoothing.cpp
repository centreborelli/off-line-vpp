// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.


#include "motion_smoothing.h"
#include "gaussian_conv_dct.h"
#include "direct_method/transformation.h"
#include "direct_method/matrix.h"

#include <stdio.h>
#include <math.h>

/**
  *
  * Function to compute the motion of the smooth camera path
  *
**/
void compute_smooth_transforms(
  float *H,       //original matrix transformations
  float *Hp,      //stabilizing transformations
  float *Ho,      //output smooth camera path
  int nparams,    //type of transformation
  int ntransforms //number of frames in the video
)
{
  float *H_1=new float[nparams];
  float *Htmp=new float[nparams];

  for(int i=0;i<nparams;i++) Ho[i]=0;
  for(int i=1;i<ntransforms;i++)
  {
      inverse_transform(&Hp[i*nparams], H_1, nparams);
      compose_transform(&H[i*nparams], &Hp[(i-1)*nparams], Htmp, nparams);
      compose_transform(H_1, Htmp, &Ho[i*nparams],nparams);
  }

  delete []H_1;
  delete []Htmp;
}



//compositional approach (static camera)
void composition
(
  float *Hmotion,  //original matrix transformations
  float *Hsmooth,  //smooth output matrix transformations
  int nparams,     //type of matrix transformation
  int ntransforms  //number of frames in the video
)
{
  //first transform unaltered
  for(int i=0;i<nparams;i++) Hsmooth[i]=Hmotion[i];
  
  //compose the following transforms
  for(int i=1;i<ntransforms;i++)
    compose_transform(
      &(Hmotion[i*nparams]), &(Hsmooth[(i-1)*nparams]), 
      &(Hsmooth[i*nparams]), nparams
    );
}


//Gaussian convolution
void gaussian
(
  float *H,          //original matrix transformations
  float *Hs,         //smooth output matrix transformations
  int   i,           //frame number
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //types of boundary conditions
)
{
  
  if(sigma>=3*ntransforms)
    sigma=ntransforms/3;
  
  int radius=3*sigma;

  if(radius>=ntransforms) 
    radius=ntransforms-1;
  
  //Gaussian convolution in  each parameter separately
  for(int p=0;p<nparams;p++)
  {
    double average=0.0;
    double sum=0.0;
    
    for(int j=i-radius;j<=i+radius;j++)
    {
      double value=0;
      
      //test boundary conditions
      if(j<0)
      {
        switch(bc){
          case CONSTANT_BC:
            value=H[p];
            break;
          case NEUMANN_BC:
            value=H[-j*nparams+p];
            break;
          case DIRICHLET_BC:
            value=2*H[p]-H[-j*nparams+p];
            break;
        }
      }
      else if(j>=ntransforms)
      {
        switch(bc){
          case CONSTANT_BC:
            value=H[(ntransforms-1)*nparams+p];
            break;
          case NEUMANN_BC:
            value=H[(2*ntransforms-1-j)*nparams+p];
            break;
          case DIRICHLET_BC:
            value=2*H[(ntransforms-1)*nparams+p]-
                    H[(2*ntransforms-1-j)*nparams+p];
            break;
        }
      }
      else value=H[j*nparams+p];
      
      //increase accumulator
      double norm=0.5*(j-i)*(j-i)/(sigma*sigma);
      double gauss=exp(-norm);
      average+=gauss*value;
      sum+=gauss;
    }
    Hs[p]=(float) (average/sum);
  }
}

//compositional smoothing approach
void compositional_smoothing
(
  float *Hin,        //original matrix transformations
  float *Hout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //types of boundary conditions
)
{
  float *Hs_1=new float[ntransforms*nparams];
  float *Hs=new float[ntransforms*nparams];
  float *Hc=new float[ntransforms*nparams];

  //compute transforms to the initial reference frame
  composition(Hin, Hc, nparams, ntransforms);

  //smooth transforms with a Gaussian
  for(int i=0;i<ntransforms;i++)
    gaussian(Hc, &(Hs[i*nparams]), i, nparams, ntransforms, sigma, bc);

  //compute inverse transformations 
  for(int i=0;i<ntransforms;i++) 
    inverse_transform(&(Hs[i*nparams]), &(Hs_1[i*nparams]), nparams);
  
  //compose the transforms
  for(int i=0;i<ntransforms;i++)
    compose_transform(
      &(Hc[(i)*nparams]), &(Hs_1[i*nparams]), 
      &(Hout[i*nparams]), nparams
    );
  
  delete []Hs_1;  
  delete []Hs;  
  delete []Hc;  
}


//Gaussian convolution for local methods
void gaussian_local
(
  float *H,          //original matrix transformations
  float *Hs,         //smooth output matrix transformations
  int   i,           //frame number
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc,          //types of boundary conditions
  float *H_1         //inverse transforms
)
{
  if(sigma>=3*ntransforms)
    sigma=ntransforms/3;
  
  int radius=3*sigma;

  if(radius>=ntransforms) 
    radius=ntransforms-1;
  
  //Gaussian convolution in  each parameter separately
  for(int p=0;p<nparams;p++)
  { 
    double average=0.0;
    double sum=0.0;
    
    for(int j=i-radius;j<=i+radius;j++)
    {
      float value=0;
      
      //test boundary conditions
      if(j<0)
      {
        switch(bc){
          case CONSTANT_BC:
            value=0;
            break;
          case NEUMANN_BC:
            value=H_1[(-j)*nparams+p];
            break;
          case DIRICHLET_BC:
            value=H[(-j)*nparams+p];
            break;
        }
      }
      else if(j>=ntransforms) 
      {
        switch(bc){
          case CONSTANT_BC:
            value=0;
            break;
          case NEUMANN_BC:
            value=H_1[(2*ntransforms-1-j)*nparams+p];
            break;
          case DIRICHLET_BC:
            value=H[(2*ntransforms-1-j)*nparams+p];
            break;
        }
      }
      else value=H[j*nparams+p];
      
      //increase accumulator
      float norm=0.5*(j-i)*(j-i)/(sigma*sigma);
      float gauss=exp(-norm);
      average+=gauss*value;
      sum+=gauss;
    }
    Hs[p]=average/sum;
  }
}


//compositional local smoothing approach
void compositional_local_smoothing
(
  float *Hin,        //original matrix transformations
  float *Hout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //types of boundary conditions
)
{
  float *Hs=new float[ntransforms*nparams];
  float *Hs_1=new float[ntransforms*nparams];
  float *Hc_1=new float[ntransforms*nparams];

  //compute inverses for the boundary condition (only for Neumann bc)
  for(int i=0;i<ntransforms;i++) 
    inverse_transform(&(Hin[i*nparams]), &(Hc_1[i*nparams]), nparams);

  //smooth transforms with a Gaussian kernel
  for(int i=0;i<ntransforms;i++)
    gaussian_local(
      Hin, &(Hs[i*nparams]), i, nparams, 
      ntransforms, sigma, bc, Hc_1
    );

  //compute inverse of smooth transformations
  for(int i=0;i<ntransforms;i++)
    inverse_transform(&(Hs[i*nparams]), &(Hs_1[i*nparams]), nparams);

  //compose the transforms
  for(int i=0;i<ntransforms;i++)
    compose_transform(
      &(Hin[i*nparams]), &(Hs_1[i*nparams]), 
      &(Hout[i*nparams]), nparams
    );

  //compute transforms to the initial reference frame
  composition(Hout, Hout, nparams, ntransforms);

  delete []Hs_1;  
  delete []Hs;  
  delete []Hc_1;
}


//local matrix based smoothing approach
void local_matrix_based_smoothing
(
  float *Hin,        //original matrix transformations
  float *Hout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video
  float sigma,       //Gaussian standard deviation
  int   bc           //types of boundary conditions
)
{
  float *H_1=new float[ntransforms*nparams];
  float *Hc=new float[ntransforms*nparams];
  float *Hs=new float[ntransforms*nparams];

  if(sigma>=3*ntransforms)
    sigma=ntransforms/3;
  
  int radius=3*sigma;

  if(radius>=ntransforms) 
    radius=ntransforms-1;

  //compute inverse transformations 
  for(int i=0;i<ntransforms;i++) 
    inverse_transform(&(Hin[i*nparams]), &(H_1[i*nparams]), nparams);

  for(int i=0;i<ntransforms;i++)
  {
    int t1=(i-radius>0)?i-radius:0;
    int t2=((i+radius)<ntransforms)?(i+radius):ntransforms-1;

    //compute backward transformations
    if(i>0)
    {
      for(int j=0;j<nparams;j++) 
         Hc[(i-1)*nparams+j]=H_1[i*nparams+j];
      for(int j=i-2;j>=t1;j--)
         compose_transform(
            &(H_1[(j+1)*nparams]), &(Hc[(j+1)*nparams]), 
            &(Hc[j*nparams]), nparams
         );
    }

    //introduce the identity matrix in the middle
    for(int j=0;j<nparams;j++) Hc[i*nparams+j]=0;

    //compute forward transformations
    if(i<ntransforms-1)
    {
      for(int j=0;j<nparams;j++) 
         Hc[(i+1)*nparams+j]=Hin[(i+1)*nparams+j];
      for(int j=i+2;j<=t2;j++)
         compose_transform(
            &(Hin[j*nparams]), &(Hc[(j-1)*nparams]), 
            &(Hc[j*nparams]), nparams
         );
    }

    //smooth transforms with a Gaussian kernel
    gaussian(Hc, &(Hs[i*nparams]), i, nparams, ntransforms, sigma, bc);

    //compute inverse transformations 
    inverse_transform(&(Hs[i*nparams]), &(Hout[i*nparams]), nparams);
  }
  
  delete []H_1;  
  delete []Hc;  
  delete []Hs;
}


//Multiplies a 3x3 homography and a vector
void Hx(float *H, float x, float y, float &xp, float &yp)
{
  float den=H[6]*x+H[7]*y+H[8];
  if(den*den>1E-10)
  {
    xp=(H[0]*x+H[1]*y+H[2])/den;
    yp=(H[3]*x+H[4]*y+H[5])/den;
  }
  else {
    printf("Division by zero\n");
    xp=x;yp=y;
  }
}

//Multiplies two 3x3 matrices
void HxH(float *H1, float *H2, float *H3)
{
  H3[0]=H1[0]*H2[0]+H1[1]*H2[3]+H1[2]*H2[6];
  H3[1]=H1[0]*H2[1]+H1[1]*H2[4]+H1[2]*H2[7];
  H3[2]=H1[0]*H2[2]+H1[1]*H2[5]+H1[2]*H2[8];
  H3[3]=H1[3]*H2[0]+H1[4]*H2[3]+H1[5]*H2[6];
  H3[4]=H1[3]*H2[1]+H1[4]*H2[4]+H1[5]*H2[7];
  H3[5]=H1[3]*H2[2]+H1[4]*H2[5]+H1[5]*H2[8];
  H3[6]=H1[6]*H2[0]+H1[7]*H2[3]+H1[8]*H2[6];
  H3[7]=H1[6]*H2[1]+H1[7]*H2[4]+H1[8]*H2[7];
  H3[8]=H1[6]*H2[2]+H1[7]*H2[5]+H1[8]*H2[8];
}


//Gaussian convolution with a set of points
double gaussian(
  float *x,          //set of points
  int   i,           //frame number
  int   ntransforms, //number of transforms
  float sigma,       //Gaussian standard deviation
  int   bc           //type of boundary condition
)
{
  double average=0.0;
  double sum=0.0;

  if(sigma>=3*ntransforms)
    sigma=ntransforms/3;
  
  int radius=3*sigma;

  if(radius>=ntransforms) 
    radius=ntransforms-1;
  
  //Gaussian convolution
  for(int j=i-radius;j<=i+radius;j++)
  {
    double value=0;
    
    //test boundary conditions
    if(j<0)
    {
      switch(bc){
        case CONSTANT_BC:
          value=x[0];
          break;
        case NEUMANN_BC:
          value=x[-j];
          break;
        case DIRICHLET_BC:
          value=2*x[0]-x[-j];
          break;
      }
    }
    else if(j>=ntransforms) 
    {
      switch(bc){
        case CONSTANT_BC:
          value=x[ntransforms-1];
          break;
        case NEUMANN_BC:
          value=x[2*ntransforms-1-j];
        case DIRICHLET_BC:
          value=2*x[ntransforms-1]-x[2*ntransforms-1-j];
          break;
      }
    }
    else value=x[j];

    double dx=j-i;
    double norm=0.5*dx*dx/(sigma*sigma);
    double gauss=exp(-norm);
    average+=gauss*value;
    sum+=gauss;
  }
  return average/sum;
}


//Computes a homography from four points
void compute_H
(
  double x1, double x2, double x3, double x4,          
  double y1, double y2, double y3, double y4,
  double x1p, double x2p, double x3p, double x4p,          
  double y1p, double y2p, double y3p, double y4p,
  double *H
)
{
  double A[64], A_1[64], b[8];
  
  //compose the independent vector
  b[0]=x1p; b[1]=y1p;
  b[2]=x2p; b[3]=y2p;
  b[4]=x3p; b[5]=y3p;
  b[6]=x4p; b[7]=y4p;

  //compose the system matrix
  int i=0; 
  int j=0;
  A[i++]=x1;A[i++]=y1;A[i++]=1;A[i++]=0;A[i++]=0;
  A[i++]=0;A[i++]=-b[j]*x1;A[i++]=-b[j++]*y1;
  A[i++]=0;A[i++]=0;A[i++]=0;A[i++]=x1;A[i++]=y1;
  A[i++]=1;A[i++]=-b[j]*x1;A[i++]=-b[j++]*y1;
  A[i++]=x2;A[i++]=y2;A[i++]=1;A[i++]=0;A[i++]=0;
  A[i++]=0;A[i++]=-b[j]*x2;A[i++]=-b[j++]*y2;
  A[i++]=0;A[i++]=0;A[i++]=0;A[i++]=x2;A[i++]=y2;
  A[i++]=1;A[i++]=-b[j]*x2;A[i++]=-b[j++]*y2;
  A[i++]=x3;A[i++]=y3;A[i++]=1;A[i++]=0;A[i++]=0;
  A[i++]=0;A[i++]=-b[j]*x3;A[i++]=-b[j++]*y3;
  A[i++]=0;A[i++]=0;A[i++]=0;A[i++]=x3;A[i++]=y3;
  A[i++]=1;A[i++]=-b[j]*x3;A[i++]=-b[j++]*y3;
  A[i++]=x4;A[i++]=y4;A[i++]=1;A[i++]=0;A[i++]=0;
  A[i++]=0;A[i++]=-b[j]*x4;A[i++]=-b[j++]*y4;
  A[i++]=0;A[i++]=0;A[i++]=0;A[i++]=x4;A[i++]=y4;
  A[i++]=1;A[i++]=-b[j]*x4;A[i++]=-b[j++]*y4;

  //solve
  inverse(A, A_1, 8);
  Axb(A_1,b,H,8);
  H[8]=1; 
}

//local point based smoothing approach
void local_point_based_smoothing
(
  float *pin,        //original matrix transformations
  float *pout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //type of boundary condition
)
{
  float *x1=new float[ntransforms];
  float *x2=new float[ntransforms];
  float *x3=new float[ntransforms];
  float *x4=new float[ntransforms];
  float *y1=new float[ntransforms];
  float *y2=new float[ntransforms];
  float *y3=new float[ntransforms];
  float *y4=new float[ntransforms];
  float *H =new float[ntransforms*9];   
  float *H_1=new float[ntransforms*9];  
  float *p_1=new float[ntransforms*nparams];
  
  int radius=3*sigma;

  //compute inverse transformations 
  for(int i=0;i<ntransforms;i++) 
    inverse_transform(&(pin[i*nparams]), &(p_1[i*nparams]), nparams);

  //convert from params to matrices
  for(int i=0;i<ntransforms;i++)
  {
    params2matrix(&(pin[i*nparams]), &(H[i*9]), nparams);
    params2matrix(&(p_1[i*nparams]), &(H_1[i*9]), nparams);
  }

  //for each frame, compute a smooth homography
  for(int i=0;i<ntransforms;i++) 
  { 
    //tracking a set of points to be smoothed
    x1[i]=0;   y1[i]=0;
    x2[i]=0;   y2[i]=500;
    x3[i]=500; y3[i]=0;
    x4[i]=500; y4[i]=500;

    //project four points in the neighbor frames
    for(int j=0;j<radius;j++)
    {
      const int p1=i+j;
      const int p2=i-j;

      if(p1<ntransforms-1)
      { 
        Hx(&(H[(p1+1)*9]),x1[p1],y1[p1],x1[p1+1],y1[p1+1]);
        Hx(&(H[(p1+1)*9]),x2[p1],y2[p1],x2[p1+1],y2[p1+1]);
        Hx(&(H[(p1+1)*9]),x3[p1],y3[p1],x3[p1+1],y3[p1+1]);
        Hx(&(H[(p1+1)*9]),x4[p1],y4[p1],x4[p1+1],y4[p1+1]);
      }

      if(p2>0)
      {
        Hx(&(H_1[p2*9]),x1[p2],y1[p2],x1[p2-1],y1[p2-1]);
        Hx(&(H_1[p2*9]),x2[p2],y2[p2],x2[p2-1],y2[p2-1]);
        Hx(&(H_1[p2*9]),x3[p2],y3[p2],x3[p2-1],y3[p2-1]);
        Hx(&(H_1[p2*9]),x4[p2],y4[p2],x4[p2-1],y4[p2-1]);
      }
    }

    //convolve trajectories with a Gaussian kernel
    double x1s=gaussian(x1,i,ntransforms,sigma,bc);
    double x2s=gaussian(x2,i,ntransforms,sigma,bc);
    double x3s=gaussian(x3,i,ntransforms,sigma,bc);
    double x4s=gaussian(x4,i,ntransforms,sigma,bc);
    double y1s=gaussian(y1,i,ntransforms,sigma,bc);
    double y2s=gaussian(y2,i,ntransforms,sigma,bc);
    double y3s=gaussian(y3,i,ntransforms,sigma,bc);
    double y4s=gaussian(y4,i,ntransforms,sigma,bc);

    //calculate the smoothed homography from the four smoothed points  
    double tmp[9];
    compute_H(
      x1s,x2s,x3s,x4s,
      y1s,y2s,y3s,y4s,      
      x1[i],x2[i],x3[i],x4[i],      
      y1[i],y2[i],y3[i],y4[i],     
      tmp
    );

    float Hout[9];
    for(int j=0; j<9; j++) Hout[j]=tmp[j];

    //convert homographies to params
    matrix2params(Hout,&(pout[i*nparams]),nparams);
  }
  
  delete []x1;  
  delete []x2;  
  delete []x3;  
  delete []x4;  
  delete []y1;  
  delete []y2;  
  delete []y3;  
  delete []y4;  
  delete []H;
  delete []H_1; 
  delete []p_1;
}


//Matrix DCT Gaussian convolution
void gaussian_dct
(
  float *H,      //original matrix transformations
  float *Hs,     //smooth output matrix transformations
  int   nparams, //type of matrix transformation
  int   N,       //number of frames of the video  
  float sigma,   //Gaussian standard deviation
  int   bc       //type of boundary condition
)
{  
  num *dest=new num[3*N-2];
  num *src=new num[3*N-2];
  dct_coeffs c;
  
  //convolution in each matrix position
  for(int p=0; p<nparams; p++)
  {
    //copy the original image
    for(int i=N-1; i<2*N-1; i++)
      src[i]=H[(i-N+1)*nparams+p];
    
    //boundary conditions
    switch(bc){
      case CONSTANT_BC:
        for(int i=0; i<N-1; i++)
          src[i]=H[p];
        for(int i=2*N-1; i<3*N-2; i++)
          src[i]=H[(N-1)*nparams+p];
        break;
      case NEUMANN_BC: 
        for(int i=0; i<N-1; i++)
          src[i]=H[(N-i-2)*nparams+p];
        for(int i=2*N-1; i<3*N-2; i++)
          src[i]=H[(3*N-i-2)*nparams+p];
        break;
      case DIRICHLET_BC: 
        for(int i=0; i<N-1; i++)
          src[i]=2*H[p]-H[(N-i-2)*nparams+p];
        for(int i=2*N-1; i<3*N-2; i++)
          src[i]=2*H[(N-1)*nparams+p]-H[(3*N-i-2)*nparams+p];
        break;
    }

    //apply DCT Gaussian convolution
    if (!(dct_precomp(&c, dest, src, 3*N-2, 1, sigma)))
      printf("Error in Gaussian convolution with DCT.");
    else {
      dct_gaussian_conv(c);
      dct_free(&c);
    }

    //copy the signal in the domain
    for(int i=0; i<N; i++)
      Hs[i*nparams+p]=dest[N-1+i];
  }

  delete []src;
  delete []dest;
}


//local linear matrix-based smoothing
void local_linear_matrix_based_smoothing
(
  float *pin,        //original matrix transformations
  float *pout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //type of boundary condition
)
{
  float *H =new float[ntransforms*9];
  float *Hi=new float[ntransforms*9];
  float *Hs=new float[ntransforms*9];

  //convert from params to matrices
  for(int i=1;i<ntransforms;i++) 
    params2matrix(&(pin[i*nparams]), &(H[i*9]), nparams);

  //identity matrix in the first position
  Hi[1]=Hi[2]=Hi[3]=0;
  Hi[5]=Hi[6]=Hi[7]=0;
  Hi[0]=Hi[4]=Hi[8]=1;

  //compute the virtual matrix trajectories
  for(int i=1;i<ntransforms;i++) 
  {
    for(int j=0;j<9;j++)
      //accumulate the homographies
      Hi[i*9+j]=Hi[(i-1)*9+j]+H[i*9+j];
    
    //subtract the identity matrix
    Hi[i*9]-=1;Hi[i*9+4]-=1;Hi[i*9+8]-=1;
  }

  //convolve the virtual trajectories with a Gaussian kernel
  gaussian_dct(Hi, Hs, 9, ntransforms, sigma, bc);

  for(int i=0;i<ntransforms;i++) 
  {
    //compute the correction matrix
    for(int j=0;j<9;j++)
      Hs[i*9+j]-=Hi[i*9+j];

    //add the identity matrix
    Hs[i*9]+=1;Hs[i*9+4]+=1;Hs[i*9+8]+=1;

    //convert homographies to params
    matrix2params(&(Hs[i*9]),&(pout[i*nparams]),nparams);

    //compute its inverse 
    inverse_transform(&(pout[i*nparams]), &(pout[i*nparams]), nparams);        
  }
  
  delete []H;
  delete []Hi;
  delete []Hs;
}



//Point DCT Gaussian convolution
void gaussian_dct
(
  float *x,    //input set of points
  float *xs,   //output set of smoothed points 
  int   N,     //number of points
  float sigma, //Gaussian standard deviation
  int   bc     //type of boundary condition
)
{  
  num *dest=new num[3*N-2];
  num *src=new num[3*N-2];
  dct_coeffs c;

  //copy the original signal
  for(int i=N-1; i<2*N-1; i++)
    src[i]=x[i-N+1];
  
  //boundary conditions
  switch(bc){
    case CONSTANT_BC:
      for(int i=0; i<N-1; i++)
        src[i]=x[0];
      for(int i=2*N-1; i<3*N-2; i++)
        src[i]=x[N-1];
      break;
    case NEUMANN_BC:
      for(int i=0; i<N-1; i++)
        src[i]=x[N-i-2];
      for(int i=2*N-1; i<3*N-2; i++)
        src[i]=x[3*N-i-2];
      break;
    case DIRICHLET_BC: 
      for(int i=0; i<N-1; i++)
        src[i]=2*x[0]-x[N-i-2];
      for(int i=2*N-1; i<3*N-2; i++)
        src[i]=2*x[N-1]-x[3*N-i-2];
      break;
  }

  //apply DCT Gaussian convolution
  if (!(dct_precomp(&c, dest, src, 3*N-2, 1, sigma)))
    printf("Error in Gaussian convolution with DCT.");
  else {
    dct_gaussian_conv(c);
    dct_free(&c);
  }

  //copy the signal in the domain
  for(int i=0; i<N; i++)
    xs[i]=dest[N-1+i];
  
  delete []src;
  delete []dest;
}

//local linear point based smoothing approach
void local_linear_point_based_smoothing
(
  float *pin,        //original matrix transformations
  float *pout,       //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video  
  float sigma,       //Gaussian standard deviation
  int   bc           //type of boundary condition
)
{ 
  float *x0=new  float[ntransforms];
  float *x1=new  float[ntransforms];
  float *x2=new  float[ntransforms];
  float *x3=new  float[ntransforms];
  float *y0=new  float[ntransforms];
  float *y1=new  float[ntransforms];
  float *y2=new  float[ntransforms];
  float *y3=new  float[ntransforms];
  float *x0s=new float[ntransforms];
  float *x1s=new float[ntransforms];
  float *x2s=new float[ntransforms];
  float *x3s=new float[ntransforms];
  float *y0s=new float[ntransforms];
  float *y1s=new float[ntransforms];
  float *y2s=new float[ntransforms];
  float *y3s=new float[ntransforms];
  float *H =new  float[ntransforms*9];

  //convert from params to matrices
  for(int i=0;i<ntransforms;i++) 
    params2matrix(&(pin[i*nparams]), &(H[i*9]), nparams);

  //choose four fixed points for all frames
  float xp[4]={0, 0, 500, 500};
  float yp[4]={0, 500, 0, 500};

  //tracking a set of points to be smoothed
  x0[0]=xp[0]; y0[0]=yp[0];
  x1[0]=xp[1]; y1[0]=yp[1];
  x2[0]=xp[2]; y2[0]=yp[2];
  x3[0]=xp[3]; y3[0]=yp[3];
  
  //compute the virtual trajectories
  for(int i=1;i<ntransforms;i++) 
  { 
    float dx, dy;
    Hx(&(H[i*9]),xp[0],yp[0],dx,dy);
    x0[i]=x0[i-1]+(dx-xp[0]);
    y0[i]=y0[i-1]+(dy-yp[0]);

    Hx(&(H[i*9]),xp[1],yp[1],dx,dy);
    x1[i]=x1[i-1]+(dx-xp[1]);
    y1[i]=y1[i-1]+(dy-yp[1]);

    Hx(&(H[i*9]),xp[2],yp[2],dx,dy);
    x2[i]=x2[i-1]+(dx-xp[2]);
    y2[i]=y2[i-1]+(dy-yp[2]);

    Hx(&(H[i*9]),xp[3],yp[3],dx,dy);
    x3[i]=x3[i-1]+(dx-xp[3]);
    y3[i]=y3[i-1]+(dy-yp[3]);
  }
  
  //DCT Gaussian convolution of each virtual trajectory
  gaussian_dct(x0, x0s, ntransforms, sigma, bc);
  gaussian_dct(x1, x1s, ntransforms, sigma, bc);
  gaussian_dct(x2, x2s, ntransforms, sigma, bc);
  gaussian_dct(x3, x3s, ntransforms, sigma, bc);
  gaussian_dct(y0, y0s, ntransforms, sigma, bc);
  gaussian_dct(y1, y1s, ntransforms, sigma, bc);
  gaussian_dct(y2, y2s, ntransforms, sigma, bc);
  gaussian_dct(y3, y3s, ntransforms, sigma, bc);
  
  for(int i=0;i<ntransforms;i++) 
  {
    x0s[i]+=xp[0]-x0[i];
    x1s[i]+=xp[1]-x1[i];
    x2s[i]+=xp[2]-x2[i];
    x3s[i]+=xp[3]-x3[i];
    y0s[i]+=yp[0]-y0[i];
    y1s[i]+=yp[1]-y1[i];
    y2s[i]+=yp[2]-y2[i];
    y3s[i]+=yp[3]-y3[i];
    
    //calculate the smoothed homography    
    double tmp[9];
    compute_H(
      x0s[i],x1s[i],x2s[i],x3s[i],
      y0s[i],y1s[i],y2s[i],y3s[i],      
      xp[0],xp[1],xp[2],xp[3],      
      yp[0],yp[1],yp[2],yp[3],     
      tmp
    );

    float Hout[9];
    for(int j=0; j<9; j++) Hout[j]=tmp[j];
    
    //convert homographies to params
    matrix2params(Hout,&(pout[i*nparams]),nparams);
  }

  delete []x0s;
  delete []x1s;
  delete []x2s;
  delete []x3s;
  delete []y0s;
  delete []y1s;
  delete []y2s;
  delete []y3s;
  delete []x0;
  delete []x1;
  delete []x2;
  delete []x3;
  delete []y0;
  delete []y1;
  delete []y2;
  delete []y3;
  delete []H;
}


/**
  *
  * Main function for motion_smoothing
  * 
  *
**/
void motion_smoothing
(
  float *Hmotion,    //original matrix transformations
  float *Hsmooth,    //smooth output matrix transformations
  int   nparams,     //type of matrix transformation
  int   ntransforms, //number of frames of the video
  float sigma,       //Gaussian standard deviation
  int   type,        //motion smoothing strategy
  int   bc,          //type of boundary condition
  int   verbose      //verbose mode
)
{
  switch(type) 
  {
    case PURE_COMPOSITION:
      if(verbose) printf("  Composition approach:\n");
      composition(Hmotion, Hsmooth, nparams, ntransforms);
      break;
    case COMPOSITIONAL_SMOOTHING:
      if(verbose) printf("  Compositional smoothing approach:\n");
      compositional_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
    case COMPOSITIONAL_LOCAL_SMOOTHING:
      if(verbose) printf("  Compositional local smoothing approach:\n");
      compositional_local_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
    case LOCAL_MATRIX_BASED_SMOOTHING:
      if(verbose) printf("  Local matrix-based smoothing approach:\n");
      local_matrix_based_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
    case LOCAL_POINT_BASED_SMOOTHING:
      if(verbose) printf("  Local point-based smoothing approach:\n");
      local_point_based_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
    case LOCAL_LINEAR_MATRIX_BASED_SMOOTHING:
      if(verbose) printf("  Local linear matrix-based smoothing approach:\n");
      
      local_linear_matrix_based_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
    case LOCAL_LINEAR_POINT_BASED_SMOOTHING:
      if(verbose) printf("  Local linear point-based smoothing approach:\n");
      
      local_linear_point_based_smoothing(
        Hmotion, Hsmooth, nparams, 
        ntransforms, sigma, bc
      );
      break;
  }
}



