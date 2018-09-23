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

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

# fba binary
FBA="src/5_deblurring/fba/fba"

# run deblurring on stabilized images
INPUT_DIR="output_data/2_stabilization/$SEQUENCE/"
OUTPUT_DIR="output_data/5_deblurring/$SEQUENCE/noisy"
mkdir -p $OUTPUT_DIR
$FBA 3 128 4 4 1 1 ${INPUT_DIR}/%03d.tif $F $L /dev/null ${OUTPUT_DIR}/%03d.tif /dev/null

