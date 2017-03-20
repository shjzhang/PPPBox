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
    m_bHeaderWritten = false;
    m_eph = 0;
    m_dSample = 10;
    m_ephStream = new NtripNavStream();
    m_ephStore = new RealTimeEphStore();
}

NtripSP3Stream::~NtripSP3Stream()
{
    closeFile();
    delete m_eph;
    delete m_ephStream;
}



void NtripSP3Stream::resolveFileName(CommonTime &dateTime)
{
    // full GPS week
    std::string gpsWeek = GPSWeekSecond(dateTime).printf("%04F");

    // day of week
    std::string dow = GPSWeekSecond(dateTime).printf("%w");

    m_sFileName = m_sFilePath + "RTP" + gpsWeek + dow + "." + "sp3";
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

void NtripSP3Stream::writeFile(CommonTime& epoTime, SatID &prn, Xvt &sv)
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
    double sp3Clk = sv.clkbias * 1e6;
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

void NtripSP3Stream::dumpEpoch()
{
    std::list<SatID> prnList;
    prnList = m_ephStore->getSatList();

    std::list<SatID>::iterator it;
    for(it=prnList.begin();it!=prnList.end();++it)
    {
        m_eph = (OrbitEph2*)m_ephStore->ephLast(*it);

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
            ephGPS->getCrd(m_lastClkCorrTime, xvt, useCorr);

            // Output the data to SP3 file
            if(m_bWriteFile)
            {
                writeFile(m_lastClkCorrTime, *it, xvt);
            }
        }
        else
        {
            return;
        }
    }
}


void NtripSP3Stream::updateEphmerisStore(RealTimeEphStore *ephStore)
{
    m_ephStore = new RealTimeEphStore(*ephStore);
}

