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

INPUT_DIR="output_data/1_preprocessing/$SEQUENCE"
SIGMAS="output_data/1_preprocessing/$SEQUENCE/sigmas.txt"
OUTPUT="output_data/1_preprocessing/$SEQUENCE/sigma.txt"

echo "	Estimating noise for sequence $SEQUENCE. Output stored in $OUTPUT"

# determine last frame
if [ $L -lt 1 ];
then
	L=$(ls input_data/$SEQUENCE/???.tif | wc -l)
fi


PONO=src/1_preprocessing/ponomarenko/ponomarenko
DOWNSA="src/utils/imscript/bin/downsa"

# downsampling factor
DOWNSAMPLE=0
ZF=2

# step (only run ponomarenko in 1/S of the frames)
S=10

for i in $(seq -f "%03g" $F $S $L)
do
	if [ $DOWNSAMPLE -eq 1 ];
	then
		# downsample to simulate the coarse-scale noise
		$DOWNSA v $ZF $INPUT_DIR/$i.tif $INPUT_DIR/tmp.tif
		# run ponomarenko's noise estimator with a single bin
		$PONO -b 1 $INPUT_DIR/tmp.tif | awk '{print $2}' >> $SIGMAS
	else
		$PONO -b 1 $INPUT_DIR/$i.png | awk '{print $2}' >> $SIGMAS
	fi
done | parallel

# compute average sigma
awk '{s+=$1} END {print s/NR}' RS=" " $SIGMAS > $OUTPUT
if [ $DOWNSAMPLE -eq 1 ];
then
	rm -f $INPUT_DIR/tmp.tif
fi

