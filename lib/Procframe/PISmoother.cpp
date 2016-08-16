#pragma ident "$Id$"

/**
 * @file PISmoother.cpp
 * This class smoothes PI code observables using the corresponding LI
 * phase observable.
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
//  Dagoberto Salazar - gAGE ( http:// www.gage.es ). 2007, 2008, 2011
//
//============================================================================


#include "PISmoother.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string PISmoother::getClassName() const
   { return "PISmoother"; }




      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& PISmoother::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double codeObs(0.0);
         double phaseObs(0.0);
         double flagObs1(0.0);
         double flagObs2(0.0);
         double weight(0.0);
	 double satArcNo(0.0);

         SatIDSet satRejectedSet;

            // Loop through all satellites
         satTypeValueMap::iterator it;

         for (it = gData.begin(); it != gData.end(); ++it)
         {
            try
            {

                  // Try to extract the values
                  // attention: PI in pppbox is defined as P2-P1, whereas LI is L1-L2
               codeObs  = (*it).second(codeType);
                  // using L2-L1 smoothing P2-P1
               phaseObs = -(*it).second(phaseType);
                  // get the SatArc number
	       satArcNo = (*it).second[TypeID::satArc];

            }
            catch(...)
            {

                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );

               continue;

            }

            try
            {

                  // Try to get the first cycle slip flag
               flagObs1  = (*it).second(csFlag1);

            }
            catch(...)
            {

                  // If flag #1 is not found, no cycle slip is assumed
                  // You REALLY want to have BOTH CS flags properly set
               flagObs1 = 0.0;

            }

            try
            {

                  // Try to get the second cycle slip flag
               flagObs2  = (*it).second(csFlag2);

            }
            catch(...)
            {

                  // If flag #2 is not found, no cycle slip is assumed
                  // You REALLY want to have BOTH CS flags properly set
               flagObs2 = 0.0;

            }

               // Get the smoothed PI.
            (*it).second[resultType] = getSmoothing( (*it).first,
                                                     codeObs,
                                                     phaseObs,
                                                     flagObs1,
                                                     flagObs2,
                                                     weight,
						     satArcNo);

               // Find weight in '(*it).second'
            typeValueMap::iterator ittvm = (*it).second.find(TypeID::weight);
            if( ittvm != (*it).second.end() )
            {
               double prevWeight = (*it).second[TypeID::weight];
               (*it).second[TypeID::weight] = weight*prevWeight;
            }
            else
            {
               (*it).second[TypeID::weight] = weight;
            }

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }

      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'PISmoother::Process()'



      /* Method to set the maximum size of filter window, in samples.
       *
       * @param maxSize       Maximum size of filter window, in samples.
       */
   PISmoother& PISmoother::setMaxWindowSize(const int& maxSize)
   {

         // Don't allow window sizes less than 1
      if (maxSize > 1)
      {
         maxWindowSize = maxSize;
      }
      else
      {
         maxWindowSize = 1;
      }

      return (*this);

   }  // End of method 'PISmoother::setMaxWindowSize()'



      /* Compute the smoothed code observable.
       *
       * @param sat        Satellite object.
       * @param code       Code measurement.
       * @param phase      Phase measurement.
       * @param flag1      Cycle slip flag in L1.
       * @param flag2      Cycle slip flag in L2.
       */
   double PISmoother::getSmoothing( const SatID& sat,
                                    const double& code,
                                    const double& phase,
                                    const double& flag1,
                                    const double& flag2,
                                    double& weight,
				    double& satArcNo)
   {


    double varPI(1.0);
    

         // In case we have a cycle slip either in L1 or L2
      if ( (flag1!=0.0) || (flag2!=0.0) || (SmoothingData[sat].satArc != satArcNo ))
      {
            // Prepare the structure for the next iteration
         SmoothingData[sat].previousCode = code;
         SmoothingData[sat].previousPhase = phase;
         SmoothingData[sat].previousVar = varPI; // Set the variance of PI as 1.0m;
         SmoothingData[sat].windowSize = 1;
	 SmoothingData[sat].satArc = satArcNo ;

            // We don't need any further processing
         return code;

      }

         // In case we didn't have cycle slip
         // Increment size of window and check limit
     
      if (SmoothingData[sat].windowSize > maxWindowSize)
      {
         SmoothingData[sat].windowSize = maxWindowSize;
      }

      double smoothedCode(0.0);
      double varSmCode(0.0);

      double wSize( static_cast<double>(SmoothingData[sat].windowSize) );
      double prevCode( SmoothingData[sat].previousCode );
      double prevPhase( SmoothingData[sat].previousPhase );
      double prevVar( SmoothingData[sat].previousVar );

         // The formula used is the following:
         //
         // CSn = (1/n)*Cn + ((n-1)/n)*(CSn-1 + Ln - Ln-1)
         //
         // shjzhang.
         // The variance formula:
         // Var(CSn) = (1/n^2)a*Var(Cn) + (n-1)^2/n^2*(Var(CSn-1))
         //
         // As window size "n" increases, the former formula gives more
         // weight to the previous smoothed code CSn-1 plus the phase bias
         // (Ln - Ln-1), and less weight to the current code observation Cn
      
      smoothedCode = (code + (wSize-1.0)*( prevCode + (phase-prevPhase) ) ) / wSize;
       
         // The variance of PI is 1.0m;
      varSmCode = (varPI + (wSize-1.0)*(wSize-1.0)*prevVar ) /(wSize*wSize);

         // Weight for smoothed code is var(PI)/var(smoothedCode)
      weight = varPI/varSmCode;

         // Store results for next iteration
      SmoothingData[sat].previousCode = smoothedCode;
      SmoothingData[sat].previousPhase = phase;
      SmoothingData[sat].previousVar = varSmCode;

      ++SmoothingData[sat].windowSize;
      return smoothedCode;

   }  // End of method 'PISmoother::getSmoothing()'


}  // End of namespace gpstk
