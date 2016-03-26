#!/bin/bash

# ==========================================================
# Program Description
# Function:
# Download rinex observation data from IGS or other agencies.
#
# WARNING:
# We will call the "get_data.sh" to download the observation 
# data, so the rules of the "get_data.sh" should be observed. 
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
	echo "get_rnx.sh,为后续PPP计算准备相关数据"
	echo "用法: bash get_rnx.sh [起始时间][终止时间][时间间隔][机构][URL路径][测站][保存路径]...[选项]"
	echo "参数说明："
	echo "  -h,  --help                       获取帮助信息"
	echo "  -b,  --bigining of time span      需要的产品的起始时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -e,  --ending of time span        需要的产品的终止时间,格式为民用时(年 月 日 时 分 秒)"
	echo "  -i,  --interval of time span      时间间隔(单位:小时,必须为整数,默认为24)"
	echo "  -a,  --agency of GNSS product     发布GNSS产品的机构(IGS,IGR,IGU,COD,ESA,JPL,etc)"
	echo "  -s,  --station's name or list file 测站名或者测站列表文件"
	echo "  -u,  --the path of url_list file  保存有各种产品的url列表文件路径"
	echo "  -p,  --the data save path         下载数据保存路径(默认为当前路径)"
	echo "  -l,  --the preffix of list file   保存下载数据文件名的文件前缀"
	echo "  -d,  --decompress                 选择是否解压,如果带有-d选项,则解压文件:若未带有-d选项,则不解压"
	echo "  -m,  --merge rinex files          选择是否将同一测站的观测数据进行合并"


	echo "示例: bash get_rnx.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -i 24 -a IGS -s staion.list -u url.list -p ./data/rnx"
	echo "      bash get_rnx.sh -b \"2014 3 1 0 0 0\" -e \"2014 3 3 0 0 0\" -a IGS -s staion.list -u url.list -p ./data/rnx -l igs1781"
	exit 1
}

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
	if [[ $intv -le 0 ]]
	then 
		echo "The parameter \"i\" must be a nonzero integer!"
		exit 1
	fi

	  # The parameter 'b','e','a','u','s' must be given
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
	if [[ $agency = "" ]]
	then
		echo "Must give parameter 'a' a value"
		exit 1
	fi

	if [[ $s = "" ]]
	then
		echo "Must give parameter 's' a value"
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
} # The end of ArgJudge()

# ============================
# Call the get_data.sh to download the GNSS data and products
# ============================
DataDownload()
{
	data_type="OBS"
	if [[ $merge = 1 ]]
	then
		if [[ -n $prefix ]]
		then
		bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $data_type -s "$s" -u $u -p $savepath -d -m -l $prefix
		else
		bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $data_type -s "$s" -u $u -p $savepath -d -m
		fi
	else
		if [[ -n $prefix ]]
		then
		bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $data_type -s "$s" -u $u -p $savepath -d -l $prefix
		else
		bash get_data.sh -b "$b" -e "$e" -i $intv -a $agency -t $data_type -s "$s" -u $u -p $savepath -d
		fi
	fi
} # The end of DataDownload()

# ============================
# Read the input arguments
# ============================
while getopts "b:e:i:u:a:s:p:l:mh" arg
do
	case $arg in
		b)  b=$OPTARG;;
		e)  e=$OPTARG;;
		i)  intv=$OPTARG;;
		u)  u=$OPTARG;;
		a)  agency=$OPTARG;;
		s)  s=$OPTARG;;
		p)  savepath=$OPTARG;;
		m)  merge=1;;
		l)  prefix=$OPTARG;;
		h)  Help;;
		*)  echo "Can not find the argument "$OPTARG""
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
