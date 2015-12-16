#pragma ident "$Id: CorrectUPDXs.cpp 2939 2012-08-10 19:55:11Z shjzhang $"

/**
* @file CorrectUPDXs.hpp
* Class to correct observables with satellite upds.
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
//  Shoujian Zhang - Wuhan University. 2011, 2012
//
//============================================================================


#include "CorrectUPDXs.hpp"

namespace gpstk
{
         // Index initially assigned to this class
      int CorrectUPDXs::classIndex = 4900000;
      

         // Returns an index identifying this object.
      int CorrectUPDXs::getIndex() const
      { return index; }


         // Returns a string identifying this object.
      std::string CorrectUPDXs::getClassName() const
      { return "CorrectUPDXs"; }

   
         /* Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      satTypeValueMap& CorrectUPDXs::Process( const CommonTime& time,
                                              satTypeValueMap& gData )
         throw(ProcessingException)
      {
         try
         {
            SatIDSet satRejectedSet;


               // Loop through all the satellites
            for (satTypeValueMap::iterator stv = gData.begin(); 
                 stv != gData.end(); 
                 ++stv)
            {
                  
               try
               {
                     // Get satellite upds from 'RinexUPDXStore'
                  satUPDX = (*pUPDXStore).getValue( (*stv).first, time );

               }
               catch(InvalidRequest& e)
               {
                  
                     // If some problem appears, then schedule this satellite
                     // for removal
                  satRejectedSet.insert( (*stv).first );

                  continue;    // Skip this SV if problems arise

               }

//             (*stv).second[TypeID::updSatWL] = satUPDX.updSatWL;
//             (*stv).second[TypeID::updSatLC] = satUPDX.updSatLC;

                  // For upd file reading
               (*stv).second[TypeID::updSatWL] = satUPDX.updSatLC;
               (*stv).second[TypeID::updSatLC] = satUPDX.updSatWL;

               (*stv).second[TypeID::updSatL1] = satUPDX.updSatL1;
               (*stv).second[TypeID::updSatL2] = satUPDX.updSatL2;

                  // Warning: for test!!!
               (*stv).second[TypeID::updSatMW] = satUPDX.updSatWL;


            }  // End of 'for (stv = gData.begin(); stv != gData.end(); ++stv)'

               // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

               // Return
            return gData;

         }
         catch(Exception& u)
         {

               // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":"
               + StringUtils::asString( getIndex() ) + ":"
               + u.what() );

            GPSTK_THROW(e);

         }

      }  // End of method 'CorrectUPDXs::Process()'

}

