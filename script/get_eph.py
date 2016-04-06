#!/usr/bin/python
#coding=UTF-8

################################################################
# @Description:
#   download ephemeris file  and clk file writen by Python
# 
# @Author
#   Gao kang, Wuhan University, 2016.04.01
#
# @Version
#  $ 0.01 Date 2016.04.01
###############################################################


import sys
import os
import getopt

#The help text
helpText='get_eph.py,为后续PPP计算准备相关数据\n'+\
         '用法: python get_eph.py [起始时间][终止时间][时间间隔][机构][URL路径][测站][保存路径]...[选项]\n'+\
         '参数说明：\n'+\
         '  -h,  --help                       获取帮助信息\n'+\
         '  -b,  --bigining of time span      需要的产品的起始时间,格式为民用时(%04Y%02M%02D%02h%02m%02s)\n'+\
         '  -e,  --ending of time span        需要的产品的终止时间,格式为民用时(%04Y%02M%02D%02h%02m%02s)\n'+\
         '  -i,  --interval of time span      时间间隔(单位:小时,必须为整数,默认为24)\n'+\
         '  -a,  --agency of GNSS product     发布GNSS产品的机构(IGS,IGR,IGU,COD,ESA,JPL,etc)\n'+\
         '  -t,  --data type list file       需要下载的产品类型列表文件\n'+\
         '  -p,  --the data save path         下载数据保存路径(默认为当前路径)\n'+\
         '  -l,  --the preffix of list file   保存下载数据文件名的文件前缀\n'+\
         '示例: python get_rnx.py -b 20140301000000 -e 20140303000000 -i 24 -a IGS -s staion.list -p ./data\n'+\
         '      python get_rnx.py -b 20140301000000 -e 20140303000000 -a IGS -s staion.list -p ./data -l igs1781\n'

#All support options
argList = 'hb:e:i:a:t:u:p:l:'

#Optional Options
argOptional = ['-i','-l','-h']

#Necessary Options
argNecessary = ['-b', '-e', '-a', '-t', '-p']

#Echo the help Text
def help():
    type_ = sys.getfilesystemencoding()
    print helpText.decode( 'utf-8' ).encode( type_ )

#Parse the Options
def parseOptions():
    
    argDict = {}
   
   # Validate the option number
    if len( sys.argv ) < 1 :
        print 'The options number at least two\n'
        help()
        return False, None
   
   # Obtain Options
    opts, args = getopt.getopt( sys.argv[1:], argList)
   
   #Convert From List To Map
    for opt in opts :
        argDict[opt[0]] = opt[1]
   
   # Check optional  Options
    for op, value in argDict.items():
        if op in argOptional:
            if '-h' == op :
                help()
                return False, None
            elif '-i' == op :
                if int(value, 10) <= 0 :
                    print 'the interval must > 0\n'
                    help()
                    return False, None

    # Check necessary Options
    for opt in argNecessary:
        if argDict.has_key(opt) == False:
            print 'lack of essential option ' + opt + '\n'
            help()
            return False, None
    
    return True, argDict


def dataDownLoad(argDict, dataTypes):
    for dataType in dataTypes:
        argDict['-t'] = dataType # Change the data Type
        argDict['-i'] = '24' # Set Default interval
        pyCmd = 'python get_data.py '
        for op, value in argDict.items():
            pyCmd = pyCmd + op + ' ' + value + ' '
        os.system(pyCmd)
        pyCmd = ''

def readDataType(typeFilePath):
    f = open( typeFilePath )
    dataTypes = []
    typeLine = f.readline()
    while typeLine:
        dataTypes.append( typeLine.strip('\n') )
        typeLine = f.readline()
    f.close()
    return len( dataTypes ) > 0, dataTypes

def main():
    parseStatus, argDict = parseOptions()
    if True == parseStatus:
        readStatus, dataTypes = readDataType(argDict['-t'])
        if True == readStatus:
            dataDownLoad(argDict, dataTypes)
        else:
            print "type.list error\n"

if __name__ == '__main__':
    main()
