#pragma ident "$Id: RinexUPDXStore.hpp 2897 2011-09-14 20:08:46Z shjzhang $"
/**
 * @file RinexUPDXStore.hpp
 * Store a tabular list of satellite biaes, and compute values at any time
 * from this table. 
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#ifndef GPSTK_RINEX_UPDX_STORE_INCLUDE
#define GPSTK_RINEX_UPDX_STORE_INCLUDE

#include <map>
#include <iostream>

#include "Exception.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "TabularSatStore.hpp"
#include "FileStore.hpp"
#include "RinexUPDXData.hpp"
#include "RinexUPDXHeader.hpp"
#include "RinexUPDXStream.hpp"
#include "SatUPDX.hpp"

namespace gpstk
{

      /** @addtogroup ephemstore */
      //@{

      /** This class store the satellite upds into a table list. And, you can 
       *  get the values at given time by calling the method 'getValue'.
       *
       *  The widelane and narrow-lane fractional-cycle upds are determined 
       *  by using the program 'NeuNet', and they are stored in the format same
       *  with the rinex clock data with unit (meter).
       *
       *  \Warning. Pleas be aware at the data unit.
       *
       */
   class RinexUPDXStore : public TabularSatStore<SatUPDX>
   {
   public:

         /// Default constructor
      RinexUPDXStore() {};


         /** Return value for the given satellite at the given time.
          *
          * @param sat      the SatID of the satellite of interest
          * @param ttag     the time (CommonTime) of interest
          * @return         object of type SatUPDX containing the data value(s).
          *
          * @throw InvalidRequest 
          *
          *  if data value cannot be computed because:
          *
          *  a) the time t does not lie within the time limits of the data table
          *  b) checkDataGap is true and there is a data gap
          *  c) checkInterval is true and the interval is larger than maxInterval
          *
          */
      virtual SatUPDX getValue(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);


         /** Dump information about the object to an ostream.
          *
          *  @param[in] os      ostream to receive the output; defaults to std::cout
          *  @param[in] detail  integer level of detail to provide; allowed values are
          *
          *     0: number of satellites, time step and time limits, flags,
          *            gap and interval flags and values, and file information
          *     1: number of data/sat
          *     2: above plus all the data tables
          */
      virtual void dump(std::ostream& os = std::cout, int detail = 0) const throw()
      {
         os << " Dump of RinexUPDXStore(" << detail << "):\n";

         os << " Interpolation is ";
         os << " Linear." << std::endl;

            // dump  the satellite upd data
         TabularSatStore<SatUPDX>::dump(os,detail);

         os << " End dump of RinexUPDXStore.\n";
      }

         /** Add a complete SatUPDX to the store; this is the preferred method
          *  of adding data to the tables.
          *
          *  \Warning. 
          *
          *  If these addXXX() routines are used more than once for the same record
          *  (sat,ttag), be aware that since ttag is used as they key in a std::map,
          *  the value used must be EXACTLY the same in all calls. (numerical noise 
          *  could cause the std::map to consider two "equal" ttags as different).
          *
          */
      void addSatUPDX(const SatID& sat, 
                      const CommonTime& ttag,
                      const SatUPDX& rec)
         throw(InvalidRequest);


         /// Load a satellite upd file 
      void loadFile(const std::string& filename) 
          throw(Exception);


         /// Destructor
      virtual ~RinexUPDXStore() {};


   protected:


         /// Store half the interpolation order, for convenience
      unsigned int Nhalf;

         /// FileStore for the satellite upd input files
      FileStore<RinexUPDXHeader> clkFiles;

   }; // end class RinexUPDXStore

      //@}

}  // End of namespace gpstk

#endif // GPSTK_RINEX_UPDX_STORE_INCLUDE
