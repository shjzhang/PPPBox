#!/bin/bash

> pppar_example.rnxlist
echo "./cagz2831.11o" >> pppar_example.rnxlist
echo "./cagz2832.11o" >> pppar_example.rnxlist
echo "./cagz2833.11o" >> pppar_example.rnxlist
echo "./cagz2834.11o" >> pppar_example.rnxlist

echo "./brus2871.11o" >> pppar_example.rnxlist
echo "./brus2872.11o" >> pppar_example.rnxlist
echo "./brus2873.11o" >> pppar_example.rnxlist
echo "./brus2874.11o" >> pppar_example.rnxlist

> pppar_example.ephlist
echo "./igs16566.sp3" >> pppar_example.ephlist
echo "./igs16570.sp3" >> pppar_example.ephlist
echo "./igs16571.sp3" >> pppar_example.ephlist
echo "./igs16572.sp3" >> pppar_example.ephlist
echo "./igs16573.sp3" >> pppar_example.ephlist
echo "./igs16574.sp3" >> pppar_example.ephlist
echo "./igs16575.sp3" >> pppar_example.ephlist
echo "./igs16576.sp3" >> pppar_example.ephlist

> pppar_example.clklist
echo "./igs16566.clk_30s" >> pppar_example.clklist
echo "./igs16570.clk_30s" >> pppar_example.clklist
echo "./igs16571.clk_30s" >> pppar_example.clklist
echo "./igs16572.clk_30s" >> pppar_example.clklist
echo "./igs16573.clk_30s" >> pppar_example.clklist
echo "./igs16574.clk_30s" >> pppar_example.clklist
echo "./igs16575.clk_30s" >> pppar_example.clklist
echo "./igs16576.clk_30s" >> pppar_example.clklist

> pppar_example.erplist
echo "./igs16567.erp" >> pppar_example.erplist
echo "./igs16577.erp" >> pppar_example.erplist

> pppar_example.updlist
echo "./whu16577.p1p2.upd" >> pppar_example.updlist


> msc.txt
# convert ssc2msc
ssc2msc -s igs11P1656.ssc 
ssc2msc -s igs11P1657.ssc 
ssc2msc -s igs11P1658.ssc
# now, let's merge all the msc files together for pppar positioning
cat igs1656.msc >> msc.txt
cat igs1657.msc >> msc.txt
cat igs1658.msc >> msc.txt

> pppar_example.outlist
echo "./cagz2831.11o.fix.out" >> pppar_example.outlist
echo "./cagz2832.11o.fix.out" >> pppar_example.outlist
echo "./cagz2833.11o.fix.out" >> pppar_example.outlist
echo "./cagz2834.11o.fix.out" >> pppar_example.outlist

echo "./brus2871.11o.fix.out" >> pppar_example.outlist
echo "./brus2872.11o.fix.out" >> pppar_example.outlist
echo "./brus2873.11o.fix.out" >> pppar_example.outlist
echo "./brus2874.11o.fix.out" >> pppar_example.outlist

# now, Let's perform the ppp positioning
pppar -r pppar_example.rnxlist -s pppar_example.ephlist -k pppar_example.clklist -e pppar_example.erplist -u pppar_example.updlist -m msc.txt -o pppar_example.outlist


