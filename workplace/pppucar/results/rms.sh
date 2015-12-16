#!/bin/bash
#

# exclude the bad file
ls /Users/shjzhang/Documents/Data/IGS/data/*fix.kin > fix.kin.filelist
rm fix.kin.filelist.good
cat fix.kin.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.fix.kin.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> fix.kin.filelist.good
    fi
done

rm fix.kin.north.rms
rm fix.kin.east.rms
rm fix.kin.up.rms
cat fix.kin.filelist.good | while read line
do

   awk '{if(NF>10) { if($11==1) print $4}}' $line >> fix.kin.north.rms
   awk '{if(NF>10) { if($11==1) print $5}}' $line >> fix.kin.east.rms
   awk '{if(NF>10) { if($11==1) print $6}}' $line >> fix.kin.up.rms

done


# exclude the bad file
ls /Users/shjzhang/Documents/Data/IGS/data/*fix.static> fix.static.filelist
rm fix.static.filelist.good
cat fix.static.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.fix.static.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> fix.static.filelist.good
    fi
done

rm fix.static.north.rms
rm fix.static.east.rms
rm fix.static.up.rms
cat fix.static.filelist.good | while read line
do

   awk '{if(NF>10) { if($11==1) print $4}}' $line >> fix.static.north.rms
   awk '{if(NF>10) { if($11==1) print $5}}' $line >> fix.static.east.rms
   awk '{if(NF>10) { if($11==1) print $6}}' $line >> fix.static.up.rms

done


# exclude the bad file
ls /Users/shjzhang/Documents/Data/IGS/data/*float.static> float.static.filelist
rm float.static.filelist.good
cat float.static.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.float.static.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> float.static.filelist.good
    fi
done

rm float.static.north.rms
rm float.static.east.rms
rm float.static.up.rms
cat float.static.filelist.good | while read line
do

   awk '{if(NF>10) { if($9==1) print $4}}' $line >> float.static.north.rms
   awk '{if(NF>10) { if($9==1) print $5}}' $line >> float.static.east.rms
   awk '{if(NF>10) { if($9==1) print $6}}' $line >> float.static.up.rms

done


# exclude the bad file
ls /Users/shjzhang/Documents/Data/IGS/data/*float.kin> float.kin.filelist
rm float.kin.filelist.good
cat float.kin.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.float.kin.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> float.kin.filelist.good
    fi
done

rm float.kin.north.rms
rm float.kin.east.rms
rm float.kin.up.rms
cat float.kin.filelist.good | while read line
do
   awk '{if(NF>10) { if($9==1) print $4}}' $line >> float.kin.north.rms
   awk '{if(NF>10) { if($9==1) print $5}}' $line >> float.kin.east.rms
   awk '{if(NF>10) { if($9==1) print $6}}' $line >> float.kin.up.rms
done

