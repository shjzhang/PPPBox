#include <iomanip>
#include "NtripSP3Stream.hpp"
#include "PositionSatStore.hpp"
#include "ClockSatStore.hpp"
#include "FileSpec.hpp"
#include "FileUtils.hpp"
#include "SatID.hpp"
#include "GPSWeekSecond.hpp"
#include "SystemTime.hpp"

using namespace gpstk::StringUtils;

NtripSP3Stream::NtripSP3Stream()
{
    m_sFileName = "";
    m_sPath = ".";
    m_bHeaderWritten = false;
    m_eph = 0;
    m_dSample = 10;
    setSavePath(m_sPath);
    m_ephStream = new NtripNavStream();
}

NtripSP3Stream::~NtripSP3Stream()
{
    closeFile();
    delete m_eph;
    delete m_ephStream;
}

void NtripSP3Stream::setSavePath(string &path)
{
    if(!path.empty())
    {
        if(path[path.size()-1]!=slash)
        {
            path += slash;
        }
        m_sPath = path;
    }
    else
    {
        m_sPath = "." + slash;
    }
}


void NtripSP3Stream::resolveFileName(CommonTime &dateTime)
{
    // full GPS week
    std::string gpsWeek = GPSWeekSecond(dateTime).printf("%04F");

    // day of week
    std::string dow = GPSWeekSecond(dateTime).printf("%w");

    m_sFileName = m_sPath + "RTP" + gpsWeek + dow + "." + "sp3";
}


void NtripSP3Stream::printHeader(const CommonTime& dateTime)
{
    SystemTime sysTime;
    CommonTime comTime(sysTime);
    resolveFileName(comTime);

    if(FileUtils::fileAccessCheck(m_sFileName))
    {
        m_outStream.open(m_sFileName.c_str(), std::ios::app);
        m_bHeaderWritten = true;
        return;
    }
    else
    {
        m_outStream.open(m_sFileName.c_str(), std::ios::out);
    }
//    int    GPSWeek;
//    double GPSWeeks;
//    GPSweekFromDateAndTime(datTim, GPSWeek, GPSWeeks);

//    double sec = fmod(GPSWeeks, 60.0);

//    int    mjd;
//    double dayfrac;
//    mjdFromDateAndTime(datTim, mjd, dayfrac);

//    int numEpo = _numSec;
//    if (_sampl > 0) {
//      numEpo /= _sampl;
//    }

//    _out << "#aP" << datTim.toString("yyyy MM dd hh mm").toAscii().data()
//         << setw(12) << setprecision(8) << sec
//         << " " << setw(7) << numEpo << " ORBIT IGS08 HLM  IGS" << endl;

//    _out << "## "
//         << setw(4)  << GPSWeek
//         << setw(16) << setprecision(8) << GPSWeeks
//         << setw(15) << setprecision(8) << double(_sampl)
//         << setw(6)  << mjd
//         << setw(16) << setprecision(13) << dayfrac << endl;

    m_outStream << "+   56   G01G02G03G04G05G06G07G08G09G10G11G12G13G14G15G16G17\n"
                << "+        G18G19G20G21G22G23G24G25G26G27G28G29G30G31G32R01R02\n"
                << "+        R03R04R05R06R07R08R09R10R11R12R13R14R15R16R17R18R19\n"
                << "+        R20R21R22R23R24  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "+          0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n"
                << "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n"
                << "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000\n"
                << "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000\n"
                << "%i    0    0    0    0      0      0      0      0         0\n"
                << "%i    0    0    0    0      0      0      0      0         0\n"
                << "/*                                                          \n"
                << "/*                                                          \n"
                << "/*                                                          \n"
                << "/*                                                          \n";
    m_bHeaderWritten = true;
    return;
}

void NtripSP3Stream::writeFile(CommonTime& epoTime, string &prn, Xvt &sv)
{
    // Assure the right second to output data
    GPSWeekSecond gws = GPSWeekSecond(epoTime);
    double second = gws.getSOW();
    if (m_dSample != 0 && fmod(second, m_dSample) != 0.0)
    {
      return;
    }

    // Print the file header
    if(!m_bHeaderWritten)
    {
        printHeader(epoTime);
    }

    // Print the data in every epoch
    if(epoTime != m_lastEpoTime)
    {
        // Check the sampling interval (print empty epochs)
        std::string dateStr;
        if((m_lastEpoTime.getDays()!=0.0 || m_lastEpoTime.getSecondOfDay()!=0.0)
           && m_dSample > 0)
        {
            for(CommonTime ep = m_lastEpoTime + m_dSample; ep < epoTime;
                ep = ep + m_dSample)
            {
                dateStr = CivilTime(ep).printf("%Y %2m %2d %2H %2M %2S");
                m_outStream << "*  " << dateStr << endl;
            }
        }

        // Print the new epoch
        dateStr = CivilTime(epoTime).printf("%Y %2m %2d %2H %2M %2S");
        m_outStream << "*  " << dateStr << endl;

        m_lastEpoTime = epoTime;       
    }

    // Print the data
    double sp3Clk = sv.clkbias * 1e6;  // Mayby has some wrong here.
    m_outStream << setiosflags(ios::fixed);
    m_outStream << "P" << prn
                << std::setw(14) << std::setprecision(6) << sv.x[0] / 1000.0
                << std::setw(14) << std::setprecision(6) << sv.x[1] / 1000.0
                << std::setw(14) << std::setprecision(6) << sv.x[2] / 1000.0
                << std::setw(14) << std::setprecision(6) << sp3Clk << endl;
    return;
}


