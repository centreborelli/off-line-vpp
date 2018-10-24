#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

INPUT_DIR="output_data/2_stabilization/$SEQUENCE"
SIGMAS="$INPUT_DIR/sigmas.txt"
OUTPUT="$INPUT_DIR/sigma.txt"

echo "	Estimating noise for sequence $SEQUENCE. Output stored in $OUTPUT"

# determine last frame
if [ $L -lt 1 ];
then
	L=$(ls $INPUT_DIR/???.tif | wc -l)
fi

PONO=src/1_preprocessing/ponomarenko/ponomarenko

# step (only run ponomarenko in 1/S of the frames)
S=10

for i in $(seq -f "%03g" $F $S $L)
do
	# run ponomarenko's noise estimator with a single bin
	$PONO -b 1 $INPUT_DIR/$i.tif | awk '{print $2}' >> $SIGMAS
done | parallel

# compute average sigma
awk '{s+=$1} END {print s/NR}' RS=" " $SIGMAS > $OUTPUT

