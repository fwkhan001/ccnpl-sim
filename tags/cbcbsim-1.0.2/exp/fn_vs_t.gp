set terminal postscript eps
set output
set title 'False Negative Percent vs. Time'
set xlabel 'time (ms)'
set yrange [0:110]
set ylabel 'False Negative %'
plot	'fn_vs_t.INL_FN_PERCENT.1' title '50 Nodes' with lines, \
	'fn_vs_t.INL_FN_PERCENT.2' title '100 Nodes' with lines, \
	'fn_vs_t.INL_FN_PERCENT.3' title '200 Nodes' with lines \
