set terminal postscript eps
set output
set title 'Memory Usage vs. Time'
set xlabel 'ticks'
set ylabel 'Memory Usage vs. Time'
plot	'mem_vs_t.INL_MEMORY_USAGE.1' title 'run1' with lines, \
	'mem_vs_t.INL_MEMORY_USAGE.2' title 'run2' with lines, \
	'mem_vs_t.INL_MEMORY_USAGE.3' title 'run3' with lines
