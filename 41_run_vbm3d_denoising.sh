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
INPUT_DIR="output_data/2_stabilization/$SEQUENCE"
OUTPUT_DIR="output_data/4_denoising/$SEQUENCE/vbm3d"

echo "Denoising sequence $SEQUENCE. Output stored in $OUTPUT_DIR"


# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

echo "Denoising sequence $SEQUENCE, from frame $F to $L."
echo "Output stored $OUTPUT_DIR"

# create directory
mkdir -p $OUTPUT_DIR

# nldct binary
DENO="src/4_denoising/vbm3d/VBM3Ddenoising"
SIGMA=$(cat "$INPUT_DIR/sigma.txt")

# run denoising (first step)
$DENO \
-i ${INPUT_DIR}/%03d.tif -f $F -l $L -sigma $SIGMA \
-add false \
-deno ${OUTPUT_DIR}/%03d.tif
