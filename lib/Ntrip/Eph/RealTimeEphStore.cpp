#include "RealTimeEphStore.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
    RealTimeEphStore::RealTimeEphStore()
    {
        m_bOnlyHealthy = true;
        m_bUseCorrection = false;
    }

    RealTimeEphStore::RealTimeEphStore(RealTimeEphStore &right)
    {
        m_bOnlyHealthy = right.m_bOnlyHealthy;
        m_bUseCorrection = right.m_bUseCorrection;
        m_ephMap = right.m_ephMap;
    }

    RealTimeEphStore& RealTimeEphStore::operator=( const RealTimeEphStore& right )
    {
        m_bOnlyHealthy = right.m_bOnlyHealthy;
        m_bUseCorrection = right.m_bUseCorrection;
        m_ephMap = right.m_ephMap;
        return *this;
    }

    RealTimeEphStore::~RealTimeEphStore()
    {
        clear();
    }

    void RealTimeEphStore::clear()
    {
        std::map<SatID, std::deque<OrbitEph2*> >::iterator it = m_ephMap.begin();
        while(it!=m_ephMap.end())
        {
            std::deque<OrbitEph2*>& q = it->second;
            for(unsigned i = 0;i<q.size();++i)
            {
                delete q[i];
            }
            ++it;
        }
    }

    Xvt RealTimeEphStore::getXvt(const SatID& sat, const CommonTime& t) const
    {
        try
        {
            // get the appropriate OrbitEph
            const OrbitEph2* eLast = this->ephLast(sat);
            const OrbitEph2* ePrev = this->ephPrev(sat);

            // compute the position, velocity and time
            Xvt sv;
            if( eLast && eLast->getCrd(t,sv,m_bUseCorrection) )
            {
                return sv;
            }
            else if( ePrev && ePrev->getCrd(t,sv,m_bUseCorrection) )
            {
                return sv;
            }
            if(!eLast || !ePrev)
            {
               GPSTK_THROW(InvalidRequest("No OrbitEph for satellite "
                                         + StringUtils::asString(sat)));
            }

            // no consideration is given to health here (OrbitEph does not have health);
            // derived classes should override isHealthy()
            //if(m_bOnlyHealthy)
            //   GPSTK_THROW(InvalidRequest("Not healthy"));


            return sv;
        }
        catch(InvalidRequest& ir)
        {
            GPSTK_RETHROW(ir);
        }
    }

    bool RealTimeEphStore::putNewEph(OrbitEph2 *eph, bool check)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        if(eph == 0)
        {
            return false;
        }
        eph->dataLoadedFlag = true;
        if(check)
        {
            checkEphmeris(eph);
        }
        const GPSEphemeris2* ephGPS = dynamic_cast<const GPSEphemeris2*>(eph);

        OrbitEph2* ephNew = 0;

        if(ephGPS)
        {
            ephNew = new GPSEphemeris2(*ephGPS);
        }
        else
        {
            return false;
        }

        const OrbitEph2* ephOld = ephLast(eph->satID);

        if(ephOld && (ephOld->getCheckState() == OrbitEph2::bad
                   || ephOld->getCheckState() == OrbitEph2::outdated))
        {
            ephOld = 0;
        }

        if((ephOld ==0 || ephNew->isNewerThan(ephOld)) &&
           (eph->getCheckState() != OrbitEph2::bad &&
            eph->getCheckState() != OrbitEph2::outdated))
        {
            std::deque<OrbitEph2*>& qq = m_ephMap[eph->satID];
            qq.push_back(ephNew);
            if(qq.size() > m_iMaxQueueSize)
            {
                delete qq.front();
                qq.pop_front();
            }
            return true;
        }
        else
        {
            delete ephNew;
            return false;
        }
    }

    void RealTimeEphStore::checkEphmeris(OrbitEph2 *eph)
    {
        if(!eph)
        {
            return;
        }

        // Simple Check - check satellite radial distance
        // ----------------------------------------------
        Xvt sv;

        if(!eph->getCrd(eph->ctToc, sv, false))
        {
            eph->setCheckState(OrbitEph2::bad);
            return;
        }

        double rr = sv.x.mag();

        const double MINDIST = 2.e7;
        const double MAXDIST = 6.e7;

        if (rr < MINDIST || rr > MAXDIST)
        {
            eph->setCheckState(OrbitEph2::bad);
            return;
        }

        // Check whether the epoch is too far away from the current time
        // --------------------------------------------------------

        CommonTime toc = eph->ctToc;
        SystemTime now;
        CommonTime currentTime(now);

        currentTime.setTimeSystem(toc.getTimeSystem());
        double timeDiff = fabs(toc - currentTime);
        SatID::SatelliteSystem system = eph->satID.system;
        if(system == SatID::systemGPS || system == SatID::systemGalileo)
        {
            // update interval: 2h, data sets are valid for 4 hours
            if(timeDiff > 4*3600)
            {
                // outdated
                eph->setCheckState(OrbitEph2::outdated);
                return;
            }
        }
        else if(system == SatID::systemGlonass)
        {
            // updated every 30 minutes
            if(timeDiff > 1*3600)
            {
                // outdated
                eph->setCheckState(OrbitEph2::outdated);
                return;
            }
        }
        else if(system == SatID::systemBeiDou)
        {
            // updates 1 (GEO) up to 6 hours
            if(timeDiff > 6*3600)
            {
                // outdated
                eph->setCheckState(OrbitEph2::outdated);
                return;
            }
        }
        else if(system == SatID::systemQZSS)
        {
            // orbit parameters are valid for 7200 seconds (at minimum)
            if(timeDiff > 4*3600)
            {
                // outdated
                eph->setCheckState(OrbitEph2::outdated);
                return;
            }
        }

        // Check consistency with older ephemerides
        // ----------------------------------------
        const double MAXDIFF = 1000.0;
        OrbitEph2* ephL = (OrbitEph2*)ephLast(eph->satID);

        if(ephL)
        {
            Xvt svL;
            ephL->getCrd(eph->ctToc, svL, false);

            double dt = eph->ctToc - ephL->ctToc;
            double diffX = (sv.x - svL.x).mag();
            double diffC = fabs(sv.clkbias - svL.clkbias) * C_MPS;

            // some lines to allow update of ephemeris data sets after outage
            if(system == SatID::systemGPS && dt > 4*3600)
            {
                ephL->setCheckState(OrbitEph2::outdated);
                return;
            }
            if(system == SatID::systemGalileo && dt > 4*3600)
            {
                ephL->setCheckState(OrbitEph2::outdated);
                return;
            }
            if(system == SatID::systemGlonass && dt > 1*3600)
            {
                ephL->setCheckState(OrbitEph2::outdated);
                return;
            }
            if(system == SatID::systemBeiDou && dt > 6*3600)
            {
                ephL->setCheckState(OrbitEph2::outdated);
                return;
            }
            if(system == SatID::systemQZSS && dt > 4*3600)
            {
                ephL->setCheckState(OrbitEph2::outdated);
                return;
            }

            if (diffX < MAXDIFF && diffC < MAXDIFF)
            {
              if (dt != 0.0) {
                eph->setCheckState(OrbitEph2::ok);
                ephL->setCheckState(OrbitEph2::ok);
              }
              else
              {
                  // do nothing here
              }
            }
            else
            {
              if (ephL->getCheckState() == OrbitEph2::ok)
              {
                eph->setCheckState(OrbitEph2::bad);
              }
              else
              {
                  // do nothing here
              }
            }
        }
    }

    const OrbitEph2* RealTimeEphStore::ephLast(const SatID &sat) const
    {
        std::map<SatID, std::deque<OrbitEph2*> >::const_iterator it;
        it = m_ephMap.find(sat);
        if(it != m_ephMap.end())
        {
            return it->second.back();
        }
        else
        {
            return 0;
        }
    }

    const OrbitEph2 *RealTimeEphStore::ephPrev(const SatID &sat) const
    {
        std::map<SatID, std::deque<OrbitEph2*> >::const_iterator it;
        it = m_ephMap.find(sat);
        if(it != m_ephMap.end())
        {
            unsigned n = it->second.size();
            if(n > 1)
            {
                return it->second.at(n-2);
            }
            else
            {
                // do nothing here
            }
        }
        else
        {
            return 0;
        }
    }

    std::list<SatID> RealTimeEphStore::getSatList()
    {
        std::list<SatID> satList;
        std::map<SatID, std::deque<OrbitEph2*> >::iterator it;
        for(it=m_ephMap.begin();it!=m_ephMap.end();++it)
        {
            satList.push_back(it->first);
        }
        return satList;
    }

}
