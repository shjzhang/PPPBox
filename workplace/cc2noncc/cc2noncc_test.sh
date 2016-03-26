#!/bin/bash
# ===============================================================
#
# Description
#
# Bash for testing the cc2noncc tool
# We will read a set of RINEX files, in these files, the receiver
# has different code observation type(C1P1, C1X2, P1P2).Then use
# two kinds of cc2noncc tool to convert "cc' to "noncc", so that
# we can compare two tools' effect.
#
# Preparation
#
# Before test, you need prepare the cc2noncc tool created by Q.Liu
# and another cc2noncc.bin tool created by Jim Ray.
#
# Q.Liu, 2015-11-20
# ===============================================================
Main()
{
	cat $rnxlist | while read line
	do
		infile=$line
		outfile1="$infile"".out1"
		outfile2="$infile"".out2"
		outfile3="$infile"".out3"
		Process
	done	
}

Process()
{
	cc2noncc -i $infile -r $recTypeFile -D $P1C1DCBFile -o $outfile1    # by LQ
   rnxfilter -i $outfile2 -o $outfile3 # read rinex data and sort them according to the SAT ID
}

rnxlist="./igs1260.rnxlist"
recTypeFile="./receiver_bernese.lis"
p1c1bias="./p1c1bias.hist"
P1C1DCBFile="/home/ww/P1C11404.DCB"

Main
