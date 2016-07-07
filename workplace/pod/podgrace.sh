#!/bin/bash

#
ln -f -s ../../tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s ../../tables/msc.txt msc.txt
ln -f -s ../../tables/URL_LIST.txt url.list
ln -f -s ../../tables/igs08.atx igs08.atx
ln -f -s ../../tables/receiver_bernese.lis recType.list
ln -f -s ../../tables/type.list type.list


proj="grace"

# firstly, download the rnx files from IGS or IGS analysis center
#provied by ESA
#get_rnx.sh -b "2011 11 23 0 0 0" -e "2011 11 24 0 0 0" -i 24 -a "IGS" -u "url.list" -s "$proj.stalist.test" -l "$proj" -p " /Users/whusgghwei/Desktop/Daily_Mac/gpstk_data/IGS/data" > get_rnx.log

## then, download the ephemeris files from IGS or IGS analysis center
get_eph.sh -b "2011 10 07 0 0 0" -e "2011 10 10 0 0 0" -i 24 -a "IGS" -u "url.list" -t "type.list" -l "$proj" -p " /Users/whusgghwei/Desktop/Daily_Mac/gpstk_data/IGS/data" > get_eph.log

# not needed as position different with time
## convert ssc2msc
#cat $proj.ssclist | while read line
#do
#   ssc2msc -s $line
#done

#rm msc.txt
## now, let's merge all the msc files together for pppar positioning
#ls *.msc | while read line
#do
#   cat $line >> msc.txt
#done

#rm $proj.outlist
#cat $proj.rnxlist | while read line
#do
#   echo "$line".out >> $proj.outlist
#done

# now, Let's perform the ppp positioning
podforgrace -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -m msc.txt -o $proj.outlist


