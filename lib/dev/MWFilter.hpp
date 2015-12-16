#pragma ident "$Id$"

/**
 * @file MWFilter.hpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
 */

#ifndef GPSTK_MWFILTER_HPP
#define GPSTK_MWFILTER_HPP

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
//  shjzhang, 2015
//
//============================================================================



#include "ProcessingClass.hpp"
#include <list>


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to compute the mean and Variance of the  MW observables.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *   ComputeMelbourneWubbena getMW;
       *   MWFilter markCSMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getMW >> markCSMW;
       *   }
       * @endcode
       *
       * The "MWFilter" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will decide if a cycle slip has happened
       * in the given observable.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the cycle slip indexes inserted along
       * their corresponding satellites. Be warned that if a given satellite
       * does not have the observations required, it will be summarily deleted
       * from the data structure.
       *
       * 
       *
       * @sa LICSDetector.hpp and LICSDetector2.hpp for more information.
       *
       */
   class MWFilter : public ProcessingClass
   {
   public:

         /// Default constructor, setting default parameters.
      MWFilter() 
         : obsType(TypeID::MWubbena), resultType1(TypeID::BWL)
      {};



         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& epoch,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { (*this).Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~MWFilter() {};


   private:


         /// Type of observation.
      TypeID obsType;


         /// Type of result #1.
      TypeID resultType1;


         /// Map to store satellite arc data
      std::map<SatID, double> satArcMap;


         /// A structure used to store filter data for a SV.
      struct filterData
      {
            // Default constructor initializing the data in the structure
         filterData() 
            : formerEpoch(CommonTime::BEGINNING_OF_TIME),
              windowSize(0), meanMW(0.0), varMW(0.0)
         {};

         CommonTime formerEpoch;    ///< The previous epoch time stamp.
         int windowSize;         ///< Size of current window, in samples.
         double meanMW;          ///< Accumulated mean value of combination.
         double varMW; /// Variance 
      };


         /// Map holding the information regarding every satellite
      std::map<SatID, filterData> MWData;


   }; // End of class 'MWFilter'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_MWFILTER_HPP
