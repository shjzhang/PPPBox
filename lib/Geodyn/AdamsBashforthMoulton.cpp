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
//  Kaifa Kuang - Wuhan University . 2016
//
//============================================================================

/**
 * @file AdamsBashforthMoulton.cpp
 * This class implements Adams Bashforth Moulton 8-th order algorithm.
 */

#include "AdamsBashforthMoulton.hpp"

using namespace std;

namespace gpstk
{

   // coefficients, cbi
   const double AdamsBashforthMoulton::cb[9] =
   { 
        14097247.0,
      - 43125206.0,
        95476786.0,
      -139855262.0,
       137968480.0,
      - 91172642.0,
        38833486.0,
      -  9664106.0,
         1070017.0,
   };

   // coefficients, cmi
   const double AdamsBashforthMoulton::cm[9] =
   {
       1070017.0,
       4467094.0,
      -4604594.0, 
       5595358.0,
      -5033120.0,
       3146338.0,
      -1291214.0,
        312874.0,
      -  33953.0,
   };

   

   /// Real implementation of abm8
   void AdamsBashforthMoulton::integrateTo(vector< double >&         currentTime,
                                           vector< Vector<double> >& currentState,
                                           EquationOfMotion*         peom,
                                           double                    nextTime)
   {
      // Derivatives at currentTime[i], computed with currentState[i]
      vector< Vector<double> > fn;

      for(int i=0; i<8; ++i)
      {
         fn.push_back( peom->getDerivatives(currentTime[i], currentState[i]) );
      }

      // Prediction
      Vector<double> yp;
      yp = currentState[8] + stepSize/3628800 * (cb[0]*fn[8] + cb[1]*fn[7] + cb[2]*fn[6]
                                               + cb[3]*fn[5] + cb[4]*fn[4] + cb[5]*fn[3]
                                               + cb[6]*fn[2] + cb[7]*fn[1] + cb[8]*fn[0]);

      // Derivatives at t(n+1), computed with yp
      double tnp1;
      tnp1 = currentTime[8] + stepSize;
      Vector<double> fnp1;
      fnp1 = peom->getDerivatives(tnp1, yp);

      // Correction
      Vector<double> yc;
      yc = currentState[8] + stepSize/3628800 *(cm[0]*fnp1  + cm[1]*fn[8] + cm[2]*fn[7]
                                              + cm[3]*fn[6] + cm[4]*fn[5] + cm[5]*fn[4]
                                              + cm[6]*fn[3] + cm[7]*fn[2] + cm[8]*fn[1]);

      // Update
      currentTime.erase( currentTime.begin() );
      currentTime.push_back( tnp1 );
      currentState.erase( currentState.begin() );
      currentState.push_back( yc );

   }



}  // End of 'namespace gpstk'
