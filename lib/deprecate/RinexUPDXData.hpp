#pragma ident "$Id: RinexUPDXData.hpp 2897 2011-09-14 20:08:46Z shjzhang $"

/**
 * @file RinexUPDXData.hpp
 * Encapsulate satellite upd data, including I/O
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
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#ifndef GPSTK_RINEX_UPDX_DATA_INCLUDE
#define GPSTK_RINEX_UPDX_DATA_INCLUDE

#include "RinexSatID.hpp"
#include "RinexUPDXBase.hpp"
#include "CommonTime.hpp"
#include <iomanip>

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

      /**
       * This class encapsulates data for satellite udp with format similar
       * with the format of the RINEX Clock file, and is used in conjuction with 
       * class RinexUPDXStream, which handles the I/O, and RinexUPDXHeader, which 
       * holds information from the satellite upd file header.
       *
       * @code
       *
       *    RinexUPDXStream ss("igs14080.clk_30s");
       *    RinexUPDXHeader sh;
       *    RinexUPDXData sd;
       *
       *    ss >> sh;
       *
       *    while (ss >> sd)
       *    {
       *       // Interesting stuff...
       *    }    
       *
       *    RinexUPDXStream ssout("myfile.clk_30s", ios::out);
       *    ssout << sh;
       *    for(...) 
       *    {
       *          // perhaps modify sd
       *       ssout << sd
       *    }
       *
       * @endcode
       *
       * @sa gpstk::RinexUPDXHeader and gpstk::RinexUPDXStream for more information.
       */
   class RinexUPDXData : public RinexUPDXBase
   {
   public:

         /// Constructor.
      RinexUPDXData() 
      { clear(); }
     
         /// Destructor
      virtual ~RinexUPDXData() 
      {}
     
         /// The next four lines is our common interface
         /// RinexUPDXData is "data" so this function always returns true.
      virtual bool isData() const 
      { return true;}

         /// Debug output function.
      virtual void dump(std::ostream& s=std::cout) const throw();

         ///@name data members
         //@{
        
      std::string datatype;   ///< Data type : AR, AS, etc
      RinexSatID sat;         ///< Satellite ID        (if AS)
      std::string site;       ///< Site label (4-char) (if AR)
      CommonTime time;        ///< Time of epoch for this record
      double updSatWL;          ///< Ldelta upd sigma in meters 
      double updSatLC;          ///< Ldelta upd sigma in meters 
      double updSatL1;          ///< L1 upd in meters 
      double updSatL2;          ///< L2 upd sigma in meters 

         //@}
      
   protected:


      void clear(void) throw()
      {
         datatype = std::string();
         sat = RinexSatID(-1,RinexSatID::systemGPS);
         time = CommonTime::BEGINNING_OF_TIME;
         updSatWL=updSatLC=updSatL1 = updSatL2 = 0.0;
      }

         /** Writes the formatted record to the FFStream \a s.
          *  @warning 
          *     This function is currently unimplemented
          */
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function reads a record from the given FFStream.
          * If an error is encountered in retrieving the record, the 
          * stream is reset to its original position and its fail-bit is set.
          *
          * @throws StringException 
          *    when a StringUtils function fails
          * @throws FFStreamError 
          *    when exceptions(failbit) is set and
          *    a read or formatting error occurs.  This also resets the
          *    stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);
   };

   //@}

}  // namespace

#endif // GPSTK_RINEX_UPDX_DATA_INCLUDE
