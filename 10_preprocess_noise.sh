#! /bin/bash

# command line inputs
SEQUENCE=$1 # sequence folder
F=${2:-1}   # first frame (optional: default 1)
L=${3:-0}   # last frame  (optional: default all frames)

# parameters
REMOVE_OUTLIERS=${4:-1}
REMOVE_FPN=${5:-0}


# check correct number of input args
if [ "$#" -lt 1 ]; 
then
	echo "Usage: $0 sequence-folder [first-frame last-frame]" >&2
	exit 1
fi

echo "Preprocessing sequence $SEQUENCE."\
     "Output stored in output_data/1_preprocessing/$SEQUENCE/"

# determine last frame
if [ $L -lt 1 ];
then
	L=$(ls input_data/$SEQUENCE/???.tif | wc -l)
fi

INPUT_DIR="input_data/${SEQUENCE}"
OUTPUT_DIR="output_data/1_preprocessing/$SEQUENCE"

# create output folder
mkdir -p $OUTPUT_DIR

# imscripts functions that we need
BLUR="src/utils/imscript/bin/blur"
VECO="src/utils/imscript/bin/veco"
MORSI="src/utils/imscript/bin/morsi"
PLAMBDA="src/utils/imscript/bin/plambda"
IMPRINTF="src/utils/imscript/bin/imprintf"


#----------------
# remove outliers
#----------------

# relative error between image and its median filter
if [ $REMOVE_OUTLIERS -eq 1 ]
then
	for i in $(seq -f "%03g" $F $L)
	do
		frame="$INPUT_DIR/f0$i.png"
		med="med_$i.tif"
		echo "$MORSI disk2.1 median $frame $OUTPUT_DIR/$med &&"\
		     "$PLAMBDA $OUTPUT_DIR/$med $frame \"x y - fabs y /\" -o $OUTPUT_DIR/re_$med";
	done | parallel

	# temporal median of relative error, and thresholding
	$VECO med $OUTPUT_DIR/re_med_*tif -o $OUTPUT_DIR/tmed_re_med.tif
	$PLAMBDA $OUTPUT_DIR/tmed_re_med.tif "x 0.002 >" -o $OUTPUT_DIR/mask.tif

	# if mask > 0.5, use median filter, else use original image
	for i in $(seq -f "%03g" $F $L);
	do
		orig="$INPUT_DIR/f0$i.png"
		mask="$OUTPUT_DIR/mask.tif"
		med="$OUTPUT_DIR/med_$i.tif"
		out="PNG16:$OUTPUT_DIR/filt_$i.png"
		echo $PLAMBDA $mask $orig $med "\"x 0.5 < y * x 0.5 > z * + \"" -o $out
	done | parallel

	rm $OUTPUT_DIR/*med_* $OUTPUT_DIR/mask.tif
else
	# just copy input data
	for i in $(seq -f "%03g" $F $L)
	do
		echo cp $INPUT_DIR/f0$i.png $OUTPUT_DIR/filt_$i.png
	done | parallel
fi



#---------------------------
# remove fixed pattern noise
#---------------------------

if [ $REMOVE_FPN -eq 1 ]
then
	# compute temporal median
	tmed=$OUTPUT_DIR/tmed.tif
	$VECO med $OUTPUT_DIR/filt_* -o $tmed

	# remove the low frequencies
	$BLUR g 5 -s $tmed $OUTPUT_DIR/blurred_tmed.tif
	$PLAMBDA $tmed $OUTPUT_DIR/blurred_tmed.tif "x y -" -o $OUTPUT_DIR/fpn.tif 

	# remove fixed pattern noise from each frame
	for i in $(seq -f "%03g" $F $L);
	do
		fpn="$OUTPUT_DIR/fpn.tif"
		in="$OUTPUT_DIR/filt_$i.png"
		out="$OUTPUT_DIR/$i.tif"
		echo "$PLAMBDA $in $fpn \"x y -\" -o $out"
	done | parallel
else
	# just copy input data
	for i in $(seq -f "%03g" $F $L)
	do
		echo "$PLAMBDA $OUTPUT_DIR/filt_$i.png x -o $OUTPUT_DIR/$i.tif"
	done | parallel
fi



#----------------------------------
# correct vertical/horizontal bands
#----------------------------------

# remove fixed pattern noise from each frame
UB="src/1_preprocessing/unband/unband.m"
for i in $(seq -f "%03g" $F $L);
do
	echo "$UB $OUTPUT_DIR/$i.tif $OUTPUT_DIR/$i.tif"
done | parallel




#---------------------------------------------------------
# scale to approx 0 255 range (required for stabilization)
#---------------------------------------------------------

# first compute the 1%-99% range for a subset of the images
S=10 # step (only 1 out of $S images)
for i in $(seq -f "%03g" $F $S $L)
do
	$IMPRINTF "%q[1] %q[99]\n" $OUTPUT_DIR/$i.tif >> $OUTPUT_DIR/ranges.txt
done

# average the previous ranges, to have a unique 
# approximate range for the sequence
awk '{for (i=1;i<=NF;i++){a[i]+=$i;}} END {for (i=1;i<=NF;i++){printf "%f ", a[i]/NR;}; printf "\n"}' \
	$OUTPUT_DIR/ranges.txt > \
	$OUTPUT_DIR/range.txt

# now scale estimated range to 0 255
RANGE=$(cat $OUTPUT_DIR/range.txt)
for i in $(seq -f "%03g" $F $L)
do
	echo "$PLAMBDA $OUTPUT_DIR/$i.tif \"$RANGE range 255 *\" -o $OUTPUT_DIR/$i.tif"
done | parallel



# ---------------------------------------
# run ponomarenko's noise sigma estimator
# ---------------------------------------

# ---------------------------------------------------------------------
# NOTE: noise estimation should be done before denoising. If the
# downsampling is performed after the stabilization, it would make 
# sense to estimate the noise of the downsampled sequence. However, it
# works better estimating the noise before stabilization. This might be
# because even if the downsampling is applied after the stabilization, the
# noise is still correlated, and this harms the estimation algorithm.
# To speed up the noise estimation, and also to take the future down
# scaling into account, we estimate the noise on downscaled images.
# ---------------------------------------------------------------------
./11_estimate_noise.sh $SEQUENCE $F $L


