#pragma ident "$Id$"

/**
 * @file ComputeSatPCOffset.hpp
 * This class computes the satellite antenna phase correction, in meters.
 */

#ifndef GPSTK_COMPUTESATPCOFFSET_HPP
#define GPSTK_COMPUTESATPCOFFSET_HPP

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================



#include <cmath>
#include <string>
#include <sstream>
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "AntexReader.hpp"
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"

namespace gpstk
{

      /** This class computes the satellite antenna phase correction, in meters.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *
       * @endcode
       *
       * \warning The ComputeSatPCOffset objects generate corrections that are
       * interpreted as an "advance" in the signal, instead of a delay.
       * Therefore, those corrections always hava a negative sign.
       *
       */
   class ComputeSatPCOffset 
   {
   public:

         /// Default constructor
      ComputeSatPCOffset()
         : pAntexReader(NULL)
      { };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeSatPCOffset( AntexReader& antexObj )
         : pAntexReader(&antexObj)
      { };


         /** Compute the value of satellite antenna phase correction, in meters
          * @param satid     Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Satellite antenna phase correction, in meters.
          */
      virtual Triple getSatPCOffset( const SatID& satid,
                                     const CommonTime& time,
                                     const Triple& satpos,
                                     const Triple& sunPosition );


         /// Returns a pointer to the AntexReader object currently in use.
      virtual AntexReader *getAntexReader(void) const
      { return pAntexReader; };

         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
      virtual ComputeSatPCOffset& setAntexReader(AntexReader& antexObj)
      { pAntexReader = &antexObj; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeSatPCOffset() {};


   private:


         /// Object to read satellite data file (PRN_GPS)
//    SatDataReader satData;


         /// Name of "PRN_GPS"-like file containing satellite data.
//    std::string fileData;


         /// Pointer to object containing satellite antenna data, if available.
      AntexReader* pAntexReader;



   }; // End of class 'ComputeSatPCOffset'

      //@}

}  // End of namespace gpstk

#endif // GPSTK_COMPUTESATPCOFFSET_HPP
