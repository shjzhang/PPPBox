#!/bin/bash

ls /Users/shjzhang/Documents/Develop/gpstk/tables/* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $9}'`
   echo $file
   ln -f -s $line ./$file
done


ls /Users/shjzhang/Documents/Develop/gpstk/data/* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $9}'`
   echo $file
   ln -f -s $line ./data/$file
done

ln -f -s /Users/shjzhang/Documents/Data/IGS/products/igs17621.sp3 ./data/igs17621.sp3
ln -f -s /Users/shjzhang/Documents/Data/IGS/products/igs17622.sp3 ./data/igs17622.sp3
ln -f -s /Users/shjzhang/Documents/Data/IGS/products/igs17623.sp3 ./data/igs17623.sp3
ln -f -s /Users/shjzhang/Documents/Data/IGS/products/igs17627.erp ./data/igs17627.erp
ln -f -s /Users/shjzhang/Documents/Data/IGS/rinex/bjnm2880.13o ./data/bjnm2880.13o



ls /Users/shjzhang/Documents/Data/IGS/products/*1656* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/IGS/products/*1657* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/IGS/rinex/*16577.11o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/IGS/rinex/*2840.11o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/NGS/data1/*17687.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/NGS/data1/*3320.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done

ls /Users/shjzhang/Documents/Data/IGS/products/*1768* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/IGS/products/*1769* | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./products/$file
done

ls /Users/shjzhang/Documents/Data/NGS/data4/*17687.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -s $line ./rinex/$file
done
ls /Users/shjzhang/Documents/Data/NGS/data2/*17687.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done
ls /Users/shjzhang/Documents/Data/NGS/data2/*3330.13o | while read line
do
   echo $line
   file=`echo $line | awk -F / '{print $8}'`
   ln -f -s $line ./rinex/$file
done
