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
* @file AdamsBashforthMoulton.hpp
* This class implements Adams Bashforth Moulton 8-th order algorithm.
*/


#ifndef GPSTK_ADAMS_BASHFORTH_MOULTON_HPP
#define GPSTK_ADAMS_BASHFORTH_MOULTON_HPP

#include "EquationOfMotion.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class implements Adams Bashforth Moulton 8-th order algorithm.
       *
       */
   class AdamsBashforthMoulton
   {
   public:

      /// Default constructor
      AdamsBashforthMoulton()
         : stepSize(300.0),
           errorTol(1.0e-12)
      {}


      /// Default destructor
      virtual ~AdamsBashforthMoulton() {}


      /// Set step size
      inline AdamsBashforthMoulton& setStepSize(const double& size)
      {
         stepSize = size;

         return (*this);
      }


      /// Get step size
      inline double getStepSize() const
      {
         return stepSize;
      }


      /// Set error tolerance
      inline AdamsBashforthMoulton& setErrorTolerance(const double& tol)
      {
         errorTol = tol;

         return (*this);
      }


      /// Get error tolerance
      inline double getErrorTolerance() const
      {
         return errorTol;
      }


      /// Real implementation of abm8
      void integrateTo(std::vector< double >&         currentTime,
                       std::vector< Vector<double> >& currentState,
                       EquationOfMotion*              peom,
                       double                         nextTime);


   private:

      /// Coefficients of Adams-Bashforth
      const static double cb[9];

      /// Coefficients of Adams-Moulton
      const static double cm[9];


      /// Step size
      double stepSize;

      /// Error tolerance
      double errorTol;


   }; // End of class 'AdamsBashforthMoulton'

      // @}

}  // End of namespace 'gpstk'


#endif // GPSTK_ADAMS_BASHFORTH_MOULTON_HPP
