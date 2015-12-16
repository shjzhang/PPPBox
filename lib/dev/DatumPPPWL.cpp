#pragma ident "$Id: DatumPPPWL.cpp 2939 2012-04-06 19:55:11Z shjzhang $"

/**
 * @file DatumPPPWL.cpp
 * Class to define and handle the ambiguity datum for PPP.
 */

//============================================================================
//
//  Function
//  --------
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
//
//  Copyright
//  ---------
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================
//
//  History
//  -------
//
//  2012.10.12  Modify this program for ambiguity datum determination in PPP.
//  2012.10.20  Change this file name from "AmbDatumPos" to "DatumPPPWL" 
//
//============================================================================


#include "SystemTime.hpp"
#include "DatumPPPWL.hpp"
#include "ARRound.hpp"
#include <iterator>
#include "Stats.hpp"

namespace gpstk
{

      // General white noise stochastic model
   WhiteNoiseModel DatumPPPWL::whiteNoiseModel;


      /* Prepare this object to carry out its work.
       *
       * @param gData     Map of GNSS data structures (GDS), indexed
       *                  by SourceID.
       */
   DatumPPPWL& DatumPPPWL::Prepare( gnssRinex& gData )
   {

         // Store the old arc.
      oldArc = newArc;

         // Firstly, clear the datum satellite data map
      ambFixedMap.clear();

         // Thirdly, Let's find 'newArc' and 'newSat ' 
      SatID newSat;

         // Source from 'gData'
      SourceID source = gData.header.source;

         // Satellite number 
      int numSat ( ambMap.size() );

         // Set to store the arc information
         // Arc  source, sat, satArc
      ArcSet currArcSet;

         // Loop all the ambiguities
      for( std::map<SatID,double>::iterator itamb = ambMap.begin();
           itamb != ambMap.end();
           ++itamb )
      {
             // Get the satelite
          SatID sat = (*itamb).first;

             // Get arc number from gnssData
          double satArc = gData.getValue(sat, TypeID::satArc);

             // Create arc for current satellite;
          Arc arc(source, sat, satArc);

             // Now, insert this 'arc' into 'currArcSet'
          currArcSet.insert(arc);

      } // End of 'for( std::map<SatID, double>::iterator ... ) '

         // If the 'oldArc' is not found in 'currArcSet'
      if(currArcSet.find(oldArc) == currArcSet.end())
      {

             // Vector to store the deviation 
          Vector<double> devVec(numSat, 0.0);
          Vector<double> elev(numSat, 0.0);

             // Loop the ambigity map
          int i = 0;
          for( std::map<SatID,double>::iterator itamb = ambMap.begin();
               itamb != ambMap.end();
               ++itamb )
          {
                 // sat
              SatID sat = (*itamb).first;

                 // Get the ambiguity
              double amb = (*itamb).second;

                 // Just fix the ambiguity to neareast integer
              double ambFixed = std::floor( amb + 0.5 );

                 // Deviation
              double dev = amb - ambFixed;

                 // Store the deviations
              devVec(i) = dev;

                 // elev
              elev(i) = gData.getValue(sat, TypeID::elevation);

                 // increment
              i++;
          }

             // Now, Let's get the median values
          double devMed, devSig;
          Vector<double> devOffVec(numSat, 0.0);
          Stats<double> vecStats;

             // The median value 
          devMed = median<double>(devVec);

          for(int i=0; i< numSat; i++)
          {
              devOffVec(i) = devVec(i) - devMed;
              vecStats.Add(devOffVec(i));
          }

             // Now, Let's compute the variance according 
             // to the deviations
          if(numSat > 1 )
          {
             devSig = vecStats.StdDev();
          }
          else
          {
               // Just give a NON zero value
             devSig = 0.1;
          }

             // If 'devSig' is near 'ZERO'
          if( devSig < 1.0E-4 )
          {
             devSig = 1.0E-4;
          }

              // Then, Let's get the satellite with the devivation equal with
              // the median value
              
          double minDev(9.0);
          double maxElev(0.0);
          double maxDec(0.0);
          double cutDec(1000.0);

          ARRound ambRes(1000,0.3,0.3);

          i = 0;
          for( std::map<SatID,double>::iterator itamb = ambMap.begin();
               itamb != ambMap.end();
               ++itamb )
          {
                 // Current satellite 
              SatID currSat = (*itamb).first ;

                 // Then, if the deviations is greater than 1.5~2 sigma 
                 // This satellite will not chosen as datum satellite.
              if( std::abs( devOffVec(i) ) < 2*devSig )
              {
                     // New bias 
                  double newAmb = devOffVec(i) ;

                     // Sigma for this ambiguity
                  double sigma = std::sqrt( ambCovMap[ (currSat) ][(currSat)] );

                     // Get fixing decision
                  double dec = ambRes.getDecision(newAmb, sigma);

                     // Look for the largest fixing decision
                  if( dec > maxDec )
                  {
                        // Store ....
                     maxDec = dec;

                        // Update the satellite
                     newSat = currSat;
                  }
                  
              }
              
                 // increment
              i++;
          }

             // If the satellite with 'maxDec' can't be fixed
          if( maxDec < cutDec )
          {
             int i(0);
             for( std::map<SatID,double>::iterator itamb = ambMap.begin();
                  itamb != ambMap.end();
                  ++itamb )
             {
                    // Current satellite 
                 SatID currSat = (*itamb).first ;

                    // if the ambiguity with 'maxDec' can't be fixed.
                 if( std::abs( elev(i) ) > maxElev)
                 {
                        // Update the max elevation
                     maxElev = elev(i);

                        // Update the satellite
                     newSat = currSat;

                 }
                    // increment
                 i++;
             }
          }

             // Get the arc number of the new satellite
          double satArc  = gData.getValue(newSat, TypeID::satArc);

             // Then, we get the newArc
          newArc = Arc(source, newSat, satArc);
      }
      else
      {
             // The datum satellite is the same with the one at 
             // the previous epoch
          newSat = oldArc.getSatellite();

             // The datum arc will continue ...
          newArc = oldArc;
      }

         // The ambiguity datum map
      double amb = ambMap[ newSat ];

         // Just fix the ambiguity to neareast integer
      double ambFixed = std::floor( amb + 0.5 );


         // Store the fixed ambiguity into 'ambFixedMap'
      ambFixedMap[newSat] = ambFixed;

         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'DatumPPPWL::Prepare()'


      /** 
       * Get the ambiguity datum set which are fixed directly.
       */
   std::map<SatID,double> DatumPPPWL::getAmbFixedMap( void )
      throw(InvalidDatumPPPWL)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidDatumPPPWL("DatumPPPWL is not prepared"));
      }

         // Return
      return ambFixedMap;

   }  // End of method 'DatumPPPWL::getAmbFixedMap()'


}  // End of namespace gpstk
