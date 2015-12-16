#pragma ident "$Id: $"

/**
 * @file ComputeElevWeights.hpp
 * This class computes satellites weights simply based on the elevation .
 */

#ifndef GPSTK_COMPUTEELEVATIONWEIGHTS_HPP
#define GPSTK_COMPUTEELEVATIONWEIGHTS_HPP

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
//  Modifications
//  -------------
//
//  2012.06.07  Create this program
//
//  Copyright
//  ---------
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include "DataStructures.hpp"
#include "ProcessingClass.hpp"
#include "geometry.hpp"             // DEG_TO_RAD 


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes simple satellites weights only based on 
       * the satellite elevation.
       *
       * It is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Declare a GDS object
       *   gnssRinex gRin;
       *
       *      // Create a 'ComputeElevWeights' object
       *   ComputeElevWeights elevWeights;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> elevWeights;
       *   }
       * @endcode
       *
       * The "ComputeElevWeights" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will try to compute its weight
       * based on a simple algorithm.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the weights inserted along their corresponding
       * satellites. Be warned that if it is not possible to compute the
       * weight for a given satellite, it will be summarily deleted from
       * the data structure.
       *
       * @sa ComputeIURAWeights.hpp, ComputeMOPSWeights.hpp.
       */
   class ComputeElevWeights: public ProcessingClass
   {
   public:

         /// Default constructor.
      ComputeElevWeights(double exp=2.0)
         : expotential(exp) 
      {  
         setIndex(); 
      };


         /** Returns a satTypeValueMap object, adding the new data
          *  generated when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeElevWeights() {};


   private:

         /// expotential for weighting function
      double expotential;

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'ComputeElevWeights'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTEELEVATIONWEIGHTS_HPP
