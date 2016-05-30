#pragma ident "$ID: CC2NONCC.hpp 2015-11-06 $"

#ifndef GPSTK_CC2NONCC_HPP
#define GPSTK_CC2NONCC_HPP

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
//  Copyright (c) 
//
//  Q.Liu, Wuhan Uniersity, 2015 
//
//============================================================================

#include "ProcessingClass.hpp"
#include "RecTypeDataReader.hpp"
#include "DCBDataReader.hpp"
#include "string"

namespace gpstk
{


   NEW_EXCEPTION_CLASS(RecTypeNotFound, gpstk::Exception);

   /** @addtogroup DataStructures */
   // @{
   //
   /** This class convert code observables which uses the CC(cross-correlation)
    * technique to pseudorange observables compatible with the modern Y-codeless
    * pseudorange tracking.
    *
    * Specifically, the C1 and X2(equivalent to C1+(P2-P1)) pseudorange observables
    * of cross-correlation receivers are replaced by
    *   C1 --> C1 + c*p1c1DCB
    *   X2 --> X2 + c*p1c1DCB  
    * here 'c' means light velocity, "p1c1DCB" is code bias between P1 and C1 which
    * published by CODE monthly.
    *
    * In addition, some non-cross-correlator receivers provides the C1 observable
    * rather than P1. In this condition, this class will replace C1 by
    *  C1 --> C1 + c*p1c1DCB
    *
    * A typical way to use this class follows:
    *
    * @code
    *
    * gnssRinex gRin;
    * CC2NONCC cc2noncc;
    * cc2noncc.setRecType("TRIMBLE 4000SSE");
    * cc2noncc.loadDCBFile("P1C11001.DCB");
    * cc2noncc.loadRecTypeFile("receiver_bernese.lis");
    *
    * while(rin >> gRin)
    * {
    *	   gRin >> cc2noncc;
    * }
    *
    * @endcode
    *
    * @sa 
    *
    */
   class CC2NONCC : public ProcessingClass
   {
      public :

           // Default constructr
         CC2NONCC()
            :copyC1ToP1(false)                          
         {};

           /** Common constructor
            *
            * @param dcbDataReader    object to read and store DCB data
            */
         CC2NONCC( DCBDataReader& dcbDataReader )
            : dcbP1C1(&dcbDataReader) {};
         

           /** Set type of receiver
            *
            * @param  rectype    Type of receiver
            */
         virtual CC2NONCC& setRecType(const std::string& rectype);


           /** Add the file containing receiver type information
         	*
            * @param  recfile    Name of the file containing receiver information
            */
         virtual CC2NONCC& loadRecTypeFile(const std::string& recfile);


           /** Set whether to create P1 .
         	*
            * @param copy    boolean to indiate whether copy P1
            */
         virtual CC2NONCC& setCopyC1ToP1(bool copy)
         { copyC1ToP1 = copy; return (*this); }
           

           /** Return a satTypeValueMap object, adding the new data generated
            *  when calling this object.
            *
            * @param  time     Epoch corresponding to the data.
            * @param  gData    Data object holding the data.
            */
         virtual satTypeValueMap& Process( const CommonTime& time,
                                           satTypeValueMap& gData)
            throw(ProcessingException);


           /** Returns a gnssSatTypeValue object, adding the new data
            *  generated when calling this object.
            *
            * @param gData     Data object holding the data.
            */
         virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException)
         { Process(gData.header.epoch, gData.body); return gData; };


           /** Returns a gnssRinex object, adding the new data generated
            *  when calling this object.
            *
            * @param gData     Data object holding the data.
            */
         virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException)
         { Process(gData.header.epoch, gData.body); return gData; };


           // Returns a string identifying this object.
         virtual std::string getClassName () const;


           // Default deconstructr
         ~CC2NONCC(){};


      private:

           // The choice if copy the C1 to P1,if it's true,we will
           // copy C1 to P1
         bool copyC1ToP1;

           // Object that reads and stores DCB data
         DCBDataReader *dcbP1C1;

           // Object to acess receiver code observable type
         RecTypeDataReader recTypeData;

           // The type of receiver
         std::string recType;
         
   }; // End of class 'CC2NONCC'

      // @}

}; // end of namespace gpstk

#endif   // GPSTK_CC2NONCC_HPP
