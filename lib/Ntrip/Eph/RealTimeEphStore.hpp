#ifndef GPSTK_REALTIMEEPHSTORE_HPP
#define GPSTK_REALTIMEEPHSTORE_HPP

#include <iostream>
#include <list>
#include <deque>
#include <map>
#include <mutex>

#include "OrbitEph2.hpp"
#include "GPSEphemeris2.hpp"
#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "XvtStore.hpp"
namespace gpstk
{
    class RealTimeEphStore : public XvtStore<SatID>
    {
    public:

        /// Constructor
        RealTimeEphStore();

        /// Copy constructor
        RealTimeEphStore(RealTimeEphStore& right);

        /// Assignment Operator.
        /// @param right a const reference to the RealTimeEphStore object to copy.
        /// @return a reference to this RealTimeEphStore object.
        RealTimeEphStore& operator=( const RealTimeEphStore& right );

        /// Destructor
        virtual ~RealTimeEphStore();

        /// Returns the position, velocity, and clock offset of the indicated
        /// object in ECEF coordinates (meters) at the indicated time.
        /// @param[in] id satellite SatID
        /// @param[in] t the time to look up
        /// @return the Xvt of the satellite at the indicated time
        /// @throw InvalidRequest if the satellite is not stored or there are no
        ///        orbit elements at time t.
        virtual Xvt getXvt(const SatID& id, const CommonTime& t) const;


        /// Set the choice if using correction data
        void usingCorrection(bool flag)
        {
            m_bUseCorrection = flag;
        }

        /// Store new ephemeris
        bool putNewEph(OrbitEph2* eph, bool check);

        /// Get the last ephmeris in the queue
        const OrbitEph2* ephLast(const SatID& sat) const;

        /// Get the previous ephmeris in the queue
        const OrbitEph2* ephPrev(const SatID& sat) const;

        /// Get the satellite prn list
        std::list<SatID> getSatList();

        /// Set the max size of epochs
        void setMaxQueueSize(int size){m_iMaxQueueSize = size;}

        /// Clear the dataset, meaning remove all data
        virtual void clear();

        /// Return true if velocity data is present in the store
        virtual bool hasVelocity() const
        { return true; }

        /// Return true if the given SatID is present in the store
        virtual bool isPresent(const SatID& sat) const
        {
           if(m_ephMap.find(sat) != m_ephMap.end()) return true;
           return false;
        }

        /// A debugging function that outputs in human readable form,
        /// all data stored in this object.
        /// @param[in] s the stream to receive the output; defaults to cout
        /// @param[in] detail the level of detail to provide
        virtual void dump(std::ostream& s = std::cout, short detail = 0)const {;}

        /// Edit the dataset, removing data outside the indicated time interval
        /// @param[in] tmin defines the beginning of the time interval
        /// @param[in] tmax defines the end of the time interval
        virtual void edit(const CommonTime& tmin,
                          const CommonTime& tmax = CommonTime::END_OF_TIME){;}

        /// Return the time system of the store
        virtual TimeSystem getTimeSystem(void) const {return TimeSystem::Unknown;}

        /// Determine the earliest time for which this object can successfully
        /// determine the Xvt for any object.
        /// @return The initial time
        /// @throw InvalidRequest This is thrown if the object has no data.
        virtual CommonTime getInitialTime(void) const { return CommonTime::BEGINNING_OF_TIME;}

        /// Determine the latest time for which this object can successfully
        /// determine the Xvt for any object.
        /// @return The final time
        /// @throw InvalidRequest This is thrown if the object has no data.
        virtual CommonTime getFinalTime(void) const { return CommonTime::END_OF_TIME;}

    private:
        /// Check the ephmeris
        void checkEphmeris(OrbitEph2* eph);

        std::mutex m_mutex;                                ///< Mutex
        int m_iMaxQueueSize;                               ///< Maximun size of ephmeris data queue
        std::map<SatID, std::deque<OrbitEph2*> > m_ephMap; ///< Map to store the ephmeris
        bool m_bUseCorrection;                             ///< If use the orbit and clock correction
        bool m_bOnlyHealthy;                               ///< If only use healthy ephemeris
    };

} // End of namespace gpstk
#endif // GPSTK_REALTIMEEPHSTORE_HPP
