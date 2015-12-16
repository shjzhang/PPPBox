#!/bin/bash
#
#ls *2820.out | while read line
#do
#
#   awk '{print $3/3600, $4, $5, $6}' $line > $line.llh
#   awk '{print $3/3600.0, $9/$8*100.0, $10/$8*100.0 }' $line > $line.fixing
#
#   gnuplot << EOF
#   set terminal png
#   set output '${line}.static.png'
#   set xlabel 'hour'
#   set ylabel 'bias(m)'
#   set size 0.5, 0.3
#   set grid
#   set yr[-0.3:0.3]
#   plot "$line.llh" using 1:2  with lines title "dn", \
#        "$line.llh" using 1:3  with lines title "de", \
#        "$line.llh" using 1:4  with lines title "du"
#EOF
#
#   gnuplot << EOF
#   set terminal png
#   set output '$line.fixing.png'
#
#   set origin 0.1,0.1
#   set xlabel 'hour'
#   set ylabel 'fixing rate(%)'
#   set key bottom
#   set size 1,1
#   set grid
#   set yr[0:100]
#   plot  '$line.fixing' using 1:2  with lines title "widelane", \
#         '$line.fixing' using 1:3  with lines title "narrowlane"
#
#
#EOF
#
#
#done


#
# plot the ambiguity fixing rate
#
   gnuplot << EOF
   set terminal png
   set output 'adis2820.out.fixing.png'
   set size 0.48,0.48
   set multiplot

   set origin 0.0,0.0
   set title "adis"
   set title offset 0,-1
   set xlabel 'hour'
   set key bottom
   set noylabel 
   set grid
   set yr[0:100]
   plot  'adis2820.out.fixing' using 1:2  with lines title "wl", \
         'adis2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.45,0.0
   set title "cagz"
   set title offset 0,-1
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   set yr[0:100]
   plot  'cagz2820.out.fixing' using 1:2  with lines title "wl", \
         'cagz2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.0,0.45
   set size 0.48,0.43
   set title "bjnm"
   set title offset 0,-1
   set noxlabel
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   set yr[0:100]
   plot  'bjnm2820.out.fixing' using 1:2  with lines title "wl", \
         'bjnm2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.45,0.45
   set size 0.48,0.43
   set title offset 0,-1
   set key bottom
   set title "brus"
   set noylabel
   set grid
   set yr[0:100]
   plot  'brus2820.out.fixing' using 1:2  with lines title "wl", \
         'brus2820.out.fixing' using 1:3  with lines title "nl"

EOF

   gnuplot << EOF
   set terminal png
   set output 'gold2820.out.fixing.png'
   set size 0.48,0.48
   set multiplot

   set origin 0.0,0.0
   set title "gold"
   set title offset 0,-1
   set xlabel 'hour'
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   set yr[0:100]
   plot  'gold2820.out.fixing' using 1:2  with lines title "wl", \
         'gold2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.45,0.0
   set title "lhaz"
   set title offset 0,-1
   set key bottom
   set noylabel 
   set grid
   set yr[0:100]
   plot  'lhaz2820.out.fixing' using 1:2  with lines title "wl", \
         'lhaz2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.0,0.45
   set size 0.48,0.43
   set title "sydn"
   set title offset 0,-1
   set noxlabel
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   set yr[0:100]
   plot  'sydn2820.out.fixing' using 1:2  with lines title "wl", \
         'sydn2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.45,0.45
   set size 0.48,0.43
   set title offset 0,-1
   set key bottom
   set title "tnml"
   set noylabel
   set grid
   set yr[0:100]
   plot  'tnml2820.out.fixing' using 1:2  with lines title "wl", \
         'tnml2820.out.fixing' using 1:3  with lines title "nl"

EOF

   gnuplot << EOF
   set terminal png
   set output 'will2820.out.fixing.png'
   set size 0.48,0.48
   set multiplot

   set origin 0.0,0.0
   set title "will"
   set title offset 0,-1
   set xlabel 'hour'
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   set yr[0:100]
   plot  'will2820.out.fixing' using 1:2  with lines title "wl", \
         'will2820.out.fixing' using 1:3  with lines title "nl"

   set origin 0.45,0.0
   set title "wsrt"
   set title offset 0,-1
   set key bottom
   set noylabel 
   set grid
   set yr[0:100]
   plot  'wsrt2820.out.fixing' using 1:2  with lines title "wl", \
         'wsrt2820.out.fixing' using 1:3  with lines title "nl"


