#! /bin/bash

### Scaling Factors.
### They are given with respect to the input sequence's range. So the scaling
### performed during the processing of the sequences (i.e. 255/(max - min))
### still need to be taken into account (min and max are given in file
### "s_m_ub_lin_irange.txt" in 1_preprocessing. See run_tonemapping.sh)
SCALE_flou_IR_1=48.347928
SCALE_flou_IR_2=48.889867
SCALE_b2_vol21_passe6=1.366168
SCALE_b3_vol21_passe6=0.581490
SCALE_vol21_passe6=0.581490
SCALE_VOL_B3_FILM_PASSE_2_vol19_capt=1.482353
SCALE_VOL_B3_FILM_PASSE_6_vol05_capt=4.703836
SCALE_PR184=0.833155
SCALE_PR250=0.802139
SCALE_seq_point2=15.402941
SCALE_seq_point7=20.017647

rm output_data
ln -s output_data_simi-stab_no-fpn output_data
./60_run_tonemapping.sh PR184      0 199 4_denoising vbm3d      $SCALE_PR184
./60_run_tonemapping.sh PR184      0 10  4_denoising kalman     $SCALE_PR184
./60_run_tonemapping.sh PR184      0 199 4_denoising rbilf      $SCALE_PR184
./60_run_tonemapping.sh PR25       0 100 4_denoising kalman     $SCALE_PR250
./60_run_tonemapping.sh PR25       0 100 4_denoising rbilf      $SCALE_PR250
./60_run_tonemapping.sh seq_point2 0 72  4_denoising kalman     $SCALE_seq_point2
./60_run_tonemapping.sh seq_point2 0 72  4_denoising rbilf      $SCALE_seq_point2
./60_run_tonemapping.sh seq_point7 0 76  4_denoising kalman     $SCALE_seq_point7
./60_run_tonemapping.sh seq_point7 0 76  4_denoising rbilf      $SCALE_seq_point7
./60_run_tonemapping.sh flou_IR_2 0 300  5_deblurring kalman    $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 300  5_deblurring rbilf     $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 100  4_denoising kalman     $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_2 0 300  4_denoising rbilf      $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_1 0 500  5_deblurring kalman    $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 500  5_deblurring rbilf     $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 500  4_denoising kalman     $SCALE_flou_IR_1
./60_run_tonemapping.sh flou_IR_1 0 500  4_denoising rbilf      $SCALE_flou_IR_1

./60_run_tonemapping.sh PR184      0 199  2_stabilization $SCALE_PR184
./60_run_tonemapping.sh PR25       0 100  2_stabilization $SCALE_PR250
./60_run_tonemapping.sh seq_point2 0 72   2_stabilization $SCALE_seq_point2
./60_run_tonemapping.sh seq_point7 0 76   2_stabilization $SCALE_seq_point7
./60_run_tonemapping.sh flou_IR_2  0 300  2_stabilization $SCALE_flou_IR_2
./60_run_tonemapping.sh flou_IR_1  0 500  2_stabilization $SCALE_flou_IR_1

rm output_data
ln -s output_data_simi-stab_fpn output_data
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising vbm3d $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising kalman $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising rbilf $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b2_vol21_passe6                0 580   4_denoising kalman $SCALE_b2_vol21_passe6
./60_run_tonemapping.sh b2_vol21_passe6                0 580   4_denoising rbilf $SCALE_b2_vol21_passe6
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   4_denoising kalman $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   4_denoising rbilf $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  4_denoising kalman $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  4_denoising rbilf $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt

./60_run_tonemapping.sh b3_vol21_passe6                300 540 2_stabilization $SCALE_b3_vol21_passe6
./60_run_tonemapping.sh b2_vol21_passe6                0 580   2_stabilization $SCALE_b2_vol21_passe6
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   2_stabilization $SCALE_VOL_B3_FILM_PASSE_6_vol05_capt
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  2_stabilization $SCALE_VOL_B3_FILM_PASSE_2_vol19_capt
