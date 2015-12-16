#!/bin/bash

ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/OCEAN-GOT00.dat OCEAN-GOT00.dat
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/msc.txt msc.txt
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/URL_LIST.txt url.list
ln -f -s /Users/shjzhang/Documents/Develop/gpstk/tables/igs08.atx igs08.atx

proj="igs16577"

# firstly, download the rnx files from IGS or IGS analysis center
get_rnx.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 24 -a IGS -u "url.list" -s "$proj.stalist.test" -l "$proj" -p "/Users/shjzhang/Documents/Data/IGS/data" > get_rnx.log 

# then, download the ephemeris files from IGS or IGS analysis center
get_eph.sh -b "2011 10 9 0 0 0" -e "2011 10 15 0 0 0" -i 24 -a IGS -u "url.list" -t "type.list" -l "$proj" -p "/Users/shjzhang/Documents/Data/IGS/data" > get_eph.log 

# exclude the bad file
rm $proj.rnxlist.good
cat $proj.rnxlist | while read line
do
    found=`awk -v file="$line" 'BEGIN{ i=0; } { if(file==substr($0,1)) { i=i+1; } } END  {print i;}' "$proj".rnxlist.bad `

    if [[ $found -eq 0 ]]
    then
       echo $line >> $proj.rnxlist.good
    fi
done

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

rm $proj.outlist
cat $proj.rnxlist.good | while read line
do
   echo "$line".out.04h.float.static >> $proj.outlist
done

# now, Let's perform the ppp positioning
ppp -r $proj.rnxlist.good -s $proj.ephlist -k $proj.clklist -e $proj.erplist -m msc.txt -o $proj.outlist


