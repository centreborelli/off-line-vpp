#! /bin/bash

### These scaling factors are given with respect to the original input
### sequence's range. During the processing of the sequence a scaling is
### performed to map the sequences roughly to a [0, 255] range.
### This is done by 60_run_tonemapping.sh
SCALE_flou_IR_1=48.347928
SCALE_flou_IR_2=48.889867
SCALE_b2_vol21_passe6=1.366168
SCALE_b3_vol21_passe6=0.3 #0.581490
SCALE_vol21_passe6=0.581490
SCALE_VOL_B3_FILM_PASSE_2_vol19_capt=1.482353
SCALE_VOL_B3_FILM_PASSE_6_vol05_capt=4.703836
SCALE_PR184=0.4 #0.833155
SCALE_PR25=0.4 #0.802139
SCALE_seq_point2=15.402941
SCALE_seq_point7=20.017647

./60_run_tonemapping.sh seq_point2 0 78  4_denoising kalman     $SCALE_seq_point2
./60_run_tonemapping.sh seq_point2 0 78  4_denoising rbilf      $SCALE_seq_point2
./60_run_tonemapping.sh seq_point2 0 78  2_stabilization ""     $SCALE_seq_point2

./60_run_tonemapping.sh seq_point7 0 76  4_denoising kalman     $SCALE_seq_point7
./60_run_tonemapping.sh seq_point7 0 76  4_denoising rbilf      $SCALE_seq_point7
./60_run_tonemapping.sh seq_point7 0 76  2_stabilization  ""    $SCALE_seq_point7

./60_run_tonemapping.sh PR184      0 199 4_denoising kalman     $SCALE_PR184
./60_run_tonemapping.sh PR184      0 199 4_denoising rbilf      $SCALE_PR184
./60_run_tonemapping.sh PR184      0 199 2_stabilization  ""    $SCALE_PR184

./60_run_tonemapping.sh PR25       0 249 4_denoising kalman     $SCALE_PR25
./60_run_tonemapping.sh PR25       0 249 4_denoising rbilf      $SCALE_PR25
./60_run_tonemapping.sh PR25       0 249 2_stabilization  ""    $SCALE_PR25

./60_run_tonemapping.sh b3_vol21_passe6 0 596 4_denoising kalman $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b3_vol21_passe6 0 596 4_denoising rbilf  $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b3_vol21_passe6 0 596 2_stabilization "" $SCALE_b3_vol21_passe6

./60_run_tonemapping.sh b2_vol21_passe6 0 596 4_denoising kalman $SCALE_b2_vol21_passe6
./60_run_tonemapping.sh b2_vol21_passe6 0 596 4_denoising rbilf  $SCALE_b2_vol21_passe6
./60_run_tonemapping.sh b2_vol21_passe6 0 596 2_stabilization "" $SCALE_b2_vol21_passe6

./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 0 315 4_denoising kalman $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 0 315 4_denoising rbilf  $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 0 315 2_stabilization "" $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt

./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229 4_denoising kalman $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229 4_denoising rbilf  $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229 2_stabilization "" $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt

./60_run_tonemapping.sh flou_IR_1 0 645  5_deblurring kalman    $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 645  5_deblurring rbilf     $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 645  4_denoising kalman     $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 645  4_denoising rbilf      $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 645  2_stabilization ""     $SCALE_flou_IR_1

./60_run_tonemapping.sh flou_IR_2 0 374  5_deblurring kalman    $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 374  5_deblurring rbilf     $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 374  4_denoising kalman     $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 374  4_denoising rbilf      $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 374  2_stabilization ""     $SCALE_flou_IR_2

