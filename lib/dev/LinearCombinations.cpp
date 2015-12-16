#pragma ident "$Id$"

/**
 * @file LinearCombinations.cpp
 * This class defines handy linear combinations of GDS data.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================
//
//  Revision
//
//  2014/02/28    modify the TypeID::prefitP1 for p1Prefit to TypeID::prefitC
//                which will make the solver to process the semi-codelss 
//                receiver easy .
//
//  2014/02/28    add 'updSatLC' for lcPrefit, and add 'updSatMW' for 
//                mwubbenaPrefit.
//
//============================================================================


#include "LinearCombinations.hpp"


namespace gpstk
{

   LinearCombinations::LinearCombinations()
   {

      double x( ( GAMMA_GPS + 1.0)/( GAMMA_GPS - 1.0 ) );
      double y( ( 2.0            )/( GAMMA_GPS - 1.0 ) );
      double z( ( 2.0 * GAMMA_GPS)/( GAMMA_GPS - 1.0 ) );

         // Definition to compute the code with minus ionospheric delays 
         // in the L1 frequency
      q1CombWithC1.header               = TypeID::Q1;
      q1CombWithC1.body[TypeID::C1]     = +x;
      q1CombWithC1.body[TypeID::P2]     = -y;

         // Definition to compute the code with minus ionospheric delays 
         // in the L1 frequency
      q1Combination.header              = TypeID::Q1;
      q1Combination.body[TypeID::P1]    = +x;
      q1Combination.body[TypeID::P2]    = -y;

         // Definition to compute the code with minus ionospheric delays 
         // in the L1 frequency
      q2CombWithC1.header               = TypeID::Q2;
      q2CombWithC1.body[TypeID::C1]     = +z;
      q2CombWithC1.body[TypeID::P2]     = -x;

         // Definition to compute the ambiguity in the L1 frequency
      q2Combination.header              = TypeID::Q2;
      q2Combination.body[TypeID::P1]    = +z;
      q2Combination.body[TypeID::P2]    = -x;


      double a(+GAMMA_GPS/0.646944444);
      double b(1.0/0.646944444);

         // Definition to compute prefit residual of C1
      c1Prefit.header                     = TypeID::prefitC1;
      c1Prefit.body[TypeID::C1]           = +1.0;
      c1Prefit.body[TypeID::rho]          = -1.0;
      c1Prefit.body[TypeID::dtSat]        = +1.0;
      c1Prefit.body[TypeID::rel]          = -1.0;
      c1Prefit.body[TypeID::gravDelay]    = -1.0;
      c1Prefit.body[TypeID::satPCenter]   = -1.0;
      c1Prefit.body[TypeID::tropoSlant]   = -1.0;
         // The DCB for C1
//    c1Prefit.body[TypeID::instC1]       = -1.0;
         // Corrections interpolated from reference 
      c1Prefit.body[TypeID::corrC1]       = -1.0;


         // Definition to compute prefit residual of P1
      p1Prefit.header                     = TypeID::prefitP1;
      p1Prefit.body[TypeID::P1]           = +1.0;
      p1Prefit.body[TypeID::rho]          = -1.0;
      p1Prefit.body[TypeID::dtSat]        = +1.0;
      p1Prefit.body[TypeID::rel]          = -1.0;
      p1Prefit.body[TypeID::gravDelay]    = -1.0;
      p1Prefit.body[TypeID::satPCenter]   = -1.0;
      p1Prefit.body[TypeID::tropoSlant]   = -1.0;
         // DCB for P1
//    p1Prefit.body[TypeID::instP1]       = -1.0;
         // Interpolated from reference stations
      p1Prefit.body[TypeID::corrP1]       = -1.0;
    


         // Definition to compute prefit residual of P2
      p2Prefit.header                     = TypeID::prefitP2;
      p2Prefit.body[TypeID::P2]           = +1.0;
      p2Prefit.body[TypeID::rho]          = -1.0;
      p2Prefit.body[TypeID::dtSat]        = +1.0;
      p2Prefit.body[TypeID::rel]          = -1.0;
      p2Prefit.body[TypeID::gravDelay]    = -1.0;
      p2Prefit.body[TypeID::satPCenter]   = -1.0;
      p2Prefit.body[TypeID::tropoSlant]   = -1.0;

         // DCB for P2
//    p2Prefit.body[TypeID::instP2]       = -1.0;
         // Interpolated from reference stations
      p2Prefit.body[TypeID::corrP2]       = -1.0;


         // Definition to compute prefit residual of L1
      l1Prefit.header                     = TypeID::prefitL1;
      l1Prefit.body[TypeID::L1]           = +1.0;
      l1Prefit.body[TypeID::rho]          = -1.0;
      l1Prefit.body[TypeID::dtSat]        = +1.0;
      l1Prefit.body[TypeID::updSatL1]     = +1.0;
      l1Prefit.body[TypeID::rel]          = -1.0;
      l1Prefit.body[TypeID::gravDelay]    = -1.0;
      l1Prefit.body[TypeID::satPCenter]   = -1.0;
      l1Prefit.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for L1 windUp is L1 wavelength/2*PI
      l1Prefit.body[TypeID::windUp]       = -L1_WAVELENGTH_GPS/TWO_PI;
         // Interpolated from reference stations
      l1Prefit.body[TypeID::corrL1]       = -1.0;


         // Definition to compute prefit residual of L2
      l2Prefit.header                     = TypeID::prefitL2;
      l2Prefit.body[TypeID::L2]           = +1.0;
      l2Prefit.body[TypeID::rho]          = -1.0;
      l2Prefit.body[TypeID::dtSat]        = +1.0;
      l2Prefit.body[TypeID::updSatL2]     = +1.0;
      l2Prefit.body[TypeID::rel]          = -1.0;
      l2Prefit.body[TypeID::gravDelay]    = -1.0;
      l2Prefit.body[TypeID::satPCenter]   = -1.0;
      l2Prefit.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for L2 windUp is L2 wavelength/2*PI
      l2Prefit.body[TypeID::windUp]       = -L2_WAVELENGTH_GPS/TWO_PI;
         // Interpolated from reference stations
      l2Prefit.body[TypeID::corrL2]       = -1.0;

         // Definition to compute PC combination
      pcCombination.header                = TypeID::PC;
      pcCombination.body[TypeID::P1]      = +a;
      pcCombination.body[TypeID::P2]      = -b;

         // Definition to compute PC combination, using C1 instead of P1
      pcCombWithC1.header                 = TypeID::PC;
      pcCombWithC1.body[TypeID::C1]       = +a;
      pcCombWithC1.body[TypeID::P2]       = -b;

         // Definition to compute prefit residual of PC
      pcPrefit.header                     = TypeID::prefitC;
      pcPrefit.body[TypeID::PC]           = +1.0;
      pcPrefit.body[TypeID::rho]          = -1.0;
      pcPrefit.body[TypeID::dtSat]        = +1.0;
      pcPrefit.body[TypeID::rel]          = -1.0;
      pcPrefit.body[TypeID::gravDelay]    = -1.0;
      pcPrefit.body[TypeID::satPCenter]   = -1.0;
      pcPrefit.body[TypeID::tropoSlant]   = -1.0;
         // Interpolated from reference stations
      pcPrefit.body[TypeID::corrPC]       = -1.0;

         // Definition to compute LC combination
      lcCombination.header                = TypeID::LC;
      lcCombination.body[TypeID::L1]      = +a;
      lcCombination.body[TypeID::L2]      = -b;

         // Definition to compute prefit residual of LC
      lcPrefit.header                     = TypeID::prefitL;
      lcPrefit.body[TypeID::LC]           = +1.0;
      lcPrefit.body[TypeID::rho]          = -1.0;
      lcPrefit.body[TypeID::dtSat]        = +1.0;
      lcPrefit.body[TypeID::updSatLC]     = +1.0;   // UPD for LC
      lcPrefit.body[TypeID::rel]          = -1.0;
      lcPrefit.body[TypeID::gravDelay]    = -1.0;
      lcPrefit.body[TypeID::satPCenter]   = -1.0;
      lcPrefit.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for LC windUp is LC wavelenght/2*PI
      lcPrefit.body[TypeID::windUp]       = -0.1069533781421467/TWO_PI;

         // Interpolated from reference stations
      lcPrefit.body[TypeID::corrLC]       = -1.0;

         // Definition to compute prefit residual of PC for clock estimation
      pcPrefitC.header                     = TypeID::prefitC;
      pcPrefitC.body[TypeID::PC]           = +1.0;
      pcPrefitC.body[TypeID::rho]          = -1.0;
      pcPrefitC.body[TypeID::rel]          = -1.0;
      pcPrefitC.body[TypeID::gravDelay]    = -1.0;
      pcPrefitC.body[TypeID::satPCenter]   = -1.0;
      pcPrefitC.body[TypeID::tropoSlant]   = -1.0;

         // Definition to compute prefit residual of LC for clock estimation
      lcPrefitC.header                     = TypeID::prefitL;
      lcPrefitC.body[TypeID::LC]           = +1.0;
      lcPrefitC.body[TypeID::rho]          = -1.0;
      lcPrefitC.body[TypeID::rel]          = -1.0;
      lcPrefitC.body[TypeID::gravDelay]    = -1.0;
      lcPrefitC.body[TypeID::satPCenter]   = -1.0;
      lcPrefitC.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for LC windUp is LC wavelenght/2*PI
      lcPrefitC.body[TypeID::windUp]       = -0.1069533781421467/TWO_PI;


         // Definition to compute PI combination
      piCombination.header                = TypeID::PI;
      piCombination.body[TypeID::P1]      = -1.0;
      piCombination.body[TypeID::P2]      = +1.0;

         // Definition to compute PI combination, using C1 instead of P1
      piCombWithC1.header                 = TypeID::PI;
      piCombWithC1.body[TypeID::C1]       = -1.0;
      piCombWithC1.body[TypeID::P2]       = +1.0;

         // Definition to compute LI combination
      liCombination.header                = TypeID::LI;
      liCombination.body[TypeID::L1]      = +1.0;
      liCombination.body[TypeID::L2]      = -1.0;


      double c( L1_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );
      double d( L2_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );
      double e( L1_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );
      double f( L2_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );

         // Definition to compute Pdelta (PW) combination
      pdeltaCombination.header            = TypeID::Pdelta;
      pdeltaCombination.body[TypeID::P1]  = +c;
      pdeltaCombination.body[TypeID::P2]  = +d;

         // Definition to compute Pdelta (PW) combination, using C1 instead
         // of P1
      pdeltaCombWithC1.header             = TypeID::Pdelta;
      pdeltaCombWithC1.body[TypeID::C1]   = +c;
      pdeltaCombWithC1.body[TypeID::P2]   = +d;

         // Definition to compute prefit residual of Pdelta (LW)
      pdeltaPrefit.header                     = TypeID::prefitPdelta;
      pdeltaPrefit.body[TypeID::Pdelta]       = +1.0;
      pdeltaPrefit.body[TypeID::rho]          = -1.0;
      pdeltaPrefit.body[TypeID::dtSat]        = +1.0;
      pdeltaPrefit.body[TypeID::rel]          = -1.0;
      pdeltaPrefit.body[TypeID::gravDelay]    = -1.0;
      pdeltaPrefit.body[TypeID::satPCenter]   = -1.0;
      pdeltaPrefit.body[TypeID::tropoSlant]   = -1.0;

         // Computed from the reference stations
      pdeltaPrefit.body[TypeID::corrPdelta]   = -1.0;

         // DCB for instPdelta
//    pdeltaPrefit.body[TypeID::instPdelta]   = -1.0;


         // Definition to compute Ldelta (LW) combination
      ldeltaCombination.header            = TypeID::Ldelta;
      ldeltaCombination.body[TypeID::L1]  = +e;
      ldeltaCombination.body[TypeID::L2]  = -f;

         // Definition to compute prefit residual of Ldelta (LW)
      ldeltaPrefit.header                     = TypeID::prefitLdelta;
      ldeltaPrefit.body[TypeID::Ldelta]       = +1.0;
      ldeltaPrefit.body[TypeID::rho]          = -1.0;
      ldeltaPrefit.body[TypeID::dtSat]        = +1.0;
      ldeltaPrefit.body[TypeID::updSatWL]     = +1.0;
      ldeltaPrefit.body[TypeID::rel]          = -1.0;
      ldeltaPrefit.body[TypeID::gravDelay]    = -1.0;
      ldeltaPrefit.body[TypeID::satPCenter]   = -1.0;
      ldeltaPrefit.body[TypeID::tropoSlant]   = -1.0;

         // Computed from reference stations
      ldeltaPrefit.body[TypeID::corrLdelta]   = -1.0;


         // Definition to compute the Melbourne-Wubbena (W) combination
      mwubbenaCombination.header           = TypeID::MWubbena;
      mwubbenaCombination.body[TypeID::L1] = +e;
      mwubbenaCombination.body[TypeID::L2] = -f;
      mwubbenaCombination.body[TypeID::P1] = -c;
      mwubbenaCombination.body[TypeID::P2] = -d;


         // Definition to compute the prefit residual of Melbourne-Wubbena (W) 
         // combination
      mwubbenaPrefit.header           = TypeID::prefitMWubbena;
      mwubbenaPrefit.body[TypeID::MWubbena]     = +1.0;
//    mwubbenaPrefit.body[TypeID::instMWubbena] = -1.0;  // DCB for MWubbena
      mwubbenaPrefit.body[TypeID::updSatMW]     = +1.0;  // UPD for MWubbena

         // Definition to compute the Melbourne-Wubbena (W) combination,
         // using C1 instead of P1
      mwubbenaCombWithC1.header           = TypeID::MWubbena;
      mwubbenaCombWithC1.body[TypeID::L1] = +e;
      mwubbenaCombWithC1.body[TypeID::L2] = -f;
      mwubbenaCombWithC1.body[TypeID::C1] = -c;
      mwubbenaCombWithC1.body[TypeID::P2] = -d;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L1 frequency
      GRAPHIC1Combination.header           = TypeID::GRAPHIC1;
      GRAPHIC1Combination.body[TypeID::P1] = +0.5;
      GRAPHIC1Combination.body[TypeID::L1] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L1 frequency (using C1 instead of P1)
      GRAPHIC1CombinationWithC1.header           = TypeID::GRAPHIC1;
      GRAPHIC1CombinationWithC1.body[TypeID::C1] = +0.5;
      GRAPHIC1CombinationWithC1.body[TypeID::L1] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L2 frequency
      GRAPHIC2Combination.header           = TypeID::GRAPHIC2;
      GRAPHIC2Combination.body[TypeID::P2] = +0.5;
      GRAPHIC2Combination.body[TypeID::L2] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L5 frequency
      GRAPHIC5Combination.header           = TypeID::GRAPHIC5;
      GRAPHIC5Combination.body[TypeID::C5] = +0.5;
      GRAPHIC5Combination.body[TypeID::L5] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L6 frequency
      GRAPHIC6Combination.header           = TypeID::GRAPHIC6;
      GRAPHIC6Combination.body[TypeID::C6] = +0.5;
      GRAPHIC6Combination.body[TypeID::L6] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L7 frequency
      GRAPHIC7Combination.header           = TypeID::GRAPHIC7;
      GRAPHIC7Combination.body[TypeID::C7] = +0.5;
      GRAPHIC7Combination.body[TypeID::L7] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L8 frequency
      GRAPHIC8Combination.header           = TypeID::GRAPHIC8;
      GRAPHIC8Combination.body[TypeID::C8] = +0.5;
      GRAPHIC8Combination.body[TypeID::L8] = +0.5;


         // Definition to compute WL combination
      wlCombination.header            = TypeID::WL;
      wlCombination.body[TypeID::L1]  = firstFactorOfLC(1,-1);
      wlCombination.body[TypeID::L2]  = secondFactorOfLC(1,-1);

         // Definition to compute prefit residual of WL
      wlPrefit.header                     = TypeID::prefitWL;
      wlPrefit.body[TypeID::WL]           = +1.0;
      wlPrefit.body[TypeID::rho]          = -1.0;
      wlPrefit.body[TypeID::dtSat]        = +1.0;
      wlPrefit.body[TypeID::rel]          = -1.0;
      wlPrefit.body[TypeID::gravDelay]    = -1.0;
      wlPrefit.body[TypeID::satPCenter]   = -1.0;
      wlPrefit.body[TypeID::tropoSlant]   = -1.0;
      wlPrefit.body[TypeID::ionoL1]       = firstFactorOfLC(1,-1)
                                           +secondFactorOfLC(1,-1)*GAMMA_GPS;
      wlPrefit.body[TypeID::windUp]       = -wavelengthOfLC(1,-1)/TWO_PI;

    
      // Definition to compute WL42combination
      wl2Combination.header            = TypeID::WL2;
      wl2Combination.body[TypeID::L1]  = firstFactorOfLC(-2,3);
      wl2Combination.body[TypeID::L2]  = secondFactorOfLC(-2,3);

      // Definition to compute prefit residual of WL2
      wl2Prefit.header                     = TypeID::prefitWL2;
      wl2Prefit.body[TypeID::WL2]           = +1.0;
      wl2Prefit.body[TypeID::rho]          = -1.0;
      wl2Prefit.body[TypeID::dtSat]        = +1.0;
      wl2Prefit.body[TypeID::rel]          = -1.0;
      wl2Prefit.body[TypeID::gravDelay]    = -1.0;
      wl2Prefit.body[TypeID::satPCenter]   = -1.0;
      wl2Prefit.body[TypeID::tropoSlant]   = -1.0;
      wl2Prefit.body[TypeID::ionoL1]       = firstFactorOfLC(-2,3)
                                             +secondFactorOfLC(-2,3)*GAMMA_GPS;
      wl2Prefit.body[TypeID::windUp]       = -wavelengthOfLC(-2,3)/TWO_PI;

         // Definition to compute WL4 combination
      wl4Combination.header            = TypeID::WL4;
      wl4Combination.body[TypeID::L1]  = firstFactorOfLC(4,-5);
      wl4Combination.body[TypeID::L2]  = secondFactorOfLC(4,-5);

      // Definition to compute prefit residual of WL4
      wl4Prefit.header                     = TypeID::prefitWL4;
      wl4Prefit.body[TypeID::WL4]           = +1.0;
      wl4Prefit.body[TypeID::rho]          = -1.0;
      wl4Prefit.body[TypeID::dtSat]        = +1.0;
      wl4Prefit.body[TypeID::rel]          = -1.0;
      wl4Prefit.body[TypeID::gravDelay]    = -1.0;
      wl4Prefit.body[TypeID::satPCenter]   = -1.0;
      wl4Prefit.body[TypeID::tropoSlant]   = -1.0;
      wl4Prefit.body[TypeID::ionoL1]       = firstFactorOfLC(4,-5)
                                            +secondFactorOfLC(4,-5)*GAMMA_GPS;
      wl4Prefit.body[TypeID::windUp]       = -wavelengthOfLC(4,-5)/TWO_PI;

   }  // End of constructor 'LinearCombinations::LinearCombinations()'


      // Return the frequency of the combination in cycles: i * L1 + j * L2 
   double LinearCombinations::freqOfLC(int i, int j, double f1 , double f2 )
   {
      return ( double(i)*f1+double(j)*f2 );
   }

      // Return the wavelength of the combination in cycles: i * L1 + j * L2 
   double LinearCombinations::wavelengthOfLC(int i,int j,double f1,double f2)
   {
      return C_MPS / freqOfLC(i,j,f1,f2);
   }

   /// Return the f1 factor of the combination in cycles: i * L1 + j * L2 
   double LinearCombinations::firstFactorOfLC(int i,int j,double f1,double f2)
   {
      return double(i)*f1/freqOfLC(i,j,f1,f2);
   }

      /// Return the f2 factor of the combination in cycles: i * L1 + j * L2 
   double LinearCombinations::secondFactorOfLC(int i,int j,double f1,double f2 )
   {
      return double(j)*f2/freqOfLC(i,j,f1,f2);
   }


} // End of namespace gpstk


