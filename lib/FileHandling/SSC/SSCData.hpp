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
//
//============================================================================
//
// @file : SSCData.hpp
#ifndef GPSTK_SSCDATA_HPP
#define GPSTK_SSCDATA_HPP

#include "SSCBase.hpp"
#include "YDSTime.hpp"
#include "TypeID.hpp"
#include "DataStructures.hpp"
#include <iomanip>
#include <string>
#include <map>


namespace gpstk
{
	class SSCData : public SSCBase
	{
	public:
		/// Constructor
		SSCData()

		/*** Modified at 2015/10/28 16:29 ***/ 
		: station(" "), stationCoorFlag(false), antennaOffsetFlag(false), 
							antennaTypeFlag(false), containVelFlag(false)
			{}
		
		/// Destructor
	virtual ~SSCData() {}
	/*** Modified at 2015/10/28 15:38 ***/
	//virtual bool isData() const {return true;}

		///@name data members
		//@{
	std::string GPSWeek;				///< GPSWeek in this file

	//SourceID source; 					///< Station name.

	gpstk::Triple coordinates;		///< Station coordinates.

	gpstk::Triple antennaOffset;	///< Antenna offset.

	gpstk::Triple vel;				///< Station velocity

	std::string station;				///< Station ID
		
	std::string antennaType;		///< Antenna type.
	
	YDSTime RefEpoch;					///< Solution reference epoch

	bool stationCoorFlag;			///< Station coordinates flag, 
											///	'+SOLUTION/ESTIMATE' in file
	bool antennaOffsetFlag;			///< Antenna offset flag,
											///	'+SITE/ECCENTRICITY' in file
	bool antennaTypeFlag; 			///< Antenna type flag,
											///	'+SITE/ANTENNA' in file 
	bool containVelFlag;				///< if contain velocity
	//}@

	protected:

		virtual void reallyGetRecord(FFStream& ffs)
			throw(std::exception, gpstk::FFStreamError,
					gpstk::StringUtils::StringException);

	
		virtual void reallyPutRecord(FFStream& ffs) const
       throw(std::exception, gpstk::FFStreamError,
             gpstk::StringUtils::StringException);
	
	}; //	end of class SSCData

} // end of "namespace gpstk"

#endif

