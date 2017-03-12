#ifndef RINEXOBSOPT_HPP
#define RINEXOBSOPT_HPP

#include <vector>

#include "Rinex3ObsHeader.hpp"
#include "CivilTime.hpp"
#include "Triple.hpp"

#define SYS_NONE    0x00                /* navigation system: none */
#define SYS_GPS     0x01                /* navigation system: GPS */
#define SYS_SBS     0x02                /* navigation system: SBAS */
#define SYS_GLO     0x04                /* navigation system: GLONASS */
#define SYS_GAL     0x08                /* navigation system: Galileo */
#define SYS_QZS     0x10                /* navigation system: QZSS */
#define SYS_BDS     0x20                /* navigation system: BeiDou */
#define SYS_LEO     0x40                /* navigation system: LEO */
#define SYS_ALL     0xFF                /* navigation system: all */

#define OBSTYPE_PR  0x01                /* observation type: pseudorange */
#define OBSTYPE_CP  0x02                /* observation type: carrier-phase */
#define OBSTYPE_DOP 0x04                /* observation type: doppler-freq */
#define OBSTYPE_SNR 0x08                /* observation type: SNR */
#define OBSTYPE_ALL 0xFF                /* observation type: all */

#define FREQTYPE_L1 0x01                /* frequency type: L1/E1 */
#define FREQTYPE_L2 0x02                /* frequency type: L2/B1 */
#define FREQTYPE_L5 0x04                /* frequency type: L5/E5a/L3 */
#define FREQTYPE_L6 0x08                /* frequency type: E6/LEX/B3 */
#define FREQTYPE_L7 0x10                /* frequency type: E5b/B2 */
#define FREQTYPE_L8 0x20                /* frequency type: E5(a+b) */
#define FREQTYPE_ALL 0xFF               /* frequency type: all */

namespace gpstk
{

    class RinexObsOpt
    {
    public:
        RinexObsOpt();

        /// Set the default obstype
        void defaultObsType();

        /// set the options value
        void setFileType(std::string& filetype) {m_sFileType = filetype;}
        void setFileSys(std::string& filesys){m_sFileSys = filesys;}

        void setPrgName(std::string& prgname){m_sPrgName = prgname;}
        void setRunBy(std::string& runby){m_sRunBy = runby;}
        void setDateStr(std::string& datestr){m_sDate = datestr;}
        void addComment(std::string& comment){m_CommentList.push_back(comment);}

        void setMarkerName(std::string& markerName){m_sMarker = markerName;}
        void setMarkNo(std::string& markerNo){m_sMarkerNo = markerNo;}
        void setMarkerType(std::string& markerType){m_sMarkerType = markerType;}
        void setObserver(std::string& observer){m_sObserver = observer;}
        void setObsAgency(std::string& agency){m_sAgency = agency;}
        void setReceiver(std::string rec[3]){for(int i=0;i<3;i++) m_sRec[i]=rec[i];}
        void setAntenna(std::string ant[3]){for(int i=0;i<3;i++) m_sAnt[i]=ant[i];}
        void setAppPos(double pos[3]){for(int i=0;i<3;i++) m_dAppPos[i]=pos[i];}
        void setAntDlt(double antdel[3]){for(int i=0;i<3;i++) m_dAntDel[i]=antdel[i];}

        void setFirstObsTime(CivilTime &ft){m_firstObsTime = ft;}
        void setLastObsTime(CivilTime &lt){m_lastObsTime = lt;}

        void setRnxVer(double version){m_dRnxVer = version;}
        void setNavSys(int sys);
        void setObsType(int type)
        {
            m_iObsType = type;
            makeObsType();
        }
        void setFreqType(int type){m_iFreqType = type;}

        /// get the option value
        void getRnxObsHeader(Rinex3ObsHeader& header);

        int getNavSys(){return m_iNavSys;}
        int getFreqType(){return m_iFreqType;}
        int getObsType(){return m_iObsType;}
        void getNavSys(SatID& fileSys);

    private:

        void makeObsType();

        CivilTime m_firstObsTime, m_lastObsTime;
        double m_dRnxVer;
        SatID m_fileSysSat;
        std::string m_sFileType,m_sFileSys;
        std::string m_sPrgName,m_sRunBy, m_sDate;
        std::string m_sMarker,m_sMarkerNo,m_sMarkerType,m_sObserver, m_sAgency ,m_sRec[3],m_sAnt[3];
        std::vector<std::string> m_CommentList;
        double m_dAppPos[3], m_dAntDel[3];
        int m_iRnxFile,m_iNavSys,m_iObsType,m_iFreqType;
        std::map<std::string,std::vector<RinexObsID> > m_mapObsTypes;

    }; // end of class "RinexObsOpt"

} // end of namespace "gpstk"

#endif // RINEXOBSOPT_HPP
