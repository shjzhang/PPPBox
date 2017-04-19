#include <iomanip>
#include "NtripSP3Stream.hpp"
#include "PositionSatStore.hpp"
#include "ClockSatStore.hpp"
#include "FileSpec.hpp"
#include "FileUtils.hpp"
#include "SatID.hpp"
#include "GPSWeekSecond.hpp"
#include "SystemTime.hpp"
#include "MJD.hpp"

using namespace gpstk::StringUtils;

NtripSP3Stream::NtripSP3Stream()
{
    m_sFileName = "";
    m_bHeaderWritten = false;
    m_eph = 0;
    m_dSample = 1.0;
}

NtripSP3Stream::~NtripSP3Stream()
{
    closeFile();
    //delete m_eph;
}



void NtripSP3Stream::resolveFileName(const CommonTime &dateTime)
{
    // full GPS week
    std::string gpsWeek = GPSWeekSecond(dateTime).printf("%04F");

    // day of week
    std::string dow = GPSWeekSecond(dateTime).printf("%w");

	if(m_sCorrMount.empty())
	{
	    m_sCorrMount = "MNT";
	}
    m_sFileName = m_sFilePath + m_sCorrMount + gpsWeek + dow + "." + "sp3";
}


void NtripSP3Stream::printHeader(const CommonTime& dateTime)
{
    if(m_sFileName.empty())
    {
        resolveFileName(dateTime);
    }

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

    GPSWeekSecond gws( dateTime );
    int gpsWeek = gws.getWeek();
    double gpsSOW = gws.getSOW();

    double sec = fmod(gpsSOW, 60.0);
    MJD mjd;
    mjd.convertFromCommonTime(dateTime);
    
    std::string utcTime = CivilTime(dateTime).printf("%Y %02m %02d %02H %02M");

    int    mjdInt = int(mjd.mjd);
    double dayfrac = mjd.mjd - mjdInt;

    m_outStream << setiosflags(ios::fixed);

    m_outStream << "#aP" << utcTime
         << setw(12) << setprecision(8) << sec
         << " " << setw(7) << 1440 << " ORBIT IGS08 HLM  IGS" << endl;

    m_outStream << "## "
         << setw(4)  << gpsWeek
         << setw(16) << setprecision(8) << gpsSOW
         << setw(15) << setprecision(8) << m_dSample
         << setw(6)  << mjdInt
         << setw(16) << setprecision(13) << dayfrac << endl;

    m_outStream << "+   32   G01G02G03G04G05G06G07G08G09G10G11G12G13G14G15G16G17\n"
                << "+        G18G19G20G21G22G23G24G25G26G27G28G29G30G31G32  0  0\n"
                << "+          0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "+          0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "+          0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "++         0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0\n"
                << "%c G  cc GPS ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n"
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

        // Print the new epoch
        dateStr = CivilTime(epoTime).printf("%Y %2m %2d %2H %2M %2S");
        m_outStream << "*  " << dateStr << ".00000000" << endl;

        m_lastEpoTime = epoTime;       
    }

    // Print the data
    int satNum = prn.id;
    string satStr;
    if(prn.system == SatID::systemGPS)
    {
        satStr = "G";
    }
    if(satNum < 10)
    {
        satStr = satStr + "0" + asString(satNum);
    }
    else
    {
        satStr += asString(satNum);
    }
    double sp3Clk = ( sv.clkbias + sv.relcorr ) * 1e6;
    m_outStream << setiosflags(ios::fixed);
    m_outStream << "P" << satStr
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
    unique_lock<mutex> lock(m_mutex);
    std::list<SatID> prnList;
    prnList = m_ephStore.getSatList();
    
    if(m_lastClkCorrTime != m_lastEpoTime)
    {
        if((m_lastEpoTime.getDays()!=0.0 || m_lastEpoTime.getSecondOfDay()!=0.0)
             && m_dSample > 0)
        {
            std::list<SatID>::iterator it;
            for(CommonTime ep = m_lastEpoTime + m_dSample; ep < m_lastClkCorrTime;
                ep = ep + m_dSample)
            {
                for(it=prnList.begin();it!=prnList.end();++it)
                {
                    m_eph = (OrbitEph2*)m_ephStore.ephLast(*it);

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
                        ephGPS->getCrd(ep, xvt, useCorr);

                        // Output the data to SP3 file
                        if(m_bWriteFile)
                        {
                            writeFile(ep, *it, xvt);
                        }

                    }
                    else
                    {
                        return;
                    }
                }
            }
        }
        else
        {
            std::list<SatID>::iterator it;
            for(it=prnList.begin();it!=prnList.end();++it)
            {
                m_eph = (OrbitEph2*)m_ephStore.ephLast(*it);

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
    }
}


void NtripSP3Stream::updateEphmerisStore(RealTimeEphStore *ephStore)
{
	std::unique_lock<std::mutex> lock(m_mutex);
    m_ephStore = *ephStore;
}

