#!/usr/bin/python
#coding=UTF-8

######################################################################
# @Description
#   The core procedure to download IGS products  
#
# @Author
#   Gao kang, Wuhan University. 
#
# @Mail:
#   whurinex@163.com
#
# @Version
#  $0.01 2016/04/05 the Basic Function, may be you can use tool teqc to merge
#####################################################################


import sys
import os
import getopt
import url_list
import timeconvert
import wget

#The help text
helpText='get_data.py,为后续PPP计算准备相关数据\n'+\
         '用法:python  get_data.py [起始时间][终止时间][时间间隔][机构][URL路径][测站][保存路径]...[选项]\n'+\
         '参数说明：\n'+\
         '  -h,  --help                       获取帮助信息\n'+\
         '  -b,  --bigining of time span      需要的产品的起始时间,格式为民用时(%04Y%02M%02D%02h%02m%02s)\n'+\
         '  -e,  --ending of time span        需要的产品的终止时间,格式为民用时(%04Y%02M%02D%02h%02m%02s)\n'+\
         '  -i,  --interval of time span      时间间隔(单位:小时,必须为整数,默认为24)\n'+\
         '  -a,  --agency of GNSS product     发布GNSS产品的机构(IGS,IGR,IGU,COD,ESA,JPL,etc)\n'+\
         "  -s,  --station's name or list file 测站名或者测站列表文件\n"+\
         "  -t,  --type of product            下载的产品类型(OBS,EPH,CLK,ERP,etc,IGS_OBS is also OK,needn't -a)\n"+\
         '  -p,  --the data save path         下载数据保存路径(默认为当前路径)\n'+\
         '  -l,  --the preffix of list file   保存下载数据文件名的文件前缀\n'+\
         '  -d,  --decompress                 选择是否解压,如果带有-d选项,则解压文件:若未带有-d选项,则不解压\n'+\
         '  -m,  --merge rinex files          选择是否将同一测站的观测数据进行合并\n'+\
         '示例: python  get_data.py -b 20140301000000 -e 20140303000000 -i 24 -a IGS -s staion.list -p ./data\n'+\
         '      python  get_data.py -b 20140301000000 -e 20140303000000 -a IGS -s staion.list  -p ./data -l igs1781\n'

#All support options
argList = 'hdmb:e:i:a:s:u:p:l:t:'

#Optional Options
argOptional =['-i','-l','-h','-d','-m']

#Necessary Options
argNecessary = ['-b','-e','-a','-s','-p','-t']

#All agency
agencyList = ['BKG', 'COD', 'EMR', 'ESA', 'GFZ', 'GRG', 'GSI', 'IAC',\
        'IERS', 'IGR', 'IGRE', 'IGRS', 'IGS', 'IGSE', 'IGSS', 'IGU',\
        'IGUE', 'IGUS', 'JAXA', 'JPL', 'MCC', 'MGEX', 'MIT', 'NGS',\
        'RTPP', 'VMF']

#Echo the help Text
def help():
    type_ = sys.getfilesystemencoding()
    print helpText.decode('utf-8').encode(type_)

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
                if int(value, 10) < 0 :
                    print 'the interval must > 0\n'
                    help()
                    return False, None

    # Check necessary Options
    for opt in argNecessary:
        if argDict.has_key(opt) == False:
            print 'lack of essential option ' + opt + '\n'
            help()
            return False, None

    ## Check begin and end Time logically
    if timeconvert.COMP_CT( argDict['-e'], argDict['-b'] ) == False:
        print 'The begin Time must less than end Time'
        help()
        return False, None

    ## Set default Value of interval
    if argDict.has_key('-i') == False:
        argDict['-i'] = 24

    return True, argDict

def readStation(stationListPath):
    stationList = []
    if ( '.' in stationListPath ) == False :
        stationList.append( stationListPath )
        return True, stationList
    f = open( stationListPath )
    stationLine = f.readline()
    while stationLine:
        stationList.append( stationLine.strip('\n') )
    f.close()
    return len( stationList ) > 1, stationList

### use the module wget to get file
def processWget(argDict):
    url = argDict['url']
    savePath = argDict['-p']
    print '[url=%s, savePath=%s]'%(url, savePath)
    file_name = wget.download(url, savePath)
    if argDict.has_key('-l'):
        dir_, file_ = os.path.split(file_name )
        new_file_name = '%s/%s%s'%(dir_, argDict['-l'], file_)
        os.renames(file_name, new_file_name)
        argDict['file_name'] = new_file_name
    else:
        argDict['file_name'] = file_name

