#pragma ident "$Id: ReceiverAttData.cpp 2897 2012-08-10 20:08:46Z shjzhang $"
//only for GOCE ATTITUDE
/**
 * @file ReceiverAttData.cpp
 * Encapsulate satellite bias file data, including I/O
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

#include <iostream>
#include <fstream>
#include "LEOReciverPosData.hpp"


using namespace std;
namespace gpstk
{
     

      //read GOCE position file
      void LEOReciverPos::ReadLEOposition( string filename,
                                          vector<LEOposition> &vLEOposition)
      {
            
            LEOposition gocepx;
            
            vLEOposition.clear();
            
           ifstream inpf(filename.c_str());
            if(!inpf)
            {
                  FileMissingException fme("Could not open file :" + filename);
                  GPSTK_THROW(fme);
            }
            
            bool ok(true);
            string line;
            
            while( !inpf.eof() && inpf.good() )
            {
                  if( inpf.eof() ) break;
                  
                  if( inpf.bad() ) { ok = false; break; }
                  
                  getline(inpf,line);
                  istringstream is(line);
                  is>> gocepx.second
                  >>gocepx.x
                  >>gocepx.y
                  >>gocepx.z
                  >>gocepx.vx
                  >>gocepx.vy
                  >>gocepx.vz;
                  
                  vLEOposition.push_back(gocepx);
                  
            }
            
            cout<<"The length of file :"<<filename<<" is "
            << vLEOposition.size()<<endl;
            
            inpf.close();
            
            if( !ok )
            {
                  FileMissingException fme( filename + " is corrupted or in wrong format");
                  GPSTK_THROW(fme);
            }
            
      }//end of read GOCE positionfile
      
      
      //read GOCE position file
      void LEOReciverPos::ReadLEOposition2(double t1, double t2,
                                           string filename,
                                           vector<LEOposition> &vLEOpositionnew)
      {
            vector<LEOposition>  vLEOposition;
            vLEOpositionnew.clear();
            
            // read GOCE position file
            ReadLEOposition(filename,vLEOposition);
            
            int length=vLEOposition.size();
            
            for (int i=0; i<length-1;i++)
            {
                  
                  if((vLEOposition[i].second > (t1-100))
                     &&(vLEOposition[i].second < (t2+100)))
                        
                        vLEOpositionnew.push_back(vLEOposition[i]);
                  
            }
            
            
            
            
      }//end of read GOCE position file
      
      //get GOCEposition value at time ttag   vGOCEattag
      void LEOReciverPos::GetLEOpostime(double ttag,
                                         vector<LEOposition> vLEOposition,
                                         LEOposition &vGOCEptag)
      {
            
            //*******************
            // choose 9 point for lagrangepoly ,change it as you need;
            int N=9;
            int mid=floor(N/2);
            //*******************
            
            vector<double> times(N), x(N), y(N), z(N);
            double err;
            
            // compute dt for GOCE prd data
            //compute  line use to interpoly
            double dt=vLEOposition[1].second-vLEOposition[0].second;
            int line,lengtht;
            line=floor((ttag-vLEOposition[0].second)/round(dt));
            
            lengtht=vLEOposition.size();
            
            if ((ttag<vLEOposition[N].second )
                ||(ttag > vLEOposition[lengtht-N].second ) )
            {
                  
                  cout<<setprecision(16);
                  cout<<"The GOCE prd position inpoly time  "<<ttag<<" out of file "
                  <<"the time should in "<<vLEOposition[N].second
                  << " and " <<vLEOposition[lengtht-N].second<<endl;
                  return ;
            }
            
            // cout<<"ttag"<<ttag<<endl;
            
            
            
            for(int i = 0; i < N; i ++)
            {
                  times[i]=vLEOposition[line-mid+i].second;
                  x[i]=vLEOposition[line-mid+i].x;
                  y[i]=vLEOposition[line-mid+i].y;
                  z[i]=vLEOposition[line-mid+i].z;
            }
            vGOCEptag.second=ttag;
            vGOCEptag.x=LagrangeInterpolation(times,x,ttag,err);
            vGOCEptag.y=LagrangeInterpolation(times,y,ttag,err);
            vGOCEptag.z=LagrangeInterpolation(times,z,ttag,err);
            
      }//end of get GOCEposition value at time ttag   vGOCEattag



} // namespace

//------------------------------------------------------------------------------------
