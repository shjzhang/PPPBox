#!/usr/bin/python
#coding=UTF-8


###################################################################################
# @Description
#   url_list.py : url address list of gnss data for rtkget, reranged by Gao kang
#
# notes :
#     keywords in a url address are replaced as follows.
#
#         %Y -> yyyy    : year (4 digits) (2000-2099)
#         %y -> yy      : year (2 digits) (00-99)
#         %m -> mm      : month           (01-12)
#         %d -> dd      : day of month    (01-31)
#         %h -> hh      : hours           (00-23)
#         %H -> a       : hour code       (a-x)
#         %M -> mm      : minutes         (00-59)
#         %n -> ddd     : day of year     (001-366)
#         %W -> wwww    : gps week        (0001-9999)
#         %w -> ww      : week of year    (01-53)
#         %D -> d       : day of gps week (0-6)
#         %s -> ssss    : station name    (lower-case)
#         %S -> SSSS    : station name    (upper-case)
#         %r -> rrrr    : station name
#         %{env} -> env : environment variable
#
#     strings after # in a line are treated as comments
#
# @Mail
#    whurinex@163.com
#
# @Version
#  $0.01 2016/07/21 
###################################################################################

#url key words
url_key_word =['%Y', '%y', '%m', '%d', '%h', '%H', '%M', '%n', '%W', '%w', '%D', '%s', '%S', '%r']


#format Dictionary
formatDict = {}
formatDict['%Y']  = '%4d'
formatDict['%y']  = '%2d'
formatDict['%m']  = '%02d'
formatDict['%d']  = '%02d'
formatDict['%h']  = '%02d'
formatDict['%H']  = '%1s'
formatDict['%M']  = '%02d'
formatDict['%n']  = '%03d'
formatDict['%W']  = '%04d'
formatDict['%w']  = '%02d'
formatDict['%D']  = '%1d'
formatDict['%s']  = '%4s'
formatDict['%S']  = '%4s'
formatDict['%r']  = '%4s'



