#!/bin/bash
#
# Input/output file setting
#

gmt gmtset FONT_ANNOT_PRIMARY 8p,Helvetica,black

# plot the stations using GMT command
gmt pscoast -R272/276/38/41 -JM274/8c -Ba1/a1f0.5 -Dc -A -Gwhite -Scyan -Lf-85/38.3/40/50+lkm -I1/0.1p -X0.55i -Y0.55i -K -P > pppar.ngs2.lr.ps
#gmt psbasemap -R243.5/245/32.2/33.6 -JM3.5 -B -P -K -O -V -X9.7 -Y8.4 >>

# write the signs and labels
gmt psxy -R  -JM -St0.25c -B  -Gred -K  -O  pppar.ngs2.lr.llh >> pppar.ngs2.lr.ps
gmt psxy -R  -JM -Si0.25c -B  -Gred -K  -O  pppar.ngs2.rover.lr.llh >> pppar.ngs2.lr.ps
gmt pstext  -R  -JM  -B  -Dj0.2c/0.2c -G -K -O pppar.ngs2.lr.llh >> pppar.ngs2.lr.ps
gmt pstext  -R  -JM  -B  -Dj0.2c/0.2c -G -O pppar.ngs2.rover.lr.llh >> pppar.ngs2.lr.ps

# change from ps to png
gmt ps2raster pppar.ngs2.lr.ps -A -Tg


exit 0
