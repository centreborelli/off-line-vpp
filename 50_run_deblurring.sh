#! /bin/bash

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

F=${2:-1} # first frame
L=${3:-0} # last frame 

SEQUENCE=$1
echo "Deblurring sequence $SEQUENCE. Output stored output_data/5_deblurring/$SEQUENCE/"

INPUT_DIR="output_data/4_denoising/$SEQUENCE"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

# ./51_run_fba_on_stabilized.sh $SEQUENCE $F $L

# ./51_run_fba_on_denoised.sh $SEQUENCE $F $L

./51_run_fba_on_stabilized_then_denoise.sh $SEQUENCE $F $L

