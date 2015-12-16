#!/bin/bash
#
rm bad_station.*.txt
ls /Users/shjzhang/Documents/Data/IGS/data/*fix.kin.ttffL1 | while read line
do
   converg_time=` cat $line | awk '{print $1}' `

   if [[ $converg_time -gt 3600 ]]
   then
      line=`echo $line | sed "s/\.ttffL1//g"`
      echo $line >> bad_station.fix.kin.txt
      floatfile=`echo $line | sed "s/fix/float/g"`
      floatfile=`echo $line | sed "s/\.ttffL1//g"`
      echo $floatfile >> bad_station.float.kin.txt
   fi

done

ls /Users/shjzhang/Documents/Data/IGS/data/*fix.static.ttffL1 | while read line
do
   converg_time=` cat $line | awk '{print $1}' `

   if [[ $converg_time -gt 3600 ]]
   then

      line=`echo $line | sed "s/\.ttffL1//g"`
      echo $line >> bad_station.fix.static.txt
      floatfile=`echo $line | sed "s/fix/float/g"`
      floatfile=`echo $line | sed "s/\.ttffL1//g"`
      echo $floatfile >> bad_station.float.static.txt
   fi

done
