#pragma ident "$Id$"

/**
 * @file CorrectObstime.hpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
 */

#ifndef GPSTK_CorrectObstime_HPP
#define GPSTK_CorrectObstime_HPP

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



#include <string>
#include "ProcessingClass.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class corrects observables from effects:reciver clock error
       *                                     Maybe ONLY NEEDED FOR POD
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *       // Create the input obs file stream
       *    RinexObsStream rin("ebre0300.02o");
       *
       *
       *       // reciver clock error (previously computed)
       *    double dtreciver=4.3E-3;              // Units in second
       *
       *
       *   gnssRinex gRin;
       *   CorrectObstime corrt( dtreciver);
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> corrt;
       *   }
       * @endcode
       *
       * The "CorrectObstime" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will correct the
       * corresponding observables from the given effects.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the observables corrected. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       */
   class CorrectObstime : public ProcessingClass
   {
   public:

         /// Default constructor
      CorrectObstime():extraDtr(0.0)
      { };


         /** Common constructor
          *
          * @param extraDtr     Reciver clock error.
          *
          */
      CorrectObstime(const double& extraDtr)
      { };



         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         
         /** Returns extra offerred  reciver clock error (unit in second)
          */
         virtual double getExtraDtr(void) const
         { return extraDtr; };
         
         
         /** Sets extra offerred  reciver clock error (unit in second)
          *
          * @param extradtr   Extra offerred  reciver clock error
          *                   (unit in second)
          */
         virtual CorrectObstime& setExtraDtr(const double& extradt)
         { extraDtr = extradt; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~CorrectObstime() {};


   private:

         /// Extra offerred  reciver clock error (unit in second)
      double extraDtr;


   }; // End of class 'CorrectObstime'

      //@}

}  // End of namespace gpstk

#endif  // GPSTK_CorrectObstime_HPP
