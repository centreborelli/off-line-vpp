#! /usr/bin/env octave-cli
%#! /usr/bin/env octave -qf


%%% Get list of arguments passed to script
arg_list = argv();
inputName = arg_list{1};
outputName = arg_list{2};
rangeFactor = str2double(arg_list{3});

%%% Load required Octave packages
pkg load statistics
pkg load image

%%% Add to path the iio_read and iio_write.
%%% They must be in the same dir as this script
addpath(fileparts(mfilename('fullpath')));

%%% Load image
img = iio_read(inputName);
[height,width,depth] = size(img);

%%% MGF parameters
epsilon = (6/rangeFactor)^2;  % typical amplitude of detail is between 4 and 8...
radius  = 1;    % small images, need to avoid contrast halo
nIt     = 1;    % for speed
nScales = 0;    % filter all scales

%%% Compute decomposition
base   = MGFoctave( img, img, epsilon, radius, nIt, nScales );
detail = img - base;

%%% Compress the base layer
baseMad    = mad(base(:));
baseMed    = median(base(:));
baseNorm   = max(0,min(1, (base-baseMed)./(3*baseMad) + 1/4 ));

%%% Compute final tone-mapped gray image
vGray      = ( 1/2 + (baseNorm - 1/2)/8 + detail/(24/rangeFactor) );

%%% Get colormap (create variable "cmap")
warning ('off', 'Octave:data-file-in-path') 
load colormap.mat

%%% compute color coefficients using the (normalized+clipped) input image
uNorm      = max(0,min(1, (img-baseMed)./(3*baseMad) + 1/4 ));
uNormCol   = ind2rgb(uint16( sqrt(uNorm)*(2^16-1) ), cmap);
uNormColCoeff = uNormCol./(sum(uNormCol,3)/3);

%%% apply color coefficient to vGray to get final colored tonemapped image
vCol       = vGray.*uNormColCoeff;

%%% save in uint8 (8bits) color tonemapped image
iio_write(outputName, uint8(255*vCol));

