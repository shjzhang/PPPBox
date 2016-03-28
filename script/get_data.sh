#!/bin/bash
# ==========================================================
# Program Description
#
# Function:
# Download GNSS data and products
#
# Warning:
# We already have several GNSS data products' URL in URL_LIST.txt,
# and we must install the toolkit called GPSTk as the same time so 
# that we can use some tools such as "timeconvert", or you can copy
# the executive program "timeconvert" to your user directory "/usr/
# local/bin". 
# When you need to segment or merge the observation file, you 
# should prepare the teqc tool;You need install the CRX2RNX tool 
# When the RINEX files need to be converted from Hatanaka compact 
# format to standard RINEX.
# 
# Copyright (c)  
# Q.Liu, Wuhan University, 2015
#
# ==========================================================
# Modifications
#
# 2015/10/08
# The former program could download the data in the same month and 
# same year only, now this limit is removed.
#
# 2015/10/21
# Added the function that we can download the observation files 
# according to a station list file.
#
# 2015/10/27
# Added the function that we can segment a whole observation file to 
# several small files according the segmentation interval, starting
# time and ending time by using the tools teqc(published by UNAVCO).
# 
# 2015/10/29
# Added several functions: merge observation files; write the files' 
# name list into a text file; extend the time span when the product 
# type is CLK, EPH and ERP.
#
# 2016/03/10
# Normalized this program.
# ==========================================================

# ============================
# Print the help message
# ============================
Help()
{
	echo "get_data.sh,下载GNSS产品的bash程序"
	echo "用法: bash get_data.sh [起始时间][终止时间][时间间隔][机构][产品类型][URL路径]...[选项]"
	echo "参数说明："
	echo "  -h,  --help                       获取帮助信息"
	echo "  -b,  --bigining of time span      需要的产品的起始时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -e,  --ending of time span        需要的产品的终止时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -i,  --interval of time span      时间间隔(单位：小时,必须为整数,默认为24)"
	echo "  -a,  --agency of GNSS product     发布GNSS产品的机构(IGS,IGR,IGU,COD,ESA,JPL,etc)"
	echo "  -s,  --station's name or list file 测站名或者测站列表文件"
	echo "  -t,  --type of product            下载的产品类型(OBS,EPH,CLK,ERP,etc,IGS_OBS is also OK,needn't -a)"	
	echo "  -u,  --the path of url_list file  保存有各种产品的url列表文件路径"
	echo "  -p,  --the data save path         下载数据保存路径(默认为当前路径./)"
	echo "  -l,  --the preffix of list file   保存下载数据文件名的文件前缀"
	echo "  -d,  --decompress                 选择是否解压,如果带有-d选项,则解压文件;若未带有-d选项,则不解压"
	echo "  -m,  --merge the rinex files      选择是否将同一测站的观测数据进行合并"

	echo "示例:bash get_data.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -t OBS -s COCO -u url.list -p ./data -d"
	echo "     bash get_data.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 6 -t IGS_OBS -s sta.list -u url.list -p ./data -d"
	echo "     bash get_data.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -t IGS_EPH -u url.list -p ./data -d -l igs1781"
	exit 1;
}

# ============================
# Assign the parameter with the revalent value according input data
# ============================
ParaAssign()
{
		# Assign the value for the time parameters
		# ( Y m d h M s ) stands for (year month day
		# hour minute second)
	Y1=$(echo $b | awk '{print $1}')
	m1=$(echo $b | awk '{print $2}')
	d1=$(echo $b | awk '{print $3}')
	h1=$(echo $b | awk '{print $4}')
	M1=$(echo $b | awk '{print $5}')
	S1=$(echo $b | awk '{print $6}')
	Y2=$(echo $e | awk '{print $1}')
	m2=$(echo $e | awk '{print $2}')
	d2=$(echo $e | awk '{print $3}')
	h2=$(echo $e | awk '{print $4}')
	M2=$(echo $e | awk '{print $5}')
	S2=$(echo $e | awk '{print $6}')

		# Give the following parameters default value
	n=0	 #day of year
	W=0	 #gpsweek
	D=0	 #day of week

		# Get the absolute path of savefile
	local workplace=$(pwd)
	if [[ ! -d $savepath ]]
	then
		mkdir $savepath
	fi
	cd $savepath
	savepath=$(pwd)"/"
	cd $workplace
} # The end of ParaAssign()

