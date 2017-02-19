#!/usr/bin/python
#coding=UTF-8

#import baseDiff.BaseDiff as BaseDiff

from baseDiff import BaseDiff as BaseDiff


class PPPDiff(BaseDiff):

    def __init__( self ):
        super( PPPDiff, self ).__init__( 'pppDiff' )


    def writeHeader( self, output_file_ ):
        output_file_.write( '# col  1 - 3:year/doy/sod\n' )
        output_file_.write( '# col  4 - 7:dx/dy/dz/status\n' )
        output_file_.write( '# END OF HEADER\n' )


    def parseLine( self, record_line_, data_map_ ):
        data_record_item = super( PPPDiff, self ).parseLine( record_line_, data_map_ )
        if data_map_['is_escape'] == False:
            data_map_['cmp_data'] = {}
            data_map_['cmp_data']['dx'] = float( data_record_item[3] )
            data_map_['cmp_data']['dy'] = float( data_record_item[4] )
            data_map_['cmp_data']['dz'] = float( data_record_item[5] )
            year  = int( data_record_item[0] )
            doy   = int( data_record_item[1] )
            sod   = float( data_record_item[2] )
            output_format = '%4d    %3d    %4.4f '%( year, doy, sod ) + ' %.4f  %.4f  %.4f  %s'
            data_map_['output_format'] = output_format
        else:
            return data_record_item

