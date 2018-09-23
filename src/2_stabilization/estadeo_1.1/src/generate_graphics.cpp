#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <vector>
#include <stdlib.h>

#include "gaussian_conv_dct.h"

using namespace std;

double *read_transforms(char *name, int &nparams, int &ntransforms, int &nx, int &ny)
{
  FILE *fd=fopen(name,"r");
  int r=fscanf(fd,"%d %d %d %d", &nparams, &ntransforms, &nx, &ny);
 
  if(r>0)
  {
    double *H=new double[nparams*ntransforms];
  
    for(int i=0;i<ntransforms;i++) {
      for(int j=0;j<nparams;j++) r=fscanf(fd, "%lf", &(H[i*nparams+j]));
    }
    
    return H;
  }
  return NULL;
}

void read_file(char *name, std::vector<double> &re)
{
  //read the input file
  FILE *fd=fopen(name, "r");
  int i=0;
  while(!feof(fd))
  {
    double r;
    int t=fscanf(fd, "%lf", &r);
    if(!feof(fd) && t>0) {
      re.push_back(r);
    }
    i++;
  }
  fclose(fd);
}


void write_file (char *name, fftw_complex *out, int N)
{
  FILE *fd=fopen(name,"w");
  for(int i=0; i<N; i++)
    fprintf(fd, "%.10lf\n", sqrt(out[i][0]*out[i][0]+ out[i][1]*out[i][1]));
  fclose(fd);
}

void read_points(char *name, std::vector<double> &x, std::vector<double> &y)
{
  FILE *fd=fopen(name,"r");
  
  while(!feof(fd))
  {
    double xx, yy;
    int r=fscanf(fd, "%lf %lf", &xx, &yy);
    
    if(!feof(fd) && r>0) {
      x.push_back(xx);
      y.push_back(yy);
    }
  }
  fclose(fd);
}


void write_points(char *name, std::vector<double> &x, std::vector<double> &y)
{
  FILE *fd=fopen(name, "w");
  
  int N=x.size();
  for(int i=0; i<N; i++)
    fprintf(fd, "%.10lf %.10lf \n", x[i], y[i]);
  
  fclose(fd);
}

void read_points(char *name, std::vector<double> &x)
{
  FILE *fd=fopen(name,"r");
  
  while(!feof(fd))
  {
    double xx;
    int r=fscanf(fd, "%lf", &xx);
    
    if(!feof(fd) && r>0) {
      x.push_back(xx);
    }
  }
  fclose(fd);
}


void write_points(char *name, std::vector<double> &x)
{
  FILE *fd=fopen(name, "w");
  
  int N=x.size();
  for(int i=0; i<N; i++)
    fprintf(fd, "%.10lf \n", x[i]);
  
  fclose(fd);
}



/**
 *
 *  Function to transform a 2D point (x,y) through a parametric model
 *
 */
void project
(
  double x,      //x component of the 2D point
  double y,      //y component of the 2D point
  double *p,  //parameters of the transformation
  double &xp, //x component of the transformed point
  double &yp, //y component of the transformed point
  int nparams //number of parameters
)
{
  switch(nparams) {
    default: case 2: //p=(tx, ty) 
      xp=x+p[0];
      yp=y+p[1];
      break;
    case 3:   //p=(tx, ty, tita)
      xp=cos(p[2])*x-sin(p[2])*y+p[0];
      yp=sin(p[2])*x+cos(p[2])*y+p[1];
      break;
    case 4:  //p=(tx, ty, a, b)
      xp=(1+p[2])*x-p[3]*y+p[0];
      yp=p[3]*x+(1+p[2])*y+p[1];
      break;
    case 6:    //p=(tx, ty, a00, a01, a10, a11)
      xp=(1+p[2])*x+p[3]*y+p[0];
      yp=p[4]*x+(1+p[5])*y+p[1];
      break;
    case 8:  //p=(h00, h01,..., h21)
      double d=p[6]*x+p[7]*y+1;
      xp=((1+p[0])*x+p[1]*y+p[2])/d;
      yp=(p[3]*x+(1+p[4])*y+p[5])/d;
      break;
  }
}



void trajectory_graphic(int argc, char *argv[]) 
{
  if(argc<4)
    printf("Usage: %s v transforms.txt out_file.txt\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    int nparams, nframes, nx, ny;

    double *H=read_transforms(file, nparams, nframes, nx, ny);

    double xm=nx/2;
    double ym=ny/2;

    FILE*fd=fopen(out, "w");

    for(int i=0; i<nframes; i++)
    {
      double xp, yp;

      project(xm, ym, &H[i*nparams], xp, yp, nparams);

      fprintf(fd,"%.10lf %.10lf \n", xp, yp);
      
      xm=xp;
      ym=yp;
    }

    fclose(fd);
  }  
}


