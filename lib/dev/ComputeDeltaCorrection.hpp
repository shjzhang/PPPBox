#pragma ident "$Id: ComputeDeltaCorrection.hpp 2115 2009-09-02 18:21:09Z architest $"

/**
 * @file ComputeDeltaCorrection.hpp
 * This is a class to compute the difference of the corrections between
 * the interpolated ones and the precise ones, which are estiamted with
 * static PPP.
 */

#ifndef GPSTK_COMPUTE_DELTA_CORRECTION_HPP
#define GPSTK_COMPUTE_DELTA_CORRECTION_HPP

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
//  Shoujian Zhang - Wuhan University. 2012
//
//============================================================================
//
//  Modifications
//
//  - Interpolate the zwd/ionoL1/updCorr with the inverse distance weighting
//    method. 2014/07/04, shjzhang
//
//  - 2014/07/08
//
//============================================================================

#include "Exception.hpp"
#include "ENUUtil.hpp"
#include "Position.hpp"
#include "SolverBase.hpp"
#include "ProcessingClass.hpp"
#include <list>


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to interpolate the rover station's corrections 
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   ComputeMelbourneWubbena getMW;
       *
       *   gdsRef = loadGnssDataMap("reference.gds");
       *
       *   ComputeDeltaCorrection interpCorr(nominalPos, gdsRef);
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> interpCorr;
       *   }
       * @endcode
       *
       *
       * @sa LICSDetector.hpp and LICSDetector2.hpp for more information.
       *
       * \warning Cycle slip detectors are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       */
   class ComputeDeltaCorrection : public SolverBase,  public ProcessingClass
   {
   public:


         /// Default constructor.
      ComputeDeltaCorrection()
         :firstTime(true)
      {};


         /** Explicit constructor, taking as input reference station
          *  coordinates, ephemeris to be used and whether TGD will
          *  be computed or not.
          *
          * @param type          Type to be interpolated
          * @param RxCoordinates Reference station coordinates.
          * @param gdsRef        Observable corrections for reference stations.
          *
          */
      ComputeDeltaCorrection( const TypeID& type,
                              const gnssDataMap& refData );

         /** Explicit constructor, taking as input reference station
          *  coordinates, ephemeris to be used and whether TGD will
          *  be computed or not.
          *
          * @param typeSet       TypeSet to be interpolated.
          * @param RxCoordinates Reference station coordinates.
          * @param gdsRef        Observable corrections for reference stations.
          *
          */
      ComputeDeltaCorrection( const TypeIDSet& typeSet,
                              const gnssDataMap& refData );


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);



         /// Method to set the initial (a priori) position of receiver.
      virtual int setReferenceData(const gnssDataMap& refData);


         /** Method to add the TypeID's to be interpolated. This method will erase
          *  previous types.
          * @param typeSet       Set of TypeID's to be interpolated.
          */
      virtual ComputeDeltaCorrection& addInterpType(const TypeID& type)
      { interpTypeSet.insert(type); return (*this); };


         /** Method to set the TypeID's to be interpolated. This method will erase
          *  previous types.
          * @param typeSet       Set of TypeID's to be interpolated.
          */
      virtual ComputeDeltaCorrection& setInterpType(const TypeID& type)
      { interpTypeSet.clear(); interpTypeSet.insert(type); return (*this); };


         /** Method to set the TypeID's to be interpolated. This method will erase
          *  previous types.
          * @param typeSet       Set of TypeID's to be interpolated.
          */
      virtual ComputeDeltaCorrection& setInterpType(const TypeIDSet& typeSet)
      { interpTypeSet.clear(); interpTypeSet = typeSet; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeDeltaCorrection() {};


   private:


         /// Either estimated or "a priori" position of receiver
      gnssDataMap gdsRef;


         /// Set to store the correction type
      TypeIDSet interpTypeSet;


         /// Boolean indicating if this filter was run at least once
      bool firstTime;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { (*this).index = classIndex++; };


   }; // End of class 'ComputeDeltaCorrection'


      //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTE_DELTA_CORRECTION_HPP
