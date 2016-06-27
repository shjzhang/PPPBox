#pragma ident "$Id: ReceiverAttData.hpp 2897 2011-09-14 20:08:46Z shjzhang $"
//only for GOCE ATTITUDE
/**
 * @file ReceiverAttData.hpp
 * Read receiver attitude data  
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#ifndef GPSTK_LEO_RECIVER_ATT_DATA_HPP
#define GPSTK_LEO_RECIVER_ATT_DATA_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>


#include "Miscmath.hpp"
#include "Quaternion.hpp"
// data structures
#include "DataStructures.hpp"

using namespace std;
namespace gpstk

{

      class LEOReciverAtt
   {
         
//         // deflaut
//         //start time of LEO PRD file in GPS second
//         //2000.1.1.12
//         LEOReciverAtt():
//         offsetReciver(0.0,0.0,0.0),Atttimestart(630763200.0),
//         AttJDaystart(2451545.0)
//         {
//               //sometimes, set Atttimestart(763200.0),
//               // reduce 63******* to keep time precise E-9.
//         }
//         
         
   public:
      
         typedef struct LEOatt
         {
               double second;
               double q1;
               double q2;
               double q3;
               double q4;
               

               LEOatt()
               {
                     second = q1 = q2 = q3 =q4 = 0.0;
               }
               
               
               
         } LEOatt;
         
         //LEO reciver offset in SRF
         Triple offsetReciver;
         
         
         //start time of LEO PRD file in GPS second
         double Atttimestart;
         
         //start time of LEO PRD file in JD day
         double AttJDaystart;
         
         // to get CommonTime time parameters
         //time.get(wday,wsod,wfsod);
         long wday,wsod;
         double wfsod;
         
         
         //read GOCE attitude file
         virtual  void ReadLEOatt( string filename, vector<LEOatt> &vLEOatt);
      
         
         //read GOCE attitude file
      virtual  void ReadLEOatt2(CommonTime time1,
                                CommonTime time2,
                                string filename,
                                vector<LEOatt> &vLEOattnew);
      
         
         //get GOCE attitude value at time ttag   vGOCEattag
      virtual   void GetLEOattime(CommonTime time,
                                  vector<LEOatt> vLEOatt,
                                  LEOatt &LEOatttag);
      
         //get GOCE reciver offset value at in ICEF
      virtual  void LEOroffsetvt(CommonTime time,
                           vector<LEOatt> vLEOatt1,
                           vector<LEOatt> vLEOatt2,
                           Triple &offsetRecivert);
      
         /// Method to set starttime of LEO PRD file
      virtual LEOReciverAtt& settimestart(double Atttime0, double JDay0)
      {
         Atttimestart = Atttime0;
         AttJDaystart = JDay0;
         return (*this);
      };
         
         /// Method to set starttime of LEO PRD file
      virtual LEOReciverAtt& setoffsetReciver(Triple offset)
      {
            offsetReciver=offset; return (*this);
      };

   };// end of class

} // namespace gpstk









#endif // GPSTK_RECEIVER_ATT_DATA_HPP
