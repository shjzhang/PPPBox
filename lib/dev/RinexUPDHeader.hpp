#pragma ident "$Id: RinexUPDHeader.hpp 2897 2012-08-10 20:08:46Z shjzhang $"

/**
 * @file RinexUPDHeader.hpp
 * Encapsulate header of satellite upd file data, including I/O
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#ifndef GPSTK_RINEX_UPD_HEADER_HPP
#define GPSTK_RINEX_UPD_HEADER_HPP

#include <string>
#include <vector>
#include <map>
#include "RinexUPDBase.hpp"
#include "RinexSatID.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
   /** @addtogroup RinexUPD */
   //@{

      /** This class models the header for a satellite upd file.
       *
       *  The format of satellite upd file is similar with that of
       *  rinex clock file.
       *
       * @sa gpstk::RinexUPDStream and gpstk::RinexUPDData for more information.
       */
   class RinexUPDHeader : public RinexUPDBase
   {

   public:

         /// constructor
      RinexUPDHeader() 
          : version(3.0), leapSeconds(0), timeSystem(TimeSystem::Any),
            pcvsSystem(1, RinexSatID::systemGPS),
            numSolnStations(0), numSolnSatellites(0)
      {}

         /**
          * @name RinexUPDHeaderFormatStrings
          * Satellite upd Header Formatting Strings
          */

         //@{
      static const std::string versionString;         ///< "RINEX VERSION / TYPE"
      static const std::string runByString;           ///< "PGM / RUN BY / DATE"
      static const std::string commentString;         ///< "COMMENT"
      static const std::string sysString;             ///< "SYS / # / OBS TYPES"
      static const std::string timeSystemString;      ///< "TIME SYSTEM ID"
      static const std::string leapSecondsString;     ///< "LEAP SECONDS"
      static const std::string sysDCBString;          ///< "SYS / DCBS APPLIED"
      static const std::string sysPCVString;          ///< "SYS / PCVS APPLIED"
      static const std::string numDataString;         ///< "# / TYPES OF DATA"
      static const std::string stationNameString;     ///< "STATION NAME / NUM"
      static const std::string stationClockRefString; ///< "STATION CLK REF"
      static const std::string analysisCenterString;  ///< "ANALYSIS CENTER"
      static const std::string numClockRefString;     ///< "# OF CLK REF"
      static const std::string analysisClkRefrString; ///< "ANALYSIS CLK REF"
      static const std::string numReceiversString;    ///< "# OF SOLN STA / TRF"
      static const std::string solnStateString;       ///< "SOLN STA NAME / NUM"
      static const std::string numSolnSatsString;     ///< "# OF SOLN SATS"
      static const std::string prnListString;         ///< "PRN LIST"
      static const std::string endOfHeaderString;     ///< "END OF HEADER"
         //@}

         /// Validity bits for the RINEX Clock Header (** optional)
      enum validBits
      {
         versionValid            = 0x01, ///< "RINEX VERSION / TYPE"
         runByValid              = 0x02, ///< "PGM / RUN BY / DATE"
         commentValid            = 0x04, ///< "COMMENT" **
         sysValid                = 0x08, ///< "SYS / # / OBS TYPES" **

         timeSystemValid        = 0x010, ///< "TIME SYSTEM ID" **
         leapSecondsValid       = 0x020, ///< "LEAP SECONDS" **
         sysDCBValid            = 0x040, ///< "SYS / DCBS APPLIED" **
         sysPCVValid            = 0x080, ///< "SYS / PCVS APPLIED" **

         numDataValid          = 0x0100, ///< "# / TYPES OF DATA"
         stationNameValid      = 0x0200, ///< "STATION NAME / NUM" **
         stationClockRefValid  = 0x0400, ///< "STATION CLK REF" **
         analysisCenterValid   = 0x0800, ///< "ANALYSIS CENTER"

         numClockRefValid     = 0x01000, ///< "# OF CLK REF" **
         analysisClkRefrValid = 0x02000, ///< "ANALYSIS CLK REF" **
         numReceiversValid    = 0x04000, ///< "# OF SOLN STA / TRF"
         solnStateValid       = 0x08000, ///< "SOLN STA NAME / NUM"

         numSolnSatsValid    = 0x010000, ///< "# OF SOLN SATS"
         prnListValid        = 0x020000, ///< "PRN LIST"
         endOfHeaderValid    = 0x040000, ///< "END OF HEADER"

         allRequiredValid    = 0x07C903, ///< this mask if for all required fields
         allValid            = 0x07FFFF, ///< all the bits
      };

         /// destructor
      virtual ~RinexUPDHeader() {}


         // The next four lines is our common interface
         /// RinexUPDHeader is a "header" so this function always returns true.
      virtual bool isHeader() const { return true; }
     

         /** Dump information about the header to an ostream.
          *
          *  @param[in] os ostream to receive the output; defaults to std::cout
          *  @param[in] detail integer level of detail to provide; allowed values are
          *
          *   0: all the header string except stations and satellites, but their num.
          *   1: above plus all the stations and satellites
          *   2: above plus all invalid header strings (dumpValid)
          */
      virtual void dump(std::ostream& s=std::cout, short detail = 0) const throw();


         /// Dump validity bits -> header strings
      void dumpValid(std::ostream& s=std::cout) const throw();

         ///@name data members
         //@{

      double version;                              ///< Satellite UPD Version or file format
      std::string program;                         ///< Program name
      std::string runby;                           ///< Run by string
      std::vector<std::string> dataTypes;          ///< list of data types
      int leapSeconds;                             ///< Leap seconds
      TimeSystem timeSystem;                       ///< Time system

      std::string analCenterDesignator;            ///< Analysis center designator (3 char)
      std::string analysisCenter;                  ///< Analysis center
      std::string terrRefFrame;                    ///< Terr Ref Frame or SINEX solution
      RinexSatID pcvsSystem;                       ///< system (G=GPS, R=GLO) for PCVs
      std::string pcvsProgram;                     ///< program used to apply PCVs
      std::string pcvsSource;                      ///< source of applied PCVs

      int numSolnStations;                         ///< Number of stations in the solution
      std::map<std::string,std::string> stationID; ///< 4-char name, station id

         // These coordinates are often more than 
         // 32 bits -- cannot store as number!
      std::map<std::string,std::string> stationX;  ///< name, station X coord in mm
      std::map<std::string,std::string> stationY;  ///< name, station Y coord in mm
      std::map<std::string,std::string> stationZ;  ///< name, station Z coord in mm

      int numSolnSatellites;                       ///< Number of satellites in the soln
      std::vector<RinexSatID> satList;             ///< List of sats (PRN LIST)

      std::vector<std::string> commentList;        ///< comments

      unsigned long valid;                         ///< valid bits for this header

         //@}

   protected:

         /// clear out the member data
      void clear(void) 
      {

         version = 3.0;
         program = std::string();
         runby = std::string();
         dataTypes.clear();
         leapSeconds = 0;
         analysisCenter = std::string();
         terrRefFrame = std::string();
         timeSystem = TimeSystem::Any;
         pcvsSystem = RinexSatID(-1, RinexSatID::systemGPS);
         pcvsProgram = std::string();
         pcvsSource = std::string();
         numSolnStations = 0;
         stationID.clear();
         stationX.clear();
         stationY.clear();
         stationZ.clear();
         numSolnSatellites = 0;
         satList.clear();
         commentList.clear();

         valid = 0;
      }

         /// Writes the record formatted to the FFStream \a s.
         /// @throws StringException when a StringUtils function fails
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               StringUtils::StringException);

         /** This function retrieves the satellite upd file header from 
          *  the given FFStream. If an error is encountered in the retrieval 
          *  of the header, the stream is reset to its original position and 
          *  its fail-bit is set.
          * 
          *  @throws StringException    
          *     when a StringUtils function fails
          *  @throws FFStreamError      
          *     when exceptions(failbit) is set and
          *     a read or formatting error occurs.  This also resets the
          *     stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               StringUtils::StringException);

   }; // end class RinexUPDHeader

   //@}

}  // namespace

#endif // GPSTK_RINEX_UPD_HEADER_HPP

