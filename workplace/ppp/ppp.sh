#!/bin/bash


proj="igs16577"

ln -f -s ../../script/get_data.sh ./get_data.sh
ln -f -s ../../script/get_eph.sh  ./get_eph.sh
ln -f -s ../../script/get_rnx.sh  ./get_rnx.sh

##############################
# get the rinex file list
##############################
> $proj.rnxlist
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 6 -a "IGS" -u "../../tables/url.list" -s "$proj.stalist.test" -l "$proj" -p "/Users/apple/Documents/data/IGS/data" > get_rnx.log 

##############################
# get the ephemeris file list
##############################
> $proj.ephlist
> $proj.clklist
> $proj.erplist
get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 6 -a "IGS" -u "../../tables/url.list" -t "type.list" -l "$proj" -p "/Users/apple/Documents/data/IGS/data" > get_eph.log 

##############################
# get the msc file list
##############################
# convert ssc2msc
cat $proj.ssclist | while read line
do
   ssc2msc -s $line
done

> msc.txt
# now, let's merge all the msc files together for pppar positioning
ls *.msc | while read line
do
   cat $line >> msc.txt
done

##############################
# get the output file list
##############################
> $proj.outlist
cat $proj.rnxlist | while read line
do
   echo "$line".out >> $proj.outlist
done

# now, Let's perform the ppp positioning
ppp -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -m msc.txt -o $proj.outlist

