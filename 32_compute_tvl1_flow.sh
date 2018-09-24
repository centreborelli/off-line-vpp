#! /bin/bash

# Computes tvl1 optical flow for a (noisy) sequence. 
D=${1:-"example/sequence/path/frame_%03d.tif"}  # sequence path, in printf format (MUST have a %d identifier)
F=${2:-1}                                       # first frame
L=${3:-150}                                     # last frame 
O=${4:-"."}                                     # output folder

# check correct number of input args
if [ "$#" -ne 4 ]; 
then
	echo "Usage: $0 sequence-path first-frame last-frame output-folder" >&2
	exit 1
fi

echo "	Computing TVL1 flow for sequence $D. Output stored in $O"


TVL1="src/3_oflow/tvl1flow_3/tvl1flow"

mkdir -p $O

DW=0.1 # weight of data attachment term 0.1 ~ very smooth 0.2 ~ noisy
FS=1   # finest scale (0 image scale, 1 one coarser level, 2 more coarse, etc...

# ---------------------------------------------------------------------
# NOTE: Recursive denoising algorithms require only the backward flow.
# NL-DCT can use both backward and forward optical flows. In this 
# script we compute both of them, but the next loop can be commented 
# for recursive algorithms.
# ---------------------------------------------------------------------

## # compute forward flow
## for i in `seq $F $((L - 1))`;
## do
## 	$TVL1 `printf ${D} $i` `printf ${D} $((i + 1))` \
## 		   ${O}/`printf %03d.f.flo $i` \
## 		   0 0.25 $DW 0.3 100 $FS 0.5 5 0.01 0; 
## done
## cp ${O}/`printf %03d.f.flo $((L - 1))` ${O}/`printf %03d.f.flo $L`
# ---------------------------------------------------------------------
# NOTE: Because of lazy programming, the denoising algorithms expect to
# receive the same number of video frames and optical flows. Of course
# there is no forward flow for the last frame, and no backward flow for
# the first. What we do is copy some flow as the last flow (or as the 
# first flow in the case of the backward flow). The denoising codes 
# do not actually use these files, but give an error if a different
# number of files is passed.
# ---------------------------------------------------------------------

# compute backward flow
for i in `seq $((F + 1)) $L`;
do
	$TVL1 `printf ${D} $i` `printf ${D} $((i - 1))` \
		   ${O}/`printf %03d.b.flo $i` \
		   0 0.25 $DW 0.3 100 $FS 0.5 5 0.01 0; 
done
cp ${O}/`printf %03d.b.flo $((F + 1))` ${O}/`printf %03d.b.flo $F`

