#!/bin/bash


   awk '{ if( $2 ==333 ) print $3, $7}' inmd332-335.out.leica > inmd.trop
   awk '{ if( $2 ==333 ) print $3, $7}' insb332-335.out.leica > insb.trop
   awk '{ if( $2 ==333 ) print $3, $7}' incr332-335.out.leica > incr.trop
   
   gnuplot << EOF
   set terminal png font "arial"
   set output 'trop.png'
   set xlabel 'Time (Second)'
   set ylabel 'Ionospheric delay on L1 (m)'
   set xr[0:80000]
   set yr[0:0.2]
   set pointsize 0.5
   set size ratio 0.65 
   set grid
   plot "inmd.trop" using 1:2 with points pointtype 5 title "inmd" , \
        "insb.trop" using 1:2 with points pointtype 7 title "insb"   , \
        "incr.trop" using 1:2 with points pointtype 9 title "incr" 
   
EOF
