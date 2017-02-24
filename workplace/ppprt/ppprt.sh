#!/bin/bash

rm rnxlist
echo "./onsa0190.15o" >> rnxlist
echo "./medi0190.15o" >> rnxlist
echo "./sofi0190.15o" >> rnxlist

rm ephlist
echo "igs18280.sp3"  >> ephlist
echo "igs18281.sp3"  >> ephlist
echo "igs18282.sp3"  >> ephlist

rm clklist
echo "igs18280.clk_30s" >> clklist
echo "igs18281.clk_30s" >> clklist
echo "igs18282.clk_30s" >> clklist

rm erplist
echo "./igs18287.erp" >> erplist


rm dcblist
echo "P1C11501.DCB" >> dcblist
rm msc.txt
ssc2msc -s igs15P1828.ssc
cat igs1828.msc >> msc.txt

ppprt -r rnxlist -s ephlist -k clklist -e erplist -m msc.txt  -D dcblist 
