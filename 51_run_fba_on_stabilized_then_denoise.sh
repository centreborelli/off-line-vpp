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

# run deblurring on stabilized images
./51_run_fba_on_stabilized.sh $SEQUENCE $F $L

INPUT_DIR="output_data/5_deblurring/$SEQUENCE/noisy"
OUTPUT_DIR="output_data/5_deblurring/$SEQUENCE/noisy_denoised"
OFLOW_DIR="output_data/3_oflow/$SEQUENCE"
mkdir -p $OUTPUT_DIR

# nldct binary
DENO="src/4_denoising/nldct/build/bin/vnlbayes"
SIGMA=$(cat "output_data/2_stabilization/$SEQUENCE/sigma.txt")

# run denoising (first step)
$DENO \
-i ${INPUT_DIR}/%03d.tif -f $F -l $L -sigma $SIGMA -has-noise \
-fof ${OFLOW_DIR}/%03d.f.flo -bof ${OFLOW_DIR}/%03d.b.flo \
-px2 0 -px1 16 -pt1 3 -wx1 54 -wt1 6 -np1 240 -b1 2.0 \
-bsic ${OUTPUT_DIR}/b_%03d.tif

mv measures.txt ${OUTPUT_DIR}/measures_basic

# run denoising (second step)
$DENO \
-i ${INPUT_DIR}/%03d.tif -f $F -l $L -sigma $SIGMA -has-noise \
-b ${OUTPUT_DIR}/b_%03d.tif \
-fof ${OFLOW_DIR}/%03d.f.flo -bof ${OFLOW_DIR}/%03d.b.flo \
-px1 0 -px2 16 -pt2 3 -wx2 54 -wt2 6 -np2 120 \
-deno ${OUTPUT_DIR}/d_%03d.tif

# clean
rm {bsic,diff}_???.png
mv measures.txt ${OUTPUT_DIR}/measures

