#!/usr/bin/python
#coding=UTF-8

import os
import sys

class BaseDiff(object):

    def __init__( self, module_name_='BaseDiff' ):
        self.old_file_name = ''
        self.new_file_name = ''
        self.threshold = 10e-14
        self.module_name = module_name_

    def setFileNameOld( self, old_file_name_ ):
        self.old_file_name = old_file_name_

    def setFileNameNew( self, new_file_name_ ):
        self.new_file_name = new_file_name_

    def setThreshold( self, threshold_ = 10e-14):
        self.threshold = threshold_

    def parseLine( self, record_line_, data_map_ ):
        data_record_item = self.splitWithoutBS( record_line_ )
        if '#' in data_record_item and data_record_item[0] == '#':
            data_map_['is_escape'] = True
        else:
            data_map_['is_escape'] = False
        return data_record_item

    def splitWithoutBS( self, record_line_ ):
        data_record_item = record_line_.split( ' ' )
        while '' in data_record_item:
            data_record_item.remove( '' )
        return data_record_item
    
    def writeHeader( self, output_file_ ):
        pass

    def parseBase( self, file_reader, data_map ):
        record_line = file_reader.readline()
        if record_line:
            record_line = record_line.strip('\n')
            data_map['is_end'] = False
            self.parseLine( record_line, data_map)
        else:
            data_map['is_end'] = True


    def diff( self ):
        if os.path.exists( self.old_file_name ) == False:
            raise Exception( 'old file not exists' )
        if os.path.exists( self.new_file_name ) == False:
            raise Exception( 'new file not exists' )
        check_output_file = open( '%s.diff'%(self.new_file_name), 'w+')
        old_file_reader = open( self.old_file_name, 'r')
        new_file_reader = open( self.new_file_name, 'r')
        old_data_map = {}
        new_data_map = {}
        self.writeHeader( check_output_file )
        while True:
            old_data_map.clear()
            new_data_map.clear()
            self.parseBase( old_file_reader, old_data_map )
            self.parseBase( new_file_reader, new_data_map )
            if old_data_map['is_end'] or new_data_map['is_end']:
                break
            else:
                if old_data_map['is_escape'] or new_data_map['is_escape']:
                    continue
                old_cmp_map = old_data_map['cmp_data']
                new_cmp_map = new_data_map['cmp_data']
                output_format = old_data_map['output_format']
                value = []
                check_status = ''
                for key in old_cmp_map.keys():
                    if new_cmp_map.has_key( key ):
                        d = new_cmp_map[key] - old_cmp_map[key]
                        if abs( d ) > self.threshold:
                            check_status = check_status + '1'
                        else:
                            check_status = check_status + '0'
                        value.append( d )
                    else:
                        old_file_reader.close()
                        new_file_reader.close()
                        check_output_file.close()
                        raise Exception('file type not consist!')
                value.append( check_status )
                check_output_file.write( output_format%tuple(value) )
                check_output_file.write('\n')
        old_file_reader.close()
        new_file_reader.close()
        check_output_file.close()
