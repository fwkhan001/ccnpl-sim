set terminal postscript eps;
set output;
set nokey; show key;
set xrange [0:50]; show xrange;
set xlabel "Matching Predicates (%)"; show xlabel;
set ylabel "Number of Messages"; show ylabel;
plot 'mmd.dat' with boxes
