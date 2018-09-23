#! /bin/bash

SEQUENCE=$1
F=${2:-1} # first frame
L=${3:-0} # last frame 
ALGORITHM=${4:-"rbilf"} # denoising algorithm

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

INPUT_DIR="output_data/2_stabilization/$SEQUENCE"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

## # run ponomarenko's noise estimator
#./41_estimate_noise.sh $SEQUENCE $F $L
cp output_data/1_preprocessing/$SEQUENCE/sigma.txt $INPUT_DIR/

# call denoiser
if [ $ALGORITHM == "kalman" ]; then
	# nl-kalman
	./41_run_kalman_denoising.sh $SEQUENCE $F $L
elif [ $ALGORITHM == "rbilf" ]; then
	# recursive bilateral filter
	./41_run_rbilf_denoising.sh $SEQUENCE $F $L
elif [ $ALGORITHM == "vbm3d" ]; then
	# recursive bilateral filter
	./41_run_vbm3d_denoising.sh $SEQUENCE $F $L
else
	# print error
	>&2 echo "Error: unknown denoising algorithm $ALGORITHM"
fi
