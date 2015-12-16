#pragma ident "$Id: AmbiguitySmoother.hpp 1311 2012-09-26 19:32:36Z shjzhang $"

/**
 * @file AmbiguitySmoother.hpp
 * This class smoothes the Melboune-Wubbena combinations
 * phase observable.
 */

#ifndef GPSTK_AMBIGUITYSMOOTHER_HPP
#define GPSTK_AMBIGUITYSMOOTHER_HPP

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
//  Shoujian Zhang - Wuhan University, 2012
//
//============================================================================


#include "PowerSum.hpp"
#include "ProcessingClass.hpp"
#include <list>
#include <deque>

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class smoothes the floating ambiguities using slide 
       * window average method.
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
       *      // We MUST mark at least some cycle slips
       *   OneFreqCSDetector markCSL1;
       *
       *      // We must compute the MW combination first.
       *
       *   AmbiguitySmoother smoothMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> markCSL1 >> smoothMW;
       *   }
       *
       * @endcode
       *
       * The "AmbiguitySmoother" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will smooth the the ambiguity of given type.
       *
       * By default, the algorithm will check both the CSL1 and CSL2 index for
       * cycle slip information. You can change these settings in the
       * constructor and also using the appropriate methods.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the PC code observation smoothed (unless the
       * resultType field is changed). Be warned that if a given satellite does
       * not have the observations required, it will be summarily deleted from
       * the data structure.
       *
       * Another important parameter is the windowSize field. By default, it
       * is set to 10 samples, which is 5min interval for 30s sampling RINEX data.
       * You may adjust that with the setWindowSize()
       * method:
       *
       * @code
       *   AmbiguitySmoother smoothMW;
       *   smoothMW.setWindowSize(10);
       * @endcode
       *
       * WARNING: 
       *
       * It should be noted that the window size should be equal with the 'sampling'
       * interval in 'Decimate' class, then after 'decimate', all the observables 
       * will be used in the later solution, and the 'smoothed' data will also be 
       * independent in time.
       *
       * This smoother are objets that store their internal state,
       * so you MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       */
   class AmbiguitySmoother: public ProcessingClass
   {
   public:

         /// Default constructor. By default, smooth the BLC
      AmbiguitySmoother() 
         : windowSize(10), varThreshold(0.01)
      { setSmoothedType( TypeID::BLC ); };


         /** Common constructor
          *
          * @param wSize         Maximum  size of filter window, in samples.
          * @param resultT       TypeID where results will be stored.
          */
      AmbiguitySmoother( const TypeID& type,
                         const int& wSize,
                         const double& varT = 0.01)
         : windowSize(wSize), varThreshold(varT)
      { setSmoothedType( type ); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& epoch,
                                        satTypeValueMap& gData)
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


         /** Method to set the maximum size of filter window, in samples.
          *
          * @param wSize       Maximum size of filter window, in samples.
          */
      virtual AmbiguitySmoother& setWindowSize(const int& wSize);


         /** Method to set the variance threshold to delete the ambiguities.
          *
          * @param varT        Variance threshold for data deletion.
          */
      virtual AmbiguitySmoother& setVarThreshold(const double& varT);


         /// Method to get the maximum size of filter window, in samples.
      virtual int getWindowSize() const
      { return windowSize; };


         /** Method to set a TypeID to be filtered. This method will erase
          *  previous types.
          * @param type      TypeID to be filtered.
          */
      virtual AmbiguitySmoother& setSmoothedType(const TypeID& type)
      { smoothType = type; return (*this); };



         /// Method to get the set of TypeID's to be filtered.
      virtual TypeID getSmoothedType() const
      { return smoothType; };



         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~AmbiguitySmoother() {};


    private:


         /// Type to be smoothed
      TypeID smoothType;


         /// Maximum size of filter window, in samples.
      int windowSize;


         /// Variance threshold, in m^2.
      double varThreshold;


         /// Map to store satellite arc data
      std::map<SatID, double> satArcMap;


         /// A structure used to store filter data for a SV.
      struct filterData
      {
            // Default constructor initializing the data in the structure
         filterData() : mean(0.0) {};

         std::deque<double> Buffer;  ///< Values of previous observables.

         double mean;        ///< Accumulated mean value of combination.
         double variance;        ///< Accumulated mean value of combination.
      };


         /// Map holding the information regarding every satellite
      std::map<SatID, filterData> smoothingData;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };



   }; // End of class 'AmbiguitySmoother'

      //@}

}
#endif   // GPSTK_AMBIGUITYSMOOTHER_HPP