#all products address
urlDict = {}
urlDict['IGS_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_EPH_GLO']        = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igl%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_CLK_30S']        = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%W%D.clk_30s.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGR_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igr%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGR_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igr%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGR_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igr%W%D.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGU_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igu%W%D_%h.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGU_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igu%W%D_%h.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_POS']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%yP%W.snx.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_ZPD']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/troposphere/zpd/%Y/%n/%s%n0.%yzpd.gz', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_TEC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/ionex/%Y/%n/igsg%n0.%yi.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGR_TEC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/ionex/%Y/%n/igrg%n0.%yi.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGS_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/igs%yP%W.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/cod%W%D.eph.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/cod%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_CLK_5S']         = ['ftp://ftp.unibe.ch/aiub/CODE/%Y/COD%W%D.CLK_05S.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/cod%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/cod%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['ESA_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/esa%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['ESA_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/esa%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['ESA_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/esa%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['ESA_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/esa%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GFZ_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/gfz%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GFZ_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/gfz%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GFZ_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/gfz%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GFZ_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/gfz%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['JPL_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/jpl%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['JPL_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/jpl%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['JPL_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/jpl%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['JPL_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/jpl%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['NGS_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/ngs%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['NGS_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/ngs%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['NGS_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/ngs%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['NGS_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/ngs%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['SIO_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/sio%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['SIO_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/sio%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['SIO_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/sio%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['EMR_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/emr%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['EMR_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/emr%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['EMR_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/emr%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['EMR_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/emr%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MIT_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/mit%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MIT_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/mit%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MIT_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/mit%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MIT_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/mit%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GRG_EPH']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/grg%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GRG_CLK']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/grg%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GRG_ERP']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/grg%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['GRG_SSC']            = ['ftp://cddis.gsfc.nasa.gov/gps/products/%W/grg%W%D.ssc.Z', 'c:\GNSS_DATA\product\%W']
urlDict['BKG_ERP']            = ['ftp://igs.ensj.ign.fr/pub/igs/products/%W/bkg%W%D.pre.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IAC_ERP']            = ['ftp://igs.ensj.ign.fr/pub/igs/products/%W/iac%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MCC_ERP']            = ['ftp://igs.ensj.ign.fr/pub/igs/products/%W/mcc%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['MGEX_EPH_COD']       = ['ftp://cddis.gsfc.nasa.gov/gps/products/mgex/%W/com%W%D.sp3.Z', 'c:\GNSS_DATA\product\mgex\%W']
urlDict['MGEX_EPH_TUM']       = ['ftp://cddis.gsfc.nasa.gov/gps/products/mgex/%W/tum%W%D.sp3.Z', 'c:\GNSS_DATA\product\mgex\%W']
urlDict['MGEX_EPH_GFZ']       = ['ftp://cddis.gsfc.nasa.gov/gps/products/mgex/%W/gfm%W%D.sp3.Z', 'c:\GNSS_DATA\product\mgex\%W']
urlDict['MGEX_EPH_GRG']       = ['ftp://cddis.gsfc.nasa.gov/gps/products/mgex/%W/grm%W%D.sp3.Z', 'c:\GNSS_DATA\product\mgex\%W']
urlDict['RTPP_CLK']           = ['ftp://cddis.gsfc.nasa.gov/gps/products/rtpp/%W/igt%W%D.clk.Z', 'c:\GNSS_DATA\product\\rtpp\%W']
urlDict['IGSE_EPH']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igs%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSE_EPH_GLO']       = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igl%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSE_CLK']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igs%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSE_CLK_30S']       = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igs%W%D.clk_30s.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSE_ERP']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igs%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRE_EPH']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igr%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRE_CLK']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igr%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRE_ERP']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igr%W%D.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGUE_EPH']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igu%W%D_%h.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGUE_ERP']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igu%W%D_%h.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSE_POS']           = ['ftp://igs.ensg.ign.fr/pub/igs/products/%W/igs%yP%W.snx.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_EPH']           = ['ftp://garner.ucsd.edu/pub/products/%W/igs%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_EPH_GLO']       = ['ftp://garner.ucsd.edu/pub/products/%W/igl%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_CLK']           = ['ftp://garner.ucsd.edu/pub/products/%W/igs%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_CLK_30S']       = ['ftp://garner.ucsd.edu/pub/products/%W/igs%W%D.clk_30s.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_ERP']           = ['ftp://garner.ucsd.edu/pub/products/%W/igs%W7.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRS_EPH']           = ['ftp://garner.ucsd.edu/pub/products/%W/igr%W%D.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRS_CLK']           = ['ftp://garner.ucsd.edu/pub/products/%W/igr%W%D.clk.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGRS_ERP']           = ['ftp://garner.ucsd.edu/pub/products/%W/igr%W%D.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGUS_EPH']           = ['ftp://garner.ucsd.edu/pub/products/%W/igu%W%D_%h.sp3.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGUS_ERP']           = ['ftp://garner.ucsd.edu/pub/products/%W/igu%W%D_%h.erp.Z', 'c:\GNSS_DATA\product\%W']
urlDict['IGSS_POS']           = ['ftp://garner.ucsd.edu/pub/products/%W/igs%yp%W%D.snx.Z', 'c:\GNSS_DATA\product\%W']
urlDict['COD_DCB_P1P2']            = ['ftp://ftp.unibe.ch/aiub/CODE/%Y/P1P2%y%m.DCB.Z', 'c:\GNSS_DATA\data\dcb']
urlDict['COD_DCB_P1C1']            = ['ftp://ftp.unibe.ch/aiub/CODE/%Y/P1C1%y%m.DCB.Z', 'c:\GNSS_DATA\data\dcb']
urlDict['COD_DCB_P2C2']            = ['ftp://ftp.unibe.ch/aiub/CODE/%Y/P2C2%y%m.DCB.Z', 'c:\GNSS_DATA\data\dcb']
urlDict['COD_DCB_P1P2_DAILY']      = ['ftp://ftp.unibe.ch/aiub/BSWUSER50/ORB/%Y/COD%y%n.DCB.Z', 'c:\GNSS_DATA\data\dcb']
urlDict['COD_REC']                 = ['ftp://ftp.unibe.ch/aiub/bcwg/cc2noncc/receiver_bernese.lis', 'c:\GNSS_DATA\data\dcb']
urlDict['JAXA_EPH']                = ['http://qz-vision.jaxa.jp/USE/archives/final/%Y/qzf%W%D.sp3', 'c:\GNSS_DATA\product\qzv\%W']
urlDict['IGS_OBS']                 = ['ftp://cddis.gsfc.nasa.gov/gps/data/daily/%Y/%n/%yd/%s%n0.%yd.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV']                 = ['ftp://cddis.gsfc.nasa.gov/gps/data/daily/%Y/%n/%yn/brdc%n0.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV_GLO']             = ['ftp://cddis.gsfc.nasa.gov/gps/data/daily/%Y/%n/%yg/brdc%n0.%yg.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV_S']               = ['ftp://cddis.gsfc.nasa.gov/gps/data/daily/%Y/%n/%yn/%s%n0.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV_S_GLO']           = ['ftp://cddis.gsfc.nasa.gov/gps/data/daily/%Y/%n/%yg/%s%n0.%yg.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_OBS_H']               = ['ftp://cddis.gsfc.nasa.gov/gps/data/hourly/%Y/%n/%h/%s%n%H.%yd.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV_H']               = ['ftp://cddis.gsfc.nasa.gov/gps/data/hourly/%Y/%n/%h/%s%n%H.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_NAV_H_GLO']           = ['ftp://cddis.gsfc.nasa.gov/gps/data/hourly/%Y/%n/%h/%s%n%H.%yg.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGS_OBS_HR']              = ['ftp://cddis.gsfc.nasa.gov/gps/data/highrate/%Y/%n/%yd/%h/%s%n%H%M.%yd.Z', 'c:\GNSS_DATA\data\hr\%Y\%n']
urlDict['IGS_NAV_HR']              = ['ftp://cddis.gsfc.nasa.gov/gps/data/highrate/%Y/%n/%yn/%h/%s%n%H%M.%yn.Z', 'c:\GNSS_DATA\data\hr\%Y\%n']
urlDict['IGS_NAV_HR_GLO']          = ['ftp://cddis.gsfc.nasa.gov/gps/data/highrate/%Y/%n/%yn/%h/%s%n%H%M.%yg.Z', 'c:\GNSS_DATA\data\hr\%Y\%n']
urlDict['MGEX_OBS']                = ['ftp://cddis.gsfc.nasa.gov/gps/data/campaign/mgex/daily/rinex3/%Y/%n/%yo/%s%n0.%yo.Z', 'd:\GNSS_DATA\data_mgex\%Y\%n']
urlDict['MGEX_NAV']                = ['ftp://cddis.gsfc.nasa.gov/gps/data/campaign/mgex/daily/rinex3/%Y/%n/%yn/%s%n0.%yn.Z', 'd:\GNSS_DATA\data_mgex\%Y\%n']
urlDict['MGEX_NAV_GLO']            = ['ftp://cddis.gsfc.nasa.gov/gps/data/campaign/mgex/daily/rinex3/%Y/%n/%yg/%s%n0.%yg.Z', 'd:\GNSS_DATA\data_mgex\%Y\%n']
urlDict['MGEX_NAV_GAL']            = ['ftp://cddis.gsfc.nasa.gov/gps/data/campaign/mgex/daily/rinex3/%Y/%n/%yl/%s%n0.%yl.Z', 'd:\GNSS_DATA\data_mgex\%Y\%n']
urlDict['MGEX_NAV_QZS']            = ['ftp://cddis.gsfc.nasa.gov/gps/data/campaign/mgex/daily/rinex3/%Y/%n/%yq/%s%n0.%yq.Z', 'd:\GNSS_DATA\data_mgex\%Y\%n']
urlDict['IGSE_OBS']                = ['ftp://igs.ensg.ign.fr/pub/igs/data/%Y/%n/%s%n0.%yd.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSE_NAV']                = ['ftp://igs.ensg.ign.fr/pub/igs/data/%Y/%n/brdc%n0.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSE_NAV_GLO']            = ['ftp://igs.ensg.ign.fr/pub/igs/data/%Y/%n/brdc%n0.%yg.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSE_NAV_S']              = ['ftp://igs.ensg.ign.fr/pub/igs/data/%Y/%n/%s%n0.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSE_NAV_S_GLO']          = ['ftp://igs.ensg.ign.fr/pub/igs/data/%Y/%n/%s%n0.%yg.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSS_OBS']                = ['ftp://garner.ucsd.edu/pub/rinex/%Y/%n/%s%n0.%yd.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['IGSS_NAV_S']              = ['ftp://garner.ucsd.edu/pub/nav/%Y/%n/%s%n0.%yn.Z', 'c:\GNSS_DATA\data\%Y\%n']
urlDict['JAXA_NAV_GPS']            = ['http://qz-vision.jaxa.jp/USE/archives/ephemeris/%Y/brdc%n0.%yn', 'c:\GNSS_DATA\data_qzss\%Y\%n']
urlDict['JAXA_NAV_QZS']            = ['http://qz-vision.jaxa.jp/USE/archives/ephemeris/%Y/brdc%n0.%yq', 'c:\GNSS_DATA\data_qzss\%Y\%n']
urlDict['GSI_OBS']                 = ['ftp://terras.gsi.go.jp/data/GPS_products/%Y/%n/%s%n0.%yo.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_NAV']                 = ['ftp://terras.gsi.go.jp/data/GPS_products/%Y/%n/%s%n0.%yn.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_OBS_QZS']             = ['ftp://terras.gsi.go.jp/data/QZSS_products/%Y/%n/%s%n0.%yo.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_NAV_QZS']             = ['ftp://terras.gsi.go.jp/data/QZSS_products/%Y/%n/%s%n0.%yN.tar.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_OBS_3H']              = ['ftp://terras.gsi.go.jp/data/GPS_products/%Y/%n/%s%n%N.%yo.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_NAV_3H']              = ['ftp://terras.gsi.go.jp/data/GPS_products/%Y/%n/%s%n%N.%yn.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_OBS_QZS_3H']          = ['ftp://terras.gsi.go.jp/data/QZSS_products/%Y/%n/%s%n%N.%yo.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['GSI_NAV_QZS_3H']          = ['ftp://terras.gsi.go.jp/data/QZSS_products/%Y/%n/%s%n%N.%yN.tar.gz', 'c:\GNSS_DATA\data_gsi\%Y\%n']
urlDict['IERS_EOP_BULA']           = ['ftp://maia.usno.navy.mil/ser7/ser7.dat', 'c:\GNSS_DATA\data_iers']
urlDict['IERS_EOP_BULB']           = ['ftp://hpiers.obspm.fr/iers/bul/bulb_new/bulletinb.%N', 'c:\GNSS_DATA\data_iers']
urlDict['IERS_EOP_BULC']           = ['ftp://hpiers.obspm.fr/iers/bul/bulc/bulletinc.dat', 'c:\GNSS_DATA\data_iers']
urlDict['IGS_ATX_08']              = ['ftp://igs.org/pub/station/general/igs08.atx', 'c:\GNSS_DATA\data\pcv']
urlDict['IGS_ATX_05']              = ['ftp://igs.org/pub/station/general/igs05.atx', 'c:\GNSS_DATA\data\pcv']
urlDict['IGS_ATX_08W']             = ['ftp://igs.org/pub/station/general/igs08_%W.atx', 'c:\GNSS_DATA\data\pcv']
urlDict['VMF_GRID']                = ['http://ggosatm.hg.tuwien.ac.at/DELAY/GRID/VMFG/%Y/VMFG_%Y%m%d.H%h', 'c:\GNSS_DATA\data_vmf\%Y']


def getUrlByName(name, formatted):
    if urlDict.has_key(name):
        url = urlDict[name][0]
        formatOrder = []
        if formatted == True:
            for i in range( len( url ) ):
                if url[i] == '%' :
                    tag = '%'+url[i+1]
                    if tag in url_key_word:
                        formatOrder.append(tag)
                    else:
                        print 'warning: the tag [\'%s\'] is not in url_key_word'%(tag)
            for key, value in formatDict.items():
                url = url.replace(key, value)
        return url, formatOrder
    else:
        return None, None

