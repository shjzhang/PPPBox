#pragma ident "$Id: EpochDiffOp.hpp 2614 2012-04-11 06:47:03Z shjzhang $"

/**
 * @file EpochDiffOp.hpp
 * This is a class to apply the time differences(.i.e.Delta Operator on Time) for the same
 * receiver's GNSS data.
 */

#ifndef GPSTK_EPOCHDIFFOP_HPP
#define GPSTK_EPOCHDIFFOP_HPP

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
//
//  Author
//  ------
//
//  Shoujian Zhang, Wuhan University, 2012..
//
//============================================================================
//
//  Modifications
//  =============
//
//  2012/04/11
//
//  Create this program, Shoujian Zhang 
//
//============================================================================


#include "TypeID.hpp"
#include "ProcessingClass.hpp"
#include "DataHeaders.hpp"
#include "DataStructures.hpp"


namespace gpstk
{

      /// Thrown when some problem appeared when reading Antex data
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(TypeNotFound, gpstk::Exception);

      /** @addtogroup GPSsolutions */
      //@{

      /** This class applies the time difference (i.e.Delta operator on time)  
       *  for the same receiver's GNSS data 
       *
       *  A typical way to use this class follows:
       *
       * @code
       *
       *     // Create an object to compute the time differences
       *     // of prefit residuals
       *  EpochDiffOp deltaT;
       *
       *
       *  while(rin >> gRin)
       *  {
       *        // By default, difference is applied on code prefit residuals
       *     gRin >> model >> deltaT >> solver;
       *  }
       *
       * @endcode
       *
       * The "EpochDiffOp" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will substract from the specified type
       * or types (code prefit residuals by default) the corresponding data in
       * the "gRef" data structure.
       *
       * Take notice that in the default case the code prefit residuals were
       * computed by objects of classes such as "ModeledPR" and
       * "ModeledReferencePR" (among others) objects, so those steps are
       * mandatory.
       *
       * Be warned that, by default, if a given satellite in "gRin" does not
       * have the data required to be differenced, it will be summarily deleted
       * from the data structure.
       *
       * @sa DeltaOp.hpp for differences on receiver-related data.
       * @sa NablaOp.hpp for differences on satellite-related data.
       *
       */
   class EpochDiffOp : public ProcessingClass
   {
   public:


         /// Default constructor. By default, it will difference prefitC data
         /// and will delete satellites present in reference station data but
         /// missing in input data.
      EpochDiffOp()
      { setIndex(); };


         /** Common constructor 
          *
          * @param difftype   TypeID of data values to be differenced.
          *
          */
      EpochDiffOp( const TypeID& difftype )
      { diffTypes.insert(difftype); setIndex(); };


         /** Common constructor.
          *
          * @param diffSet    TypeIDSet of data values to be differenced.
          *
          */
      EpochDiffOp( const TypeIDSet& diffSet )
         : diffTypes(diffSet)
      { setIndex(); };


         /** Method to set the data values to be differenced.
          *
          * @param difftype      TypeID of data values to be differenced.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual EpochDiffOp& setDiffType(const TypeID& difftype)
      { diffTypes.clear(); diffTypes.insert(difftype); return (*this); };


         /** Method to add a data value type to be differenced.
          *
          * @param difftype      TypeID of data values to be added to the ones
          *                      being differenced.
          */
      virtual EpochDiffOp& addDiffType(const TypeID& difftype)
      { diffTypes.insert(difftype); return (*this); };


         /** Method to set a set of data values to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be differenced.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual EpochDiffOp& setDiffTypeSet(const TypeIDSet& diffSet)
      { diffTypes.clear(); diffTypes = diffSet; return (*this); };


         /** Method to add a set of data value types to be differenced.
          *
          * @param diffSet       TypeIDSet of data values to be added to the
          *                      ones being differenced.
          */
      virtual EpochDiffOp& addDiffTypeSet(const TypeIDSet& diffSet);


         /// Method to get the set of data value types to be differenced.
      virtual TypeIDSet getDiffTypeSet(void) const
      { return diffTypes; };


         /** Returns a reference to a satTypeValueMap object after
          *  differencing data type values given in 'diffTypes' field with
          *  the previous data in 'gPrev' field.
          *
          * @param gData      Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException, TypeIDNotFound);


         /** Returns a reference to a gnssSatTypeValue object after
          *  differencing data type values given in 'diffTypes' field the
          *  previous epoch's gnssData in 'gPrev' .
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException, TypeIDNotFound);


         /** Returns a reference to a gnnsRinex object after differencing
          *  data type values given in 'diffTypes' field with respect to
          *  previous epoch's gnssData in 'gPrev' .
          *
          * @param gData      Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException, TypeIDNotFound);


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~EpochDiffOp() {};


   private:


         /// Set (TypeIDSet) containing the types of data to be differenced.
      TypeIDSet diffTypes;

         /// Former gnss data
      satTypeValueMap gPrev;

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'EpochDiffOp'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_EPOCHDIFFOP_HPP
