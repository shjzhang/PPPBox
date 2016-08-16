#!/bin/bash

rm rnxlist
echo "./gmsd0320.16o" >> rnxlist
echo "./nnor0320.16o" >> rnxlist
echo "./mal20320.16o" >> rnxlist
echo "./mayg0320.16o" >> rnxlist
echo "./jfng0320.16o" >> rnxlist
echo "./ohi30320.16o" >> rnxlist
echo "./unb30320.16o" >> rnxlist
echo "./kour0320.16o" >> rnxlist
echo "./wtzr0320.16o" >> rnxlist


rm ephlist
echo "./gbm18820.sp3" >> ephlist
echo "./gbm18821.sp3" >> ephlist
echo "./gbm18822.sp3" >> ephlist



rm clklist
echo "./gbm18820.clk" >> clklist
echo "./gbm18821.clk" >> clklist
echo "./gbm18822.clk" >> clklist


rm navlist
echo "./brdc0320.16g" >> navlist

rm erplist
echo "./igs18827.erp" >> erplist

rm dcblist
echo "./P1C11602_RINEX.DCB" >> dcblist


rm outlist
echo "./result/gmsd0320.16o.out.grce.kine" >> outlist
echo "./result/nnor0320.16o.out.grce.kine" >> outlist
echo "./result/mal20320.16o.out.grce.kine" >> outlist
echo "./result/mayg0320.16o.out.grce.kine" >> outlist
echo "./result/jfng0320.16o.out.grce.kine" >> outlist
echo "./result/ohi30320.16o.out.grce.kine" >> outlist
echo "./result/unb30320.16o.out.grce.kine" >> outlist
echo "./result/kour0320.16o.out.grce.kine" >> outlist
echo "./result/wtzr0320.16o.out.grce.kine" >> outlist

rm msc.txt
# convert ssc2msc
 ssc2msc -s igs16P1882.ssc 

# now, let's mertemp all the msc files togrcether for pppar positioning
 cat igs1882.msc >> msc.txt

# now, Let's perform the ppp positioning
pppgnss -r rnxlist -s ephlist -e erplist  -k clklist -g navlist -D dcblist -m msc.txt -o outlist