void NtripSP3Stream::closeFile()
{
    m_outStream << "EOF" << endl;
    m_outStream.close();
}

void NtripSP3Stream::printSP3Ephmeris()
{
    std::list<std::string> prnList;
    prnList = m_ephStream->getPrnList();

    std::list<std::string>::iterator it;
    for(it=prnList.begin();it!=prnList.end();++it)
    {
        std::string prn = *it;
        m_eph = m_ephStream->ephLast(prn);

        if(m_eph == 0)
        {
            return;
        }

        GPSEphemeris2* ephGPS = dynamic_cast<GPSEphemeris2*>(m_eph);

        if(ephGPS)
        {
            Xvt xvt;

            // Get the satellite position and clock correction
            bool useCorr = true;
            ephGPS->dataLoadedFlag = true;;
            ephGPS->getCrd(m_lastClkCorrTime,xvt,useCorr);
            //PositionRecord position;
            //ClockRecord clk;

            ///////////////////////////
            // Here is a Test:
            // Output the data to SP3 file
            ///////////////////////////
            writeFile(m_lastClkCorrTime,prn,xvt);

    //        position.Pos = xvt.getPos();
    //        clk.bias = clkcorr;
    //        sp3EphStore.rejectBadPositions(true);
    //        sp3EphStore.addPositionRecord(sat, time, position);
    //        sp3EphStore.addClockRecord(sat, time, clk);
        }
        else
        {
            return;
        }
    }
}

void NtripSP3Stream::newGPSEph(GPSEphemeris2& eph)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    this->putEphmeris(&eph);
}

void NtripSP3Stream::putEphmeris(const OrbitEph2 *eph)
{
    OrbitEph2* newEph = 0;
    const GPSEphemeris2* ephGPS = dynamic_cast<const GPSEphemeris2*>(eph);
    if(ephGPS)
    {
        newEph = new GPSEphemeris2(*ephGPS);
    }
    else
    {
    }

    if(m_ephStream)
    {
        m_ephStream->putNewEph(newEph);
    }
    else
    {
        // Wrong ephmeris data
    }
}

void NtripSP3Stream::newOrbCorr(list<t_orbCorr> orbCorr)
{
    if(orbCorr.size() == 0)
    {
        return;
    }

    list<t_orbCorr>::iterator it = orbCorr.begin();
//    if(m_bRealTime)
//    {
//        if(m_sCorrMount.empty() || m_sCorrMount != it->_staID)
//        {
//            return;
//        }
//    }

    for(;it!=orbCorr.end();++it)
    {
        string prn = asString(it->_prn);
        OrbitEph2* ephLast = m_ephStream->ephLast(prn);
        OrbitEph2* ephPrev = m_ephStream->ephPrev(prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setOrbCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setOrbCorr(&(*it));
        }
    }
    return;
}

void NtripSP3Stream::newClkCorr(list<t_clkCorr> clkCorr)
{
    if(clkCorr.size() == 0)
    {
        return;
    }

    list<t_clkCorr>::iterator it = clkCorr.begin();
//    if(m_bRealTime)
//    {
//        if(m_sCorrMount.empty() || m_sCorrMount != it->_staID)
//        {
//            return;
//        }
//    }

    for(;it!=clkCorr.end();++it)
    {
        string prn = asString(it->_prn);
        OrbitEph2* ephLast = m_ephStream->ephLast(prn);
        OrbitEph2* ephPrev = m_ephStream->ephPrev(prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setClkCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setClkCorr(&(*it));
        }
    }
    return;
}

void NtripSP3Stream::updateEphmerisStore(NtripNavStream *ephStore)
{
    m_ephStream = new NtripNavStream(*ephStore);
}

NtripSP3Stream& operator>>(NtripSP3Stream& sp3Stream,
                           OrbitEphStore2& sp3EphStore)
{
    //sp3Stream.addSP3Ephmeris(sp3EphStore);
    return sp3Stream;
}
