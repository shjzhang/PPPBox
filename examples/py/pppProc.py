#!/usr/bin/python
#coding=UTF-8

from baseProc import BaseProc as BaseProc
from pppDiff  import PPPDiff  as PPPDiff 

class PPPProc(BaseProc):


    def __init__( self ):
        super( PPPProc, self ).__init__( 'ppp' )
        self.setDiffInstance( PPPDiff() )

    def process( self,  proc_dict_ ):
        super( PPPProc, self ).process( proc_dict_ )
        work_dir = self.module_name
        out_list_name = proc_dict_['-o']
        out_list_file = open(  out_list_name, 'r' )
        out_list_line = out_list_file.readline()
        while out_list_line:
            out_list_line = out_list_line.strip( '\n' )
            self.diff_instance.setFileNameOld( out_list_line+'.old' )
            self.diff_instance.setFileNameNew( out_list_line )
            if proc_dict_.has_key('threshold'):
                self.diff_instance.setThreshold( proc_dict_['threshold'] )
            self.diff_instance.diff()
            out_list_line = out_list_file.readline()
        out_list_file.close()

