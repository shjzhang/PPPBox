#pragma ident "$Id: PrefitCSDetector.hpp 1308 2008-07-22 20:01:04Z architest $"

/**
 * @file PrefitCSDetector.hpp
 * This is a class to detect cycle slips using prefit residuals of observables.
 */

#ifndef GPSTK_PREFITCSDETECTOR_HPP
#define GPSTK_PREFITCSDETECTOR_HPP

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
//  Shoujian Zhang, Wuhan University, 2013
//
//============================================================================



#include "geometry.hpp"                   // DEG_TO_RAD
#include "ProcessingClass.hpp"
#include "Stats.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to detect cycle slips using prefit residuals.
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
       *   ComputeLI getLI;
       *   PrefitCSDetector markCSLI;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getLI >> markCSLI;
       *   }
       * @endcode
       *
       * The "PrefitCSDetector" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will decide if a cycle slip has happened
       * in the given observable.
       *
       * The algorithm will use LI observables, and the LLI1 and LLI2 indexes.
       * The result (a 1 if a cycle slip is found, 0 otherwise) will be stored
       * in the data structure both as the CSL1 and CSL2 indexes.
       *
       * The default values are usually fine, but you may change them with the
       * appropriate methods. This is of special importance for the maximum
       * interval time, that should be adjusted for your sampling rate. It is
       * 61 seconds by default, which is appropriate for 30 seconds per sample
       * RINEX observation files.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the cycle slip indexes inserted along their
       * corresponding satellites. Be warned that if a given satellite does not
       * have the observations required, it will be summarily deleted from the
       * data structure.
       *
       * Be aware that some combinations of cycle slips in L1 and L2 may result
       * in a false negative when using a cycle slip detector based on LI.
       * Therefore, to be on the safe side you should complement this with
       * another kind of detector, such as one based on the Melbourne-Wubbena
       * combination.
       *
       * @sa MWCSDetector.hpp for more information.
       *
       * \warning Cycle slip detectors are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       */
   class PrefitCSDetector : public ProcessingClass
   {
   public:

         /// Default constructor, setting default parameters.
      PrefitCSDetector() 
         : obsType(TypeID::prefitL), 
           lliType1(TypeID::LLI1), lliType2(TypeID::LLI2), 
           resultType1(TypeID::CSL1), resultType2(TypeID::CSL2), 
           minThreshold(0.1), useLLI(true)
      { };


         /** Common constructor
          *
          * @param mThr    Minimum threshold to declare cycle slip, in meters.
          */
      PrefitCSDetector( const double& mThr,
                        const bool& use = true);


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          * @param epochflag Epoch flag.
          */
      virtual satTypeValueMap& Process( const CommonTime& epoch,
                                        satTypeValueMap& gData,
                                        const short& epochflag = 0 )
         throw(ProcessingException);


         /** Method to get the minimum threshold for cycle slip detection, in
          *  meters.
          */
      virtual double getMinThreshold() const
      { return minThreshold; };


         /** Method to set the minimum threshold for cycle slip detection, in
          *  meters.
          *
          * @param mThr    Minimum threshold for cycle slip detection, in
          *                meters.
          */
      virtual PrefitCSDetector& setMinThreshold(const double& mThr);


         /// Method to know if the LLI check is enabled or disabled.
      virtual bool getUseLLI() const
      { return useLLI; };


         /** Method to set whether the LLI indexes will be used as an aid
          *  or not.
          *
          * @param use   Boolean value enabling/disabling LLI check
          */
      virtual PrefitCSDetector& setUseLLI(const bool& use)
      { useLLI = use; return (*this); };


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
      virtual ~PrefitCSDetector() {};


   private:


         /// Type of observable.
      TypeID obsType;


         /// Type of LLI1 record.
      TypeID lliType1;


         /// Type of LLI2 record.
      TypeID lliType2;


         /// Type of result #1.
      TypeID resultType1;


         /// Type of result #2.
      TypeID resultType2;


         /// Minimum threshold to declare cycle slip, in meters.
      double minThreshold;


         /// This field tells whether to use or ignore the LLI indexes as
         /// an aid.
      bool useLLI;

         
         /// satTypeValueMap data containing the last epoch data
      satTypeValueMap formerData;


         /** Method that implements the prefit cycle slip detection algorithm
          *
          * @param epochflag Epoch flag.
          * @param diffValue Current difference of the prefit residuals between epochs 
          * @param lli1      LLI1 index.
          * @param lli2      LLI2 index.
          */
      virtual double getDetection( const short& epochflag,
                                   const double& diffValue,
                                   const double& lli1,
                                   const double& lli2 );


   }; // End of class 'PrefitCSDetector'

      //@}

}
#endif   // GPSTK_PREFITCSDETECTOR_HPP
