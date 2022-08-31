set terminal png
set title "TLB Cost"
set xlabel "Number of Pages"
set ylabel "Time in Nanoseconds"
set output "graph.png"
set style data linespoints
plot "dataset" title "Cost"
