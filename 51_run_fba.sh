#! /bin/bash

# check correct number of input args
if [ "$#" -lt 5 ]; 
then
	echo "Usage: $0 sequence-folder first-frame last-frame input-dir output-dir" >&2
	exit 1
fi

SEQUENCE=$1
F=${2} # first frame
L=${3} # last frame
INPUT_DIR=$4
OUTPUT_DIR=$5

# fba binary
FBA="src/5_deblurring/fba/fba"

mkdir -p $OUTPUT_DIR
$FBA 3 128 11 3 1 1 ${INPUT_DIR}/%03d.tif $F $L /dev/null ${OUTPUT_DIR}/%03d.tif /dev/null

