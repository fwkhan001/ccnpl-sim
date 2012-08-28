set terminal postscript eps
set output
set title 'SR/UR Packets Per Second vs. Network Size'
set xlabel 'Network Size in Nodes'
set ylabel 'SR/UR Packets Per Second'
set nokey
plot	'scalability.dat' using ($1):($2/1800) with linespoints
