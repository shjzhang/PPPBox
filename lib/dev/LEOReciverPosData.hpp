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

#ifndef GPSTK_LEO_RECIVER_POS_DATA_HPP
#define GPSTK_LEO_RECIVER_POS_DATA_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>


#include "Miscmath.hpp"
// data structures
#include "DataStructures.hpp"

using namespace std;
namespace gpstk

{

      class LEOReciverPos
   {
   
         
//         // deflaut
//         //start time of LEO PRD file in GPS second
//         //2000.1.1.12
//         Postimestart(630763200.0),PosJDaystart(2451545.0)
//         {
//               //sometimes, set Postimestart(763200.0),
//               // reduce 63******* to keep time precise E-9.
//         }
         
   public:
      

         typedef struct LEOposition
         {
               double second;
               double x;
               double y;
               double z;
               double vx;
               double vy;
               double vz;
               double tag;
               
               
               LEOposition()
               {
                     second = x = y = z = vx = vy =vz = 0.0;
                     tag=0.0;
               }
               
               
               
         } LEOposition;
         
         //start time of LEO PRD file in GPS second
         double Postimestart;
         
         //start time of LEO PRD file in JD day
         double PosJDaystart;
         
         
         // to get CommonTime time parameters
         //time.get(wday,wsod,wfsod);
         long wday,wsod;
         double wfsod;
         
         //read GOCE position file
         virtual void ReadLEOposition( string filename,
                                      vector<LEOposition> &vLEOposition);
         
         //read GOCE position file
         virtual void ReadLEOposition2(CommonTime time1,
                                       CommonTime time2,
                                      string filename,
                                       vector<LEOposition> &vLEOpositionnew);
         
         //get GOCEposition value at time ttag   vGOCEattag
         virtual void GetLEOpostime(CommonTime time,
                                     vector<LEOposition> vLEOposition,
                                     LEOposition &vGOCEptag);

         /// Method to set starttime of LEO PRD file
         virtual LEOReciverPos& settimestart(double Postime0, double JDay0)
         {
               Postimestart = Postime0;
               PosJDaystart = JDay0;
               return (*this);
         };
         
         
   };// end of class

} // namespace gpstk









#endif // GPSTK_RECEIVER_ATT_DATA_HPP
