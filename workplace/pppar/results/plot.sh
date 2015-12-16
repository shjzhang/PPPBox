#!/bin/bash
#
ls *04h.float.static | while read line
do

   awk '{if(NF>10) {print $3, $4, $5, $6}}' $line > tempfile
#  awk '{if(NF>10) {print $2+$3/86400.0, $8, $9, $10} }' $line > $line.fixing

   gnuplot << EOF
   set terminal png
   set output '${line}.png'
   set xlabel 'day'
   set ylabel 'diff(m)'
   set size 1.0, 1.0
   set grid
   set yr[-1:1]
   plot "tempfile" using 1:2  with linespoints title "dn", \
        "tempfile" using 1:3  with linespoints title "de", \
        "tempfile" using 1:4  with linespoints title "du"
EOF

#   gnuplot << EOF
#   set terminal png
#   set output '$line.fixing.png'
#   set xlabel 'hour'
#   set ylabel 'bias(m)'
#   set grid
#   plot  '$line.fixing' using 1:2  with lines title "total", \
#         '$line.fixing' using 1:3  with lines title "widelane", \
#         '$line.fixing' using 1:4  with lines title "narrowlane"
#   
#EOF
rm tempfile

done
