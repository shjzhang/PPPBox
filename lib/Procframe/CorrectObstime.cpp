#pragma ident "$Id$"

/**
 * @file CorrectObstime.cpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
//
//============================================================================


#include "CorrectObstime.hpp"
#include "GPSEllipsoid.hpp"

namespace gpstk
{

      // Returns a string identifying this object.
   std::string CorrectObstime::getClassName() const
   { return "CorrectObstime"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CorrectObstime::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {


               // Compute corrections = reciver clock corrct.
            GPSEllipsoid ellipsoid;
            double dctreciver;
            dctreciver=extraDtr*ellipsoid.c();
               
            double corrL1(dctreciver);
            double corrL2(dctreciver);
            double corrL5(dctreciver);
            double corrL6(dctreciver);
            double corrL7(dctreciver);
            double corrL8(dctreciver);


               // Find which observables are present, and then
               // apply corrections

               // Look for C1
            if( (*it).second.find(TypeID::C1) != (*it).second.end() )
            {
               (*it).second[TypeID::C1] = (*it).second[TypeID::C1] + corrL1;
            };

               // Look for P1
            if( (*it).second.find(TypeID::P1) != (*it).second.end() )
            {
               (*it).second[TypeID::P1] = (*it).second[TypeID::P1] + corrL1;
            };

               // Look for L1
            if( (*it).second.find(TypeID::L1) != (*it).second.end() )
            {
               (*it).second[TypeID::L1] = (*it).second[TypeID::L1] + corrL1;
            };

               // Look for C2
            if( (*it).second.find(TypeID::C2) != (*it).second.end() )
            {
               (*it).second[TypeID::C2] = (*it).second[TypeID::C2] + corrL2;
            };

               // Look for P2
            if( (*it).second.find(TypeID::P2) != (*it).second.end() )
            {
               (*it).second[TypeID::P2] = (*it).second[TypeID::P2] + corrL2;
            };

               // Look for L2
            if( (*it).second.find(TypeID::L2) != (*it).second.end() )
            {
               (*it).second[TypeID::L2] = (*it).second[TypeID::L2] + corrL2;
            };

               // Look for C5
            if( (*it).second.find(TypeID::C5) != (*it).second.end() )
            {
               (*it).second[TypeID::C5] = (*it).second[TypeID::C5] + corrL5;
            };

               // Look for L5
            if( (*it).second.find(TypeID::L5) != (*it).second.end() )
            {
               (*it).second[TypeID::L5] = (*it).second[TypeID::L5] + corrL5;
            };

               // Look for C6
            if( (*it).second.find(TypeID::C6) != (*it).second.end() )
            {
               (*it).second[TypeID::C6] = (*it).second[TypeID::C6] + corrL6;
            };

               // Look for L6
            if( (*it).second.find(TypeID::L6) != (*it).second.end() )
            {
               (*it).second[TypeID::L6] = (*it).second[TypeID::L6] + corrL6;
            };

               // Look for C7
            if( (*it).second.find(TypeID::C7) != (*it).second.end() )
            {
               (*it).second[TypeID::C7] = (*it).second[TypeID::C7] + corrL7;
            };

               // Look for L7
            if( (*it).second.find(TypeID::L7) != (*it).second.end() )
            {
               (*it).second[TypeID::L7] = (*it).second[TypeID::L7] + corrL7;
            };

               // Look for C8
            if( (*it).second.find(TypeID::C8) != (*it).second.end() )
            {
               (*it).second[TypeID::C8] = (*it).second[TypeID::C8] + corrL8;
            };

               // Look for L8
            if( (*it).second.find(TypeID::L8) != (*it).second.end() )
            {
               (*it).second[TypeID::L8] = (*it).second[TypeID::L8] + corrL8;
            };

         }


         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'CorrectObservables::Process()'


}  // End of namespace gpstk
