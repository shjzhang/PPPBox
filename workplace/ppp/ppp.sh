#!/bin/bash

proj="ppp"

##############################
# get the rinex file list
##############################
> $proj.rnxlist
get_rnx.sh -b "2011 10 1 0 0 0" -e "2011 10 3 0 0 0" -i 24 -t "IGS_OBS" -u "../../tables/url.list" -s "$proj.stalist" -l "$proj" -p "$HOME/data/IGS/data"  


##############################
# get the ephemeris file list
##############################
> $proj.ephlist
> $proj.clklist
> $proj.erplist
get_eph.sh -b "2011 10 1 0 0 0" -e "2011 10 3 0 0 0" -i 24 -t "type.list" -u "../../tables/url.list" -l "$proj" -p "$HOME/data/IGS/data" 


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
ls $HOME/data/IGS/data/*.msc | while read line
do
   echo $line;
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
ppp -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -m msc.txt -D $proj.dcblist -o $proj.outlist

