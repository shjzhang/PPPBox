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
         
         
         //read GOCE position file
         virtual void ReadLEOposition( char *filename,
                                      vector<LEOposition> &vLEOposition);
         
         //read GOCE position file
         virtual void ReadLEOposition2(double t1, double t2,
                                      char *filename,
                                       vector<LEOposition> &vLEOpositionnew);
         
         //get GOCEposition value at time ttag   vGOCEattag
         virtual void GetLEOpostime(double ttag,
                                     vector<LEOposition> vLEOposition,
                                     LEOposition &vGOCEptag);

         
         
   };// end of class

} // namespace gpstk









#endif // GPSTK_RECEIVER_ATT_DATA_HPP
