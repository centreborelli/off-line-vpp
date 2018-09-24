#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

STABILIZE=${4:-1} # toggle stabilization on/off

# downsample the sequence after stabilization
DOWNSAMPLE=0

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

if [ $STABILIZE -eq 1 ]; then
	echo "Stabilizing sequence $SEQUENCE."
else
	echo "Skipping stabilization for sequence $SEQUENCE."
fi
echo "Output stored in output_data/2_stabilization/$SEQUENCE/"

STABI="src/2_stabilization/estadeo_1.1/bin/estadeo"

INPUT_DIR="output_data/1_preprocessing/$SEQUENCE"
OUTPUT_DIR="output_data/2_stabilization/$SEQUENCE"
mkdir -p $OUTPUT_DIR

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi


if [ $STABILIZE -eq 1 ];
then
	$STABI $INPUT_DIR/%03d.tif $F $L -1 -1 -1 -o $OUTPUT_DIR/%03d.tif
	#$STABI $INPUT_DIR/%03d.tif $F $L -1 -1 -1 -t 4 -p 2 -o $OUTPUT_DIR/%03d.tif
else
	# just copy to bypass stabilization
	for i in $(seq -f "%03g" $F $L)
	do
		cp $INPUT_DIR/$i.tif $OUTPUT_DIR/$i.tif
	done | parallel
fi

# downsample the sequence
if [ $DOWNSAMPLE -eq 1 ]; 
then
	echo "  Downsampling stabilized sequence $SEQUENCE."
	DOWNSA="src/utils/imscript/bin/downsa"
	ZF=2 # downsampling factor
	for i in $(seq -f "%03g" $F $L)
	do
		$DOWNSA v $ZF $OUTPUT_DIR/$i.tif $OUTPUT_DIR/$i.tif
	done | parallel
fi

# clean
rm $INPUT_DIR/*.tif
