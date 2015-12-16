#pragma ident "$Id: CorrectUPDXs.hpp 2475 2012-08-10 02:53:06Z shjzhang $"

/**
* @file CorrectUPDXs.hpp
* Class to correct observables with satellite upds.
* 
*/

#ifndef GPSTK_CORRECT_UPDXS_HPP
#define GPSTK_CORRECT_UPDXS_HPP

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
//  Shoujian Zhang - Wuhan University. 2011, 2012
//
//============================================================================

#include <string>
#include "ProcessingClass.hpp"
#include "RinexUPDXStore.hpp"
#include "SatUPDX.hpp"

namespace gpstk
{

   using namespace std;

      /** @addtogroup DataStructures */
      //@{

      /** This class corrects the Melbourne-Wubbena (MW) combination and 
       *  ionosphere-free (L3) phase combination observables with corresponding 
       *  satellite upds. 
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *
       *   gnssRinex gRin;
       *   CorrectUPDXs corr;
       *   coor.setUPDXFile("whu16572.upd");
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> corr;
       *   }
       *
       * @endcode
       *
       * The "CorrectUPDXs" object will visit every satellite in the
       * GNSS data structure, i.e. "gRin" and will correct the corresponding 
       * observables with the corresponding values.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the observables corrected. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       */
   class CorrectUPDXs : public ProcessingClass
   {
   public:



         /// Default constructor
      CorrectUPDXs() 
         : pUPDXStore(NULL)
      { setIndex(); };


         /** Common constructor
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      CorrectUPDXs( RinexUPDXStore& updStore )
         : pUPDXStore(&updStore)
      { setIndex(); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
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
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };

   
         /// Returns an index identifying this object.
      virtual int getIndex() const;


         /// Returns a string identifying this object.
      virtual std::string getClassName() const;


         /// Default deconstructor
      virtual ~CorrectUPDXs(){};


   protected:


         /// Satellite upd file
      string rinexUPDXFile;

         /// Satellite upd data 
      SatUPDX satUPDX;

         /// Class to read satellite upds
      RinexUPDXStore* pUPDXStore;
         
      
   private:

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'CorrectUPDXs'

      //@}

}  // End of namespace gpstk



#endif   // GPSTK_CORRECT_PHASE_BIASES_HPP

