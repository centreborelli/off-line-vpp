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
FLOW_DIR="output_data/3_oflow/$SEQUENCE"

echo "Computing optical flow for sequence $SEQUENCE. Output stored in $FLOW_DIR."

# determine last frame
if [ $L -lt 1 ];
then
	N=$(ls $INPUT_DIR/???.tif | wc -l)
	L=$((F + N - 1))
fi

# --------------------------------------------------------------------
# NOTE: the following lines are for computing the flow in a downscaled
# version of the given sequence (note that the sequence might already
# have been downsampled after the stabilization, therefore this means
# that we are computing the flow in 1/4 of the original size). This 
# lines are commented because we use a version of the tvl1 code
# slightly modified with respect to the IPOL version, which takes care
# of this downsampling, simply by ignoring the finest scale in the
# pyramid. In case you use any other optical flow, the following lines
# and the scripts they use might be usefull.
# -------------------------------------------------------------------- 
# # downsample to half resolution
# ./31_downsample_sequence.sh $SEQUENCE $F $L
# 
# # compute forwared and backward optical flow
# IN_DIR="output_data/3_oflow/$SEQUENCE/downscaled/"
# FLOW_DIR="output_data/3_oflow/$SEQUENCE/downscaled/"
# ./32_compute_tvl1_flow.sh $IN_DIR/%03d.tif $F $L $FLOW_DIR
# 
# # upsample to full resolution
# ./33_upsample_oflow.sh $SEQUENCE $F $L
# 
# # clean intermediate data
# rm -rf $FLOW_DIR


# compute forward and backward optical flow
./32_compute_tvl1_flow.sh $INPUT_DIR/%03d.tif $F $L $FLOW_DIR

# compute occlusion masks (only used by the NL-Kalman method)
./34_compute_occlusion_masks.sh $FLOW_DIR $F $L