void velocity_graphic(int argc, char *argv[]) 
{
  if(argc<4)
    printf("Usage: %s v transforms.txt out_file.txt\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    int nparams, nframes, nx, ny;

    double *H=read_transforms(file, nparams, nframes, nx, ny);

    double xm=nx/2;
    double ym=ny/2;

    FILE*fd=fopen(out, "w");
    double vx=0;
    double vy=0;

    //fprintf(fd,"%f %f \n", vx, vy);

    for(int i=0; i<nframes; i++)
    {
      double xp, yp;

      project(xm, ym, &H[i*nparams], xp, yp, nparams);

      vx+=(xp-xm);
      vy+=(yp-ym);
      fprintf(fd,"%.10lf %.10lf \n", vx, vy);     
    }

    fclose(fd);
  }  
}


void rotation_graphic(int argc, char *argv[]) 
{
  if(argc<4)
    printf("Usage: %s r transforms.txt out_file.txt\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    int nparams, nframes, nx, ny;

    double *H=read_transforms(file, nparams, nframes, nx, ny);

    double x0=0, x1=nx;
    double y0=ny/2;
    

    FILE*fd=fopen(out, "w");
    double vtheta=0;
    double avgtheta=0;

    for(int i=1; i<nframes; i++)
    {
      double xp1, yp1;
      double xp2, yp2;

      project(x0, y0, &H[i*nparams], xp1, yp1, nparams);
      project(x1, y0, &H[i*nparams], xp2, yp2, nparams);
      
      double x=xp2-xp1;
      double y=yp2-yp1;

      vtheta=atan2(y,x)*180/3.1415926;
      avgtheta+=vtheta;
      fprintf(fd,"%.10lf \n", vtheta);     
    }

    delete []H;
    fclose(fd);

    char name[500];
    sprintf(name, "%s_average", out);
    fd=fopen(name, "w");
    fprintf(fd,"%.10lf\n", avgtheta/nframes);     
    fclose(fd);
  }  
}

void zoom_graphic(int argc, char *argv[]) 
{
  if(argc<4)
    printf("Usage: %s z transforms.txt out_file.txt\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    int nparams, nframes, nx, ny;

    double *H=read_transforms(file, nparams, nframes, nx, ny);

    FILE*fd=fopen(out, "w");
    double zoom=0;
    double avgzoom=0;

    for(int i=1; i<nframes; i++)
    {
      double xp0, yp0;
      double xp1, yp1;
      double xp2, yp2;
      double xp3, yp3;

      project(0, 0, &H[i*nparams], xp0, yp0, nparams);
      project(0, ny, &H[i*nparams], xp1, yp1, nparams);
      project(nx, 0, &H[i*nparams], xp2, yp2, nparams);
      project(nx, ny, &H[i*nparams], xp3, yp3, nparams);

      //area del triangulo segun formula de Heron
      const double dx1=xp1-xp0;
      const double dx2=xp2-xp0;
      const double dx3=xp2-xp1;
      const double dx4=xp3-xp2;
      const double dx5=xp3-xp1;
      const double dy1=yp1-yp0;
      const double dy2=yp2-yp0;
      const double dy3=yp2-yp1;
      const double dy4=yp3-yp2;
      const double dy5=yp3-yp1;
      const double a=sqrt(dx1*dx1+dy1*dy1);
      const double b=sqrt(dx2*dx2+dy2*dy2);
      const double c=sqrt(dx3*dx3+dy3*dy3);
      const double d=sqrt(dx4*dx4+dy4*dy4);
      const double e=sqrt(dx5*dx5+dy5*dy5);
      const double s1=(a+b+c)/2;
      const double s2=(e+d+c)/2;
      const double A1=nx*ny;
      const double A2=sqrt(s1*(s1-a)*(s1-b)*(s1-c));
      const double A3=sqrt(s2*(s2-e)*(s2-d)*(s2-c));
      
      zoom=(A2+A3)/A1;
      avgzoom+=zoom;
      fprintf(fd,"%f \n", zoom);     
    }

    delete []H;
    fclose(fd);
    
    char name[500];
    sprintf(name, "%s_average", out);
    fd=fopen(name, "w");
    fprintf(fd,"%.10lf \n", avgzoom/nframes);     
    fclose(fd);

  }  
}

void fft(int argc, char *argv[])
{
  if(argc < 3)
    printf("\nUsage: %s f infile [outfile] [Nrows] \n\n", argv[0]);
  else
  {
    std::vector<double> re;
    read_file(argv[2], re);

    int N=re.size();
    fftw_complex *in=
        (fftw_complex *) fftw_malloc ( sizeof ( fftw_complex ) * N );
    fftw_complex *out=
        (fftw_complex *) fftw_malloc ( sizeof ( fftw_complex ) * N );
    fftw_plan p;

    for(int i=0;i<N;i++)
    {
      in[i][0]=re[i];
      in[i][1]=0;
    }
    
    p = fftw_plan_dft_1d ( N, in, out, FFTW_FORWARD, FFTW_ESTIMATE );

    fftw_execute(p);
    
    if(argc>=3)
      write_file(argv[3], out, N);
    else
      for(int i=0; i<N; i++)
        printf("%.10lf\n", sqrt(out[i][0]*out[i][0]+ out[i][1]*out[i][1]));

    fftw_destroy_plan(p);
    // fftw_destroy_plan(p);  
    fftw_free ( in );
    fftw_free ( out );

  }
}


