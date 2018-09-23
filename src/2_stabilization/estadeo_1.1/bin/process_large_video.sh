
inc=10
second=0
for min in `seq 0 10`
do
  for sec in `seq 0 $inc 50`
  do
    start=00:$min:$sec
    echo avconv -v 0 -ss $start -i $1 -t 00:00:$inc -pix_fmt yuv420p -y tmp.mp4
    avconv -v 0 -ss $start -i $1 -t 00:00:$inc -pix_fmt yuv420p -y tmp.mp4
    cmdline_execute.sh tmp.mp4 outvideo_${min}_$sec.mp4 0 30 transforms_${min}_$sec.mat 0
  done
done
