#!/bin/bash

ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/msc.txt msc.txt
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/URL_LIST.txt url.list
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08.atx igs08.atx

proj="igs16577"

# firstly, download the rnx files from IGS or IGS analysis center
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 24 -a IGS -u "url.list" -s "$proj.stalist.test" -l "$proj" -p "/Users/shjzhang/Documents/Data/IGS/data"              

# then, download the ephemeris files from IGS or IGS analysis center
get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 24 -a IGS -u "url.list" -t "type.list" -l "$proj" -p "/Users/shjzhang/Documents/Data/IGS/data"              

# convert ssc2msc
cat $proj.ssclist | while read line
do
   ssc2msc -s $line
done

rm msc.txt
# now, let's merge all the msc files together for pppar positioning
ls *.msc | while read line
do
  cat $line >> msc.txt
done

# Thirdly, create output file list
rm $proj.outlist
cat $proj.rnxlist | while read line
do
  echo "$line".out.fix.kin >> $proj.outlist
done

# ppp with ambiguity resolution
pppar -r $proj.rnxlist -s $proj.ephlist -k $proj.clklist -e $proj.erplist -u $proj.updlist -m msc.txt -o $proj.outlist
