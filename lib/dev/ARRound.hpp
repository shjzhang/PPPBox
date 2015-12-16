#pragma ident "$Id: ARRound.hpp ?? 2012-01-19 14:42:13Z shjzhang $"

/**
 * @file ARRound.hpp
 * 
 */

#ifndef GPSTK_AR_ROUND_HPP
#define GPSTK_AR_ROUND_HPP

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
//  Copyright
//
//  Shoujian Zhang, Wuhan University . 2011
//
//============================================================================
//
//  Revision
//
//  - Create this program, 2012.01.19 
//  - Modify some variable names, which is more meaningful. 2012.06.19
//  - Change the 'ARRound' from the inherited class of ARBase to independent
//    one, 2014.03.13, shjzhang.
//
//============================================================================

#include "Matrix.hpp"
#include "Exception.hpp"
#include "SpecialFunctions.hpp"

namespace gpstk
{

   using namespace std;


      /// New exception for Ambiguity Resolution
   NEW_EXCEPTION_CLASS(ARRoundException, Exception);


      /** This class resolve integer ambiguity using the rounding criterion
       *  based on a decision function adobted by (Dong D., Bock Y., 1988).
       */
   class ARRound 
   {
   public:
      
         /// Default constructor
      ARRound()
         : cutDec(1000.0), cutDev(0.3), cutSig(0.3)
      {};      


         /** Common constructor
          *
          * @param cutDecision      cutting decision
          * @param cutDeviation     cutting deviation of the ambiguity
          * @param cutSigma         cutting sigma of the ambiguity
          */
      ARRound( double cutDecision, 
               double cutDeviation, 
               double cutSigma )
         : cutDec(cutDecision), cutDev(cutDeviation), cutSig(cutSigma)
      {};


         /** Resolve the ambiguities to integers according to the round 
          *  method from Dong Danan's paper.
          *
          * @param ambFloat         Float ambiguities
          * @param ambCov           Ambiguities' covariance 
          */
      virtual Vector<double> resolve(const Vector<double>& ambFloat, 
                                     const Matrix<double>& ambCov )
          throw(ARRoundException)
      {

            // Check the size of the ambiguity vector and the covariance matrix 
         if( ambFloat.size()!=ambCov.rows() )
         {
            ARRoundException e("The dimension of ambFloat doesn't match \
tha   t of ambCov");
            GPSTK_THROW(e);
         }

         if( ambCov.rows() != ambCov.cols() )
         {
            ARRoundException e("The ambCov is not square");
            GPSTK_THROW(e);
         }

            // Initialize the flags, if it is empty
         flags.resize( ambFloat.size(), false);

           // Define size of ambiguity vector
         const size_t namb = ambFloat.size();

            // fixed ambiguity
         Vector<double> ambFixed(namb,0.0);


            // number of ambiguity to be fixed, always == 1, here
         int nfixed = 0;

            // ambiguity decision, ambigity value and sigma
         double decision, value, sigma;

            // Search the ambiguity with largest decision value
         for(int i = 0; i < namb; i++)
         {
               // If the ambiguity has not been fixed, then continue ...
            if( !flags(i) ) 
            {
                  // Float ambiguity value
               value = ambFloat(i);

                  // Get sigma
               sigma = std::sqrt(ambCov(i,i));

                  // Compute the decision for ambiguity 
               decision = getDecision(value, sigma);

                  // fix the ambiguity
               ambFixed(i) = std::floor( ambFloat(i) + 0.5 );

                  // Fix the ambiguities with reasonable condition
               if( decision > cutDec )
               {
                     // Number of the fixed ambiguities 
                  nfixed = nfixed + 1;

                     // update the ambiguity flags 
                  flags(i) = true;
               }
            }
         }

         return ambFixed;

      }  // End of method 'ARRound::resolve()'




         /** Resolve the ambiguities to integers according to the round 
          *  method from Dong Danan's paper.
          *
          * @param ambFloat         Float ambiguities
          * @param ambCov           Ambiguities' covariance 
          */
      virtual double resolve(const double& ambFloat, 
                             const double& ambCov )
      {
            // Define a temporary vector to store the 'ambFloat'
         Vector<double> ambFloatVec(1,ambFloat);

            // Define a temporary matrix to store the 'ambFloat'
         Matrix<double> ambCovMatrix(1,1,ambCov);

            // Define a temporary matrix to store the 'ambFloat'
         Vector<double> ambFixedVec(1,0.0);

            // Try to fix the ambiguity
         ambFixedVec = resolve(ambFloatVec, ambCovMatrix);

            // Extract the first fixed ambiguity
         double ambFixed(ambFixedVec(0));

            // Return the fixed ambiguity
         return ambFixed;
         

      }  // End of method 'ARRound::resolve()'



