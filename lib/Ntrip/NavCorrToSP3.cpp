/* -------------------------------------------------------------------------
 * PPPBox
 * -------------------------------------------------------------------------
 *
 * Class:      NavCorrToSP3
 *
 * Purpose:    Make the sp3 file according to the broadcast ephemeris and 
 *             SSR correction file
 *
 * Author:     Q. Liu
 *
 * Created:    18-Apr-2017
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include "NavCorrToSP3.hpp"

#include "Rinex3NavData2.hpp"
#include "Rinex3NavStream.hpp"
#include "corrfile.h"
#include "StringUtils.hpp"

using namespace gpstk::StringUtils;


NavCorrToSP3::NavCorrToSP3()
{
    m_dSample = 900.0;
    m_ephStore = 0;
    m_corrFile = 0;
    m_sp3Stream = 0;
    m_sp3Stream = new NtripSP3Stream();
    m_sp3Stream->setWriteFile(true);
    m_ephStore =  new RealTimeEphStore();
}

NavCorrToSP3::~NavCorrToSP3()
{
    delete m_sp3Stream;
    for (unsigned ii = 0; ii < m_ephList.size(); ii++) 
    {
        delete m_ephList[ii];
    }
}

NavCorrToSP3* NavCorrToSP3::instance()
{
    static NavCorrToSP3 toSp3;
    return &toSp3;
}

void NavCorrToSP3::setCorrFile(std::string &corrFile)
{
    m_sCorrFile = corrFile;
    std::string::size_type len = corrFile.size();
    std::string tempPath(corrFile, 0, len-4);
    m_sSP3File = tempPath + ".sp3";
    m_sp3Stream->setSP3FileName(m_sSP3File);
}

// Read the brdc ephmeris file
void NavCorrToSP3::readNavFile()
{
    try
    {
        Rinex3NavStream navStream(m_sNavFile.c_str());
        Rinex3NavData2 navData;

        while (navStream >> navData)
        {
            OrbitEph2* eph = 0;
            if(navData.satSys == "G")
            {
                eph = new GPSEphemeris2(navData);
            }
            else if(navData.satSys == "R")
            {
            }
            else if(navData.satSys == "E")
            {
            }
            else if(navData.satSys == "J")
            {
            }
            else if(navData.satSys == "C")
            {
            }

            SatID prn = eph->satID;
            if(eph)
            {
                m_ephList.push_back(eph);
            }
            else
            {
                delete eph;
            }
        }
    }
    catch(gpstk::Exception& e)
    {
        std::cout << e.what() << "in class: " << getClassName();
        exit(1);
    }
    catch(...)
    {
        std::cout << "unknown error.  Done." << std::endl;
        exit(1);
    }
}

// Read Next Ephemeris
OrbitEph2* NavCorrToSP3::getNextEph(CommonTime& tt, const std::map<SatID, unsigned int>* corrIODMap)
{
    // Get Ephemeris according to IOD
    // ------------------------------
    if(corrIODMap)
    {
        std::map<SatID, unsigned int>::const_iterator itIOD = corrIODMap->begin();
        while(itIOD != corrIODMap->end())
        {
            SatID prn = itIOD->first;
            unsigned int iod = itIOD->second;
            std::vector<OrbitEph2*>::iterator itEph = m_ephList.begin();
            {
                while (itEph != m_ephList.end())
                {
                    OrbitEph2* eph = *itEph;
                    tt.setTimeSystem(eph->ctToc.getTimeSystem());
                    double dt = eph->ctToc - tt;
                    SatID sat = eph->satID;
                    unsigned int id = eph->IOD();
                    if(dt < 8*3600.0 && eph->satID == prn 
                        && eph->IOD() == iod)
                    {
                        m_ephList.erase(itEph);
                        sat = eph->satID;
                        return eph;
                    }
                    ++itEph;
                }
            }
            ++itIOD;
        }
    }

    // Get Ephemeris according to time
    // -------------------------------
    else
    {
        std::vector<OrbitEph2*>::iterator itEph = m_ephList.begin();
        {
            while (itEph != m_ephList.end())
            {
                OrbitEph2* eph = *itEph;
                double dt = eph->ctToc - tt;
                if(dt < 2*3600.0 )
                {
                    m_ephList.erase(itEph);
                    SatID sat = eph->satID;
                    return eph;
                }
                ++itEph;
            }
        }
    }

    return 0;
}

void NavCorrToSP3::newOrbCorrections(std::list<t_orbCorr> orbCorr)
{
    if(orbCorr.size() == 0)
    {
        return;
    }

    list<t_orbCorr>::iterator it = orbCorr.begin();
    for(;it!=orbCorr.end();++it)
    {
        OrbitEph2* ephLast = (OrbitEph2*)m_ephStore->ephLast(it->_prn);
        OrbitEph2* ephPrev = (OrbitEph2*)m_ephStore->ephPrev(it->_prn);
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

void NavCorrToSP3::newClkCorrections(std::list<t_clkCorr> clkCorr)
{
    if(clkCorr.size() == 0)
    {
        return;
    }

    list<t_clkCorr>::iterator it = clkCorr.begin();
    CommonTime lastClkCorrTime = it->_time;
    for(;it!=clkCorr.end();++it)
    {
        OrbitEph2* ephLast = (OrbitEph2*)m_ephStore->ephLast(it->_prn);
        OrbitEph2* ephPrev = (OrbitEph2*)m_ephStore->ephPrev(it->_prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setClkCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setClkCorr(&(*it));
        }
    }
    std::string utcTime = CivilTime(lastClkCorrTime).printf("%02H:%02M:%02S");
    std::cout << "New ClkCorr at time -> " << utcTime << std::endl;
    m_sp3Stream->setLastClkCorrTime(lastClkCorrTime);
    m_sp3Stream->updateEphmerisStore(m_ephStore);
    m_sp3Stream->dumpEpoch();
    return;
}

void NavCorrToSP3::getFirstCorrTime(CommonTime& firstEpo)
{
    if(!m_sCorrFile.empty())
    {
        m_corrFile = new t_corrFile(m_sCorrFile.c_str());
    }
    else
    {
        return;
    }

    if(m_corrFile)
    {
        CommonTime tempTime;
        m_corrFile->syncRead(tempTime);
        firstEpo = m_corrFile->getLastEpoTime();
    }
    delete m_corrFile;
    m_corrFile = 0;
}

// Convert brdc ephemeris file and correction file to SP3 file
void NavCorrToSP3::makeSP3File()
{
    // Read and store the ephemeris data 
    readNavFile();

    // Set the sample
    m_sp3Stream->setOutputSample(m_dSample);

    // Get the first correction record time
    CommonTime epoch;
    getFirstCorrTime(epoch);

    if(!m_sCorrFile.empty())
    {
        m_corrFile = new t_corrFile(m_sCorrFile.c_str());
    }
    else
    {
        return;
    }

    while(m_corrFile)
    {
        try
        {
            m_corrFile->syncRead(epoch);
            OrbitEph2* eph = 0;
            const std::map<SatID, unsigned int>* corrIODs = m_corrFile ? &m_corrFile->corrIODs() : 0;
            
            // Read all the ephemeris according to IODMap
            while((eph = getNextEph(epoch,corrIODs)) != 0)
            {
                m_ephStore->putNewEph(eph, false);
                delete eph;
                eph = 0;
            }

        }
        catch (const char* msg) 
        {
            std::cout <<msg;
            break;
        }
        catch (const string& msg) 
        {
            std::cout <<msg;
            break;
        }
        catch (...) 
        {
            std::cout << "unknown exceptions in corrFile";
            break;
        }

        epoch += 10;
    }  

}
