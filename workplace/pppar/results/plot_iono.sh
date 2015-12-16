#!/bin/bash


    
   i=18
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' inmd332-335.model > inmd."$i"
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' insb332-335.model > insb."$i"
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' incr332-335.model > incr."$i"
   
   gnuplot << EOF
   set terminal png font "arial"
   set output 'iono.$i.png'
   set xlabel 'Time (Second)'
   set ylabel 'Ionospheric delay on L1 (m)'
   set xr[70000:80000]
   set yr[-18:0]
   set pointsize 0.5
   set size ratio 0.65 
   set grid
   plot "inmd.$i" using 1:2 with points pointtype 5 title "inmd" , \
        "insb.$i" using 1:2 with points pointtype 7 title "insb"   , \
        "incr.$i" using 1:2 with points pointtype 9 title "incr" 
   
EOF

   i=21
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' inmd332-335.model > inmd."$i"
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' insb332-335.model > insb."$i"
   awk -v PRN=$i '{ if( $2 ==333 && $5==PRN ) print $3, $97}' incr332-335.model > incr."$i"
   
   gnuplot << EOF
   set terminal png font "arial"
   set output 'iono.$i.png'
   set xlabel 'Time (Second)'
   set ylabel 'Ionospheric delay on L1 (m)'
   set xr[70000:80000]
   set yr[-14:0]
   set pointsize 0.5
   set size ratio 0.65 
   set grid
   plot "inmd.$i" using 1:2 with points pointtype 5 title "inmd" , \
        "insb.$i" using 1:2 with points pointtype 7 title "insb"   , \
        "incr.$i" using 1:2 with points pointtype 9 title "incr" 
   
EOF

