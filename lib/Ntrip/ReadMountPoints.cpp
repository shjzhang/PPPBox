#pragma ident "$Id$"

/**
* @file GPSTK_READMOUNTPOINTS_CPP
* Class to opearate the map of mountpoints.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright (c)
//
//  XY.Cao, Wuhan Uniersity, 2016 
//
//============================================================================

#include "ReadMountPoints.hpp"
#include "StringUtils.hpp"
#include <map>
#include <string>

using namespace gpstk;
using namespace std;
using namespace StringUtils;

namespace gpstk
{
	// return a string identifying this class.
   string ReadMountPoints::getClassName() const
   { return "ReadMountPoints"; }

   	// constructor1: only initialize url of mountpoint
   ReadMountPoints::ReadMountPoints()
   {
	   /// erase map
	   MountPointsMap.clear();
	   numMountPoints = 0;
   }

   // added a mountpoint to the map
   void ReadMountPoints::addMountPoint(MountPoint& mountpoint)
   {
	   
	   typedef pair<string,MountPoint> str_mnt_pair;

	   pair<map<string,MountPoint >::iterator,bool> mappr;

	   string tempID = mountpoint.getMountPointID();
	   ///cout << tempID << endl;

	   // insert
	   mappr = MountPointsMap.insert(str_mnt_pair(tempID,mountpoint));
	   
	   
	   if(mappr.second)
	   {
		   cout << "added " << (mappr.first)->first << " successfully!" << endl;
		   numMountPoints++;
	   }
	   else
	   {
		   cout << (mappr.first)->first << " has already existes!" << endl;
		   /// the number of mountpoint will not be changed!
	   }	/// end if-else
	   
   }


   void ReadMountPoints::addMountPoint(const string &mountpointID,
	   const NetUrl &url,
	   const SourceTableReader::mountpointSTR &stream)
   {
	   MountPoint mntpoint;

	   mntpoint.setMountPointUrl(url);
	   mntpoint.setFormat(stream.format);
	   mntpoint.setCountry(stream.country);
	   mntpoint.setLatitude(stream.latitude);
       mntpoint.setLongitude(stream.longitude);
	   mntpoint.setNmeaFlag(stream.nmeaFlag);

	   typedef pair<string,MountPoint> str_mnt_pair;

	   pair<map<string,MountPoint >::iterator,bool> mappr;

	   mappr = MountPointsMap.insert(str_mnt_pair(mountpointID,mntpoint));

	   // 
	   if(mappr.second)
	   {
		   cout << "added " << (mappr.first)->first << " successfully!" << endl;
		   numMountPoints++;
	   }
	   else
	   {
		   cout << (mappr.first)->first << " has already existes!" << endl;
		   // the number of mountpoint will not be changed!
	   }
	  
   }


   // delete a mountpoint according to its ID
   void ReadMountPoints::deleteMountPoint(const string mountpointID)
   {
	   map<string,MountPoint>::const_iterator itpr = MountPointsMap.find(mountpointID);
	   /// find
	   if(itpr != MountPointsMap.end())
	   {
		   MountPointsMap.erase(mountpointID);
		   numMountPoints--;
		   cout << "delete " << mountpointID << " successfully!" << endl;
	   }
	   else
	   {
		   cout << mountpointID << " has not exsit in mountpoint map!" << endl;
		   /// the number of mountpoint will not be changed!
	   }
   }


   // get the mountpoint from mountpoint map
   MountPoint ReadMountPoints::getMountPoint(const string mountpointID)
   {
	   map<string,MountPoint>::const_iterator mntpr = MountPointsMap.find(mountpointID);
	   if(mntpr != MountPointsMap.end())
	   {
		   return MountPointsMap[mountpointID];
	   }
	   else
	   {
		   cout << mountpointID << " has not exsit in mountpoint map!" << endl;
		   /// may be something wrong
		   return (*mntpr).second;
	   }
	   
   }


   // dump
   void ReadMountPoints::dump() const
   {
	   cout << "number of mountpoint:\t" << numMountPoints << endl;
	   map<string,MountPoint>::const_iterator mntpr;
	   for(mntpr = MountPointsMap.begin();mntpr != MountPointsMap.end(); ++mntpr)
	   {
		   ((*mntpr).second).dump();
	   }

   }

   // get the map
   const map<string,MountPoint> ReadMountPoints::getMountPointMap()
   {
	   return MountPointsMap;
   }






}  // End of namespace gpstk
