#!/usr/bin/python
#coding=UTF-8

from pppProc import PPPProc as PPPProc


def prepareDiff( argDict ):
    #### ppp configuration
    pppDict = {}
    pppDict['-r']  = './ppp/igs16577.obslist'
    pppDict['-s']  = './ppp/igs16577.ephlist'
    pppDict['-k']  = './ppp/igs16577.clklist'
    pppDict['-e']  = './ppp/igs16577.erplist'
    pppDict['-o']  = './ppp/igs16577.outlist'
    pppDict['-m']  = './data/igs16577.msc'
    pppDict['proc']   = PPPProc()
    argDict['ppp'] = pppDict



def main():
    argDict = {}

    ### set some parameters
    prepareDiff( argDict )
    
    ### process diff
    for key in argDict.keys():
        moduleDict = argDict[key]
        moduleDict['proc'].process( moduleDict )


if __name__ == '__main__':
    main()

