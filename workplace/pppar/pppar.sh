#!/bin/bash

##################################
# The project name 
##################################

proj="igs16577"

ln -f -s ../../script/get_data.sh ./get_data.sh
ln -f -s ../../script/get_eph.sh  ./get_eph.sh
ln -f -s ../../script/get_rnx.sh  ./get_rnx.sh

mypath="./data"
if [ ! -d "$mypath" ];then
    mkdir $mypath
fi

##################################
# get the rinex file list
##################################
rm  $proj.rnxlist
# firstly, download the rnx files from IGS or IGS analysis center
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 6 -a IGS -u "../../tables/url.list" -s "$proj.stalist.test" -l "$proj" -p "$mypath"

##################################
# get the ephemeris file list
##################################
rm $proj.ephlist
rm $proj.clklist
rm $proj.erplist
rm $proj.ssclist
# then, download the ephemeris files from IGS or IGS analysis center
get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 10 0 0 0" -i 6 -a IGS -u "../../tables/url.list" -t "type.list" -l "$proj" -p "$mypath"            

##################################
# Now, Let's prepare the upd file list
##################################
rm $proj.updlist
echo "./whu16577.p1p2.upd" >> $proj.updlist

##################################
# get the msc file list
##################################

# convert ssc2msc
cat $proj.ssclist | while read line
do
   ssc2msc -s $line
done

rm  msc.txt
# now, let's merge all the msc files together for pppar positioning
ls *.msc | while read line
do
  cat $line >> msc.txt
done

##################################
# set the output file list
##################################

rm  $proj.outlist
# Thirdly, create output file list
cat $proj.rnxlist | while read line
do
  echo "$line".out.fix.static >> $proj.outlist
done

# ppp with ambiguity resolution
pppar -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -u $proj.updlist -m msc.txt -o $proj.outlist

