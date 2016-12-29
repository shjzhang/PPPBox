#ifndef SIGNALCENTER_HPP
#define SIGNALCENTER_HPP

#include <condition_variable>
#include <mutex>

#include "NtripNavStream.hpp"
#include "GPSEphemeris.hpp"

using namespace gpstk;
class SignalCenter
{
public:
    /// Constructor
    SignalCenter();

    /// Destructor
    ~SignalCenter();

    void newGPSEph(GPSEphemeris eph);
    void writeGPSEph(OrbitEph *eph);
    static SignalCenter* instance();
private:
    std::mutex m_ephMutex;

    std::condition_variable cond_EphDecoding;
    std::condition_variable cond_EphDecoded;

    NtripNavStream* m_navStream;   ///< Ntrip ephmeris stream

};

#define SIG_CENTER (SignalCenter::instance())

#endif // SIGNALCENTER_HPP
