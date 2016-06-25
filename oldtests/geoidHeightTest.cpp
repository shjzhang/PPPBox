#pragma ident "$Id$"

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include "GeoidHeight.hpp"
#include "GNSSconstants.hpp"


using namespace std;
using namespace gpstk;

int main()
{
      GeoidHeight geoid;
      double lat=30.0/180*PI,lon=114.0/180*PI;
      
      
      string filename="../../tables/egm96.gfc";
      int degree=360;
      double Ngeoid;
      
      geoid.computeGeoid(lat,lon,filename,degree,Ngeoid);
      
      cout<<"geoid"<<Ngeoid<<endl;
      cout<<"sin(lat)"<<std::sin(lat)<<endl;

}
