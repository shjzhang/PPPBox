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


/**
 * @file SSCStream.hpp
 * Read/Write the set of station coordinate file data
 */

#ifndef SSCSTREAM_INCLUDE
#define SSCSTREAM_INCLUDE

#include "FFTextStream.hpp"
#include "SSCHeader.hpp"

namespace gpstk
{
      /** @addtogroup MSC */
      //@{

      /// Stream used to obtain data from a Set of Station Coordinates File
   class SSCStream : public gpstk::FFTextStream
   {
   public:
         /// Default constructor
      SSCStream()
			: line(std::string())
				/// if we need to write a file
				// wroteENDSNX(false),
		  	 	// writingMode(false),
			  
		{}

         /**
          * Common Constructor : open (default: read)
          * @param filename : the file to open
          * @param mode the ios::openmode to be used in opening a file 
          */
      SSCStream(const char* filename, std::ios::openmode mode=std::ios::in)
				: FFTextStream(filename, mode)
		{ open(filename, mode); }
      
         /// Destructor
      virtual ~SSCStream() {}
		
		virtual void open(const char* filename, std::ios::openmode mode) 
		{
			FFTextStream::open(filename, mode);
			header = SSCHeader();
		}
		
			///@name data members
			//@{
		SSCHeader header;			///< SSCHeader for this file
		std::string line;			///< line read
			//@}

   }; // class SSCStream

} // namespace gpstk

#endif   
