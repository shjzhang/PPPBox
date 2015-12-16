#!/bin/bash

# download p1c1bias data
#wget ftp://dgn6.esoc.esa.int/CC2NONCC/p1c1bias.hist

# download GPS satellite constellation file
#wget -r -c -k -nd -nc -np ftp://sideshow.jpl.nasa.gov/pub/gipsy_products/gipsy_params/PRN_GPS.gz

# download igs antenna file
wget --tries=5 -r -c -k -nd -np ftp://igs.ensg.ign.fr/pub/igs/igscb/station/general/igs08_1788.atx

#wget ftp://igs.org/pub/station/general/igs08_1788.atx 

# download GPS satellite constellation file from CODE
#wget --tries=5 -r -c -k -nd -np ftp://ftp.unibe.ch/aiub/BSWUSER52/GEN/SATELLIT.I08
