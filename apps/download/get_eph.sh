#!/bin/bash

# ==========================================================
# Program Description
# Function:
# Download ephemeris file and clk file, erp file,etc,for next
# PPP procedure.
#
# Warning:
# We will call the "get_data.sh" to download the relevant data,
# which published by the IGS and other agencies.The rules made
# by "get_data.sh" should be observed here!
#
# Copyright (c)  
# Q.Liu, Wuhan University, 2015.10.20
#
# ==========================================================

# ============================
# Print the help message
# ============================
Help()
{
	echo "get_eph.sh, 为后续PPP计算准备相关数据"
	echo "用法: bash get_eph.sh [起始时间][终止时间][时间间隔][机构][类型文件][URL路径][保存路径]...[选项]"
	echo "参数说明："
	echo "  -h,  --help                      获取帮助信息"
	echo "  -b,  --bigining of time span     需要的产品的起始时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -e,  --ending of time span       需要的产品的终止时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -i,  --interval of time span     时间间隔(单位:小时,必须为整数,默认为24) "
	echo "  -a,  --agency of GNSS product    发布GNSS产品的机构(IGS,IGR,ESA,COD,JPL,etc)"
	echo "  -t,  --data type list file       需要下载的产品类型列表文件"
	echo "  -u,  --the path of url_list file 保存有各种产品的url列表文件路径"
	echo "  -p,  --the data save path        数据保存路径(默认为当前路径)"
	echo "  -l,  --the prefix of list file   保存下载数据文件名的文件前缀"
	echo "示例: bash get_eph.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -t type.list -u url_list.txt -p ./data/eph"
	echo "    : bash get_eph.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -t type.list -u url_list.txt -l igs1781 -p ./data/eph"
	exit 1
} # The end of Help()

# ============================
# Judge the validity of arguments
# ============================
ArgJudge()
{
	  # Judge the validity of arguments
	if [[ $NUMARGS -le 1 ]]
	then
		 Help
	fi

	if [[ -z $intv ]]
	then
		intv=24
	fi
	if [[ $intv -le 0 ]]
	then 
		echo "The parameter \"i\" must be a nonzero integer!"
		exit 1
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
		echo here
		echo "Must give parameter 'u' a value"
		exit 1
	fi
	echo savepath:$savepath
	if [ -z $savepath ]
	then 
		savepath="./"
	fi
} # The end of ArgJudge()

# ============================
# Read data type from a list file.
# ============================
ReadDataType()
{
	local linenum=0
	while read line
	do
		data_type[$linenum]=$line
		let linenum=$linenum+1
	done
} # The end of ReadDataType

# ============================
# Call the get_data.sh to download the GNSS data and products
# ============================
DataDownload()
{
	  # Construct the array of data type
	ReadDataType <$data_type_file
	for datatype in ${data_type[@]}
	do
		intv=24
		if [[ -z $agency ]]
		then
			if [[ -n $prefix ]]
			then
				bash get_data.sh -b "$b" -e "$e" -i $intv -t $datatype -u $u -l $prefix -p $savepath -d 
			else
				bash get_data.sh -b "$b" -e "$e" -i $intv -t $datatype -u $u -p $savepath -d 
			fi
		else
			if [[ -n $prefix ]]
			then
				bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $datatype -u $u -l $prefix -p $savepath -d 
			else
				bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $datatype -u $u -p $savepath -d 
			fi
		fi
   done	
} # The end of DataDownload()

# ============================
# Read the input arguments
# ============================
while getopts "b:e:i:u:a:t:p:l:h" arg
do
	case $arg in
		b)  b=$OPTARG;;
		e)  e=$OPTARG;;
		i)  intv=$OPTARG;;
		u)  u=$OPTARG;;
		a)  agency=$OPTARG;;
		t)  data_type_file=$OPTARG;;
		p)  savepath=$OPTARG;;
		l)  prefix=$OPTARG;;
		h)  Help;;
		*)	echo "Can not find the argument "$OPTARG""
		exit 1;;
	esac
done
#number of arguments
NUMARGS=$#

ArgJudge
DataDownload

# ============================
# The end.
# ============================
