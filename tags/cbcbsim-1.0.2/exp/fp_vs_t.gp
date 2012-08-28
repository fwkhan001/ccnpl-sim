set terminal postscript eps
set output
set title 'False Positive Percent vs. Time (Predicate Change Delay: 20 min.)
set xlabel 'time (ms)'
set yrange [0:110]
set ylabel 'False Positive %'
plot	'fp_vs_t.INL_FP_PERCENT.1' title '15 min.' with lines, \
	'fp_vs_t.INL_FP_PERCENT.2' title '15 min. w/counts' with lines, \
	'fp_vs_t.INL_FP_PERCENT.3' title '10 min.' with lines
