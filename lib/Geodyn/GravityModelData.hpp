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
 * @file GravityModelData.hpp
 */

#ifndef GPSTK_GRAVITY_MODEL_DATA_HPP
#define GPSTK_GRAVITY_MODEL_DATA_HPP

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.hpp"

using namespace std;
namespace gpstk
{
   /** @addtogroup GeoDynamics */
   //@{

   /** Gravity Model Data.
    *
    */
   class GravityModelData
   {
   public:
         
         /// Default constructor
      GravityModelData()
      {
         // model name
         gmData.modelName = "EGM96";
            
         // tide_system
         gmData.tide_system = "tide_free";

         // earth gravitation constant
         gmData.GM = 3.9860044150E+14;

         // radius
         gmData.ae = 6378136.300000;

         // tide free
         gmData.includesPermTide = false;

         // reference time
         gmData.refMJD =  51544.0;

         // max degree
         gmData.maxDegree = 360;

         // coefficients and errors
         int size = (gmData.maxDegree+2)*(gmData.maxDegree+1)/2;
         gmData.normalizedCS.resize(size,4,0.0);
      };
         
 

         /// Load EGM file
      virtual  void loadEGMFile(string filename)
         throw(FileMissingException);
      
         /// Load EGM file at degree n
      virtual  void loadEGMFile(string filename,const int n)
         throw(FileMissingException);


      virtual string modelName() const
      { return gmData.modelName; }

      struct GravityModelStore
      {
         std::string modelName;
         std::string tide_system;
         
         double GM;
         double ae;

         bool includesPermTide;

         double refMJD;

         int maxDegree;

         Matrix<double> normalizedCS;

      } gmData;

   }; // End of class 'ReadGravityModel'

   // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_GRAVITY_MODEL_DATA_HPP
