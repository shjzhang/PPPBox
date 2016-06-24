//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//  Kaifa Kuang - Wuhan University . 2015
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file GeoidHeight.hpp
 */

#ifndef GPSTK_Geoid_Height_HPP
#define GPSTK_Geoid_Height_HPP

#include "GravityModelData.hpp"
#include "Legendre.hpp"


using namespace std;
namespace gpstk
{
   /** @addtogroup GeoDynamics */
   //@{

   /** Gravity Model Data.
    *
    */
      GravityModelData GraModel;
      
      Vector<double> leg0,leg1,leg2;
      
      int degree;
      
      string filename;
      
      double Tdisturb=0.0;
      
      
   class GeoidHeight
   {
   public:
         
         GeoidHeight()
         {
            //nomal gravity;
            gama=9.78;
         }

         //nomal gravity;
         double gama;
         
        
         
         // compute Geoid
         // compute Geoid.default EGM96,MAXdegree=360;
         virtual  void computeGeoid(const double lat,
                                    const double lon,
                                    double& Ngeoid )
         throw(FileMissingException);


         // compute Geoid.default MAXdegree=360;
         virtual  void computeGeoid(const double lat,
                                    const double lon,
                                    const string filename,
                                    double& Ngeoid )
         throw(FileMissingException);
         
         
         virtual  void computeGeoid(const double lat,
                                    const double lon,
                                    const string filename,
                                    const int degree,
                                    double& Ngeoid )
         throw(FileMissingException);


   }; // End of class 'GeoidHeight'

   // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_GRAVITY_MODEL_DATA_HPP
