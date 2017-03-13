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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//=============================================================================

/**
 * @file SolverIonoDCB2.hpp
 * Class to model the ionosphere and estimate DCBs of satellite and receivers
 * together using spherical harmonic expansion.
 *
 * created by Wei Wang, Wuhan University, 2016/03/01
 */

#ifndef GPSTK_SolverIonoDCB2_HPP
#define GPSTK_SolverIonoDCB2_HPP

#include "SolverBase.hpp"
#include "Variable.hpp"
#include <vector>
#include <map>

namespace gpstk
{

   class SolverIonoDCB2 :  public SolverBase
   {
     public:
         // Constructor
      SolverIonoDCB2(int SHOrder):order(SHOrder) {};

         /** Compute the LMS solution
          *  NormMatrix = AT*P*A
          *  wVector = AT*P*L
          */
      virtual int Compute( const Matrix<double>& normMatrix,
                           const Vector<double>& wVector )
						   
      throw(InvalidSolver);

         /** Returns a reference to a gnssDataMap object after
          *  solving the previously defined equation system.
          *
          * @param gData          Data object holding the data.
          * @param interval       Data of a few hours are used once,
          *                       if interval = 24, use data of one day
          */
      virtual gnssDataMap& Process( gnssDataMap& gData,
                                    SourceIDSet& p1p2RecSet,
                                    SourceIDSet& c1p2RecSet,
                                    int interval );
        
          // estiamte the P1-C1 DCB 
      virtual gnssDataMap p1c1Estimate(gnssDataMap& gdsMap, 
                                       SourceIDSet& p1p2RecSet)
      throw (InvalidSolver);

        // get the ionosphereic coefficients
      Vector<double> getIonoCoef(void)
      { return IonoCoef; }

        // get the DCB for receivers
      double getRecDCB( const SourceID& rec ); 

        // get the DCB for satellites
      double getSatDCB( const SatID& sat );
        // get the P1-C1 DCB for satellites
      double getSatP1C1DCB( const SatID& sat );
        /// Norm Factor
      double norm(int n, int m);

        /// Legendre Polynomial
      double legendrePoly(int n, int m, double u);

         /// Returns an index identifying this object.
      virtual int getIndex(void) const;

         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;

         /// Destructor.
      virtual ~SolverIonoDCB2() {};


   private:
     
          // the max order of spherical harmonic(SH), dedault 4;
      int order;
         /// Number of SH coefficents
      int numIonoCoef;
         /// Number of unknowns
      int numUnknowns;
         /// Number of measurements
      int numMeas;
        
	/// the geographic latitude of geomagnetic north pole
      const double NGPLat = 80.27;
         /// the geographic longitude of geomagnetic north pole
      const double NGPLon = -72.58;
         
	 /// Map holding receiver information
      std::map<SourceID,double> recState;
        /// Map holding receiver P1-C1 DCB 
      std::map<SourceID,double> recP1C1DCB;
	 /// Map holding satellite information 
      std::map<SatID,double> satState;
        /// Map holding satellite P1-C1 DCB 
      std::map<SatID,double> satP1C1DCB;
         /// The value of  ionospheric coefficients 
	/// the sequence is A00,A10,A11,B11,A20,A21,B21,...
      Vector<double> IonoCoef;
         /// design Vector, 1*numUnknowns
      Vector<double> hVector;
         /// Measurements vector (Prefit-residuals),numMeas*1
      Vector<double> measVector;
         /// Covariance Matrix, numUnknowns*numUnknowns
      Matrix<double> NormMatrix;
           /// AT*P*L , numUnkonwns*1
      Vector<double> wVector;
         /// solution Vector
      Vector<double> sol;
         ///Precison 
      Vector<double> sigma;
      
      double sigma0;
       
      /// Set with all satellites being processed this epoch
      SatIDSet currSatSet;
	  /// Set with all receivers being processed with epoch
      SourceIDSet recSet;
         /// Boolean indicating if this filter was run at least once

         /// Initializing method.
      void Init(void);

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'SolverIonoDCB2'


}  // End of namespace gpstk

#endif   // GPSTK_SolverIonoDCB2_HPP
