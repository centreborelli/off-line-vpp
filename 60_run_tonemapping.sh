#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2}   # first frame (optional: default 1)
L=${3}   # last frame  (optional: default all frames)
STEP=${4:-"4_denoising"} # pipeline step
ALGO=${5:-""} # algorithm

# number of parallel threads
NUM_PROCS=30

# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [algorithm] [first-frame last-frame]" >&2
	exit 1
fi

#echo "Preprocessing $STEP/$ALGO results for sequence $SEQUENCE."
#echo "Output stored in output_data/6_tonemapping/$SEQUENCE/$ALGO"

# create the output folder
OUTPUT_DIR="output_data/6_tonemapping/$SEQUENCE/$STEP/$ALGO"
mkdir -p $OUTPUT_DIR
BASE_DIR=$(pwd)

INPUT_DIR="output_data/$STEP/$SEQUENCE/$ALGO"

# add bin folder to path
export PATH=`pwd`/bin/:$PATH

PLAMBDA="src/utils/imscript/bin/plambda"

# read sequence original range
#echo output_data/1_preprocessing/$SEQUENCE/range.txt
read -r -a RANGE <<< $(cat output_data/1_preprocessing/$SEQUENCE/range.txt)
#RFACTOR=$($PLAMBDA -c "255 ${RANGE[1]} ${RANGE[0]} - /")
RFACTOR=$($PLAMBDA -c "${RANGE[1]} ${RANGE[0]} - 255 /")
#echo ${RANGE[0]}
#echo ${RANGE[1]}
#echo $RFACTOR
TM="src/6_tonemapping/tonemapping.m"
for i in $(seq -f "%03g" $F $L)
do
	$TM $INPUT_DIR/$i.tif $OUTPUT_DIR/$i.png $RFACTOR
done


