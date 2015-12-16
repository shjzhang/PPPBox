#pragma ident "$Id: PrefitFilter.cpp 1325 2012-02-23 14:33:43Z shjzhang $"

/**
 * @file PrefitFilter.cpp
 * This class filters out satellites with prefit residual grossly out of bounds.
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


#include "PrefitFilter.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int PrefitFilter::classIndex = 1010000;


      // Returns an index identifying this object.
   int PrefitFilter::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string PrefitFilter::getClassName() const
   { return "PrefitFilter"; }


      // Returns a satTypeValueMap object, filtering the target observables.
      //
      // @param gData     Data object holding the data.
      //
   satTypeValueMap& PrefitFilter::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;

            // Take the median value of the prefit residual as 
            // the receiver clock error
         double medianPrefit = median<double>(gData.getVectorOfTypeID(codeType));

            // define the prefit for each satellite
         double prefit;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it) 
         {
            try
            {
                  // Try to extract the values
               prefit = (*it).second(codeType);

                  // Now, compute the 'true' prefit-residual
               prefit = prefit - medianPrefit;

                  // Remove this satellite, if it exceeds the threshold.
               if ( std::abs(prefit) > threshold )
               {
                     // If value is out of bounds, then schedule this
                     // satellite for removal
                   satRejectedSet.insert( (*it).first );

               }
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );

//             cout << "prefitResidualFilter" << (*it).first << endl;
            }
         }

            // Before checking next TypeID, let's remove satellites with
            // data out of bounds
         gData.removeSatID(satRejectedSet);
         
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

   }  // End of 'PrefitFilter::Process()'


} // End of namespace gpstk