EOF

    ####
    #### output bias ploting
    ####

   gnuplot << EOF
   set terminal png
   set output 'cagz2820.out.png'
   set size 0.5,0.3
   set yr[-0.25:0.25]
   set ytics -0.2,0.1, 0.2
   set key bottom horizontal samplen 0.3 width 0
   set multiplot

   set origin 0.0,0.0
   set title "gold"
   set title offset +9,-2
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   plot  'gold2820.out.llh' using 1:2  with lines title "dn", \
         'gold2820.out.llh' using 1:3  with lines title "de", \
         'gold2820.out.llh' using 1:4  with lines title "du"

   set origin 0.0,0.24
   set title "sydn"
   set title offset +9,-2
   set key bottom
   set noxlabel 
   set key bottom
   set grid
   plot  'sydn2820.out.llh' using 1:2  with lines title "dn", \
         'sydn2820.out.llh' using 1:3  with lines title "de", \
         'sydn2820.out.llh' using 1:4  with lines title "du"

   set origin 0.0,0.49
   set title "adis"
   set title offset +9,-2
   set key bottom
   set noxlabel 
   set key bottom
   set grid
   plot  'adis2820.out.llh' using 1:2  with lines title "dn", \
         'adis2820.out.llh' using 1:3  with lines title "de", \
         'adis2820.out.llh' using 1:4  with lines title "du"

   set origin 0.0,0.74
   set title "bjnm"
   set title offset +9,-2
   set key bottom
   set noxlabel 
   set key bottom
   set grid
   plot  'bjnm2820.out.llh' using 1:2  with lines title "dn", \
         'bjnm2820.out.llh' using 1:3  with lines title "de", \
         'bjnm2820.out.llh' using 1:4  with lines title "du"

   set origin 0.5,0
   set title "lhaz"
   set title offset +9,-2
   set noxlabel
   set noylabel
   set key bottom
   set key bottom
   set grid
   plot  'lhaz2820.out.llh' using 1:2  with lines title "dn", \
         'lhaz2820.out.llh' using 1:3  with lines title "de", \
         'lhaz2820.out.llh' using 1:4  with lines title "du"

   set origin 0.5,0.24
   set title "tnml"
   set title offset +9,-2
   set noxlabel
   set noylabel
   set key bottom
   set key bottom
   set grid
   plot  'tnml2820.out.llh' using 1:2  with lines title "dn", \
         'tnml2820.out.llh' using 1:3  with lines title "de", \
         'tnml2820.out.llh' using 1:4  with lines title "du"

   set origin 0.5,0.49
   set title "cagz"
   set title offset +9,-2
   set noxlabel
   set noylabel
   set key bottom
   set key bottom
   set grid
   plot  'cagz2820.out.llh' using 1:2  with lines title "dn", \
         'cagz2820.out.llh' using 1:3  with lines title "de", \
         'cagz2820.out.llh' using 1:4  with lines title "du"

   set origin 0.5,0.74
   set title offset +9,-2
   set noxlabel
   set noylabel
   set key bottom
   set title "brus"
   set grid
   plot  'brus2820.out.llh' using 1:2  with lines title "dn", \
         'brus2820.out.llh' using 1:3  with lines title "de", \
         'brus2820.out.llh' using 1:4  with lines title "du"

EOF


   gnuplot << EOF
   set terminal png
   set output 'will2820.out.png'
   set size 0.5,0.4
   set yr[-0.25:0.25]
   set ytics -0.2,0.1, 0.2
   set key bottom horizontal samplen 0.3 width 0
   set multiplot

   set origin 0.0,0.0
   set title "will"
   set title offset +9,-2
   set xlabel 'hour'
   set ylabel "rate(%)"
   set ylabel offset +2,0
   set key bottom
   set grid
   plot  'will2820.out.llh' using 1:2  with lines title "dn", \
         'will2820.out.llh' using 1:3  with lines title "de", \
         'will2820.out.llh' using 1:4  with lines title "du"

   set origin 0.5,0.0
   set title "wsrt"
   set title offset +9,-2
   set key bottom
   set noylabel 
   set grid
   plot  'wsrt2820.out.llh' using 1:2  with lines title "dn", \
         'wsrt2820.out.llh' using 1:3  with lines title "de", \
         'wsrt2820.out.llh' using 1:4  with lines title "du"


EOF
