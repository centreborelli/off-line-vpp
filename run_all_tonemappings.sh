#! /bin/bash

rm output_data
ln -s output_data_simi-stab_no-fpn output_data
./60_run_tonemapping.sh PR184      0 199 4_denoising vbm3d
./60_run_tonemapping.sh PR184      0 10  4_denoising kalman
./60_run_tonemapping.sh PR184      0 199 4_denoising rbilf
./60_run_tonemapping.sh PR25       0 100 4_denoising kalman
./60_run_tonemapping.sh PR25       0 100 4_denoising rbilf
./60_run_tonemapping.sh seq_point2 0 72  4_denoising kalman
./60_run_tonemapping.sh seq_point2 0 72  4_denoising rbilf
./60_run_tonemapping.sh seq_point7 0 76  4_denoising kalman
./60_run_tonemapping.sh seq_point7 0 76  4_denoising rbilf
./60_run_tonemapping.sh flou_IR_2 0 300  5_deblurring kalman
./60_run_tonemapping.sh flou_IR_2 0 300  5_deblurring rbilf
./60_run_tonemapping.sh flou_IR_2 0 100  4_denoising kalman
./60_run_tonemapping.sh flou_IR_2 0 300  4_denoising rbilf
./60_run_tonemapping.sh flou_IR_1 0 500  5_deblurring kalman
./60_run_tonemapping.sh flou_IR_1 0 500  5_deblurring rbilf
./60_run_tonemapping.sh flou_IR_1 0 500  4_denoising kalman
./60_run_tonemapping.sh flou_IR_1 0 500  4_denoising rbilf

./60_run_tonemapping.sh PR184      0 199  2_stabilization
./60_run_tonemapping.sh PR25       0 100  2_stabilization
./60_run_tonemapping.sh seq_point2 0 72   2_stabilization
./60_run_tonemapping.sh seq_point7 0 76   2_stabilization
./60_run_tonemapping.sh flou_IR_2  0 300  2_stabilization
./60_run_tonemapping.sh flou_IR_1  0 500  2_stabilization

rm output_data
ln -s output_data_simi-stab_fpn output_data
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising vbm3d
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising kalman
./60_run_tonemapping.sh b3_vol21_passe6                300 540 4_denoising rbilf
./60_run_tonemapping.sh b2_vol21_passe6                0 580   4_denoising kalman
./60_run_tonemapping.sh b2_vol21_passe6                0 580   4_denoising rbilf
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   4_denoising kalman
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   4_denoising rbilf
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  4_denoising kalman
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  4_denoising rbilf

./60_run_tonemapping.sh b3_vol21_passe6                300 540 2_stabilization
./60_run_tonemapping.sh b2_vol21_passe6                0 580   2_stabilization
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_6_vol05_capt 0 229   2_stabilization
./60_run_tonemapping.sh VOL_B3_FILM_PASSE_2_vol19_capt 40 280  2_stabilization
