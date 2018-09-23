#! /bin/bash

SEQUENCE=$1
F=${2:-1} # first frame
L=${3:-0} # last frame

# zoom factor
ZF=2

# check correct number of input args
if [ "$#" -lt 1 ];
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

PLAMBDA="src/utils/imscript/bin/plambda"

INPUT_DIR="output_data/3_oflow/$SEQUENCE/downscaled/"
OUTPUT_DIR="output_data/3_oflow/$SEQUENCE"
echo "	Upscaling sequence $INPUT_DIR. Output stored in $OUTPUT_DIR"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

# upsample optical flow
UPSA="src/utils/imscript/bin/upsa"
mkdir -p $OUTPUT_DIR
for i in $(seq $F $L)
do
	N=$(printf %03d $i)
	$UPSA $ZF 2 $INPUT_DIR/$N.f.flo $OUTPUT_DIR/$N.f.flo
	$UPSA $ZF 2 $INPUT_DIR/$N.b.flo $OUTPUT_DIR/$N.b.flo
	$PLAMBDA $OUTPUT_DIR/$N.f.flo "x $ZF *" -o $OUTPUT_DIR/$N.f.flo
	$PLAMBDA $OUTPUT_DIR/$N.b.flo "x $ZF *" -o $OUTPUT_DIR/$N.b.flo
done

