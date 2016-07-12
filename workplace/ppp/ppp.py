#!/usr/bin/python
#coding=UTF-8

import os
import sys

proj='igs16577'
station='%s/%s.stalist.test'%(sys.path[0],proj)
#print station
workdir='/home/rinex/Documents/data/IGS'#'/Users/shjzhang/Documents/Develop/CynoNav/PPPBox/workplace/ppp'
beginTime = '20111009000000'
endTime = '20111010000000'
agency = 'IGS'


def processData(argDict):
    #### download obs data
    obsDwCmd = 'get_data.py -b %s -e %s -t %s -a %s -i 24 -p %s -d -s %s -l %s'%(beginTime, endTime,'OBS', agency,  workdir, station, proj)
    print obsDwCmd
    os.system(obsDwCmd)
    ##### download eph data
    ephDwCmd = 'get_data.py -b %s -e %s -t %s -a %s -i 24 -p %s -d -s %s -l %s'%(beginTime, endTime,'EPH', agency,  workdir, station, proj)
    os.system(ephDwCmd)
    ##### download clk data
    clkDwCmd = 'get_data.py -b %s -e %s -t %s -a %s -i 24 -p %s -d -s %s -l %s'%(beginTime, endTime,'CLK', agency,  workdir, station, proj)
    os.system(clkDwCmd)
    ##### download erp data
    erpDwCmd = 'get_data.py -b %s -e %s -t %s -a %s -i 24 -p %s -d -s %s -l %s'%(beginTime, endTime,'ERP', agency,  workdir, station, proj)
    os.system(erpDwCmd)
    ##### download ssc data
    sscDwCmd = 'get_data.py -b %s -e %s -t %s -a %s -i 24 -p %s -d -s %s -l %s'%(beginTime, endTime,'SSC', agency,  workdir, station, proj)
    os.system(sscDwCmd)
    ##### ssc2msc
    processSSC2MSC(argDict)
    processDir(argDict)

def processSSC2MSC(argDict):
    ssc_list_path = '%s/%s.ssclist'%(workdir, proj)
    ssc_list_file = open( ssc_list_path, 'r' )
    ssc_file_record = ssc_list_file.readline()
    msc_list_name = []
    while ssc_file_record:
        ssc_file_record = ssc_file_record.strip( '\n' )
        dir_, file_ = os.path.split(ssc_file_record)
        file_ = file_.replace('.ssc', '.msc')
        msc_name = '%s/%s'%(dir_,file_)
        ssc2mscCmd = '%s -s %s -m %s'%('ssc2msc',ssc_file_record, msc_name)
        msc_list_name.append(msc_name)
        os.system(ssc2mscCmd)
        ssc_file_record = ssc_list_file.readline()
    msc_file_path = '%s/%s.msc'%(workdir, proj)
    msc_file = open(msc_file_path,'a+')
    for msc_list_path in msc_list_name:
        msc_item_file = open(msc_list_path, 'r')
        msc_file.write( msc_item_file.read() )
        msc_item_file.close()
    msc_file.close()
    #print  msc_file_path
    argDict['-m']=msc_file_path

def processDir(argDict):
    argDict['-r']='%s/%s.obslist'%(workdir, proj)
    argDict['-s']='%s/%s.ephlist'%(workdir, proj)
    argDict['-e']='%s/%s.erplist'%(workdir, proj)
    argDict['-k']='%s/%s.clklist'%(workdir, proj)
    argDict['-o']='%s/%s.outlist'%(workdir, proj)
    obs_list_file = open(argDict['-r'],'r')
    out_list_file = open(argDict['-o'],'w+')
    obs_record = obs_list_file.readline()
    while obs_record:
        obs_record = obs_record.strip('\n')
        obs_record = '%s.out\n'%(obs_record)
        out_list_file.write(obs_record)
        obs_record = obs_list_file.readline()
    obs_list_file.close()
    out_list_file.close()

def processPPP(argDict):
    pppCmd = 'ppp '
    for k, v in argDict.items():
        pppCmd = pppCmd + ' %s %s'%(k, v)
    #print pppCmd
    os.system(pppCmd)

def main():
    argDict={}
    processData(argDict)
    #processSSC2MSC(argDict)
    #processDir(argDict)
    #print argDict
    processPPP(argDict)

if __name__ == '__main__':
    main()
