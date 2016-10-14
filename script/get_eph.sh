#!/bin/bash

# ==========================================================
# Program Description
#
# Function
#
# Download ephemeris from IGS ftp for PPP positioning
#
# Warning
#
# We will call the get_data.sh to download the relevant data,
# which published by the IGS and other agency. Data type may
# be: clock file, antenna file, ephemeris file and so on.
#
# Copyright (c)  
#
# Q.Liu, Wuhan University, 2015.10.20
#
# ==========================================================

# Print the help message
Help()
{
	echo "get_eph.sh, 为后续PPP计算准备相关数据"
	echo "用法: bash get_eph.sh [起始时间][终止时间][时间间隔][机构][URL路径][测站][保存路径]...[选项]"
	echo "示例: bash get_eph.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -t \"data_type.lis\" -u \"url_list.txt\" -p \"./data\" "
	echo "参数说明："
	echo "-h,   --help                      获取帮助信息"
	echo "-b,   --bigining of time span     需要的产品的起始时间，格式为民用时（年 月 日 时 分 秒）"
	echo "-e,   --ending of time span       需要的产品的终止时间，格式为民用时（年 月 日 时 分 秒）"
	echo "-i,   --interval of time span     时间间隔（单位：小时，必须为整数） "
	echo "-t,   --data type list file       需要下载的产品列表文件"
	echo "                                  机构的选项有：IGS、IGR、IGU、COD、ESA、JPL、MGEX、GFZ、IGSE
	                                        、IERS、JAXA、RTPP、GSI、SIO、IGUE、IGSS、VMF等"
	echo "-u,   --the path of url_list file 保存有各种产品的url列表文件的路径"
	echo "-p,   --the data save path        下载数据保存路径"
	echo "-l,   --the prefix of list file   保存下载数据文件名的文件前缀"
}

# Judge the validity of arguments
ArgJudge()
{
	# Judge the validity of arguments
	if [[ $NUMARGS -le 1 ]]
	then
	echo "Invalid parameter input, please use \"bash get_data.sh -h\" command to get help"
		exit 1
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
	if [[ $data_type_file = "" ]]
	then
		echo "Must give parameter 't' a value"
		exit 1
	fi
	if [[ $u = "" ]]
	then
		echo "Must give parameter 'u' a value"
		exit 1
	fi
	if [ -z savepath ]
	then 
		echo "The data file save path must be given! "
		exit 1
	fi
}

ReadDataType()
{
	linenum=0
	while read line
	do
		data_type[$linenum]=$line
		let linenum=$linenum+1
	done
}

# Call the get_data.sh to download the GNSS data and products
DataDownload()
{
	# Construct the array of data type
	ReadDataType <$data_type_file
	for var in ${data_type[@]}
	do
		intv=24
		if [[ -z $savepath ]]
		then 
			savepath="./"
		fi
		if [[ -n $prefix ]]
		then
			bash get_data.sh -b "$b" -e "$e" -i $intv -t $var -u $u -l $prefix -p $savepath -d 
		else
			bash get_data.sh -b "$b" -e "$e" -i $intv -t $var -u $u -p $savepath -d 
		fi
   done	
}

# Read the input arguments
while getopts "b:e:i:u:t:p:l:h" arg
do
	case $arg in
		b)  b=$OPTARG;;
		e)  e=$OPTARG;;
		i)  intv=$OPTARG;;
		u)  u=$OPTARG;;
		t)  data_type_file=$OPTARG;;
		p)  savepath=$OPTARG;;
		l)  prefix=$OPTARG;;
		h)  Help
		exit 1;;
		*)	echo "Can not find the argument "$OPTARG""
		exit 1;;
	esac
done
#number of arguments
NUMARGS=$#

ArgJudge
declare -a data_type
DataDownload
