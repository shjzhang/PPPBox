#!/bin/bash
#
# Input/output file setting
#

gmt gmtset FONT_ANNOT_PRIMARY 8p,Helvetica,black

# plot the stations using GMT command
gmt pscoast -R273/279/40.5/44.5 -JM273/12c -B2/1 -Dc -A -Gwhite -Scyan -Lf-82/40.8/42/100+lkm -I1/0.1p -X0.55i -Y0.55i -K -P > pppar.ngs1.ps
#gmt psbasemap -R243.5/245/32.2/33.6 -JM3.5 -B -P -K -O -V -X9.7 -Y8.4 >>

# write the signs and labels
gmt psxy -R  -JM -St0.2c -B  -Gred -K  -O  pppar.ngs1.llh >> pppar.ngs1.ps
gmt psxy -R  -JM -Si0.15c -B  -Gred -K  -O  pppar.ngs1.rover.llh >> pppar.ngs1.ps
gmt pstext  -R  -JM  -B  -Dj0.1c/0.1c -G -K -O pppar.ngs1.llh >> pppar.ngs1.ps
gmt pstext  -R  -JM  -B  -Dj0.1c/0.1c -G -O pppar.ngs1.rover.llh >> pppar.ngs1.ps

# change from ps to png
gmt ps2raster pppar.ngs1.ps -A -Tg


exit 0
