#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

# parameters of preprocessing
REMOVE_OUTLIERS=${4:-0}
REMOVE_FPN=${5:-0}

# toggle deblurring
DEBLURRING=${6:-0}

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder first-frame last-frame" >&2
	echo "See source for more command line options."
	exit 1
fi


echo "Running full pipeline for sequence $SEQUENCE"
echo "		- outlier removal is $REMOVE_OUTLIERS"
echo "		- fixed noise removal is $REMOVE_FPN"
echo "		- debluring is $DEBLURRING"

export PATH=`pwd`/bin/:$PATH


./10_preprocess_noise.sh $SEQUENCE $F $L $REMOVE_OUTLIERS $REMOVE_FPN
./20_stabilize_video.sh $SEQUENCE $F $L
./30_compute_optical_flow.sh $SEQUENCE $F $L
./40_run_denoising.sh $SEQUENCE $F $L "kalman"
./40_run_denoising.sh $SEQUENCE $F $L "rbilf"
if [ $DEBLURRING -eq 1 ];
then
#	./50_run_deblurring.sh $SEQUENCE $F $L "kalman" # TODO
#	./50_run_deblurring.sh $SEQUENCE $F $L "rbilf"  # TODO
	./60_run_tonemapping.sh $SEQUENCE $F $L "5_deblurring" "kalman"
	./60_run_tonemapping.sh $SEQUENCE $F $L "5_deblurring" "rbilf"
else
	./60_run_tonemapping.sh $SEQUENCE $F $L "4_denoising" "kalman"
	./60_run_tonemapping.sh $SEQUENCE $F $L "4_denoising" "rbilf"
fi

