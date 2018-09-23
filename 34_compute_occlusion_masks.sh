#! /bin/bash

INPUT_DIR=$1
F=${2:-1} # first frame
L=${3:-0} # last frame

# check correct number of input args
if [ "$#" -lt 1 ];
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

PLAMBDA="src/utils/imscript/bin/plambda"

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.b.flo | wc -l)
	L=$((F + N - 1))
fi

OUTPUT_DIR=$INPUT_DIR
echo "	Computing occlusion masks for sequence $INPUT_DIR for frames $F til $L."
echo "	Output stored in $OUTPUT_DIR."

# compute occlusion masks (only backward flow)
mkdir -p $OUTPUT_DIR
for i in $(seq $F $L);
do
	file=$(printf $INPUT_DIR"/occ.%03d.b.1.00.png" $i)
	if [ ! -f $file ]
	then
		$PLAMBDA $(printf $OUTPUT_DIR/%03d.b.flo $i) " x(0,0)[0] x(-1,0)[0] - x(0,0)[1] x(0,-1)[1] - + fabs 1.0 > 255 * " -o $file
	fi
done | parallel

