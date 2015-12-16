#pragma ident "$Id$"

/**
 * @file ComputeSatPCOffset.cpp
 * This class computes the satellite antenna phase correction, in meters.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  shjzhang
//
//============================================================================


#include "ComputeSatPCOffset.hpp"

using namespace std;

namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeSatPCOffset::getClassName() const
   { return "ComputeSatPCOffset"; }


      /* Compute the value of satellite antenna phase correction, in meters.
       * @param satid     Satellite ID
       * @param time      Epoch of interest
       * @param satpos    Satellite position, as a Triple
       * @param sunpos    Sun position, as a Triple
       *
       * @return Satellite antenna phase correction, in meters.
       */
   Triple ComputeSatPCOffset::getSatPCOffset( const SatID& satid,
                                              const CommonTime& time,
                                              const Triple& satpos,
                                              const Triple& sunPosition )
   {

         // Unitary vector from satellite to Earth mass center (ECEF)
      Triple rk( ( (-1.0)*(satpos.unitVector()) ) );

         // Unitary vector from Earth mass center to Sun (ECEF)
      Triple ri( sunPosition.unitVector() );

         // rj = rk x ri: Rotation axis of solar panels (ECEF)
      Triple rj(rk.cross(ri));

         // Redefine ri: ri = rj x rk (ECEF)
      ri = rj.cross(rk);

         // Let's convert ri to an unitary vector. (ECEF)
      ri = ri.unitVector();

         // This variable that will hold the correction, 0.0 by default
      Triple svPCOffset;

         // Check is Antex antenna information is available or not, and if
         // available, whether satellite phase center information is absolute
         // or relative
      bool absoluteModel( false );
      if( pAntexReader != NULL )
      {
         absoluteModel = pAntexReader->isAbsolute();

            // Get satellite information in Antex format. Currently this
            // only works for GPS and Glonass.
         if( satid.system == SatID::systemGPS )
         {
            std::stringstream sat;
            sat << "G";
            if( satid.id < 10 )
            {
               sat << "0";
            }
            sat << satid.id;

               // Get satellite antenna information out of AntexReader object
            Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

               // Get antenna eccentricity for frequency "G01" (L1), in
               // satellite reference system.
               // NOTE: It is NOT in ECEF, it is in UEN!!!
            Triple satAnt( antenna.getAntennaEccentricity( Antenna::G01) );

                  // Warning: 
                  // the elevation can't be considered because the receiver
                  // position has not been given

//             // Now, get the phase center variation.
//          Triple var( antenna.getAntennaPCVariation( Antenna::G01, elev) );
//             // We must substract them
//          satAnt = satAnt - var;

                  // Change to ECEF
            Triple svAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

            svPCOffset = svAntenna;

         }
         else
         {
               // Check if this satellite belongs to Glonass system
            if( satid.system == SatID::systemGlonass )
            {
               std::stringstream sat;
               sat << "R";
               if( satid.id < 10 )
               {
                  sat << "0";
               }
               sat << satid.id;

                  // Get satellite antenna information out of AntexReader object
               Antenna antenna( pAntexReader->getAntenna( sat.str(), time ) );

                  // Get antenna offset for frequency "R01" (Glonass), in
                  // satellite reference system.
                  // NOTE: It is NOT in ECEF, it is in UEN!!!
               Triple satAnt( antenna.getAntennaEccentricity( Antenna::R01) );

                  
                  // Warning: 
                  // the elevation can't be considered because the receiver
                  // position has not been given

//                // Now, get the phase center variation.
//             Triple var( antenna.getAntennaPCVariation( Antenna::R01, elev) );
//                // We must substract them
//             satAnt = satAnt - var;

                     // Change to ECEF
               Triple svAntenna( satAnt[2]*ri + satAnt[1]*rj + satAnt[0]*rk );

               svPCOffset = svAntenna;

            }

         }  // End of 'if( satid.system == SatID::systemGPS )...'

      }
      else
      {
         Exception e("please input the antex file");
         GPSTK_THROW(e);
      }

         // This correction is interpreted as an "advance" in the signal,
         // instead of a delay. Therefore, it has negative sign
      return (svPCOffset);

   }  // End of method 'ComputeSatPCOffset::getSatPCOffset()'



}  // End of namespace gpstk
