#! /bin/bash

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame algo]" >&2
	exit 1
fi

F=${2:-1} # first frame
L=${3:-0} # last frame 
ALGO=${4} # algorithm

SEQUENCE=$1
echo "Deblurring sequence $SEQUENCE. Output stored output_data/5_deblurring/$SEQUENCE/$ALGO"

INPUT_DIR="output_data/4_denoising/$SEQUENCE/$ALGO"
OUTPUT_DIR="output_data/5_deblurring/$SEQUENCE/$ALGO"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

./51_run_fba.sh $SEQUENCE $F $L $INPUT_DIR $OUTPUT_DIR

