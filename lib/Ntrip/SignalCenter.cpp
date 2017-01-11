#include <iostream>
#include "SignalCenter.hpp"


SignalCenter* SignalCenter::instance()
{
    static SignalCenter sigCenter;
    return &sigCenter;
}

SignalCenter::SignalCenter()
{
    m_navStream = 0;
    m_sCorrPath = ".";
    m_navStream = new NtripNavStream();
}

SignalCenter::~SignalCenter()
{
    delete m_navStream;
}

void SignalCenter::newGPSEph(GPSEphemeris2& eph)
{
    std::cout << "New GPS ephmeris! " << std::endl;
    std::lock_guard<std::mutex> guard(m_ephMutex);
    writeGPSEph(&eph);
}

void SignalCenter::writeGPSEph(GPSEphemeris2 *eph)
{
    m_navStream->checkPrintEph(eph);
}