         /** Compute and return the decision of the ambiguity according to the 
          *  decision fucntion
          *
          * @param ambiguity        Float ambiguity
          * @param sigma            Variance of the ambiguity
          *
          * @return the decision values
          */
      double getDecision(double value, double sigma )
      {
          double INT, dev;

             // Integer of the float ambiguity       
          INT = std::floor( value+0.5 );

             // Deviation of the float ambigity and the integer ones.
          dev = std::abs  ( value-INT );

             // Taper function variable
          double t1, t2, taper;

             // Compute the taper function
          if( dev > cutDev ) 
          { 
              t1 = 0.0; 
          }
          else                  
          { 
              t1 = (1-dev/cutDev); 
          }
          
             // t2
          if( sigma > cutSig )    
          { 
              t2 = 0.0; 
          }
          else                  
          { 
              t2 = ( 1 - 3*sigma ); 
          }

             // computing taper function
          taper = std::pow(t1,2)*t2;

          double Q0(0.0);
          double b1(0.0), b2(0.0);
          double e1(0.0), e2(0.0);
          double de;

             // Compute the Q(0) according to equation A-12 in Dong and Bock
          for( int i=1;i<50;i++ )
          {
                // b1
             b1 = (i-dev)/(std::sqrt(2)*sigma);

                // b1
             b2 = (i+dev)/(std::sqrt(2)*sigma);

                // e1
             if( b1<0.0 || b1>15.0 ) { e1 = 0.0; }
             else                    { e1 = erfc(b1); }

               // e2
             if( b2<0.0 || b2>15.0 ) { e2 = 0.0; }
             else                    { e2 = erfc(b2); }

               // de
             de = e1 - e2;

               // rounding criterion values
             Q0 = Q0 + de;

               // break if the de less than a small value
             if(de < 1.0E-8) { break; }
          }

          double decision;

          if(Q0 < 1.0E-8) { Q0 = 1.0E-8; }

             // threshold decision
          decision = taper/Q0;

             // Reset the decision if it is less than 'cutDec'
          if(decision <= cutDec )
          { 
             decision = 1.0E-8; 
          }

          return decision;

      }  // End of method 'ARRound::getDecision()'


         /** Set the ambiguity cutting decision
          *
          * @param cutDeviation     Cutting decision 
          */
      void setCutDev(double cutDeviation)
      {
          cutDev = cutDeviation;
      };


         /* Set the cutting sigma
          *
          *@param cutSigma          Cutting sigma for the ambiguity fixing
          */
      void setCutSig(double cutSigma)
      {
          cutSig = cutSigma;
      };


         /* Set the ambiguity flags for the input ambiguities 
          *
          *@param ambflags         Flags denoting that which ambiguity has been fixed. 
          */
      void setFlags(Vector<bool> ambFlags)
      {
         flags = ambFlags;
      }  


         /* Get the ambiguity flag for the input single ambiguity
          *
          *@param ambFloat        Float ambiguities. 
          *@param ambCov          Ambiguity covariance. 
          * 
          *@warning The input parameters is just used as the indicator, which 
          *         is needed for the overloading of the method 'getFlags'
          */
      Vector<bool> getFlags(const Vector<double>& ambFloat, 
                            const Matrix<double>& ambCov)
      {
         return flags;
      }  


         /* Get the ambiguity flag for the input single ambiguity
          *
          *@param ambFloat        Single float ambiguity. 
          *@param ambCov          Ambiguity covariance. 
          *
          *@warning The input parameters is just used as the indicator 
          *         for the overloading of the method 'getFlags'
          */
      bool getFlags(const double& ambFloat, 
                    const double& ambCov)
      {
          if(flags.size() != 1)
          {
            flags.resize( 1, false);
          }

             // Define a bool flag
          bool flag(flags(0));

             // Return the flag
          return flag;
      };

         /// Destructor
      virtual ~ARRound(){}

      
   private:

         // Threshold decision for fixing ambiguity
      double cutDec;


         // Threshold deviation for taper function
      double cutDev;


         // Threshold sigma for taper function
      double cutSig;


         // Flags indicating whether the ambiguity is fixed or not
      Vector<bool> flags;


   };   // End of class 'ARRound'
   
}   // End of namespace gpstk


#endif  //GPSTK_AR_ROUND_HPP

