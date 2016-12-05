#pragma ident "$Id 2016-10-10 $"
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
//  XY.Cao, Wuhan University, 2016
//
//============================================================================

/**
 * @file NtripToolVersion.hpp
 * Define the version of this software 
 */

#ifndef GPSTK_NTRIPTOOLVERSION_H
#define GPSTK_NTRIPTOOLVERSION_H

namespace gpstk
{
	// version
#define NTRIPTOOLVERSION "1.0"

	// agency name
#define NTRIPTOOLPGMNAME "PPPBox" NTRIPTOOLVERSION

	// operating system
#ifdef _WIN32
#define NTRIPTOOLOS "WIN32"		// tested
#elif defined(__APPLE__)
#define NTRIPTOOLOS "MAC"
#elif defined(__linux__)
#define NTRIPTOOLOS "LINUX"		// tested
#elif defined(__unix__)
#define NTRIPTOOLOS "UNIX"
#else
#define NTRIPTOOLOS "UNKOWMN"
#endif


}	// End of namespace gpstk

#endif	// End GPSTK_NTRIPTOOLVERSION_H


