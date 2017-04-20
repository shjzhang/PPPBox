# ifndef NAVCORR_TO_SP3_HPP
# define NAVCORR_TO_SP3_HPP

#include <vector>
#include <string>
#include <map>
#include <list>

#include "NtripSP3Stream.hpp"
#include "OrbitEph2.hpp"
#include "GPSEphemeris2.hpp"
#include "RealTimeEphStore.hpp"
#include "StringUtils.hpp"

using namespace gpstk;

class t_corrFile;

class NavCorrToSP3
{
public:

    /// Default constructor
    NavCorrToSP3();

    /// Destructor
    ~NavCorrToSP3();

    std::string getClassName()
    { return "NavCorrToSP3"; }

    /// Set and save the broadcast ephmeris file
    void setNavFile(const std::string& navFile)
    { m_sNavFile = navFile; }

    /// Set and save the SSR correction file
    void setCorrFile(std::string &corrFile);

    /// Set the sample of output sp3 file
    void setSample(double sample) 
    { m_dSample = sample; }

    /// Read Navigation file
    void readNavFile();

    /// Get next ephmeris data
    OrbitEph2* getNextEph(CommonTime& tt, const std::map<SatID, unsigned int>* corrIODMap);

    /// Make SP3 file from broadcast ephmeris file 
    /// and SSR correction file
    void makeSP3File();

    /// Set the type of ephemeris reference point
    void setEphRefPoint(NtripSP3Stream::ReferencePoint refPoint)
    { m_sp3Stream->setEphRefPoint(refPoint); }

    /// Set the antex file
    void setAntexFile(std::string& file)
    {m_sp3Stream->setAntexFile(file);}

    /// Instance of NavCorrToSP3 class
    static NavCorrToSP3* instance();

    /// Get the first epoch of correction file record
    void getFirstCorrTime(CommonTime&);

    /// New orbit correction
    void newOrbCorrections(std::list<t_orbCorr>);

    /// New clock correction
    void newClkCorrections(std::list<t_clkCorr>);

    void newCodeBiases(std::list<t_satCodeBias>){;}
    void newPhaseBiases(std::list<t_satPhaseBias>){;}
    void newTec(t_vTec){;}

private:

    std::string m_sNavFile;             ///< Name of Broadcast ephmeris file 
    std::string m_sCorrFile;            ///< Name of SSR correction file
    std::string m_sSP3File;             ///< Name of Outputed SP3 file
    std::vector<OrbitEph2*> m_ephList;  ///< Ephmeris list
    double m_dSample;                   ///< Output sample
    NtripSP3Stream* m_sp3Stream;        ///< SP3 file writer
    RealTimeEphStore* m_ephStore;       ///< Ephemeris store
    t_corrFile* m_corrFile;             ///< t_corrfile object pointer
};

#define navCorrToSp3 (NavCorrToSP3::instance())

#endif
