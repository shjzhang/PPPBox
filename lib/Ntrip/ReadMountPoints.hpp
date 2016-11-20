#pragma ident "$ID: ReadMountPoints.hpp 2016-06-30 $"

#ifndef GPSTK_READMOUNTPOINTS_HPP
#define GPSTK_READMOUNTPOINTS_HPP

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
//  Copyright (c) 
//
//  XY.Cao, Wuhan Uniersity, 2016 
//
//============================================================================
//  
//  Deifine some operations of mountpoint map
//  add/delete/get et.
//
//============================================================================

#include "MountPoint.hpp"
#include "SourceTableReader.hpp"
#include <map>
#include <string>


using namespace gpstk;

namespace gpstk
{
	class ReadMountPoints
	{
	public:

		/**
		*	default constructor
		*/
		ReadMountPoints();
		
		/**
		*	Add a specified mountpoint to map
		*	warning: MountPointsMap will be changed!
		*	@param	mountpoint mountpoint
		*/
		void addMountPoint(MountPoint& mountpoint);

		/**
		 * add mountpointSTR from sourcetable
		 *
		 */
		void addMountPoint(const string &mountpointID,
			const NetUrl &url,
			const SourceTableReader::mountpointSTR &stream);

		/**
		*	Delete a specified mountpoint according to its ID
		*	warning: MountPointsMap will be changed!
		*	@param	mountpointID mountpoint ID
		*/
		void deleteMountPoint(const string mountpointID);
		
		/**	
		*	Get the number of all mountpoints
		*/
        int getnumber(){return numMountPoints;}

		/**
		*	retrieve the specified mountpoint
		*	@param	mountpointID	mountpoint ID
		*/
		MountPoint getMountPoint(const string mountpointID);

		
		/**
		*	get the map storing all mountpoints
		*/
		const map<string,MountPoint> getMountPointMap();

		// destructor
		~ReadMountPoints(){};

		/**
		*	return a string identifying this class
		*/
		string getClassName() const;

		/**
		*	dump all mountpoints
		*/
		void dump() const;

	private:

			/// map stores all mountpoints 
		map<string,MountPoint> MountPointsMap;

		/// number of mountpoints
		int numMountPoints;



	};	// End of class 'ReadMountPoints'

		//@}


} // End of namespace gpstk

#endif   // GPSTK_READMOUNTPOINTS_HPP
