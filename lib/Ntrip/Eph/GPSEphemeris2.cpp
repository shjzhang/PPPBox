/// @file GPSEphemeris2.cpp Encapsulates the GPS legacy broadcast ephemeris and clock.
/// Inherits OrbitEph2, which does most of the work; this class adds health and
/// accuracy information, fit interval, ionospheric correction terms and data
/// flags.

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
//  Copyright 2004, The University of Texas at Austin
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

#include <string>
#include "Exception.hpp"
#include "SVNumXRef.hpp"
#include "GPSWeekSecond.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

#include "GPSEphemeris2.hpp"
#include "GPSEllipsoid.hpp"
#include "ReferenceFrame.hpp"

using namespace std;

namespace gpstk
{
   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph2 object.
   // @throw Invalid Request if the required data has not been stored.
   bool GPSEphemeris2::isValid(const CommonTime& ct) const
   {
      try {
         if(ct >= beginValid && ct <= endValid) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // This function returns the health status of the SV.
   bool GPSEphemeris2::isHealthy(void) const
   {
      try {
         OrbitEph2::isHealthy();     // ignore the return value; for dataLoaded check
         if(health == 0) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Compute satellite position at the given time.
   // throw Invalid Request if the required data has not been stored.
   bool GPSEphemeris2::svXvt(const CommonTime& t, Xvt& sv) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      if(checkState == bad || A == 0.0)
      {
          return false;
      }
      double ea;              // eccentric anomaly
      double delea;           // delta eccentric anomaly during iteration
      double elapte;          // elapsed time since Toe
      //double elaptc;          // elapsed time since Toc
      //double dtc,dtr;
      double q,sinea,cosea;
      double GSTA,GCTA;
      double amm;
      double meana;           // mean anomaly
      double F,G;             // temporary real variables
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double xef,yef,zef,dek,dlk,div,domk,duv,drv;
      double dxp,dyp,vxef,vyef,vzef;

      GPSEllipsoid ell;
      double sqrtgm = SQRT(ell.gm());
      double twoPI = 2.0e0 * PI;
      double lecc;            // eccentricity
      double tdrinc;          // dt inclination
      double Ahalf = SQRT(A); // A is semi-major axis of orbit
      double ToeSOW = GPSWeekSecond(ctToe).sow;    // SOW is time-system-independent

      lecc = ecc;
      tdrinc = idot;

      // Compute time since ephemeris & clock epochs
      elapte = t - ctToe;

      // Compute A at time of interest (LNAV: Adot==0)
      double Ak = A + Adot * elapte;

      // Compute mean motion (LNAV: dndot==0)
      double dnA = dn + 0.5*dndot*elapte;
      amm  = (sqrtgm / (A*Ahalf)) + dnA;     // Eqn specifies A0, not Ak

      // In-plane angles
      //     meana - Mean anomaly
      //     ea    - Eccentric anomaly
      //     truea - True anomaly
      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      ea = meana + lecc * ::sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - (ea - lecc * ::sin(ea));
         G = 1.0 - lecc * ::cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ((fabs(delea) > 1.0e-11) && (loop_cnt <= 20));

      // Compute clock corrections
      sv.relcorr = svRelativity(t);
      sv.clkbias = svClockBias(t);
      sv.clkdrift = svClockDrift(t);
      sv.frame = ReferenceFrame::WGS84;

      // Compute true anomaly
      q     = SQRT(1.0e0 - lecc*lecc);
      sinea = ::sin(ea);
      cosea = ::cos(ea);
      G     = 1.0e0 - lecc * cosea;

      //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

      //  True anomaly
      truea = atan2 (GSTA, GCTA);

      // Argument of lat and correction terms (2nd harmonic)
      alat  = truea + w;
      talat = 2.0e0 * alat;
      c2al  = ::cos(talat);
      s2al  = ::sin(talat);

      du  = c2al * Cuc +  s2al * Cus;
      dr  = c2al * Crc +  s2al * Crs;
      di  = c2al * Cic +  s2al * Cis;

      // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = Ak*G  + dr;
      AINC = i0 + tdrinc * elapte  +  di;

      //  Longitude of ascending node (ANLON)
      ANLON = OMEGA0 + (OMEGAdot - ell.angVelocity()) *
              elapte - ell.angVelocity() * ToeSOW;

      // In plane location
      cosu = ::cos(U);
      sinu = ::sin(U);
      xip  = R * cosu;
      yip  = R * sinu;

      //  Angles for rotation to earth fixed
      can  = ::cos(ANLON);
      san  = ::sin(ANLON);
      cinc = ::cos(AINC);
      sinc = ::sin(AINC);

      // Earth fixed coordinates in meters
      xef  =  xip*can  -  yip*cinc*san;
      yef  =  xip*san  +  yip*cinc*can;
      zef  =              yip*sinc;
      sv.x[0] = xef;
      sv.x[1] = yef;
      sv.x[2] = zef;

      // Compute velocity of rotation coordinates
      dek = amm * Ak / R;
      dlk = Ahalf * q * sqrtgm / (R*R);
      div = tdrinc - 2.0e0 * dlk * (Cic  * s2al - Cis * c2al);
      domk = OMEGAdot - ell.angVelocity();
      duv = dlk*(1.e0+ 2.e0 * (Cus*c2al - Cuc*s2al));
      drv = Ak * lecc * dek * sinea - 2.e0 * dlk * (Crc * s2al - Crs * c2al);
      dxp = drv*cosu - R*sinu*duv;
      dyp = drv*sinu + R*cosu*duv;

      // Calculate velocities
      vxef = dxp*can - xip*san*domk - dyp*cinc*san
               + yip*(sinc*san*div - cinc*can*domk);
      vyef = dxp*san + xip*can*domk + dyp*cinc*can
               - yip*(sinc*can*div + cinc*san*domk);
      vzef = dyp*sinc + yip*cinc*div;

      // Move results into output variables
      sv.v[0] = vxef;
      sv.v[1] = vyef;
      sv.v[2] = vzef;

      // Add the Relativistic Correction
      sv.clkbias += sv.computeRelativityCorrection();

      return true;
   }

   // adjustBeginningValidity determines the beginValid and endValid times.
   // @throw Invalid Request if the required data has not been stored.
   void GPSEphemeris2::adjustValidity(void)
   {
      try {
         OrbitEph2::adjustValidity();   // for dataLoaded check

	      // Beginning of Validity
               // New concept.  Admit the following.
	      //  (a.) The collection system may not capture the data at earliest transmit.
	      //  (b.) The collection system may not capture the three SFs consecutively.
	      // Consider a couple of IS-GPS-200 promises,
	      //  (c.) By definition, beginning of validity == beginning of transmission.
	      //  (d.) Except for uploads, cutovers will only happen on hour boundaries
	      //  (e.) Cutovers can be detected by non-even Toc.
	      //  (f.) Even uploads will cutover on a frame (30s) boundary.
               // Therefore,
	      //   1.) If Toc is NOT even two hour interval, pick lowest HOW time,
	      //   round back to even 30s.  That's the earliest Xmit time we can prove.
	      //   NOTE: For the case where this is the SECOND SF 1/2/3 after an upload,
	      //   this may yield a later time as such a SF 1/2/3 will be on a even
	      //   hour boundary.  Unfortunately, we have no way of knowing whether
	      //   this item is first or second after upload without additional information
	      //   2.) If Toc IS even two hour interval, pick time from SF 1,
	      //   round back to nearest EVEN two hour boundary.  This assumes collection
	      //   SOMETIME in first hour of transmission.  Could be more
	      //   complete by looking at fit interval and IODC to more accurately
	      //   determine earliest transmission time.
         long longToc = static_cast<GPSWeekSecond>(ctToc).getSOW();
         long XmitWeek = static_cast<GPSWeekSecond>(transmitTime).getWeek();
         double XmitSOW = 0.0;
         if ( (longToc % 7200) != 0)     // NOT an even two hour change
         {
            long Xmit = HOWtime - (HOWtime % 30);
	         XmitSOW = (double) Xmit;
         }
         else
         {
            long Xmit = HOWtime - HOWtime % 7200;
	         XmitSOW = (double) Xmit;
         }
         beginValid = GPSWeekSecond( XmitWeek, XmitSOW, TimeSystem::GPS );

	      // End of Validity.
	      // The end of validity is calculated from the fit interval
	      // and the Toe.  The fit interval is either trivial
	      // (if fit interval flag==0, fit interval is 4 hours)
	      // or a look-up table based on the IODC.
	      // Round the Toe value to the hour to elminate confusion
	      // due to possible "small offsets" indicating uploads
         long epochWeek = static_cast<GPSWeekSecond>(ctToe).getWeek();
         double Toe = static_cast<GPSWeekSecond>(ctToe).getSOW();
         long ToeOffset = (long) Toe % 3600;
         double adjToe = Toe;                  // Default case
         if (ToeOffset)
         {
            adjToe += 3600.0 - (double)ToeOffset;  // If offset, then adjust to remove
         }
         long endFitSOW = adjToe + (fitDuration/2)*3600;
         short endFitWk = epochWeek;
         if (endFitSOW >= FULLWEEK)
         {
            endFitSOW -= FULLWEEK;
            endFitWk++;
         }
         endValid = GPSWeekSecond(endFitWk, endFitSOW, TimeSystem::GPS);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }
      
   // Dump the overhead information as a string containing a single line.
   // @throw Invalid Request if the required data has not been stored.
   string GPSEphemeris2::asString(void) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));
      try {
         ostringstream os;
         CivilTime ct;
         os << "EPH G" << setfill('0') << setw(2) << satID.id << setfill(' ');
         ct = CivilTime(beginValid);
         os << printTime(ct," | %4Y %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToe);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToc);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(endValid);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(transmitTime);
         os << printTime(ct," %3j %02H:%02M:%02S | ");
         os << setw(3) << IODE << " | " << setw(3) << IODC << " | " << health;
         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   // Dump the overhead information to the given output stream.
   // @throw Invalid Request if the required data has not been stored.
   void GPSEphemeris2::dumpHeader(std::ostream& os) const
   {
      try {
         // copy from OrbitEph2::dumpHeader() ...
         if(!dataLoadedFlag)
            GPSTK_THROW(InvalidRequest("Data not loaded"));

         os << "****************************************************************"
            << "************" << endl
            << "Broadcast Orbit Ephemeris of class " << getName() << endl;
         os << "Satellite: " << SatID::convertSatelliteSystemToString(satID.system)
            << " " << setfill('0') << setw(2) << satID.id << setfill(' ');

         // ... and add this for GPS
         os << " SVN ";
         SVNumXRef svNumXRef; 
         try {
            os << svNumXRef.getNAVSTAR(satID.id, ctToe );
         }
         catch(NoNAVSTARNumberFound) {
            os << "Unknown";
         }
         os << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Dump the orbit, etc information to the given output stream.
   // @throw Invalid Request if the required data has not been stored.
   void GPSEphemeris2::dumpBody(std::ostream& os) const
   {
      try {
         OrbitEph2::dumpBody(os);

         os << "           GPS-SPECIFIC PARAMETERS\n"
            << scientific << setprecision(8)
            << "Tgd (L1/L2) : " << setw(16) << Tgd << " meters" << endl
            << "HOW time    : " << setw(6) << HOWtime << " (sec of GPS week "
               << setw(4) << static_cast<GPSWeekSecond>(ctToe).getWeek() << ")"
            << "   fitDuration: " << setw(2) << fitDuration << " hours" << endl
            << "TransmitTime: " << OrbitEph2::timeDisplay(transmitTime) << endl
            << "Accuracy    : flag(URA): " << accuracyFlag << " => "
            << fixed << setprecision(2) << getAccuracy() << " meters" << endl
            << "IODC: " << IODC << "   IODE: " << IODE << "   health: " << health
            << " (0=good)   codeflags: " << codeflags << "   L2Pdata: " << L2Pdata
            << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Get the fit interval in hours from the fit interval flag and the IODC
   short GPSEphemeris2::getFitInterval(const short IODC, const short fitIntFlag)
   {
      // TD This is for Block II/IIA, need to update for Block IIR and IIF

      if(IODC < 0 || IODC > 1023) // error in IODC, return minimum fit
         return 4;

      if((fitIntFlag == 0 && (IODC & 0xFF) < 240) || (IODC & 0xFF) > 255)
         return 4;

      else if(fitIntFlag == 1) {

         if(((IODC & 0xFF) < 240 || (IODC & 0xFF) > 255))
            return 6;
         else if(IODC >=240 && IODC <=247)
            return 8;
         else if((IODC >= 248 && IODC <= 255) || IODC == 496)
            return 14;

         // Revised in IS-GPS-200 Revision D for Block IIR/IIR-M/IIF
         else if((IODC >= 497 && IODC <=503) || (IODC >= 1021 && IODC <= 1023))
            return 26;
         else if(IODC >= 504 && IODC <=510)
            return 50;
         else if(IODC == 511 || (IODC >= 752 && IODC <= 756))
            return 74;

         // NOTE:
         // The following represents old fit intervals for Block II (not IIA)
         // and is present only in old versions of the ICD-GPS-200 Rev. C.
         // Please do not remove them as there are still people that may
         // want to process old Block II data and none of the IODC intervals
         // overlap (so far) so there is no need to remove them.
         else if(IODC >= 757 && IODC <= 763)
            return 98;
         else if((IODC >= 764 && IODC <=767) || (IODC >=1008 && IODC <=1010))
            return 122;
         else if(IODC >= 1011 && IODC <=1020)
            return 146;
         else              // error in the IODC or ephemeris, return minimum fit
            return 4;
      }
      else                 // error in ephemeris/IODC, return minimum fit
         return 4;

      return 0; // never reached
   }

} // end namespace
