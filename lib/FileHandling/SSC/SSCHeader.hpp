//#pragma ident "$Id$"

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

/**
 * @file SSCHeader.hpp
 * Encapsulate SSC header, including I/O
 */

#ifndef GPSTK_SSCHEADER_HPP
#define GPSTK_SSCHEADER_HPP

#include <vector>
#include <list>
#include <map>
#include <string>

#include "FFStream.hpp"
#include "SSCBase.hpp"
#include "StringUtils.hpp"
#include "YDSTime.hpp"



namespace gpstk
{
   /** @addtogroup SSC */
   //@{

   /** 
    * This class does not really do anything.  It is here to conform to the
    * other file types, even though the SSC file type 
    * does not have any header information.
    *
    * @sa tests/SSC for examples
    * @sa SSCStream.
    * @sa SSCData for more information on reading SSC files.
    */
   class SSCHeader : public SSCBase
   {
	
   public:
      /// Default Constructor.
      SSCHeader() : releaseTime(0,0,0), beginningTime(0,0,0), 
		endTime(0,0,0) 
		{}

      /// Destructor
      virtual ~SSCHeader() {}
      
		/*** Modified at 2015/10/28 14:35 ***/
      //virtual void dump(std::ostream& s) const {};
      
		/*** Modified at 2015/10/28 14:35 ***/
      //! This class is a "header" so this function always returns "true". 
      //virtual bool isHeader() const {return true;}
		
		
		YDSTime releaseTime, beginningTime, endTime;
		
		friend class SSCData;

	
   protected:      
      virtual void reallyPutRecord(FFStream& s) const 
      		throw(std::exception, FFStreamError, 
      		   	gpstk::StringUtils::StringException);
     	
  
      virtual void reallyGetRecord(FFStream& ffs)  
         throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException);
      
      
   }; // class SSCHeader

   //@}

} // namespace

#endif
