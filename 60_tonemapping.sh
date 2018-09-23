#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

# check correct number of input args
if [ "$#" -lt 1 ];
then
  echo "Usage: $0 sequence_folder [first_frame last_frame]" >&2
  exit 1
fi

PREVSTEP_DIR="output_data/2_stabilization"
CURRSTEP_DIR="output_data/6_tonemapping"

INPUT_DIR="$PREVSTEP_DIR/$SEQUENCE"
OUTPUT_DIR="$CURRSTEP_DIR/$SEQUENCE"
mkdir -p $OUTPUT_DIR

# Octave script path
TONEMAP="src/6_tonemapping/tonemapping.m"

echo "Tone-mapping sequence $SEQUENCE."
echo "Input loaded from $PREVSTEP_DIR/"
echo "Output stored in $CURRSTEP_DIR/"

# determine last frame
if [ $L -lt 1 ];
then
  N=$(ls $INPUT_DIR/???.tif | wc -l)
  L=$((F + N - 1))
fi

for i in $(seq -f "%03g" $F $L)
do
  $TONEMAP $INPUT_DIR/$i.tif $OUTPUT_DIR/$i.png &
done
wait

