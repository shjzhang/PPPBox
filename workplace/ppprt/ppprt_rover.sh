#!/bin/bash

rm rnxlist
echo "./rover.14o" >> rnxlist

rm ephlist
echo "igs18180.sp3"  >> ephlist

rm clklist
echo "igs18180.clk_30s" >> clklist

rm erplist
echo "./igs18187.erp" >> erplist
echo "./igs18177.erp" >> erplist

rm outlist
echo "./rover.14o.ppprt.out" >> outlist

rm dcblist
echo "P1C11411_RINEX.DCB" >> dcblist

ppprt -r rnxlist -s ephlist -e erplist -k clklist -D dcblist -o outlist
