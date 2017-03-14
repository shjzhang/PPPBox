#!/bin/bash
#

Help()
{
    echo "Example: plot -f ppp_albn0.out"
}

while getopts "f:h" arg                                                                                                                  
 do
     case $arg in
         f)  file=$OPTARG;;
         h)  Help
         exit 1;;
         *)  echo "Can not find the argument "$OPTARG""
         exit 1;;
     esac
done

NUMARGS=$#  #number of arguments

if [[ $NUMARGS -lt 1 ]]
then
    Help
    exit 1
fi

ls $file | while read line
do

  gnuplot << EOF
  set terminal png
  set output '${line}.png'
  set xlabel 'day'
  set ylabel 'diff(m)'
  set size 1.0, 1.0
  set grid
  set yrange[-1.5:1.5]
  plot "$line" using 1:2  with lines title "dn", \
       "$line" using 1:3  with lines title "de", \
       "$line" using 1:4  with lines title "du"
EOF

done

