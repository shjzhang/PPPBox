#!/bin/bash

ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/PRN_GPS PRN_GPS
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08.atx igs08.atx
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08_1771.atx igs08_1771.atx
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/p1c1bias.hist p1c1bias.hist

ls /Users/shjzhang/Documents/Data/IGS/products/*1768* | while read line
do
       echo $line
          file=`echo $line | awk -F / '{print $8}'`
             ln -f -s $line ./products/$file
done

ls ~/Documents/Data/NGS/data4/*17687* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done

ls ~/Documents/Data/NGS/data2/*17687* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/IGS/products/*1769* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./products/$file
done

ls ~/Documents/Data/NGS/data4/*3330.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done
ls ~/Documents/Data/NGS/data2/*3330.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done
