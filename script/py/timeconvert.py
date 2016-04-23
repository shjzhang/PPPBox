#!/usr/bin/python
#coding=UTF-8

####################################################################
# @Description
#    Simply realization the time convertion between GSPT Common Time
#    And Julian Date.
#
# @Author
#    Gao kang, Wuhan University
#
# @Mail
#    whurinex@163.com
#
# @Version
#  $0.01  2016/07/22   the basic function
###################################################################

import sys

def tStringToDict(ct):
    time={}
    time['year']   = ct[0:4]
    time['month']  = ct[4:6]
    time['day']    = ct[6:8]
    time['hour']   = ct[8:10]
    time['minute'] = ct[10:12]
    time['second'] = ct[12: len(ct) ]
    return time

def tListToDict(ct):
    time={}
    time['year']   = ct[0]
    time['month']  = ct[1]
    time['day']    = ct[2]
    time['hour']   = ct[3]
    time['minute'] = ct[4]
    time['second'] = ct[5]
    return time


def CT2JD(ct):
    timeKey = ['year', 'month', 'day', 'hour', 'minute', 'second']
    time = {}
    if isinstance(ct, str):
        if len( ct ) < 14:
            print 'common time string must formatted[%04dY%02dM%02dD%02dh%02dm%4.2fs]!'
            sys.exit(1)
        else:
            time = tStringToDict(ct)
    elif isinstance(ct, list):
        if len( ct ) < 6:
            print 'common time List must have six elements!'
            sys.exit(1)
        else:
            time = tListToDict(ct)
    elif isinstance(ct, dict):
        if len( ct ) < 6 :
            print 'common time dictionay must have six elements!'
            sys.exit(1)
        else:
            for key in timeKey:
                if ct.has_key(key) == False:
                    print 'common time dictionary must have key ',key
                    sys.exit(1)
            time = ct
    else:
        print 'common time not support this instance!'
        sys.exit(1)

    if int(time['month']) <= 2:
        y = int( time['year'] )  - 1
        m = int( time['month'] ) + 12
    else:
        y = int( time['year'] )
        m = int( time['month'] )
    jd = []
    jd.append( int(365.25 * y) + int( 30.6001*(m+1) ) + int( time['day'] ) + 1720981)
    UT  =  int( time['hour'] ) * 3600  + float( time['minute'] ) * 60 + float( time['second'] ) + 0.5 * 24 * 3600
    jd.append( int( UT ) )
    jd.append( UT - int( UT ) )
    return jd

def JD2CT(jd, tag):
    if isinstance(jd, list):
        jdd = int(jd[0]) + ( int(jd[1]) + float(jd[2]) ) / (60*60*24)
    else:
        jdd = float(jd)
    a = int( jdd + 0.5 )
    b = a + 1537
    c = int( (b-122.1)/365.25 )
    d = int( 365.25*c )
    e = int( (b-d)/30.6001 )
    f = jdd + 0.5 - int( jdd + 0.5 )
    D = b - d - int( 30.6001*e )
    M = e - 1 - 12 * int(e/14.0)
    Y = c - 4715 - int( (7+M)/10.0 )
    if isinstance(jd, list):
        h = int( int(jd[1]/3600) + 12 ) % 24
        m = int( ( int(jd[1]) % 3600 ) / 60 )
        s = int(jd[1]) % 60 + float(jd[2])
    else:
        h = int( f * 24  )
        m = int( (f*24 - h ) * 60 )
        s = f*24*3600 - h *3600 - m*60
    if tag == 's':
        time = '%04d%02d%02d%02d%02d%4.2f'%(Y,M,D,h,m,s)
    elif tag == 'l':
        time = [Y,M,D,h,m,s]
    elif tag == 'd':
        time={}
        time['year'] = Y
        time['month'] = M
        time['day'] = D
        time['hour'] = h
        time['minute'] = m
        time['second'] = s
    else:
        time = "%04d%02d%02d%02d%02d%4.2f"%(Y,M,D,h,m,s)
    return time

def CT2GPST(ct):
    jd = CT2JD(ct)
    jdd = jd[0] + (jd[1]+jd[2])/(24*3600)
    gpsweek = int( (jdd - 2444244.5) / 7 )
    weeksecond = (jd[0]-2444244.5)*24*3600 + jd[1] + jd[2] - gpsweek*7*24*3600
    gpst = [gpsweek,int(weeksecond),weeksecond-int(weeksecond)]
    return gpst

def CT2GPSWeek(ct):
    gpst = CT2GPST(ct)
    return gpst[0]

def CT2GPSDOW(ct):
    gpst = CT2GPST(ct)
    return int( gpst[1]/86400 )

def CT2WOY(ct):
    doy = CT2DOY(ct)
    return int( doy/7 ) + 1

def GPST2CT(gpst, tag):
    if isinstance(gpst, list):
        if len( gpst ) < 2:
            print 'GPST Must have two elements!'
            sys.exit(1)
        else:
            jd = []
            jd00 = gpst[0]*7 + gpst[1]/(24*3600.0) + 2444244.5
            jd0 = int(jd00)
            jd11 = gpst[0]*7*24*3600 + (2444244.5 - jd0)*24*3600 + gpst[1] + gpst[2]
            jd1  = int(jd11)
            jd2  = jd11 - jd1
            jd.append(jd0)
            jd.append(jd1)
            jd.append(jd2)
            return JD2CT(jd, tag)
    else:
        print 'GPST not support instance!'
        sys.exit(1)

def JD2DOY(jd):
    ct = JD2CT(jd, 'd')
    start_ct = '%s0101000000'%(str(ct['year']))
    start_jd = CT2JD(start_ct)
    s_jd = start_jd[0] + ( start_jd[1] + start_jd[2] )/(24*3600)
    n_jd = jd[0] + ( jd[1] + jd[2] ) /(24*3600)
    return int(n_jd - s_jd) + 1

def CT2DOY(time):
    jd = CT2JD(time)
    return JD2DOY(jd)

def COMP_JD(jd_left, jd_right):
    jd_d_left   = jd_left[0] + ( jd_left[1] + jd_left[2] )/(24*3600)
    jd_d_right  = jd_right[0] + ( jd_right[1] + jd_right[2] )/(24*3600)
    return jd_d_left > jd_d_right

def COMP_CT(time_left, time_right):
    jd_left = CT2JD(time_left)
    jd_right = CT2JD(time_right)
    return COMP_JD(jd_left, jd_right)

def ADD_HOUR(time, dTime, tag='s'):
    second = int( dTime )* 3600
    jd = CT2JD(time)
    jd[1] = jd[1] + second
    return JD2CT(jd, tag)

def SUB_TIME(time_left, time_right):
    jd_left   = CT2JD( time_left )
    jd_right  = CT2JD( time_right )
    d_day = jd_left[0] - jd_right[0]
    d_sec = jd_left[1] + jd_left[2] - jd_right[1] - jd_right[2]
    return d_day * 24 * 3600 + d_sec

def TEXT_TIME(time_):
    time = tStringToDict( time_ )
    return '%04d %02d %02d %02d:%02d:%02d'%( int( time['year'] ), int( time['month'] ), int( time['day'] ), int( time['hour'] ), int( time['minute'] ), int( float( time['second'] ) ))

