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
//  Kaifa Kuang - Wuhan University . 2015
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
 * @file GeoidHeight.cpp
 */

#include "GeoidHeight.hpp"

using namespace std;

namespace gpstk
{

      
      void GeoidHeight::computeGeoid(const double lat,
                     const double lon,
                     const string filename,
                     const int degree,
                     double& Ngeoid )
      throw(FileMissingException)
      {

            
            GraModel.loadEGMFile(filename,degree);
            
            legendre(degree,lat,leg0,leg1,leg2);
      
            //cout<<degree<<lat<<leg0(1);

            for(int n=0; n<=degree; ++n)
            {
                  for(int m=0; m<=n; ++m)
                  {
                  //Legnedre
                  double P0 = leg0(index(n,m)-1);
                  //Cnm Snm
                  double Cnm = GraModel.gmData.normalizedCS(index(n,m)-1, 0);
                  double Snm = GraModel.gmData.normalizedCS(index(n,m)-1, 1);
                  // sin(m*lon) and cos(m*lon)
                  double smlon = std::sin(m*lon);
                  double cmlon = std::cos(m*lon);
                  
                        if(n%2==0&&m==0){ Cnm=0.0;};
                        
                  Tdisturb +=P0 * (Cnm*cmlon+Snm*smlon);
                        
                  }
            }
            
            Ngeoid= Tdisturb*(GraModel.gmData.GM/GraModel.gmData.ae)/gama;
      
      }//end of computeGeoid
      
      // compute Geoid.default EGM2008,MAXdegree=360;
      void GeoidHeight::computeGeoid(const double lat,
                        const double lon,
                        const string filename,
                        double& Ngeoid )
      throw(FileMissingException)
      {
            
            degree=360;
            computeGeoid(lat,lon,filename,degree,Ngeoid );
            
      } //end of computeGeoid
            
            
      
      // compute Geoid.default EGM2008,MAXdegree=360;
      void GeoidHeight::computeGeoid(const double lat,
                     const double lon,
                     double& Ngeoid )
      throw(FileMissingException)
      {
         
            filename="EGM2008";
         
            degree=360;
         
            computeGeoid(lat,lon,filename,degree,Ngeoid );
         
               
      } //end of computeGeoid


      
      
      

}   // End of namespace 'gpstk'
