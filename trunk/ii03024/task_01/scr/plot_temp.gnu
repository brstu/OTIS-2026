set encoding utf8
set grid
set title "Модель 11 (Линейная)"
set xlabel "Шаг времени (tau)"
set ylabel "Выход y(tau)"
set datafile separator ","
plot 'Model11_Step.csv' skip 1 using 1:3 with lines linewidth 2 title 'y(tau)'
pause -1 "Нажмите Enter, чтобы закрыть график..."
