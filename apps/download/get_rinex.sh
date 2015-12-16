#!/bin/bash

#year=2013;
#
#   # Now, download the rinex data
#for (( d=236; d<=243; d++ ))
#do
#   
#   day=`echo "$d" | awk '{printf("%03s",$1)}'`
#
#      # download the rinex data 
##  wget --tries=10 -c -r -k -nd  -np -P ./rinex/ ftp://igs.ensg.ign.fr/pub/igs/data/$year/$day/*d.Z
#   wget --tries=10 -c -r -k -nd  -np -P ./rinex/ ftp://cddis.gsfc.nasa.gov:21//pub/gps/data/daily/$year/$day/13o/*o.Z
#
#done

wget --tries=10 -c -r -k -nd  -np -P ./rinex/ ftp://cddis.gsfc.nasa.gov:21//pub/gps/data/daily/2013/288/13o/bjnm*o.Z

#year=2013;
#
#   # Now, download the rinex data
#for (( d=236; d<=243; d++ ))
#do
#   
#   day=`echo "$d" | awk '{printf("%03s",$1)}'`
#
#      # download the rinex data 
##  wget --tries=10 -c -r -k -nd  -np -P ./rinex/ ftp://igs.ensg.ign.fr/pub/igs/data/$year/$day/*d.Z
#   wget --tries=10 -c -r -k -nd  -np -P ./rinex/ ftp://cddis.gsfc.nasa.gov:21//pub/gps/data/daily/$year/$day/13o/*o.Z
#
#done
/pub/gps/products/ionex/2011/282
