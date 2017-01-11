/// @file OrbElem.cpp Encapsulates the "least common denominator" orbit parameters
/// that determine a satellite ephemeris, that is, clock model, Kepler orbit elements
/// plus harmonic perturbations with time of ephemeris, satellite ID, and begin and
/// end times of validity.
/// Although it can also be used alone, this class is most often to be used as a base
/// class for a fuller implementation of the ephemeris and clock, by adding health
/// and accuracy information, fit interval, ionospheric correction terms and data
/// flags. It serves as the base class for broadcast ephemerides for GPS, QZSS,
/// Galileo, and BeiDou, with RINEX Navigation input, among others.

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

#include "OrbitEph2.hpp"

#include "MathBase.hpp"
#include "GNSSconstants.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GALWeekSecond.hpp"
#include "BDSWeekSecond.hpp"
#include "QZSWeekSecond.hpp"
#include "GPSEllipsoid.hpp"
#include "TimeString.hpp"
#include "Matrix.hpp"

using namespace std;

namespace gpstk {

   // Destructor
   OrbitEph2::~OrbitEph2()
   {
      if (orbCorr)
        delete orbCorr;
      if (clkCorr)
        delete clkCorr;
   }

   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph2 object.
   // throw Invalid Request if the required data has not been stored.
   bool OrbitEph2::isValid(const CommonTime& ct) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));
      if(ct < beginValid || ct > endValid) return false;
      return true;
   }

   // Compute the satellite clock bias (seconds) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph2::svClockBias(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      double dtc, elaptc;
      elaptc = t - ctToc;
      dtc = af0 + elaptc * (af1 + elaptc * af2);
      return dtc;
   }

   // Compute the satellite clock drift (sec/sec) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph2::svClockDrift(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      double drift, elaptc;
      elaptc = t - ctToc;
      drift = af1 + elaptc * af2;
      return drift;
   }



   // Compute satellite relativity correction (sec) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph2::svRelativity(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      GPSEllipsoid ell;
      double twoPI  = 2.0 * PI;
      double sqrtgm = SQRT(ell.gm());
      double elapte = t - ctToe;

      // Compute A at time of interest
      double Ak = A + Adot*elapte;                 // LNAV: Adot==0
      //double dnA = dn + 0.5*dndot*elapte;          // LNAV: dndot==0
      double Ahalf = SQRT(A);
      double amm = (sqrtgm / (A*Ahalf)) + dn;      // Eqn specifies A0 not Ak
      double meana,F,G,delea;

      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      double ea = meana + ecc * ::sin(meana);

      int loop_cnt = 1;
      do {
         F     = meana - (ea - ecc * ::sin(ea));
         G     = 1.0 - ecc * ::cos(ea);
         delea = F/G;
         ea    = ea + delea;
         loop_cnt++;
      } while ((ABS(delea) > 1.0e-11) && (loop_cnt <= 20));

      return (REL_CONST * ecc * SQRT(Ak) * ::sin(ea));
   }

   // Dump the overhead information as a string containing a single line.
   // @throw Invalid Request if the required data has not been stored.
   string OrbitEph2::asString(void) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      try {
         ostringstream os;
         string sys;
         switch(satID.system) {
            case SatID::systemGPS: sys = "G"; break;
            case SatID::systemGalileo: sys = "E"; break;
            case SatID::systemBeiDou: sys = "C"; break;
            case SatID::systemQZSS: sys = "J"; break;
            default:
               os << "EPH Error - invalid satellite system "
                  << SatID::convertSatelliteSystemToString(satID.system) << endl;
               return os.str();
         }

         CivilTime ct;
         os << "EPH " << sys << setfill('0') << setw(2) << satID.id << setfill(' ');
         ct = CivilTime(beginValid);
         os << printTime(ct," | %4Y %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToe);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToc);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(endValid);
         os << printTime(ct," %3j %02H:%02M:%02S |");

         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   // Utility routine for dump(); override if GPSWeekSecond is not right
   string OrbitEph2::timeDisplay(const CommonTime& t, bool showHead) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      try {
         if(showHead) return string("Week( mod)     SOW     DOW   UTD     SOD"
                                    "   MM/DD/YYYY   HH:MM:SS SYS");

         ostringstream os;
         WeekSecond *ptr;
         if(     t.getTimeSystem() == TimeSystem::GAL)
            ptr = new GALWeekSecond(t);
         else if(t.getTimeSystem() == TimeSystem::BDT)
            ptr = new BDSWeekSecond(t);
         else if(t.getTimeSystem() == TimeSystem::QZS)
            ptr = new QZSWeekSecond(t);
         else 
            ptr = new GPSWeekSecond(t);

         os << setw(4) << ptr->week << "(";
         os << setw(4) << (ptr->week & ptr->bitmask()) << ")  ";
         os << setw(6) << setfill(' ') << ptr->sow << "   ";

         switch (ptr->getDayOfWeek())
         {
            case 0: os << "Sun-0"; break;
            case 1: os << "Mon-1"; break;
            case 2: os << "Tue-2"; break;
            case 3: os << "Wed-3"; break;
            case 4: os << "Thu-4"; break;
            case 5: os << "Fri-5"; break;
            case 6: os << "Sat-6"; break;
            default: break;
         }

         os << printTime(t,"   %3j   %5.0s   %02m/%02d/%04Y   %02H:%02M:%02S %P");

         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   // Dump the overhead information to the given output stream.
   // throw Invalid Request if the required data has not been stored.
   void OrbitEph2::dumpHeader(ostream& os) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      os << "****************************************************************"
        << "************" << endl
        << "Broadcast Orbit Ephemeris of class " << getName() << endl;
      os << "Satellite: " << SatID::convertSatelliteSystemToString(satID.system)
         << " " << setfill('0') << setw(2) << satID.id << setfill(' ') << endl;
   }

   // Dump the orbit, etc information to the given output stream.
   // throw Invalid Request if the required data has not been stored.
   void OrbitEph2::dumpBody(ostream& os) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      os << "           TIMES OF INTEREST" << endl;
      os << "              " << timeDisplay(beginValid,true) << endl;
      os << "Begin Valid:  " << timeDisplay(beginValid) << endl;
      os << "Clock Epoch:  " << timeDisplay(ctToc) << endl;
      os << "Eph Epoch:    " << timeDisplay(ctToe) << endl;
      os << "End Valid:    " << timeDisplay(endValid) << endl;

      os << scientific << setprecision(8)
         << "           CLOCK PARAMETERS\n"
         << "Bias T0:     " << setw(16) << af0 << " sec" << endl
         << "Drift:       " << setw(16) << af1 << " sec/sec" << endl
         << "Drift rate:  " << setw(16) << af2 << " sec/(sec**2)" << endl;

      os << "           ORBIT PARAMETERS\n"
         << "Semi-major axis:       " << setw(16) <<  A  << " m" << endl
         << "Motion correction:     " << setw(16) <<  dn << " rad/sec" << endl
         << "Eccentricity:          " << setw(16) << ecc << endl
         << "Arg of perigee:        " << setw(16) << w << " rad" << endl
         << "Mean anomaly at epoch: " << setw(16) << M0 << " rad" << endl
         << "Right ascension:       " << setw(16) << OMEGA0 << " rad    "
         << setw(16) << OMEGAdot << " rad/sec" << endl
         << "Inclination:           " << setw(16) << i0 << " rad    "
         << setw(16) << idot << " rad/sec" << endl;

      os << "           HARMONIC CORRECTIONS\n"
         << "Radial        Sine: " << setw(16) << Crs << " m    Cosine: "
         << setw(16) << Crc << " m" << endl
         << "Inclination   Sine: " << setw(16) << Cis << " rad  Cosine: "
         << setw(16) << Cic << " rad" << endl
         << "In-track      Sine: " << setw(16) << Cus << " rad  Cosine: "
         << setw(16) << Cuc << " rad" << endl;
   }

/*
   // Define this OrbitEph2 by converting the given RINEX navigation data.
   // NB this will be both overridden and called by the derived classes
   // NB currently has fixes for MGEX data.
   // @param rnd Rinex3NavData
   // @return true if OrbitEph2 was defined, false otherwise
   bool OrbitEph2::load(const Rinex3NavData& rnd)
   {
      try {
         // Glonass and Geosync do not have a orbit-based ephemeris
         if(rnd.satSys == "R" || rnd.satSys == "S") return false;

         // first get times and TimeSytem
         CommonTime gpstoe = rnd.time;
         unsigned int year = static_cast<CivilTime>(gpstoe).year;

         // Get week for clock, to build Toc
         double dt = rnd.Toc - rnd.HOWtime;
         int week = rnd.weeknum;
         if(dt < -HALFWEEK) week++;
         else if(dt > HALFWEEK) week--;
      
         //MGEX NB MGEX data has GPS week numbers in all systems except BeiDou,
         //MGEX so must implement temporary fixes: use GPS Toc for GAL and QZSS
         CommonTime gpstoc = GPSWeekSecond(week, rnd.Toc, TimeSystem::GPS);   //MGEX
         //cout << "gpstoc is " << printTime(gpstoc,"%Y/%m/%d %H:%M:%S %P")
         //<< " year " << year << " week " << week << " rnd.Toc " << rnd.Toc <<endl;

         // based on satellite ID, define Toc with TimeSystem
         if(rnd.satSys == "G") {
            ctToc = GPSWeekSecond(week, rnd.Toc, TimeSystem::GPS);
            ctToc.setTimeSystem(TimeSystem::GPS);
         }
         else if(rnd.satSys == "E") {
            //MGEX GALWeekSecond galws(week, rnd.Toc, TimeSystem::GAL);
            //MGEX galws.adjustToYear(year);
            //MGEX ctToc = CommonTime(galws);
            ctToc = gpstoc;        //MGEX
            ctToc.setTimeSystem(TimeSystem::GAL);
         }
         else if(rnd.satSys == "C") {
            BDSWeekSecond bdsws(week, rnd.Toc, TimeSystem::BDT);
            bdsws.adjustToYear(year);
            ctToc = CommonTime(bdsws);
            ctToc.setTimeSystem(TimeSystem::BDT);
         }
         else if(rnd.satSys == "J") {
            //MGEX QZSWeekSecond qzsws(week, rnd.Toc, TimeSystem::BDT);
            //MGEX qzsws.adjustToYear(year);
            //MGEX ctToc = CommonTime(qzsws);
            ctToc = gpstoc;        //MGEX
            ctToc.setTimeSystem(TimeSystem::QZS);
         }
         else
            GPSTK_THROW(Exception("Unknown satellite system: " + rnd.satSys));

         //cout << "ctToc " << printTime(oeptr->ctToc,"%Y/%m/%d %H:%M:%S %P") << endl;

         // Overhead
         RinexSatID sat;
         sat.fromString(rnd.satSys + StringUtils::asString(rnd.PRNID));
         satID = SatID(sat);
         //obsID = ?? ObsID obsID; // Defines carrier and tracking code
         ctToe = rnd.time;

         // clock model
         af0 = rnd.af0;
         af1 = rnd.af1;
         af2 = rnd.af2;
   
         // Major orbit parameters
         M0 = rnd.M0;
         dn = rnd.dn;
         ecc = rnd.ecc;
         A = rnd.Ahalf * rnd.Ahalf;
         OMEGA0 = rnd.OMEGA0;
         i0 = rnd.i0;
         w = rnd.w;
         OMEGAdot = rnd.OMEGAdot;
         idot = rnd.idot;
         // modern nav msg
         dndot = 0.;
         Adot = 0.;
   
         // Harmonic perturbations
         Cuc = rnd.Cuc;
         Cus = rnd.Cus;
         Crc = rnd.Crc;
         Crs = rnd.Crs;
         Cic = rnd.Cic;
         Cis = rnd.Cis;
   
         dataLoadedFlag = true;
         adjustValidity();

         return true;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }
*/

   // Output object to stream
   ostream& operator<<(ostream& os, const OrbitEph2& eph)
   {
      eph.dump(os);
      return os;
   }

   // Set orbit correction
   void OrbitEph2::setOrbCorr(const t_orbCorr *orbcorr)
   {
      delete orbCorr;
      orbCorr = new t_orbCorr(*orbcorr);
   }

   // Set clock correction
   void OrbitEph2::setClkCorr(const t_clkCorr *clkcorr)
   {
      delete clkCorr;
      clkCorr = new t_clkCorr(*clkcorr);
   }

   // Judge if this ephmeris is newer than eph2
   bool OrbitEph2::isNewerThan(const OrbitEph2 *eph2)
   {
       return (this->ctToc > eph2->ctToc);
   }

   // Get satellite position at the given time
   bool OrbitEph2::getCrd(const CommonTime &t, Xvt &xvt, double& clkcorr, bool useCorr) const
   {
      if(checkState == bad)
      {
         return false;
      }

      int updateInt[16] = {1, 2, 5, 10, 15, 30,
                           60, 120, 240, 300, 600,
                           900, 1800, 3600, 7200, 10800};

      svXvt(t,xvt,clkcorr);


      if(useCorr)
      {
         if(orbCorr && clkCorr)
         {
            double dtO = t - orbCorr->_time;
            if(orbCorr->_updateInt)
            {
               dtO -= (0.5 * updateInt[orbCorr->_updateInt]);  // ??? 0.5 ???
            }
            Triple dx;
            dx[0] = orbCorr->_xr[0] + orbCorr->_dotXr[0] * dtO;
            dx[1] = orbCorr->_xr[1] + orbCorr->_dotXr[1] * dtO;
            dx[2] = orbCorr->_xr[2] + orbCorr->_dotXr[2] * dtO;

            // RSW to XYZ for satellite position correction
            Triple along = xvt.v.unitVector();
            Triple cross = xvt.x.cross(xvt.v);
            cross = cross.unitVector();
            Triple radia = along.cross(cross);

            Matrix<double> matR(3,3,0.0);
            Matrix<double> matD(3,1,0.0);
            for(int i=0; i<3; ++i)
            {
                matR(i,0) = radia[i];
                matR(i,1) = along[i];
                matR(i,2) = cross[i];

                matD(i,0) = dx[i];
            }
            Matrix<double> matDx = matR*matD;

            // Correct the satellite position
            xvt.x[0] -= matDx(0,0);
            xvt.x[1] -= matDx(1,0);
            xvt.x[2] -= matDx(2,0);

            // RSW to XYZ for satellite velocity
            cross = xvt.x.cross(xvt.v);
            cross = cross.unitVector();
            radia = along.cross(cross);
            for(int i=0; i<3; ++i)
            {
                matR(i,0) = radia[i];
                matR(i,1) = along[i];
                matR(i,2) = cross[i];

                matD(i,0) = orbCorr->_dotXr[i];
            }
            Matrix<double> matDv = matR*matD;

            // Correct the satellite velocity
            xvt.v[0] -= matDv(0,0);
            xvt.v[1] -= matDv(1,0);
            xvt.v[2] -= matDv(2,0);

            double dtC = t - clkCorr->_time;
            if (clkCorr->_updateInt)
            {
               dtC -= (0.5 * updateInt[clkCorr->_updateInt]);
            }


            clkcorr += (clkCorr->_dClk + clkCorr->_dotDClk * dtC
                       + clkCorr->_dotDotDClk * dtC * dtC) / C_MPS;
         }
      } // end of 'if(useCorr)'
   }

}  // end namespace
