#!/bin/bash
#
# Input/output file setting
#

gmt gmtset FONT_ANNOT_PRIMARY 8p,Helvetica,black

# plot the stations using GMT command
gmt pscoast -R269/277/37/43 -JM273/12c -B2/1 -Dc -A -Gwhite -Scyan -Lf-84/37.5/40/100+lkm -I1/0.1p -X0.55i -Y0.55i -K -P > pppar.ngs2.ps
#gmt psbasemap -R243.5/245/32.2/33.6 -JM3.5 -B -P -K -O -V -X9.7 -Y8.4 >>

# write the signs and labels
gmt psxy -R  -JM -St0.2c -B  -Gred -K  -O  pppar.ngs2.llh >> pppar.ngs2.ps
gmt psxy -R  -JM -Si0.15c -B  -Gred -K  -O  pppar.ngs2.rover.llh >> pppar.ngs2.ps
gmt pstext  -R  -JM  -B  -Dj0.1c/0.1c -G -K -O pppar.ngs2.llh >> pppar.ngs2.ps
gmt pstext  -R  -JM  -B  -Dj0.1c/0.1c -G -O pppar.ngs2.rover.llh >> pppar.ngs2.ps

# change from ps to png
gmt ps2raster pppar.ngs2.ps -A -Tg


exit 0
