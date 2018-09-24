#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

# parameters of preprocessing
REMOVE_OUTLIERS=${4:-0}
REMOVE_FPN=${5:-0}

# toggle stabilization
STABILIZE=${6:-1}

# toggle deblurring
DEBLUR=${7:-0}

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
echo "		- stabilization is $STABILIZE"
echo "		- debluring is $DEBLUR"

export PATH=`pwd`/bin/:$PATH


./10_preprocess_noise.sh $SEQUENCE $F $L $REMOVE_OUTLIERS $REMOVE_FPN
./20_stabilize_video.sh $SEQUENCE $F $L $STABILIZE
./30_compute_optical_flow.sh $SEQUENCE $F $L
./40_run_denoising.sh $SEQUENCE $F $L "kalman"
./40_run_denoising.sh $SEQUENCE $F $L "rbilf"
if [ $DEBLUR -eq 1 ];
then
	./50_run_deblurring.sh $SEQUENCE $F $L "kalman"
	./50_run_deblurring.sh $SEQUENCE $F $L "rbilf"

	# NOTE: for the moment we don't have an automatic selection of the
	#       tonemapping parameters. Instead, we have chosen manually
	#       a parameter for each sequence. To tonemap the sequences, you
	#       can 60_run_tonemapping.sh with the parameters given in
	#       run_all_tonemappings.

#	./60_run_tonemapping.sh $SEQUENCE $F $L "5_deblurring" "kalman"
#	./60_run_tonemapping.sh $SEQUENCE $F $L "5_deblurring" "rbilf"
else
#	./60_run_tonemapping.sh $SEQUENCE $F $L "4_denoising" "kalman"
#	./60_run_tonemapping.sh $SEQUENCE $F $L "4_denoising" "rbilf"
fi

