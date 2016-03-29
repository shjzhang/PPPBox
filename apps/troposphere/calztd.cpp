// @calTropoDelay.cpp
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
//  Copyright 2004
//
//============================================================================ 
//
//  2016/01/21 : Kemin Zhu, Wuhan University
//               creat this file. 
//  2016/03/01 : Kemin Zhu, Wuhan University
//               
//
//
//============================================================================

#include "CalFrame.hpp"
#include "BasicFramework.hpp"

#include "DataStructures.hpp"
#include "TypeID.hpp"

#include "TimeString.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <vector>

#define PI 3.141592653589793

using namespace std;
using namespace gpstk;
using namespace StringUtils;

   // I: the interval of grid.
   // NCEP Grid Interval : 2.5 dgree.
const double I = 2.5;

   // a1-a7: the parameters of Saastamonine Model
const double a1 = -0.00266;
const double a2 = -0.00028;
const double a3 = 6.11; 
const double a4 = 7.5; 
const double a5 = 0.002277;
const double a6 = 0.05;
const double a7 = 1255.0;

   // T0: a constant when converting Celsius degree
   //     to Fahrenheit degree.
const double T0 = 273.15;


class CalTropoDelay : public CalFrame
{
public :

   CalTropoDelay(char* arg0)
      : CalFrame(arg0, std::string(" Tropospheric delay from NECP model"))
   {}
   
protected:

   double calZTD(double& la, double& t, double& p, double& rh, double& h);
   double proximal_grid(double& a);  
   virtual void process();
};

   

   // Calculate zenith tropospheric delay
double CalTropoDelay::calZTD(double& la, double& t,
                             double& p,  double& rh,
                             double& h)
{
   double f = 1 + a1*cos(2*PI*la/180) + a2*(h/1000);
   double e = (rh/100)*a3*pow(10,(a4*(t-T0)/t));
   double ztd = a5*( p/100 + (a6+(a7/t)*e ))/f;
   return ztd;
}




   // Find the most proximal grid point to stations.
double CalTropoDelay::proximal_grid(double& a)
{
      // the digit of a.
   int digit;
   int i;
   
      // the grid point's lat/lon sourrounding the station. 
   double ang,ang2;

      // @param:  gi: the grid interval
   double gi;

   digit = int(a/10);

      // considering a<0, gi=-I when a<0 
   if(a >= 0) gi =  I;
   else       gi = -I;
   
   // find out four grid points sourrounding the station.
   for( i=0; i<(10/I); i++)
   {
      if( (a-(digit*10+i*gi)) * (a-(digit*10+(i+1)*gi)) < 0 )
      {
         ang = digit *10 + i *gi;
         ang2 = digit *10 + (i+1) *gi;
         break;
      }
   } // end of for
   
   
   // find out the most proximal grid point to the station.
   if ( abs(a-ang) - abs(a-ang2) <= 0 ) return ang;
   else                                 return ang2;

} // end of double proxmial_grid()