void linear_scale_space(int argc, char *argv[]) 
{
  if(argc<5)
    printf("Usage: %s l velocity1.txt velocity2.txt outfile\n", argv[0]);
  else
  {
    char *file1=argv[2];
    char *file2=argv[3];
    char *out=argv[4];
    vector<double> x1, y1, x2, y2;
    vector<double> dx, dy;
    
    read_points(file1, x1, y1);
    read_points(file2, x2, y2);
    
    int N1=x1.size();
    int N2=x2.size();
    for(int i=0; i<((N1>N2)?N2:N1); i++)
    {
      dx.push_back(x2[i]-x1[i]);
      dy.push_back(y2[i]-y1[i]);
    }
    
    write_points(out, dx, dy);

  }  
}


void scale_space_value(int argc, char *argv[]) 
{
  if(argc<5)
    printf("Usage: %s e data1.txt data2.txt outfile\n", argv[0]);
  else
  {
    char *file1=argv[2];
    char *file2=argv[3];
    char *out=argv[4];
    vector<double> x1, x2;
    vector<double> dx;
    
    read_points(file1, x1);
    read_points(file2, x2);
    
    int N1=x1.size();
    int N2=x2.size();
    for(int i=0; i<((N1>N2)?N2:N1); i++)
      dx.push_back(x2[i]-x1[i]);
    
    write_points(out, dx);

  }  
}


void dct_convolution(vector<double> &x, vector<double> &sx, float sigma)
{
  int N=x.size();
  
  double *dest=new double[3*N-2];
  double *src=new double[3*N-2];
  dct_coeffs c;

  
  for(int i=N-1; i<2*N-1; i++)
    src[i]=x[i-N+1];

  int bc=1;
  //boundary conditions
  switch(bc){
    case 0: //CONSTANT_BC:
      for(int i=0; i<N-1; i++)
	src[i]=x[0];
      for(int i=2*N-1; i<3*N-2; i++)
	src[i]=x[N-1];
      break;
    case 1://NEUMANN_BC:
      for(int i=0; i<N-1; i++)
	src[i]=x[N-i-2];
      for(int i=2*N-1; i<3*N-2; i++)
	src[i]=x[3*N-i-2];
      break;
    case 2: //DIRICHLET_BC: 
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
    for(int i=0; i<N; i++)
      sx.push_back(dest[N-1+i]);  
    dct_free(&c);
  }
}

void smooth_points(int argc, char *argv[]) 
{
  if(argc<5)
    printf("Usage: %s s velocity.txt out_file.txt sigma\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    float sigma=atof(argv[4]);
    vector<double> x, y;
    vector<double> sx, sy;
    
    read_points(file, x, y);
        
    dct_convolution(x, sx, sigma);
    dct_convolution(y, sy, sigma);
    write_points(out, sx, sy);
    
  }  
}

void smooth_scalars(int argc, char *argv[]) 
{
  if(argc<5)
    printf("Usage: %s m velocity.txt out_file.txt sigma\n", argv[0]);
  else
  {
    char *file=argv[2];
    char *out=argv[3];
    float sigma=atof(argv[4]);
    vector<double> x;
    vector<double> sx;
    
    read_points(file, x);
    
    int N=x.size();
    
    double *dest=new double[3*N-2];
    double *src=new double[3*N-2];
    dct_coeffs c;

    for(int i=N-1; i<2*N-1; i++)
      src[i]=x[i-N+1];

    int bc=1;
    //boundary conditions
    switch(bc){
      case 0: //CONSTANT_BC:
        for(int i=0; i<N-1; i++)
          src[i]=x[0];
        for(int i=2*N-1; i<3*N-2; i++)
          src[i]=x[N-1];
        break;
      case 1://NEUMANN_BC:
        for(int i=0; i<N-1; i++)
          src[i]=x[N-i-2];
        for(int i=2*N-1; i<3*N-2; i++)
          src[i]=x[3*N-i-2];
        break;
      case 2: //DIRICHLET_BC: 
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

    for(int i=0; i<N; i++)
      sx.push_back(dest[N-1+i]);

    write_points(out, sx);

  }  
}



int main(int argc, char *argv[])
{
  if(argc>1) 
  {
    if(argv[1][0]=='t')
      trajectory_graphic(argc, argv);
    if(argv[1][0]=='v')
      velocity_graphic(argc, argv);
    if(argv[1][0]=='r')
      rotation_graphic(argc, argv);
    if(argv[1][0]=='z')
      zoom_graphic(argc, argv);
    if(argv[1][0]=='s')
      smooth_points(argc, argv);
    if(argv[1][0]=='m')
      smooth_scalars(argc, argv);
    if(argv[1][0]=='l')
      linear_scale_space(argc, argv);
    if(argv[1][0]=='e')
      scale_space_value(argc, argv);
    if(argv[1][0]=='f')
      fft(argc, argv);
  }
  
  return 0;
}


