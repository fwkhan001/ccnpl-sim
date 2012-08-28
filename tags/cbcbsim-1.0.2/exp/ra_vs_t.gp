set terminal postscript eps
set output
set title 'RA Control Traffic vs. Time'
set xlabel 'time (ms)'
set yrange [0:110]
set ylabel 'RA Control Traffic %'
plot	'ra_vs_t.INL_CTRL_PKT_PCT.1' title 'No Change' with lines, \
	'ra_vs_t.INL_CTRL_PKT_PCT.2' title '10 min.' with lines, \
	'ra_vs_t.INL_CTRL_PKT_PCT.3' title '5 min.' with lines
