#pragma ident "$Id$"

/**
* @file GPSTK_CC2NONCC_CPP
* Class to convert CC(cross-correlation) to NONCC(non cross-correlation).
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
//  Copyright (c)
//
//  Q.Liu, Wuhan Uniersity, 2015 
//
//  Modified
//  2016-1-26 For C1/P2 receiver, the observable list maybe has P1(0),then
//  this Satelite would be filtered, this result is unreasonable,so we delete
//  the P1 filter.
//============================================================================

#include "CC2NONCC.hpp"

using namespace std;

namespace gpstk
{

      // Returns a string identifying this object.
   string CC2NONCC::getClassName() const
   { return "CC2NONCC"; }

     // recType gets a value
   CC2NONCC& CC2NONCC::setRecType(const string& rectype)
   {
		recType = rectype;
		return (*this);
   }

      /* Sets name of file containing DCBs data.
       * @param name      Name of the file containing DCB(P1-C1)
       */
   CC2NONCC& CC2NONCC::setDCBFile(const string& fileP1C1)
   {
      dcbP1C1.open(fileP1C1);
      
      return (*this);
   }

      /* Sets name of file containing receiver code observable type.
       * @param name      Name of the file containing receiver code
	   *                  observable type
       */
   CC2NONCC& CC2NONCC::setRecTypeFile(const string& recfile)
   {
      recTypeData.open(recfile);
      
      return (*this);
   }
   
      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CC2NONCC::Process( const CommonTime& time,
                                       satTypeValueMap& gData )
      throw(ProcessingException)
   {
      
      try
      {
         SatIDSet satRejectedSet;


	 bool RecHasC1(false);
	 bool RecHasP1(false);	
	 bool RecHasP2(false);	
	 bool RecHasX2(false);	

         //
         // Firstly, read the obs types that the 'recType' supports
         //
	 set<string> recCodeSet = recTypeData.getCode(recType);

         if( recCodeSet.size() == 0 )
         {
            GPSTK_THROW(RecTypeNotFound("receiver type is not found in bernese.lis"));
         }

         int C1Code = recCodeSet.count("C1");
	 int P1Code = recCodeSet.count("P1");
         int P2Code = recCodeSet.count("P2");
	 int X2Code = recCodeSet.count("X2");

         if(C1Code) RecHasC1 = true;
	 if(P1Code) RecHasP1 = true;
	 if(P2Code) RecHasP2 = true;
	 if(X2Code) RecHasX2 = true;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {
            SatID sat = it->first;
            
            typeValueMap::iterator ittC1 = it->second.find(TypeID::C1);
            typeValueMap::iterator ittP1 = it->second.find(TypeID::P1);
            typeValueMap::iterator ittP2 = it->second.find(TypeID::P2);

            bool hasC1( ittC1 != it->second.end() );
            bool hasP1( ittP1 != it->second.end() );
            bool hasP2( ittP2 != it->second.end() );


               // For receiver noncc (C1,P2)
               // For the noncc: only C1 should be corrected
	       // C1->C1+(P1-C1)
            if( RecHasC1 && RecHasP2 )
            {
		/* Changed in 2016-1-26, by Q.Liu
		 * Because in some C1/P2 observable files,the observable
		 * list has P1 */
		if( hasC1 )
		{
                   double Bp1c1(0.0);      // in ns
                   try
                   {
                      Bp1c1 = dcbP1C1.getDCB(sat);
                   }
                   catch(...)
                   {
                      Bp1c1 = 0.0;
                   }

		     // Correct
		   it->second[TypeID::C1] = it->second[TypeID::C1] + 
                                            Bp1c1 * C_MPS * 1.0e-9;
		     // Copy C1 to P1
                   if(copyC1ToP1)
                   {
		      it->second[TypeID::P1] = it->second[TypeID::C1]; 
                   }
		}
		else 
		{
		    satRejectedSet.insert(it->first);
	        }
            }

               // For receiver CC (C1,X2)
	       // C1->C1+(P1-C1); X2->X2+(P1-C1)
            if( RecHasC1 && RecHasX2 )
            {
               if( hasC1 && hasP2 )
               {
                  double Bp1c1(0.0);      // in ns
                  try
                  {
                     Bp1c1 = dcbP1C1.getDCB(sat);
                  }
                  catch(...)
                  {
                     Bp1c1 = 0.0;
                  }

		    // Correct
		  it->second[TypeID::C1] = it->second[TypeID::C1] 
                                         + Bp1c1 * C_MPS * 1.0e-9;

	          it->second[TypeID::P2] = it->second[TypeID::P2] 
                                         + Bp1c1 * C_MPS * 1.0e-9;

		    // Copy C1 to P1
                  if(copyC1ToP1)
                  {
		      it->second[TypeID::P1] = it->second[TypeID::C1]; 
                  }
               }
               else
               {
                  satRejectedSet.insert(it->first);
               }
            }

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'
         gData.removeSatID(satRejectedSet);
         return gData;
      }
      catch(RecTypeNotFound& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
            + u.what() );

         GPSTK_THROW(e);
      }
      catch(Exception& u)
      {

         // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
            + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'CC2NONCC::Process()'


}  // End of namespace gpstk
