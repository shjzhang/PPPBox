#!/bin/bash

# ==========================================================
# Program Description
#
# Function: Prepare the data for the PPP procedure.
# We will call the get_data.sh to download the relevant data,
# data type may be: observation file, clock file, antenna 
# file, ephemeris file and so on.
#
# Copyright (c)  Q.Liu, Wuhan University, 2015.10.20
# ==========================================================

# Print the help message
Help()
{
	echo "get_data_ppp.sh, 为后续PPP计算准备相关数据"
	echo "用法: bash get_data_ppp.sh [起始时间][终止时间][时间间隔][机构][URL路径][测站][保存路径]...[选项]"
	echo "示例: bash get_data_ppp.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -u \"URL_LIST.txt\" -s \"STA_IGS08.txt\" -p \"./data\" -l \".listname\" "
	echo "参数说明："
	echo "-h,   --help                      获取帮助信息"
	echo "-b,   --bigining of time span     需要的产品的起始时间，格式为民用时（年 月 日 时 分 秒）"
	echo "-e,   --ending of time span       需要的产品的终止时间，格式为民用时（年 月 日 时 分 秒）"
	echo "-i,   --interval of time span     时间间隔（单位：小时，必须为整数） "
	echo "-a,   --agency of GNSS product    发布GNSS产品的机构"
	echo "                                  机构的选项有：IGS、IGR、IGU、COD、ESA、JPL、MGEX、GFZ、IGSE
	                                        、IERS、JAXA、RTPP、GSI、SIO、IGUE、IGSS、VMF等"
	echo "-s,   --station name or station   测站名或者测站列表文件
                  list file            	    "
	echo "-u,   --the path of url_list file 保存有各种产品的url列表文件的路径"
	echo "-p,   --the data save path        下载数据保存路径"
	echo "-m,   --merge the observation     将观测值文件进行合并，合并后文件名由用户提供
	              file                       "
}

# Judge the validity of arguments
ArgJudge()
{
	# Judge the validity of arguments
	if [ $NUMARGS -lt 14 -a $NUMARGS -gt 2 ]
	then
	echo "The arguments' quantity is not enough, please use \"bash get_data_ppp.sh -h\" command to get help"
		exit 1
	fi
	if [ $NUMARGS -eq 0 ]
	then
	echo "The arguments' quantity is not enough, please use \"bash get_data_ppp.sh -h\" command to get help"
		exit 1
	fi
	if [ $intv -le 0 ]
	then 
		echo "The parameter \"intv\" must be a nonzero integer!"
		exit 1
	fi
	if [ -d savepath ]
	then 
		echo "The data file save path must be given! "
		exit 1
	fi
}

# Call the get_data.sh to download the GNSS data and products
DataDownload()
{
	# Construct the array of data type
	data_type=("OBS" "EPH" "CLK" "CLK_30S" "ATX_08" "ERP")
	for var in ${data_type[@]}
	do
		if [[ $var != "OBS" ]]
		then
			intv=24
		fi
		if [[ $merge = 1 ]]
		then
			bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $var -p $savepath -u $u -s $s -d -l "$listname" -m
		else
			bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $var -p $savepath -u $u -s $s -d -l "$listname"
		fi
    done	
}

# Read the station list from station list file.
ReadFile()
{
	i=0
	while read line
	do
		FileList[i]=$line
		let i=$i+1
	done
}

# Read the input arguments
while getopts "b:e:i:u:a:s:p:l:mh" arg
do
	case $arg in
		b)  b=$OPTARG;;
		e)	e=$OPTARG;;
		i)	intv=$OPTARG;;
		u)	u=$OPTARG;;
		a)  agency=$OPTARG;;
		s)	s=$OPTARG;;
		p)  savepath=$OPTARG;;
		m)  merge=1;;
		l)  listname=$OPTARG;;
		h)  Help
		exit 1;;
		*)	echo "Can not find the argument "$OPTARG""
		exit 1;;
	esac
done
#number of arguments
NUMARGS=$#

ArgJudge
DataDownload
