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
 * @file GravityModelData.cpp
 */

#include "GravityModelData.hpp"
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"


using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

   // Load EGM file
      void GravityModelData::loadEGMFile(string filename)
      throw(FileMissingException)
   {
      ifstream inpf(filename.c_str());

      if(!inpf)
      {
         FileMissingException fme("Could not open EGM file :" + filename);
         GPSTK_THROW(fme);
      }

      // First, file header
      string temp;
      while( getline(inpf,temp) )
      {
            if(temp.substr(0,9) == "modelname")
            {
                  gmData.modelName=temp.substr(28,16);
            
               //   cout<<gmData.modelName<<endl;
            }
            
            if(temp.substr(0,22) == "earth_gravity_constant")
            {
                  gmData.GM=for2doub(temp.substr(28,16));
                  
             //     cout<<gmData.GM<<endl;
            }
            
            if(temp.substr(0,6) == "radius")
            {
                  gmData.ae=for2doub(temp.substr(28,16));
                  
              //    cout<<gmData.ae<<endl;
            }
            
            if(temp.substr(0,10) == "max_degree")
            {
                  gmData.maxDegree=asInt(temp.substr(28,16));
            
              //    cout<<gmData.maxDegree<<endl;
            }
            
            
            
            if(temp.substr(0,11) == "tide_system")
            {
                  gmData.tide_system=temp.substr(28,9);
              //    cout<<gmData.tide_system<<endl;
            }
            
            if(temp.substr(0,11) == "end_of_head") break;
      }

         // set vector size to store Cnm, Snm, sigmaCnm, sigmaSnm
      int size = (gmData.maxDegree+2)*(gmData.maxDegree+1)/2;
      gmData.normalizedCS.resize(size,4,0.0);
         
         
      bool ok(true);

      string line;

      // Then, file data
      while( !inpf.eof() && inpf.good() )
      {
      
         if( inpf.eof() ) break;

         if( inpf.bad() ) { ok = false; break; }
 
         // degree, order
         int L, M;
            
         // Cnm, Snm, sigmaCnm, sigmaSnm
         double C, S, sigmaC, sigmaS;
            
         string gfc;
         
         getline(inpf,line);
         
         if(line.substr(0,3) == "gfc")
            
         {
            //read data aas formmat
            //gfc   L    M     C         S        sigma C    sigma S
               
         istringstream is(line);
         is>> gfc >> L >> M >> C >> S >> sigmaC >> sigmaS;
               
        // cout<<gfc << L <<" "<< M <<" "<< C <<" "<< S <<" "<< sigmaC<<" " << sigmaS<<endl;
         
         }
            
         int i = L*(L+1)/2 + (M+1);
            
            gmData.normalizedCS(i-1,0) = C;
            gmData.normalizedCS(i-1,1) = S;
            gmData.normalizedCS(i-1,2) = sigmaC;
            gmData.normalizedCS(i-1,3) = sigmaS;


//         cout << setw(22) << doub2sci(gmData.normalizedCS(i,0),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,1),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,2),22,3) << " "
//              << setw(22) << doub2sci(gmData.normalizedCS(i,3),22,3) << endl;
      }

      inpf.close();

      if( !ok )
      {
         FileMissingException fme("EGM file " + filename + " is corrupted or in wrong format");
         GPSTK_THROW(fme);
      }

   }  // End of method "GravitationMoldeData::loadEGMFile()"

   void GravityModelData::loadEGMFile(string filename,int degree)
      throw(gpstk::FileMissingException)
   {
        //load GravityModel file
      loadEGMFile(filename);
      
         
      if(degree>gmData.maxDegree||degree<0)
      {
            FileMissingException fme("degree set too big for read EGM file:" + filename);
            GPSTK_THROW(fme);
      }
            
        //get Model at degree you need
      Matrix<double> normalizedCS=gmData.normalizedCS;
            
      int size = (degree+2)*(degree+1)/2;
      gmData.normalizedCS.resize(size,4,0.0);
       
      
         for(int i=1;i<=size;i++)
         {
               gmData.normalizedCS(i-1,0)=normalizedCS(i-1,0);
               gmData.normalizedCS(i-1,1)=normalizedCS(i-1,1);
               gmData.normalizedCS(i-1,2)=normalizedCS(i-1,2);
               gmData.normalizedCS(i-1,3)=normalizedCS(i-1,3);
         }
         
            
   }// End of method "GravitationMoldeData::loadEGMFile()"
      
      
      

}   // End of namespace 'gpstk'
