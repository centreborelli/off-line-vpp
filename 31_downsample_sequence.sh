#! /bin/bash

# zoom factor
ZF=2

# check correct number of input args
if [ "$#" -lt 1 ];
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

F=${2:-1} # first frame
L=${3:-0} # last frame

SEQUENCE=$1
OUTPUT_DIR="output_data/3_oflow/$SEQUENCE/downscaled/"
INPUT_DIR="output_data/2_stabilization/$SEQUENCE"
echo "	Downscaling sequence $INPUT_DIR. Output stored in $OUTPUT_DIR"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

# downsample the sequence
DOWNSA="src/utils/imscript/bin/downsa"
mkdir -p $OUTPUT_DIR
for i in $(seq -f "%03g" $F $L)
do
	$DOWNSA v $ZF ${INPUT_DIR}/$i.tif $OUTPUT_DIR/$i.tif
done


