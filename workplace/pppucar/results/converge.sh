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

ls /Users/shjzhang/Documents/Data/IGS/data/*fix.static > fix.static.filelist
rm fix.static.filelist.good
cat fix.static.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.fix.static.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> fix.static.filelist.good
    fi
done


ls /Users/shjzhang/Documents/Data/IGS/data/*float.static > float.static.filelist
rm float.static.filelist.good
cat float.static.filelist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "bad_station.float.static.txt"`

    if [[ $found -eq 0 ]]
    then
       echo $line >> float.static.filelist.good
    fi
done


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

rm converge.fix.kin.txt
cat fix.kin.filelist.good | while read line
do
   cat "$line".ttffL1 >> converge.fix.kin.txt 
done

rm converge.fix.static.txt
cat fix.static.filelist.good | while read line
do
   cat "$line".ttffL1 >> converge.fix.static.txt 
done

rm converge.float.kin.txt
cat float.kin.filelist.good | while read line
do
   cat "$line".ttfc >> converge.float.kin.txt 
done

rm converge.float.static.txt
cat float.static.filelist.good | while read line
do
   cat "$line".ttfc >> converge.float.static.txt 
done

