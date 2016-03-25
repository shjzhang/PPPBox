#!/bin/bash

rm ppp_example.rnxlist
echo "./cagz2820.11o" >> ppp_example.rnxlist
echo "./cagz2830.11o" >> ppp_example.rnxlist
echo "./cagz2820.11o" >> ppp_example.rnxlist

rm ppp_example.ephlist
echo "./igs16566.sp3" >> ppp_example.ephlist
echo "./igs16570.sp3" >> ppp_example.ephlist
echo "./igs16571.sp3" >> ppp_example.ephlist
echo "./igs16572.sp3" >> ppp_example.ephlist
echo "./igs16573.sp3" >> ppp_example.ephlist
echo "./igs16574.sp3" >> ppp_example.ephlist
echo "./igs16575.sp3" >> ppp_example.ephlist
echo "./igs16576.sp3" >> ppp_example.ephlist

rm ppp_example.clklist
echo "./igs16566.clk_30s" >> ppp_example.clklist
echo "./igs16570.clk_30s" >> ppp_example.clklist
echo "./igs16571.clk_30s" >> ppp_example.clklist
echo "./igs16572.clk_30s" >> ppp_example.clklist
echo "./igs16573.clk_30s" >> ppp_example.clklist
echo "./igs16574.clk_30s" >> ppp_example.clklist
echo "./igs16575.clk_30s" >> ppp_example.clklist
echo "./igs16576.clk_30s" >> ppp_example.clklist

rm ppp_example.erplist
echo "./igs16567.erp" >> ppp_example.erplist
echo "./igs16577.erp" >> ppp_example.erplist


rm msc.txt
# convert ssc2msc
ssc2msc -s igs11P1656.ssc 
ssc2msc -s igs11P1657.ssc 
ssc2msc -s igs11P1658.ssc
# now, let's merge all the msc files together for pppar positioning
cat igs1656.msc >> msc.txt
cat igs1657.msc >> msc.txt
cat igs1658.msc >> msc.txt

rm ppp_example.outlist
echo "./cagz2820.11o.out" >> ppp_example.outlist
echo "./cagz2830.11o.out" >> ppp_example.outlist
echo "./cagz2840.11o.out" >> ppp_example.outlist

# now, Let's perform the ppp positioning
ppp -r ppp_example.rnxlist -s ppp_example.ephlist -k ppp_example.clklist -e ppp_example.erplist -m msc.txt -o ppp_example.outlist


