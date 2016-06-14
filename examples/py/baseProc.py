#!/usr/bin/python
#coding=UTF-8

import os
import sys


class BaseProc(object):

    def __init__( self, module_name_ = None ):
        self.diff_instance = None
        self.proc_dict = {}
        self.module_name = module_name_

    def setDiffInstance( self, diff_instance_ ):
        self.diff_instance = diff_instance_

    def setProcDict( self, proc_dict_ ):
        self.proc_dict = proc_dict_

    def process( self, proc_dict_ ):
        self.proc_dict = proc_dict_
        executeString = self.module_name+' '
        for key in self.proc_dict.keys():
            if key.find( '-' ) != -1:
                executeString = '%s %s %s '%(executeString, key, self.proc_dict[key])
            else:
                continue
        os.system( executeString )

