#!/bin/bash

mypath="./result"

if [ ! -d "$mypath" ];then
	mkdir $mypath
fi

rm rnxlist
echo "./gmsd0370.16o" >> rnxlist
echo "./jfng0370.16o" >> rnxlist
echo "./nnor0370.16o" >> rnxlist


rm ephlist
echo "./gbm18825.sp3" >> ephlist
echo "./gbm18826.sp3" >> ephlist
echo "./gbm18830.sp3" >> ephlist


rm clklist
echo "./gbm18825.clk" >> clklist
echo "./gbm18826.clk" >> clklist
echo "./gbm18830.clk" >> clklist

rm navlist
echo "./brdc0370.16g" >> navlist          

rm erplist
echo "./igs18827.erp" >> erplist
echo "./igs18837.erp" >> erplist


rm dcblist
echo "./P1C11602_RINEX.DCB" >> dcblist

rm outlist
echo "$mypath/gmsd0370.out.grce.kine" >> outlist
echo "$mypath/jfng0370.out.grce.kine" >> outlist
echo "$mypath/nnor0370.out.grce.kine" >> outlist

rm msc.txt
# convert ssc2msc
 ssc2msc -s igs16P1882.ssc 

# now, let's merge all the msc files  for ppp positioning
 cat igs1882.msc >> msc.txt

# now, Let's perform the ppp positioning
pppgnss -r rnxlist -s ephlist -e erplist  -k clklist  -g navlist -D dcblist -m msc.txt -o outlist