void CalTropoDelay::process()
{
   try
   {
         // coordinates filename
    //cout << "1" << endl;
      string coords_filename = coordsFileOption.getValue()[0];
      ifstream coords( coords_filename.c_str(), ios::in);
      if( !coords.is_open() )
      {
         cerr << "We need a coordinates file of the stations" << endl
              << "Please check your files!" << endl;
         exit(-1);
      }
      
         // define a string to store the stream.
      string line, word;

         // @name : station name
         // @lat : station latitude
         // @lon : station longtitude
      string name;
      double lat, lon, p_lat;
      std::vector<std::string> tempvector;
      std::vector<double> coordinates;
      std::map<std::string, std::vector<double> > staCoordsMap;
      std::map<std::vector<double>, double> staLatMap;
         
         // Read the coordinates file.
      while (getline(coords, line))
      {
            // Read data : name, latitude, longtitude.
       //istringstream record(line);

       //while (record >> word)
       //{
       //   tempvector.push_back(word);
       //}         
         name = line.substr(0,4);
       //name = tempvector[0];
       //cout << name << endl;

         lat = atof( line.substr(5,7).c_str() );
       //lat = atof( tempvector[1].c_str() );
       //cout << lat << endl;

         p_lat = lat;
       //
         lon = atof( line.substr(13,7).c_str() );
       //lon = atof( tempvector[2].c_str() );
       //cout << lon << endl; 
               

            // make sure the latitude and longtitude 
            // are in a limitation.
         if (lat >= -90.0 && lat <= 90.0 &&
             lon >= 0.0 && lon < 360.0)
         {
          //cout << "2" << endl;
            // find the most proximal grid point and
            // store them in a vector.
            lat = proximal_grid(lat);
            coordinates.push_back(lat);
            
          //cout << lat << endl;
          //cout << coordinates[0] << endl;

            lon = proximal_grid(lon);
            coordinates.push_back(lon);

          //cout << lon << endl;
          //cout << coordinates[1] << endl;
               
               // store the station and the corresponding
               // coordinates in a map.
            staCoordsMap[name] = coordinates;
            
            staLatMap[coordinates] = p_lat;
          //cout << "Map imported" << endl;
               // clear the vector.
            coordinates.clear();
         }
            // If there is an error, throw a warning.
         else 
         {
            cerr << "There is a wrong data in station: " << name << endl
                 << "Please check your coordinates files!" << endl;
         }
         
      } // end of while
      






         // Now, Let's read the meteorological data from NWP model.
      
         // define several maps to store meteorogical data.
      std::vector<double> grid_coords;
      
      typeValueMap mtvMap,
                   mtvMap2,
                   mtvMap3,
                   mtvMap4;
      
      std::map<std::vector<double>, typeValueMap> coordsTypeValueMap,
                                                  coordsTypeValueMap2,
                                                  coordsTypeValueMap3,
                                                  coordsTypeValueMap4;

      string mline, mword;
      
      double grid_t, grid_p, grid_rh, grid_h, grid_lat, grid_lon;
    
         // Define a stringstream to store the line and
         // a temp vector to store the data.
      std::vector<std::string> interim_vector;
    
         // Now, read the temperature file
      string temp_filename = tempFileOption.getValue()[0];
      ifstream temp( temp_filename.c_str(), ios::in );
      if ( !temp.is_open() )
      {
         cerr << "We need a temperature file of the stations" << endl
              << "Please check your files!" << endl;
         exit(-1);
      }

      while (getline(temp, mline))
      {
       //cout << "temp2mline" << endl;
         if( mline[0] == '1')
         {
            istringstream record(mline);
            while (record >> mword)
            {
               interim_vector.push_back(mword);
            }

          //cout << "SIGN!" << endl;

            grid_lat = atof( interim_vector[1].c_str() );
          //cout << "grid_lat " << grid_lat << endl;

            grid_lon = atof( interim_vector[2].c_str() );
          //cout << "grid_lon " << grid_lon << endl;

            grid_coords.push_back( grid_lat );
            grid_coords.push_back( grid_lon );

            grid_t = atof( interim_vector[3].c_str() );
            
          //cout << "grid_t" << grid_t << endl;
            
            mtvMap[TypeID::Temp] = grid_t;
            coordsTypeValueMap[grid_coords] = mtvMap;
            
               // clear the data in temporary vector.
            grid_coords.clear();
            interim_vector.clear(); 
         }
      }

         // test the mtvMap
    //for (std::map<std::vector<double>, typeValueMap>::iterator ctvIter
    //     = coordsTypeValueMap.begin();
    //     ctvIter != coordsTypeValueMap.end();
    //     ++ctvIter)
    //{
    //   cout << "T" << (*tvIter).second << endl;
    //   std::vector<double> tv = (*ctvIter).first;
    //   cout << "Lat&Lon " << tv[0] << " " << tv[1] << endl;
    //   cout << "T " << (*ctvIter).second.getValue(TypeID::Temp) << endl;
    //}


         // Now, read the pressure file
      string pres_filename = presFileOption.getValue()[0];
      ifstream pres( pres_filename.c_str(), ios::in );
      if ( !pres.is_open() )
      {
         cerr << "We need a pressure file of the stations" << endl
              << "Please check your files!" << endl;
         exit(-1);
      }

      while (getline(pres, mline))
      {
         if( mline[0] == '1')
         {
            istringstream record(mline);
            while (record >> mword)
            {
               interim_vector.push_back(mword);
            }

            grid_lat = atof( interim_vector[1].c_str() );
            grid_lon = atof( interim_vector[2].c_str() );

            grid_coords.push_back( grid_lat );
            grid_coords.push_back( grid_lon );

            grid_p = atof( interim_vector[3].c_str() );
            
            mtvMap2[TypeID::Pres] = grid_p;
            coordsTypeValueMap2[grid_coords] = mtvMap2;

            grid_coords.clear();
            interim_vector.clear(); 

         }
      }

         // Now, read the relative humidity file
      string rhum_filename = rhumFileOption.getValue()[0];
      ifstream rhum( rhum_filename.c_str(), ios::in );
      if ( !rhum.is_open() )
      {
         cerr << "We need a relative humidity file of the stations" << endl
              << "Please check your files!" << endl;
         exit(-1);
      }
    //cout << "RHUM " << rhum.is_open() << endl;

      while (getline(rhum, mline))
      {
         if( mline[0] == '1')
         {
            istringstream record(mline);
            while (record >> mword)
            {
               interim_vector.push_back(mword);
            }

            grid_lat = atof( interim_vector[1].c_str() );
            grid_lon = atof( interim_vector[2].c_str() );

            grid_coords.push_back( grid_lat );
            grid_coords.push_back( grid_lon );

            grid_rh = atof( interim_vector[3].c_str() );
            
            mtvMap3[TypeID::Rhum] = grid_rh;
            coordsTypeValueMap3[grid_coords] = mtvMap3;
            
            grid_coords.clear();
            interim_vector.clear(); 

         }
      }


         // Now, read the height file
      string height_filename = heightFileOption.getValue()[0];
      ifstream height( height_filename.c_str(), ios::in );
      if ( !height.is_open() )
      {
         cerr << "We need a height file of the stations" << endl
              << "Please check your files!" << endl;
         exit(-1);
      }

    //cout << "HGT " << height.is_open() << endl;
      
    //while (height >> mline)
      while (getline(height, mline))
      {
         if( mline[0] == '1')
         {
            istringstream record(mline);
            while (record >> mword)
            {
               interim_vector.push_back(mword);
            }

            grid_lat = atof( interim_vector[1].c_str() );
            grid_lon = atof( interim_vector[2].c_str() );

            grid_coords.push_back( grid_lat );
            grid_coords.push_back( grid_lon );

            grid_h = atof( interim_vector[3].c_str() );
            
            mtvMap4[TypeID::Hgt] = grid_h;
            
            coordsTypeValueMap4[grid_coords] = mtvMap4;
            grid_coords.clear();
            interim_vector.clear(); 

         }
      }


         // Now, we find the corresponding data in coordsTypeValueMap
         // according to staCoordsMap to calculate the ZTDs in grid point.
           
         // Firstly, define a ofstream to store the results.
      std::string outFilename("ztd" + temp_filename.substr(4,7) + "out");
    //std::string outFilename("ztds.out");
      ofstream out(outFilename.c_str(), ios::out);
      if (out.is_open())
      {
         out << setw(4) << "Name" << setw(6) << "ZTD" << endl;
      }
      else 
      {
         cout << "file " << outFilename << " didn't generate successfully" 
               << endl;
      }

         // We need test the map : coordsTypeValueMap
    //for (std::map<std::vector<double>, typeValueMap>::iterator tctvIter
    //     = coordsTypeValueMap.begin();
    //     tctvIter != coordsTypeValueMap.end();
    //     ++tctvIter)
    //{
    //   std::vector<double> tempvector = (*tctvIter).first;
    //   cout << setw(5) << setfill(' ') << tempvector[0] << " " 
    //        << setw(5) << tempvector[1] << " "
    //        << setw(6) << (*tctvIter).second.getValue(TypeID::Temp) << " "
    //        << setw(7) << (*tctvIter).second.getValue(TypeID::Pres) << " "
    //        << setw(5) << (*tctvIter).second.getValue(TypeID::Rhum) << " "
    //        << setw(4) << (*tctvIter).second.getValue(TypeID::Hgt) << " "
    //        << endl;        
    //}
         // We need a loop.

      for ( std::map<std::string, std::vector<double> >::iterator scIter
            = staCoordsMap.begin();
            scIter != staCoordsMap.end();
            ++scIter )
      {
            // The current nearest grid point's coordinates
            // of a station we handle now.
         std::vector<double> currStation = (*scIter).second;
         std::string name = (*scIter).first;
            // Find the current station in coordsTypeValueMap.
            
            // @param:
            //    currLat  : current latitude of grid point
            //    currTemp : current temperature
            //    currPres : current pressure
            //    currRhum : current relative humidity
            //    currHgt  : current height
            //
         double currLat, currTemp, currPres, currRhum, currHgt;
       //double currLat, currHgt;
         std::vector<double> temp_vector,
                             pres_vector,
                             rhum_vector;
                             

         double ztds;
       //currLat = currStation[0];

         std::map< std::vector<double>, typeValueMap >::iterator ctvIter
            = coordsTypeValueMap.find(currStation);
         if ( ctvIter != coordsTypeValueMap.end() )
         {
            
            currTemp = (*ctvIter).second.getValue(TypeID::Temp);
          //cout << "temp " << currTemp << endl;
          //temp_vector.push_back(currTemp);
         }

            // 
         std::map< std::vector<double>, typeValueMap >::iterator ctvIter2
            = coordsTypeValueMap2.find(currStation);
         if ( ctvIter2 != coordsTypeValueMap2.end() )
         {
            currPres = (*ctvIter2).second.getValue(TypeID::Pres);
          //pres_vector.push_back(currPres);
         }
         

         std::map< std::vector<double>, typeValueMap >::iterator ctvIter3
            = coordsTypeValueMap3.find(currStation);
         if ( ctvIter3 != coordsTypeValueMap3.end() )
         {
            currRhum = (*ctvIter3).second.getValue(TypeID::Rhum);
          //rhum_vector.push_back(currRhum);
         }


         std::map< std::vector<double>, typeValueMap >::iterator ctvIter4
            = coordsTypeValueMap4.find(currStation);
         if ( ctvIter4 != coordsTypeValueMap4.end() )
         {
            currHgt = (*ctvIter4).second.getValue(TypeID::Hgt);
         }


         std::map< std::vector<double>, double >::iterator ctvIter5
            =staLatMap.find(currStation);
         if ( ctvIter5 != staLatMap.end() )
         {
            currLat = (*ctvIter5).second;
         }

       //cout << currTemp << " "
       //     << currPres << " "
       //     << currRhum << " "
       //     << currHgt  << " "
       //     << currLat  << endl;

         if (out.is_open())
         {

          //out << setw(4) << name; 
          //int j;
          //for ( j=0; j<30; j++)
          //{
          //   ztds = calZTD(currLat, temp_vector[j], pres_vector[j], 
          //                 rhum_vector[j], currHgt);
         
             //cout << j << ' ' 
             //     << temp_vector[j] << ' ' 
             //     << pres_vector[j] << ' ' 
             //     << rhum_vector[j] << endl;
             //     
             //cout << ztds << endl;      
            
               // export the result
            
            ztds = calZTD(currLat, currTemp, currPres, currRhum,currHgt);
            out  << setw(4) << name<< setfill(' ') 
                   << setprecision(5) << setw(8) 
                   << ztds << endl; 
            
          //}         
            
          //out << endl;
         }
      }
   
      cout << "file " << outFilename << " is generated!" << endl;

   } // end of try
   
   catch(InvalidRequest& e)
   {
      cout << e << endl;
   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating..." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating..." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }

} // end of "void CalTropoDelay::process()"





int main(int argc, char* argv[])
{
   try
   {
      CalTropoDelay m(argv[0]);
      
         if (!m.initialize(argc, argv))   
            return 0;

         if (!m.run())  
         {
            return 1;
         }
         return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(...)
   {
      cout << "Unknow error!" << endl;
   }
   return 1;

}












