#!/bin/bash

   #
   # Step: get the stations with receiver reporting p1/p2 code 
   #

:>pppar.ngs2.llh
while read line
do 

         # get station name with large character
       sta_name=`echo $line | awk '{print $1}' `

       x=`echo $line | awk '{print $2}' `
       y=`echo $line | awk '{print $3}' `
       z=`echo $line | awk '{print $4}' `

       llh=`poscvt --ecef=""$x" "$y" "$z"" -F "%A %L %h"`

       lat=`echo $llh | awk '{print $1}'`
       lon=`echo $llh | awk '{print $2}'`

       echo $lon $lat 8 0 4 CB $sta_name>> pppar.ngs2.llh

done < pppar.ngs2.coord
