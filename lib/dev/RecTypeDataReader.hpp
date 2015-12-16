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
//  Copyright (c), 2015
//  Q.Liu, Wuhan University
//============================================================================


/**
 * @file RecTypeDataReader.hpp
 * Class to read receiver type from CODE.
 */

#ifndef GPSTK_RecTypeDataReader_HPP
#define GPSTK_RecTypeDataReader_HPP

#include <string>
#include <map>
#include <set>

#include "Exception.hpp"
#include "FFTextStream.hpp"
#include "StringUtils.hpp"
#include "TypeID.hpp"

namespace gpstk
{ 
	/** @addtogroup foermattedfile */
	//@{

	/** This is a class to read receiver type file("receiver_bernese.lis") from CODE.
     * 
	 * You can download it from:
	 * ftp://ftp.unibe.ch/aiub/bcwg/cc2noncc/receiver_bernese.lis
     * 
	 * The typical way to use this class follows:
	 *
	 * @code
	 *  // Declare a receiver_type object
	 * RecTypeDataReader recTypeData("receiver_bernese.lis")
	 *
	 * std::set<std::string> recCodeSet = recTypeData.getCode("LEICA MC500");
	 *
	 * @endcode
	 *
	 * @sa RecTypeDataReader.hpp
	 */
	class RecTypeDataReader : public FFTextStream
	{
		public:

			  // Default constructor
			RecTypeDataReader()
			{};
				
			  /** Common constructor. It will always open file for read and will
			   *  load Receiver data in one pass
			   *
			   * @param fn Receiver data file to read
			   *
			   */
			RecTypeDataReader(const char* fn)
		      throw( FileMissingException )
         {
            try
            {
				   FFTextStream(fn, std::ios::in);
               if( !FFTextStream::is_open() )
               {
                  GPSTK_THROW(FileMissingException("RecType does not exist!"));
               }

               loadData(); 
            }
            catch(FileMissingException& e)
            {
               GPSTK_RETHROW(e);
            }
         }
				 
			  /** Common constructor. It will always open file for read and will
			   *  load Receiver data in one pass
			   *
			   * @param fn Receiver data file to read
			   *
			   */
			RecTypeDataReader(const std::string& fn)
		      throw( FileMissingException )
			{
            try
            {
				   FFTextStream(fn.c_str(), std::ios::in);
               if( !FFTextStream::is_open() )
               {
                  GPSTK_THROW(FileMissingException("RecType does not exist!"));
               }

               loadData(); 
            }
            catch(FileMissingException& e)
            {
               GPSTK_RETHROW(e);
            }
         };

			  /// Method to open and load receiver data file.
			virtual void open(const char* fn)
		      throw( FileMissingException );
			
			  /// Method to open and load receiver data file.
			virtual void open(const std::string& fn)
		      throw( FileMissingException );
			
			  /// Get receiver's code observable type
			  /// @param    recType    the receiver type 
			  /// @return              C1/P2 or C1/X2 or P1/P2 depend the recType
			std::set<std::string> getCode(const std::string& recType); 
      typedef std::set<std::string> codeType;      // Type of code observable
			std::map<std::string, codeType> recTypeMap;  // Receiver type data

			virtual ~RecTypeDataReader(){};
		private:
			
			int freqNum;                                 // Number of receiver frequency    
			

			/// Method to store ocean tide harmonics data in this class' data map
			virtual void loadData()
			throw( FFStreamError, gpstk::StringUtils::StringException );


	}; // End of class 'RecTypeDataReader'

	   // @}

} // End of namespace gpstk

#endif  // GPSTK_RecTypeDataReader_HPP
