set terminal postscript eps
set output
set title 'SR/UR Control Packet Count vs. Time'
set xlabel 'time (ms)'
set ylabel 'SR/UR Control Packet Count'
plot	'srur_vs_t.INL_SRUR_COUNT.1' title 'Msg. Count' with lines, \
	'srur_vs_t.INL_SRUR_COUNT.2' title '10 min.' with lines, \
	'srur_vs_t.INL_SRUR_COUNT.3' title '8 min.' with lines
