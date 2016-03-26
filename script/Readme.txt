=========================================================================================
get_data.sh: bash program for downloading the GNSS data

1.Usage:
  (1) bash get_data.sh -b [time] -e [time] -i [interval] -a [agency] -t [product type] 
      -s [station] -u [url list file]  -p [save path] -l [list prefix][Option]

      example: 
		bash get_data.sh -b "2012 1 1 0 0 0" -e "2012 1 3 0 0 0" -i 24 -a IGS 
-t EPH -u url.list -p ./ -d
		You can add the agency and its type,then let the merged parameter as the '-t' value
		bash get_data.sh -b "2012 1 1 0 0 0" -e "2012 1 3 0 0 0" -i 24 -t IGS_EPH -u url.list 


  (3) When the observation file need to be downloaded, you need use to the 's' parameter 
by providing the station name or the station list file. The 's' usage can be summar-
ized as following:
      bash get_data.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -i 24 -a IGS -t OBS -u 
url.list -p ./ -s ONSA -d
      bash get_data.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -i 24 -a IGS -t OBS -u 
url.list -p ./ -s station.list -d
      
  (4) Merge observation files: you can use the option "-m" 
      bash get_data.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -a IGS -t OBS -u 
url.list -p ./ -s ONSA -d -m
      The program will merge all the observation files which have the same station into
a single file.

  (5) Segment observation file: if we give the parameter 'i' a value which is less than
24, the program will accord to the 'i' to segment the observation file automatically.
      bash get_data.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -a IGS -i 12 -t OBS -u 
url.list -p ./ -s ONSA -d
     
2.Preparation: 
  (1) You need to copy the url list file to your bash program working directory;
  (2) You also need to install the GPSTk  or copy the "timeconvert" tool to your 
user's executive directory--"/usr/local/bin";
  (3) The teqc tool (published by UNAVCO) is also needed  when we need to segment or 
merge the observation files.
  (4) The CRX2RNX tool need to be installed when we need to download the RINEX files 
which use the Hatana format(.%Yd-->.%Yo). 

3.Other notes:
  (1) The option '-l' will make a file list of the relavent downloaded data, and the 
file's prefix is $l.
  (2) When the needed data product's downloading address isn't included in the url list
file, you need write this  downloading address as regulated format in the url list file.
  (3) When you want to segment or merge the observation files, the '-d' option must be
choosen.
  (4) The option '-i' and '-p' have the default value, they don't have to get the value
from the user, and the i's default value is 24h, the p's is current work path(./).


=========================================================================================

get_rnx.sh: Download the rinex observation data for the next ppp procedure

1. Usage:
   (1) bash get_rnx.sh -b [time] -e [time] -i [time interval] -a [agency] -s [station]
-u [url list] -p [save path][Option]

   example: 
	bash get_rnx.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -i 24 -a IGS -u url.list 
-s station.list -p ./rnx
	bash get_rnx.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -i 6 -a IGS -u url.list 
-s station.list -p ./rnx -l igs1781

2.Note:
   (1) Because we need to call the above bash program "get_data.sh", so we must refer to 
its rules.




=========================================================================================

get_eph.sh: Download ephemeris from IGS ftp for PPP positioning

1. Usage:
   (1) bash get_rnx.sh -b [time] -e [time] -i [time interval] -a [agency] -u [url list] 
-p [save path][Option]

   example: 
	bash get_eph.sh -b "2014 3 1 0 0 0" -e "2014 3 3 0 0 0" -a IGS -u url.list -t type.list 
-p ./eph

2.Note:
   (1) Because we need to call the above bash program "get_data.sh", so we must refer to
its rules.
	(2) The "type.list" is a prepared text document, its container may like "IGS_EPH","COD_DCB_P1P2" 
"ERP"(If you add the agency before the datatype, you need't add the option '-a' any more.)

