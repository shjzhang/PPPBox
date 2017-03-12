#include "RinexObsOpt.hpp"

namespace gpstk
{

    RinexObsOpt::RinexObsOpt()
    {
        m_dRnxVer = 3.01;
        m_sFileType = "";
        m_sPrgName = "";
        m_sRunBy = "";
        m_sMarker = "";
        m_sMarkerNo = "";
        m_sMarkerType = "";
        m_sObserver = "";
        m_sAgency = "";

        m_sRec[0] = m_sRec[1] = m_sRec[2] = "";
        m_sAnt[0] = m_sAnt[0] = "";
        m_dAppPos[0] = m_dAppPos[1] = m_dAppPos[2] = 0.0;
        m_dAntDel[0] = m_dAntDel[1] = m_dAntDel[2] = 0.0;

        defaultObsType();
    }

    void RinexObsOpt::defaultObsType()
    {
        std::string gpsObsType[9] = {"C1C", "L1C","S1C",
                                     "C1W", "L1W","S1W",
                                     "C2W", "L2W","S2W"};
        for(int i=0; i!=9; ++i)
        {
            RinexObsID ro(gpsObsType[i]);
            m_mapObsTypes["G"].push_back(ro);
        }
    }


    void RinexObsOpt::getRnxObsHeader(Rinex3ObsHeader &header)
    {
        header.version = m_dRnxVer;
        header.fileType = m_sFileType;
        header.agency = m_sAgency;
        header.fileSys = m_sFileSys;
        header.fileSysSat = m_fileSysSat;

        header.fileProgram = m_sPrgName;
        header.fileAgency = m_sRunBy;
        header.date = m_sDate;

        header.commentList = m_CommentList;

        header.markerName = m_sMarker;
        header.markerNumber = m_sMarkerNo;
        header.markerType = m_sMarkerType;

        header.observer = m_sObserver;
        header.agency = m_sAgency;

        header.recNo = m_sRec[0];
        header.recType = m_sRec[1];
        header.recVers = m_sRec[2];

        header.antNo = m_sAnt[0];
        header.antType = m_sAnt[2];

        header.antennaPosition = Triple(m_dAppPos[0],
                         m_dAppPos[1], m_dAppPos[2]);
        header.antennaDeltaHEN = Triple(m_dAntDel[0],
                         m_dAntDel[1], m_dAntDel[2]);

        header.firstObs = m_firstObsTime;
        header.lastObs = m_lastObsTime;

        header.mapObsTypes = m_mapObsTypes;
    }

    void RinexObsOpt::setNavSys(int sys)
    {
        m_iNavSys = sys;
        if(m_iNavSys == SYS_GPS)
        {
            m_fileSysSat.system = SatID::systemGPS;
        }
        else if(m_iNavSys == SYS_GLO)
        {
            m_fileSysSat.system = SatID::systemGlonass;
        }
        else if(m_iNavSys == SYS_GAL)
        {
            m_fileSysSat.system = SatID::systemGalileo;
        }
        else if(m_iNavSys == SYS_BDS)
        {
            m_fileSysSat.system = SatID::systemBeiDou;
        }
        else if(m_iNavSys == SYS_QZS)
        {
            m_fileSysSat.system = SatID::systemQZSS;
        }
        else if(m_iNavSys == SYS_SBS)
        {
            m_fileSysSat.system = SatID::systemGeosync;
        }
        else
        {
            m_fileSysSat.system = SatID::systemMixed;
        }
        m_sFileSys = m_fileSysSat.convertSatelliteSystemToString(m_fileSysSat.system);

    }

    void RinexObsOpt::makeObsType()
    {

        std::string freq[8] = {""};
        std::string obs[4] = {""};
        /* supported codes by rtcm3 */
        std::string trackingCode_GPS[5] = {"1C","1W","2W","2X","5X"};
        std::string trackingCode_GLO[2] = {"C","P"};
        //std::string trackingCode_Gal    = "X";
        std::string trackingCode_BDS    = "I";

        // get frequencies
        if ((m_iFreqType & FREQTYPE_L1) == FREQTYPE_L1)
        {
            freq[0] = "1";
        }
        if ((m_iFreqType & FREQTYPE_L2) == FREQTYPE_L2)
        {
            freq[1] = "2";
        }
        if ((m_iFreqType & FREQTYPE_L5) == FREQTYPE_L5)
        {
            freq[4] = "5";
        }

        // get observation type
        if ((m_iObsType & OBSTYPE_PR) == OBSTYPE_PR)
        {
            obs[0] = "C";
        }
        if ((m_iObsType & OBSTYPE_CP) == OBSTYPE_CP)
        {
            obs[1] = "L";
        }
        if ((m_iObsType & OBSTYPE_DOP) == OBSTYPE_DOP)
        {
            obs[2] = "D";
        }
        if ((m_iObsType & OBSTYPE_SNR) == OBSTYPE_SNR)
        {
            obs[3] = "S";
        }


        std::string sysStr;
        if((m_iNavSys & SYS_GPS) == SYS_GPS)
        {
            sysStr = "G";
            std::vector<RinexObsID> obsTypeList;
            for(int i=0;i<5;++i)
            {
                std::string typeTemp;
                for(int j=0;j<8;++j)
                {
                    if(freq[j]!="")
                    {

                        for(int k=0;k<4;++k)
                        {
                            if(obs[k]!="")
                            {
                                std::string temp = trackingCode_GPS[i];
                                char frq = temp[0];
                                temp = frq;
                                if(freq[j] == temp)
                                {
                                  typeTemp = obs[k] + trackingCode_GPS[i];
                                  RinexObsID rt(typeTemp);
                                  obsTypeList.push_back(rt);
                                }
                            }

                        }
                    }
                }
            }
            m_mapObsTypes[sysStr] = obsTypeList;
        }
        if((m_iNavSys & SYS_GLO) == SYS_GLO)
        {
            sysStr = "R";
            std::vector<RinexObsID> obsTypeList;
            for(int i=0;i<4;++i)
            {
                std::string typeTemp;
                if(obs[i]!="")
                {
                    for(int j=0;j<2;++j)
                    {
                        if(freq[j]!="")
                        {
                            for(int k=0;k<2;++k)
                            {
                                typeTemp = obs[i] + freq[j]
                                        + trackingCode_GLO[k];
                                RinexObsID rt(typeTemp);
                                obsTypeList.push_back(rt);
                            }
                        }
                    }
                }
            }
            m_mapObsTypes[sysStr] = obsTypeList;
        }
        if((m_iNavSys & SYS_GAL) == SYS_GAL)
        {

        }
        if((m_iNavSys & SYS_BDS) == SYS_BDS)
        {
            sysStr = "C";
            std::string freq_bds[8] = {""};
            freq_bds[1] = "2";
            freq_bds[5] = "6";
            freq_bds[6] = "7";
            std::vector<RinexObsID> obsTypeList;
            for(int i=0;i<4;++i)
            {
                std::string typeTemp;
                if(obs[i]!="")
                {
                    for(int j=0;j<8;++j)
                    {
                        if(freq[j]!="")
                        {
                            typeTemp = obs[i] + freq[j]
                                    + trackingCode_BDS;
                            RinexObsID rt(typeTemp);
                            obsTypeList.push_back(rt);
                        }
                    }
                }
            }
            m_mapObsTypes[sysStr] = obsTypeList;
        }
    }

}// end of namespace "gpstk"
