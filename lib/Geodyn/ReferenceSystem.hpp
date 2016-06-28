/**
 * @file ReferenceSystem.hpp
 * This class ease handling reference system transformation.
 */

#ifndef GPSTK_REFERENCE_SYSTEM_HPP
#define GPSTK_REFERENCE_SYSTEM_HPP

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
//  Last Modified:
//
//  2013/05/29   Create this file
//
//  Shoujian Zhang, Wuhan University
//
//============================================================================

#include <string>
#include "GNSSconstants.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "CommonTime.hpp"
#include "EOPDataStore.hpp"
#include "LeapSecStore.hpp"
#include "Exception.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** Class to handle the reference system transformation, i.e. 
       *  Transforming the ICRS to ITRS and vice versa.
       *
       *  All the transformation matrix are computed using the 
       *  analytic fomulas.
       *
       *  The Earth Orentation Parameters are from the EOPDataStore, 
       *  and the leap seconds data are from LeapSecStore. 
       *
       *  When convert the ICRS to ITRS, you will use the time system 
       *  conversion, and the relation between different time system
       *  follows as:
       * 
       *           -14s
       *     -----------------> BDT(Compass Time)
       *     |
       *     |         +19s             +32.184s           +rel.effects
       *    GPS -------------> TAI ----------------> TT -----------------> TDB
       *                       T |
       *            -(UT1-TAI) | |    -leap seconds
       *    UT1 ---------------| |--------------------> UTC
       *     |
       *     |   earth rotation
       *     ---------------------> GAST
       *
       *  Most of the methods are modified from the code provided by
       *  Montenbruck, Oliver, 2003, as the attachement of the book
       *  "Satellite Orbits: Models, Methods and applications".
       *
       */
   class ReferenceSystem
   {
   public:

         /// Default constructor.
      ReferenceSystem()
        : pEopStore(NULL), pLeapSecStore(NULL), isPrepared(false)
      { }


         /** Constructor.
          */
      ReferenceSystem(EOPDataStore& eopStore,
                      LeapSecStore& leapSecStore)
        : isPrepared(false)
      {
         pEopStore = &eopStore;
         pLeapSecStore = &leapSecStore;
      }


         /// Set the EOP data store.
      ReferenceSystem& setEOPDataStore(EOPDataStore& eopStore)
      { pEopStore = &eopStore; return (*this); };


         /// Get the EOP data store.
      EOPDataStore* getEOPDataStore() const
      { return pEopStore; };


         /// Set the leapsec data store.
      ReferenceSystem& setLeapSecStore(LeapSecStore& leapSecStore)
      { pLeapSecStore = &leapSecStore; return (*this); };


         /// Get the leapsec data store.
      LeapSecStore* getLeapSecStore() const
      { return pLeapSecStore; };


         /// Get the x pole displacement parameter, in arcseconds
      double getXPole(const CommonTime& UTC) const
         throw (InvalidRequest);


         /// Get the y pole displacement parameter, in arcseconds
      double getYPole(const CommonTime& UTC) const
         throw (InvalidRequest);


         /// Get the value of (UT1 - UTC), in seconds
      double getUT1mUTC(const CommonTime& UTC) const
         throw (InvalidRequest);

         /// Get the value of dPsi, in arcseconds
      double getDPsi(const CommonTime& UTC) const
          throw(InvalidRequest);

         /// Get the value of dEps, in arcseconds
      double getDEps(const CommonTime& UTC) const
          throw(InvalidRequest);


         /// Get the EOP data at the given UTC time.
      EOPDataStore::EOPData getEOPData(const CommonTime& UTC) const
         throw(InvalidRequest);


         /// Get the leapsec data at the given UTC time.
      double getLeapSec(const CommonTime& UTC) const
         throw(InvalidRequest);


      ///------ Methods to get the difference of different time systems ------//


         /// Get the value of (TAI - UTC) (= leapsec).
      double getTAImUTC(const CommonTime& UTC)
         throw(InvalidRequest);


         /// Get the value of (TT - UTC) (= 32.184 + leapsec).
      double getTTmUTC(const CommonTime& UTC)
         throw(InvalidRequest);


         /// Get the value of (TT - TAI) (= 32.184s).
      double getTTmTAI(void)
         throw(InvalidRequest);


         /// Get the value of (TAI - GPS) (= -19s).
      double getTAImGPS(void)
         throw(InvalidRequest);



      ///------ Methods to convert between different time systems ------//


         /// Convert GPS to UTC.
      CommonTime GPS2UTC(const CommonTime& GPS);


         /// Convert UTC to GPS.
      CommonTime UTC2GPS(const CommonTime& UTC);


         /// Convert UT1 to UTC.
      CommonTime UT12UTC(const CommonTime& UT1);


         /// Convert UTC to UT1.
      CommonTime UTC2UT1(const CommonTime& UTC);
      CommonTime UTC2UT1(const CommonTime& UTC, const double& UT1mUTC);


         /// Convert TAI to UTC.
      CommonTime TAI2UTC(const CommonTime& TAI);


         /// Convert UTC to TAI.
      CommonTime UTC2TAI(const CommonTime& UTC);


         /// Convert TT to UTC.
      CommonTime TT2UTC(const CommonTime& TT);


         /// Convert UTC to TT.
      CommonTime UTC2TT(const CommonTime& UTC);



         /// Convert TDB to UTC.
//    CommonTime TDB2UTC(const CommonTime& TDB);


         /// Convert UTC to TDB.
//    CommonTime UTC2TDB(const CommonTime& UTC);


      ///------ Methods to compute eop corrections for tide effects ------//

         /// Evaluate the effects of zonal earth tides on the rotation of the
         /// earth, see IERS Conventions 2010.
      Vector<double> RG_ZONT2(const CommonTime& TT);


         /// Provide the diurnal/subdiurnal tidal effects on polar motions ("),
         /// UT1 (s) and LOD (s), in time domain.
      Vector<double> PMUT1_OCEANS(const CommonTime& UTC);


         /// Provide the diurnal lunisolar effect on polar motion ("), in time
         /// domain.
      Vector<double> PMSDNUT2(const CommonTime& UTC);


         /// Evaluate the model of subdiurnal libration in the axial component
         /// of rotation, expressed by UT1 and LOD.
      Vector<double> UTLIBR(const CommonTime& UTC);



      ///------ Methods to compute reference system transformation ------//


         /// X,Y coordinates of celestial intermediate pole from series based
         /// on IAU 2006 precession and IAU 2000A nutation.
      void XY06(const CommonTime& tt, double& x, double& y);


         /// The CIO locator s, positioning the Celestial Intermediate Origin on
         /// the equator of the Celestial Intermediate Pole, given the CIP's X,Y
         /// coordinates. Compatible with IAU 2006/2000A precession-nutation.
      double S06(const CommonTime& tt, const double& x, const double& y);


         /// Form the celestial to intermediate-frame-of-date matrix given the CIP
         /// X,Y and the CIO locator s.
      Matrix<double> C2IXYS(const double& x, const double& y, const double& s);


         /// Earth rotation angle (IAU 2000 model).
      double ERA00(const CommonTime& ut1);


         /// The TIO locator s', positioning the Terrestrial Intermediate Origin
         /// on the equator of the Celestial Intermediate Pole.
      double SP00(const CommonTime& tt);


         /// Form the matrix of polar motion for a given date, IAU 2000.
      Matrix<double> POM00(const double& xp, const double& yp, const double& sp);


         /// Transformation matrix from CRS to TRS coordinates for a given date
      Matrix<double> C2TMatrix(const CommonTime& UTC);

   
         /// Transformation matrix form TRS to CRS coordinates for a given date
      Matrix<double> T2CMatrix(const CommonTime& UTC);


         /// Earth rotation angle first order rate
      double dERA00(const CommonTime& UT1);

         /// Time derivative of transformation matrix from CRS to TRS coordinates
         /// for a given date
      Matrix<double> dC2TMatrix(const CommonTime& UTC);

   
         /// Time derivative of transformation matrix from TRS to CRS coordinates
         /// for a given date
      Matrix<double> dT2CMatrix(const CommonTime& UTC);


         /// Greenwich mean sidereal time (consistent with IAU 2006 precession)
      double GMST06(const CommonTime& UT1, const CommonTime& TT);


         /// Compute doodson's fundamental arguments (BETA)
         /// and fundamental arguments for nutation (FNUT)
      void DoodsonArguments(const  CommonTime& UT1,
                            const  CommonTime& TT,
                            double BETA[6],
                            double FNUT[5]         );


        /**Normalize angle into the range -PI <= a <= +PI.
         *
         * @param  a    Angle (radians)
         * @return      Angle in range +/-PI
         */
      double Anpm(double a)
      {
          double w = std::fmod(a, TWO_PI);
          if(fabs(w) >= PI)
          {
              if(a > 0.0)   w -=  TWO_PI;
              else          w -= -TWO_PI;
          }

          return w;
      }


        /**Normalize angle into the range 0 <= a <= 2PI.
         *
         * @param  a    Angle (radians)
         * @return      Angle in range 0-2PI
         */
      double Anp(double a)
      {
          double w = std::fmod(a, TWO_PI);

          if(w < 0.0)
          {
              w += TWO_PI;
          }

          return w;
      }

         // Deconstrutor
      virtual ~ReferenceSystem() {};


   private:

         /// Pointer to the EOPDataStore
      EOPDataStore* pEopStore;


         /// Pointer to the leap second store 
      LeapSecStore* pLeapSecStore;


         /// whether the transformation matrix is prepared
      bool isPrepared;


   }; // End of class 'ReferenceSystem'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_REFERENCE_SYSTEM_HPP
