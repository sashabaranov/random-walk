set term postscript eps enhanced "Helvetica,24" adobeglyphnames
set out "graph.eps"
set encoding utf8


set autoscale
set xrange [0:6]
set yrange [0: 1100]
#set log y

set xlabel "Threads num."
set ylabel "Time, sec."


#set grid

set title "Time(Threads)"

#plot "points/10x10x10" using 1:2 t "10x10x10" smooth bezier linestyle 1 lw 10

#plot "points/5x5x5" using 1:2 t "5x5x5" smooth bezier linestyle 1 lw 6,\
#     "points/10x10x10" using 1:2 t "10x10x10" smooth bezier linestyle 2 lw 6,\
#     "points/20x20x20" using 1:2 t "20x20x20" smooth bezier linestyle 3 lw 6, \
#     "points/30x30x30" using 1:2 t "30x30x30" smooth bezier linestyle 4 lw 6
    
plot "points/time" using 1:2 notitle with linespoints lw 6