def processTime(argDict):
    if argDict['status'] == 0:
        argDict['status'] = 1
        argDict['timestamp'] = argDict['-b'] ### this place you can forward or backward interval
        argDict['%s'] = argDict['station_name'].lower()
        argDict['%S'] = argDict['station_name'].upper()
        argDict['%r'] = argDict['station_name']
    else:
        if timeconvert.COMP_CT( argDict['timestamp'], argDict['-e'] ): ### this place you can forward or backward interval
            return False
        else:
            argDict['timestamp'] = timeconvert.ADD_HOUR( argDict['timestamp'], argDict['-i'] )
            if timeconvert.SUB_TIME( argDict['timestamp'], argDict['-e'] ) >= int( argDict['-i'] ) * 3600 :
                return False

    #### calculate the all support time format value
    timestampDict = timeconvert.tStringToDict( argDict['timestamp'] )
    argDict['%Y'] = int( timestampDict['year'] )
    argDict['%y'] = int( argDict['timestamp'][2:4] )
    argDict['%m'] = int( timestampDict['month'] )
    argDict['%d'] = int( timestampDict['day'] )
    argDict['%h'] = int( timestampDict['hour'] )
    argDict['%H'] = chr( int(timestampDict['hour']) + ord('a') )
    argDict['%M'] = int( timestampDict['minute'] )
    argDict['%n'] = int( timeconvert.CT2DOY(timestampDict) )
    argDict['%W'] = int( timeconvert.CT2GPSWeek(timestampDict) )
    argDict['%w'] = int( timeconvert.CT2WOY(timestampDict) )
    argDict['%D'] = int( timeconvert.CT2GPSDOW(timestampDict) )
    return True

### get the url
def processUrl(argDict):
    agency    = argDict['-a']
    dataType  = argDict['-t']
    name = '%s_%s'%(agency, dataType)
    url_formatted, formatOrder = url_list.getUrlByName(name, True)
    if url_formatted == None:
        return False
    else:
        time = []
        for tag in formatOrder:
            time.append(  argDict[tag]  )
        url = url_formatted%tuple(time)
        argDict['url'] = url
        return True

### Decompress the download file, this may have some problem, Use processDecompress_s
def processDecompressEnd(argDict):
    if argDict['file_name'].find('.Z') != -1 or argDict['file_name'].find('.gz') != -1:
        if argDict['file_name'].find('.Z') != -1:
            path = argDict['file_name'].replace('.Z',  '')
        else:
            path = argDict['file_name'].replace('.gz', '')
        argDict['file_name'] = path 

### Decompress the download, in like unix, gzip -d, in window you must 
### set gzip.exe in the same work dir
def processDecompress(argDict):
    zipCmd = 'gzip -d %s'%(argDict['file_name'])
    os.system(zipCmd)
    processDecompressEnd(argDict)


### this is the procedure for one station 
def process(argDict):
    if processTime(argDict):
        if processUrl(argDict):
            processWget(argDict)
            if argDict.has_key('-d'):
                processDecompress( argDict )
                return True
            else:
                return True
        else:
            return False
    else:
        return False

def processMakeListBegin(argDict):
    path = argDict['-p']
    dataType = argDict['-t'].lower()
    if argDict.has_key('-l'):
        list_path = '%s/%s.%slist'%(path, argDict['-l'], dataType)
    else:
        list_path = '%s/%s.%slist'%(path, dataType, dataType)
    make_list_file = open( list_path ,'w+')
    argDict['make_list_file'] = make_list_file

def processMakeList(argDict):
    make_list_file = argDict['make_list_file']
    make_list_file.write( argDict['file_name'] )
    make_list_file.write('\n')

def processMakeListEnd(argDict):
    make_list_file = argDict['make_list_file']
    make_list_file.close()

### main procedure
def main():
    parseStatus, argDict = parseOptions() ## Parse the Options
    if parseStatus :
        readStatus, stationList = readStation( argDict['-s'] ) ## Read Station Files
        if readStatus :
            if argDict['-a'] in agencyList :
                processMakeListBegin(argDict)
                for stationName in stationList:
                    argDict['status']  = 0 
                    argDict['station_name'] = stationName
                    while process(argDict):
                        processMakeList(argDict)
                        print '\n[timestamp=%s]'%( timeconvert.TEXT_TIME( argDict['timestamp'] ) )
                processMakeListEnd(argDict)
            else:
                print 'Not include this agency!'
        else:
            print 'read station failure!'
    else:
        print 'parse options failure!'

if __name__ == '__main__':
    main()
