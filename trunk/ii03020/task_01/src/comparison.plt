set terminal pngcairo size 1200,700 enhanced font 'Verdana,11'
set output 'comparison.png'
set encoding utf8
set datafile separator ','
set title 'Model comparison'
set xlabel 'step'
set ylabel 'y'
set grid
set key top left
plot 'linear_model.csv' every ::1 using 1:2 with linespoints lw 2 title 'Linear', \
     'nonlinear_model.csv' every ::1 using 1:2 with linespoints lw 2 title 'Nonlinear', \
     'sde_model.csv' every ::1 using 1:2 with linespoints lw 2 title 'SDE'