# ============================
# Read the station list from station list file.
# ============================
ReadStation()
{
	local i=0
	while read line
	do
		local strlen=`echo "$line" | awk '{print length($0)}'`
		if [[ $strlen -eq 4 ]]
		then
			StationList[i]=$line
			let i=$i+1
		fi
	done
} # The end of ReadStation()

# ============================
# Judge the validity of product type.
# ============================
TypeJudge()
{
	local declare agencyArr
	local agentemp=$(echo $product_type | awk -F '_' '{print $1}')
	agencyArr=("BKG" "COD" "EMR" "ESA" "GFZ" "GRG" "GSI"
           "IAC" "IERS" "IGR" "IGRE" "IGRS" "IGS" "IGSE"
           "IGSS" "IGU" "IGUE" "IGUS" "JAXA" "JPL" "MCC" 
			  "MGEX" "MIT" "NGS" "RTPP" "VMF") 
	if [[ -n `echo ${agencyArr[@]} | grep -w $agentemp` ]] 
	then 
		agency=$agentemp 
		product_type=$(echo ${product_type#*_})
	else
		agency=$agencyini
		if [[ -z $agency ]]
		then 
			echo "Must give "agency" a right value"
			exit 1
		fi
	fi
} # The end of TypeJudge()

# ============================
# Judge the validity of arguments
# ============================
ArgJudge()
{
	if [[ $argnum -le 1 ]]
	then
		Help
	fi
		# The parameter 'b','e','a','u' must be given
	if [[ $b = "" ]]
	then
		echo "Must give parameter 'b' a value"
		exit 1
	fi
	if [[ $e = "" ]]
	then
		echo "Must give parameter 'e' a value"
		exit 1
	fi
	if [[ $u = "" ]]
	then
		echo "Must give parameter 'u' a value"
		exit 1
	fi

		# If not give the "intv" and "savepath" value, we give them the
		# default value.
	if [[ $segment_intv = "" ]]
	then
		intv=24
		segment_intv=24
	fi
	if [[ $segment_intv -le 0 ]]
	then 
		echo "The parameter \"i\" must be a nonzero integer!"
		exit 1
	fi
	if [[ -z $savepath ]]
	then 
		savepath="./"
	fi

	if [[ ${savepath: -1} != "/" ]]
	then
		savepath=$savepath"/"
	fi

	TypeJudge
		# Judge if the parameter 's' is null when the pruduct type is "OBS"
	if [[ $product_type = "OBS" ]] && [[ -z $s ]]
	then
		echo "The station name is not provided!"
		exit 1
	fi
} # The end of ArgJudge()

# ============================
# Calculate the number of time span
# ============================
CalTimeSpan()
{
		# Calculate the e's julian date
	Date2=($Y2 $m2 $d2 $h2 $M2 $S2)
	CT2JD ${Date2[@]}
	jd2=${JD%.*}

		# Calculate the b's julian date
	Date1=($Y1 $m1 $d1 $h1 $M1 $S1)
	CT2JD ${Date1[@]}
	jd1=${JD%.*}

		# Calculate the number of time span
	let day_num=jd2-jd1

		# Judge if the ending time is bigger than the starting time.
	if [[ $day_num -le 0 ]]
	then
		echo "Error: The ending time must bigger than the starting time!"
		exit 1
	fi
		# When the data type is "EPH", "CLK", we need to extend one day foward and backward.
	if [[ $product_type = "EPH" ]] || [[ $product_type = "CLK" ]] || [[ $product_type = "CLK_30S" ]]
	then
		let day_num=$day_num+2
		JD=$(awk 'BEGIN{printf("%.1f\n",'$JD-1')}')
	fi
	JD1=$JD
} # The end of CalTimeSpan()

# ============================
# Calculate the number of interval
# ============================
CalIntvNum()
{
		# When the data type is "ERP", we need to extend one week foward and backward.
	intv=24
	if [[ $product_type = "ERP" ]] || [[ $product_type = "SSC" ]]
	then
		let day_num=$day_num+14
		JD=$(awk 'BEGIN{printf("%.1f\n",'$JD-7')}')
		let intv=24*7
	fi
	let intv_num=(day_num*24+h2-h1)/intv
	let hour_remain=day_num*24+h2-h1-intv*intv_num
	if [[ $hour_remain -ne 0 ]]
	then
		let intv_num=$intv_num+1
	fi
} # The end of CalIntvNum()

# ============================
# Create the ftp address which can download GNSS data products 
# ============================
URLMatch()
{
		# Convert the lower letter to the upper case
	agency=$(echo $agency | tr '[a-z]' '[A-Z]')
		# Link the angency and product type 
	local str="$agency"_"$product_type"

		# Locate the str in given file
	if [[ -n u ]] && [[ -f $u ]]
	then
		local loc=$(grep -w $str $u) 
	else
		echo "u:$u is not a valid file."
	fi
	if [[ -z $loc ]]
	then
			# if can't find the str in the file
		echo "所输入的机构($agency)或产品类型参数($product_type)在本列表中无效"
		exit 1
	fi
    
		# Replace the parameter by its value
	url=$(echo $loc| awk '{print $2}')
	[[ -n $Y ]] && [[ $url =~ "%Y" ]] && url=${url//%Y/$Y} 
	[[ -n $y ]] && [[ $url =~ "%y" ]] && url=${url//%y/$y} 
	[[ -n $m ]] && [[ $url =~ "%m" ]] && url=${url//%m/$m} 
	[[ -n $d ]] && [[ $url =~ "%d" ]] && url=${url//%d/$d} 
	[[ -n $h ]] && [[ $url =~ "%h" ]] && url=${url//%h/$h} 
	[[ -n $H ]] && [[ $url =~ "%H" ]] && url=${url//%H/$H} 
	[[ -n $M ]] && [[ $url =~ "%M" ]] && url=${url//%M/$M} 
	[[ -n $n ]] && [[ $url =~ "%n" ]] && url=${url//%n/$n} 
	[[ -n $W ]] && [[ $url =~ "%W" ]] && url=${url//%W/$W}
	[[ -n $D ]] && [[ $url =~ "%D" ]] && url=${url//%D/$D} 
	[[ -n $s ]] && [[ $url =~ "%s" ]] && url=${url//%s/$s} 
	[[ -n $w ]] && [[ $url =~ "%w" ]] && url=${url//%w/$w} 
} # The end of URLMatch()

# ============================
# Convert the time to the needed format
# ============================
TimeConvert()
{
		# Use the timeconvert tool to convert julian date to 
		# the needed time  	
	local gpsweek=$(timeconvert -j "$JD" -F "%F")
	  
		# Judge if the timeconvert tool exists.
   if [[ $? -ne 0 ]]
	then
		 echo "Failed: timeconvert tool doesn't exist!"
		 echo "Please install the timeconvert tool in advance."
	fi
	local doy=$(timeconvert -j "$JD" -F "%j")
	local dow=$(timeconvert -j "$JD" -F "%w")

		# Convert Year(4 digits) to year(2 digits)
	y=${Y:2:2}

	n=$doy
	W=$gpsweek
	D=$dow
	local gpsweek1=$(timeconvert -c "1 1 $Y 0:0:0" -F "%F")
	local woy=0
	let woy=$gpsweek-$gpsweek1+1  # week of year

		# Calculate the length of the parameter string
	local doylen=`echo "$n" | awk  '{print length($0)}'`
	local mlen=`echo "$m" | awk  '{print length($0)}'`
	local dlen=`echo "$d" | awk  '{print length($0)}'`
	local hlen=`echo "$h" | awk  '{print length($0)}'`
	local Mlen=`echo "$M" | awk  '{print length($0)}'`
	local Slen=`echo "$S" | awk  '{print length($0)}'`
	local woylen=`echo "$woy" | awk  '{print length($0)}'`

		# When the length of n,m,d,h and M is 1 or 2,
		# add the 0 to its front position
	if [[ $mlen -eq 1 ]]
	then
		m="0"$m
	fi	
	if [[ $dlen -eq 1 ]]
	then
		d="0"$d
	fi	
	if [[ $hlen -eq 1 ]]
	then
		h="0"$h
	fi	
	if [[ $Mlen -eq 1 ]]
	then
		M="0"$M
	fi
	if [[ $Slen -eq 1 ]]
	then
		S="0"$S
	fi	
	if [[ $doylen -eq 1 ]]
	then
		n="00"$n
	fi	
	if [[ $doylen -eq 2 ]]
	then
		n="0"$n
	fi
	if [[ $woylen -eq 1 ]]
	then
		woy="0"$woy
	fi
	w=$woy
} # The end of TimeConvert()

# ============================
# Convert civil time to julian date
# ============================
CT2JD()
{
	Y=$1
	m=$2
	d=$3
	h=$4
	M=$5
	S=$6
	JD=$(timeconvert -c "$m $d $Y $h:$M:$S" -F "%J")
} # The end of CT2JD()

# ============================
# Convert julian date to civil time
# ============================
JD2CT()
{
	Y=$(timeconvert -j "$JD" -F "%Y")
	m=$(timeconvert -j "$JD" -F "%m")
	d=$(timeconvert -j "$JD" -F "%d")
	h=$(timeconvert -j "$JD" -F "%H")
	M=$(timeconvert -j "$JD" -F "%M")
	S=$(timeconvert -j "$JD" -F "%S")
} # The end of JD2CT()

# ============================
# Encode the hour from number to alphabet, 0-a,1-b,...,23-x
# ============================
HourCode()
{
	local temp=0
	let temp=$h+97
	temp=$(echo "ibase=10;obase=16;$temp"|bc )
	H=$(echo -e "\\x$temp")
} # The end of HourCode()

# ============================
# Download data and save it
# ============================
Download()
{
		# Extract data's downloading filename
	compressed_file=$(echo $url|awk -F '/' '{print $NF}')
	zfile="$savepath""$compressed_file"

		# Wipe off the compressed file's suffix ".Z"
	filename=$(echo $compressed_file | cut -d "." -f 1,2)
		
	savefile="$savepath""$filename"
	   # If the observation's format is 31(Hatanaka Compact RINEX),
	   # we should use CRX2RNX tool to convert its format.
	if [[ ${filename: -1} = "d" ]] && [[ $product_type = "OBS" ]]
	then
	      # Change the filename's suffix(.yyd to .yyo)
		local file_suffix_tail=`echo $filename | cut -c 12-`
		local file_suffix_head=`echo $filename | cut -c -11`
		if [[ $file_suffix_tail = "d" ]]
		then
			file_suffix_tail='o'
			filename2="$file_suffix_head""$file_suffix_tail"
		fi

		savefile2="$savepath""$filename2"

	fi

	if [[ -e $savefile ]]
	then
		echo "The file $savefile already exists!"
   elif [[ $product_type = "OBS"  ]] && [[ -e $savefile2 ]]
	then
		echo "The file $savefile2 already exists!"
		savefile=$savefile2
	else
		wget -c -P $savepath $url
	fi
} # The end of Download()

# ============================
# Decompress the downloaded data file
# ============================
Decompress()
{
	compressed_file=$(echo $url|awk -F '/' '{print $NF}')
	   # Get the data file's suffix
	local suffix=${compressed_file##*.}
	
	   # Judge if the "compressed_file" is a file and a compressed file
	if [[ -f $zfile ]] || [[ -e $savefile ]]
	then
		if [[ $suffix = "Z" ]] || [[ $suffix = "gz" ]]
		then	
         if [[ ! -e $savefile ]]
         then
			   gzip -d "$zfile"
         fi
		fi

	   # If the observation's format is 31(Hatanaka Compact RINEX),
	   # we should use CRX2RNX tool to convert its format.
	if [[ ${filename: -1} = "d" ]] && [[ $product_type = "OBS" ]]
	then
		if [[ ! -e $savefile2 ]]
		then
            # Convert
	      CRX2RNX "$savefile"

	         # Judge if the CRX2RNX tool exists.
         if [[ $? -ne 0 ]]
	      then
		      echo "Failed: CRX2RNX tool doesn't exist!"
		      echo "Please install the CRX2RNX tool in advance."
	      fi
      fi
         # Now, observation file name has already changed to '*.yyo'
      filename=$filename2
      savefile=$savefile2
	fi
	fi
} # The end of Decompress()

# ============================
# Segment the observation file
# ============================
Segment()
{
		# "workplace" remebers current work directory
	local workplace=$(pwd)
	cd $savepath     # Go to the file's savepath

	local seg_num=0
	local list_num=0
	local time_quantum=0
	local segfilename1=0
	local segfilename2=0
	let seg_num=24/segment_intv

		# divide the filename into the file_head and file_tail
	local file_tail=`echo $filename | cut -c 9-`
	local file_head=`echo $filename | cut -c -7`

		# Convert the julian date to the civil time
	JD2CT
	if [[ -e $filename ]]
	then
		for ((f=0;f<seg_num;++f))
		do
				# "list_num" means the current segmentation file's identifier 
				# during all the segmentation files which have the same station
			let list_num=j*seg_num*intv_num+f+i*seg_num
			TimeConvert
			local start_time="$Y$m$d$h$M$S"
			let time_quantum=$f+1
			if [[ $f+1 -lt 10 ]]
			then
					# Call the teqc toolkit to segment the observation file
				segfilename1="$file_head""$time_quantum""$file_tail"
				if [[  -e $segfilename1 ]]
				then
					echo "The file $savepath$segfilename1 already exists!"
				else
					teqc -st $start_time +dh $segment_intv $filename>"$segfilename1"  
	  
					  # Judge if the teqc tool exists.
					if [[ $? -ne 0 ]]
					then
						echo "Failed: teqc tool doesn't exist!"
						echo "Please install the teqc tool in advance."
					fi
				fi
					# Push the segmentation file's name into the FileList array.
				FileList[list_num]="$savepath""$file_head""$time_quantum""$file_tail"
			else
					# when the "time_quantum" bigger than 9, we code it as "10-A,11-B,..."
				let time_quantum=$time_quantum+55
				local temp=$(echo "ibase=10;obase=16;$time_quantum"|bc)
				local time_code=$(echo -e "\\x$temp")
				segfilename2="$file_head""$time_code""$file_tail"

				if [[ -e $segfilename2 ]]
				then
					echo "The file $savepath$segfilename2 already exists!"
				else
					teqc -st $start_time +dh $segment_intv $filename>"$segfilename2"  
				fi
				FileList[list_num]="$savepath""$file_head""$time_code""$file_tail"
			fi
			local h_head=`echo $h | cut -c -1`
			if [[  $h_head = 0 ]]
			then
				h=`echo $h | cut -c 2-`
			fi
			let h=$h+$segment_intv
		done
	else
		echo "Can't segment! The file $filename doesn't exist."
	fi
		# Return the workplace
	cd $workplace
} # The end of Segment()

# ============================
# Store the downloaded file which has the same data type into a text file.
# Observation file need having the same station.
# ============================
MakeList()
{
		# The list file gets the suffix  
	if [[ -n $project ]] 
	then
		# If the file *.$listname already exists, then we need to remove it
	if [[ $product_type = "OBS" ]] && [[ -e "$project.rnxlist" ]]
	then
		rm "$project.rnxlist"
	fi
	if [[ $product_type = "EPH" ]] && [[ -e "$project.ephlist" ]]
	then
		rm "$project.ephlist"
	fi
	if [[ $product_type = "ERP" ]] && [[ -e "$project.erplist" ]]
	then
		rm "$project.erplist"
	fi
	if [[ $product_type = "ATX" ]] && [[ -e "$project.atxlist" ]]
	then
		rm "$project.atxlist"
	fi
	if [[ $product_type = "CLK" ]] && [[ -e "$project.clklist" ]]
	then
		rm "$project.clklist"
	fi
	if [[ $product_type = "CLK_30S" ]] && [[ -e "$project.clklist" ]]
	then
		rm "$project.clklist"
	fi
	if [[ $product_type = "TEC" ]] && [[ -e "$project.teclist" ]]
	then
		rm "$project.teclist"
	fi
	if [[ $product_type = "POS" ]] && [[ -e "$project.snxlist" ]]
	then
		rm "$project.snxlist"
	fi
	if [[ $product_type = "NAV" ]] && [[ -e "$project.navlist" ]]
	then
		rm "$project.navlist"
	fi
	if [[ $product_type = "SSC" ]] && [[ -e "$project.ssclist" ]]
	then
		rm "$project.ssclist"
	fi
	if [[ $product_type = "DCB_P1C1" ]] && [[ -e "$project.dcb_p1c1list" ]]
	then
		rm "$project.dcb_p1c1list"
	fi
	if [[ $product_type = "DCB_P1P2" ]] && [[ -e "$project.dcb_p1p2list" ]]
	then
		rm "$project.dcb_p1p2list"
	fi
	if [[ $product_type = "DCB_P1P2_DAILY" ]] && [[ -e "$project.dcb_p1p2list" ]]
	then
		rm "$project.dcb_p1p2list"
	fi
	if [[ $product_type = "REC" ]] && [[ -e "$project.reclist" ]]
	then
		rm "$project.reclist"
	fi

	  # Delete the repeated lines in FileList
	FileList=($(awk -v RS=' ' '!a[$1]++' <<< ${FileList[@]}))

		# Generate the data's name list file
	local var=""
	for var in ${FileList[@]}
	do
		if [[ $product_type = "OBS" ]]
		then
		  echo $var >> "$project.rnxlist"
		elif [[ $product_type = "EPH" ]]
		then
			echo $var >> "$project.ephlist"
		elif [[ $product_type = "ERP" ]]
		then 
			echo $var >> "$project.erplist"
		elif [[ $product_type = "ATX_08" ]] 
		then
			echo $var >> "$project.atxlist"
		elif [[ $product_type = "CLK" ]]
		then
			echo $var >> "$project.clklist"
		elif [[ $product_type = "CLK_30S" ]]
		then
			echo $var >> "$project.clklist"
		elif [[ $product_type = "NAV" ]]
		then
			echo $var >> "$project.navlist"
		elif [[ $product_type = "TEC" ]]
		then
			echo $var >> "$project.teclist"
		elif [[ $product_type = "POS" ]]
		then
			echo $var >> "$project.snxlist"
		elif [[ $product_type = "DCB_P1C1" ]]
		then
			echo $var >> "$project.dcb_p1c1list"
		elif [[ $product_type = "DCB_P1P2" ]]
		then
			echo $var >> "$project.dcb_p1p2list"
		elif [[ $product_type = "DCB_P1P2_DAILY" ]]
		then
			echo $var >> "$project.dcb_p1p2list"
		elif [[ $product_type = "SSC" ]]
		then
			echo $var >> "$project.ssclist"
		elif [[ $product_type = "REC" ]]
		then
			echo $var >> "$project.reclist"
		fi
	done
	fi
} # The end of MakeList()

# ============================
# Merge the several observation which has the same station into a whole file
# ============================
Merge()
{
	  # Read the observation files list
	local rnxlist=`echo ${FileList[*]}`

	  # Assign for the merged file.
	local store_file="$s"."$y""o"

	unset FileList
	declare -a FileList
	savefile="$savepath""$store_file"

	  # Merge the observation files from the rnxlist.
	teqc $rnxlist > $savefile
	if [[ -f $StationListFile ]]
	then
		MergeFile[j]=$savefile
	else
		MergeFile[0]=$savefile
	fi
} # The end of Merge()

# ============================
# Main function
# ============================
Main()
{
	declare -a FileList
	product_type=$(echo $product_type | tr '[a-z]' '[A-Z]')
	if [[ $product_type = "OBS" ]] && [[ -f $s ]]
	then 
			# When the product type is OBS and 's' displays a list file.
		StationListFile=$s
		declare -a StationList
			# Store the stations' name into the array "StationList"
		ReadStation <$s
		j=0
		local var=""
		   # loop all the stations
		for var in ${StationList[@]}
		do
			JD=$JD1
				# Convert the capital letter to the lower case
			s=$(echo $var | tr '[A-Z]' '[a-z]')  
				# loop all the time
			for ((i=0;i<intv_num;++i))
			do
				TimeConvert
				HourCode
				URLMatch
				Download
				if [[ -e $zfile ]] || [[ -e $savefile ]]
				then
					if [[ "$de" = "1" ]]
					then 
						Decompress
					fi
					if [[ $segment_intv = 24 ]] && [[ $merge = 1 ]]
					then
						FileList[$i]=$savefile
					fi
					if [[ $segment_intv = 24 ]] && [[ $merge != 1 ]]
					then
						FileList[$i+$j*$intv_num]=$savefile
					fi
					if [[ $segment_intv -lt 24 ]]
					then
						if [[ $de = 1 ]]
						then
							Segment 
						else
								# If the downloaded file has not been decompressed, we 
								# can't segment this file.
							echo "Please decompress the downloaded file."
							exit 1
						fi
					fi
				else
					JD=$(awk 'BEGIN{printf("%.1f\n",'$JD+$intv/24')}')
					continue
				fi
			JD=$(awk 'BEGIN{printf("%.1f\n",'$JD+$intv/24')}')
			done
			if [[ $merge = 1 ]] && [[ $segment_intv -ge 24 ]]
			then
				Merge
			fi
			let j=j+1
		done
		if [[ $merge = 1 ]] && [[ $segment_intv -ge 24 ]]
		then
			FileList=("${MergeFile[@]}")
			MakeList
		fi
		if [[ $merge != 1 ]]
		then
			MakeList
		fi
		exit 1 
	else
			# The normal mode
		for ((i=0;i<intv_num;++i))
		do
			if [[ $product_type = "OBS" ]]
			then
				s=$(echo $s | tr '[A-Z]' '[a-z]')  
			fi
			TimeConvert
			HourCode
			URLMatch
			Download
			if [[ -e $zfile ]] || [[ -e $savefile ]]
			then
				if [[ "$de" = "1" ]]
				then 
					Decompress
				fi
				if [[ $segment_intv = 24 ]]
				then
					FileList[i]=$savefile
				fi
				if [[ $product_type = "OBS" ]] && [[ $segment_intv -lt 24 ]]
				then
					if [ $de = 1 ]
					then
						Segment 
					else
							# If the downloaded file has not been decompressed, we 
							# can't segment this file.
						echo "Please decompress the downloaded file first."
						exit 1
					fi
				fi
			else
					# When the $savefile doesn't exist, jump out the loop
				JD=$(awk 'BEGIN{printf("%.1f\n",'$JD+$intv/24')}')
				continue
			fi
			JD=$(awk 'BEGIN{printf("%.1f\n",'$JD+$intv/24')}')
		done
		if [[ $merge != 1 ]]
		then
			MakeList
		fi
		if [[ $product_type = "OBS" ]] && [[ $merge = 1 ]] && [[ $segment_intv -ge 24 ]]
		then
			Merge
			FileList=("${MergeFile[@]}")
			MakeList
		fi
	fi
} # The end of Main()

	# Read the input arguments
while getopts "b:e:i:t:u:a:s:p:l:dmh" arg
do
	case $arg in
		b)  b=$OPTARG;;
		e)  e=$OPTARG;;
		i)  segment_intv=$OPTARG;; 
		t)  product_type=$OPTARG;;
		u)  u=$OPTARG;;
		a)  agencyini=$OPTARG;;
		s)  s=$OPTARG;;
		d)  de=1;;
		p)  savepath=$OPTARG;;
		m)  merge=1;;
		l)  project=$OPTARG;;
		h)  Help ;;
		*)  echo "Can not find the argument "$OPTARG""
			 exit 1;;
	esac
done
	# Get the number of arguments
argnum=$# 

	# Judge the validity of the input arguments
ArgJudge

	# Assign the parameter with the revalent value according input data
ParaAssign

	# Calculate the number of time span
CalTimeSpan

	# Calculate the number of interval
CalIntvNum

	# Main function
Main

# =========================================================
# The End
# =========================================================
