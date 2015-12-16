#pragma ident "$Id: RinexUPDXStore.hpp 2897 2011-09-14 20:08:46Z shjzhang $"

/**
 * @file RinexUPDXStore.hpp
 * Store a tabular list of satellite biaes, and compute values at any time
 * from this table. 
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

#include "RinexUPDXStore.hpp"
#include "MiscMath.hpp"

using namespace std;


namespace gpstk
{

         /** Return value for the given satellite at the given time.
          *
          * @param sat      the SatID of the satellite of interest
          * @param time     the time (CommonTime) of interest
          * @return         object of type SatUPDX containing the data value(s).
          *
          * @throw InvalidRequest 
          *
          *  if data value cannot be computed because:
          *
          *  a) the time t does not lie within the time limits of the data table
          *  b) checkDataGap is true and there is a data gap
          *  c) checkInterval is true and the interval is larger than maxInterval
          *
          */
   SatUPDX RinexUPDXStore::getValue(const SatID& sat, const CommonTime& time)
      const throw(InvalidRequest)
   {
      try {

         bool isExact;
         SatUPDX rec;
         DataTableIterator it1, it2, kt;  

            // Get the data range
         isExact = getTableInterval(sat, time, 1, it1, it2, true );


            // If the time is found in data table, then return directly
         if(isExact) 
         {
            rec = it1->second;
            return rec;
         }

            // Initial time
         CommonTime time0(it1->first);

            // Variables for linear interpolate
         vector<double> times, updSatWL, updSatLC, updSatL1, updSatL2;

         kt = it1; 
         while(1) 
         {
               // Inesert time 
            times.push_back( kt->first - time0 );    // sec

               // Insert wl upds
            updSatWL.push_back( kt->second.updSatWL );    

               // Insert wl upds
            updSatLC.push_back( kt->second.updSatLC);    

               // Insert L1 upds
            updSatL1.push_back( kt->second.updSatL1 );     

               // Insert L2 upds
            updSatL2.push_back( kt->second.updSatL2 );    
 

               // End of the data table
            if(kt == it2) break;

               // Increment
            ++kt;
         };

            // interpolate
         double dt(time-time0), slope;

         slope = (updSatWL[1]-updSatWL[0]) / (times[1]-times[0]);  
         rec.updSatWL = updSatWL[0] + (dt-times[0])*slope;    // sec/sec

         slope = (updSatLC[1]-updSatLC[0]) / (times[1]-times[0]);  
         rec.updSatLC = updSatLC[0] + (dt-times[0])*slope;    // sec/sec

            // linear interpolation
         slope = (updSatL1[1]-updSatL1[0]) / (times[1]-times[0]);  
         rec.updSatL1 = updSatL1[0] + (dt-times[0])*slope;    // sec/sec

            // linear interpolation
         slope = (updSatL2[1]-updSatL2[0]) / (times[1]-times[0]);  
         rec.updSatL2 = updSatL2[0] + (dt-times[0])*slope;    // sec/sec


         return rec;
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }
   }


      // Add a SatUPDX to the store.
   void RinexUPDXStore::addSatUPDX(const SatID& sat, 
                                   const CommonTime& time,
                                   const SatUPDX& rec)
      throw(InvalidRequest)
   {
      try 
      {

         if( tables.find(sat) != tables.end() ) 
         {
            if( tables[sat].find(time) != tables[sat].end() )
            {
                  // Update the old data record
               SatUPDX& oldrec(tables[sat][time]);

               oldrec.updSatWL = rec.updSatWL;
               oldrec.updSatLC = rec.updSatLC;
               oldrec.updSatL1 = rec.updSatL1;
               oldrec.updSatL2 = rec.updSatL2;

            }
            else
            {
               tables[sat][time] = rec;
            }

         }
         else  // Add directly
         {
            tables[sat][time] = rec;
         }
      }
      catch(InvalidRequest& ir) 
      { 
         GPSTK_RETHROW(ir); 
      }
   }


      /// Load a satellite upd file 
   void RinexUPDXStore::loadFile(const std::string& filename) 
       throw(Exception)
   {
      try 
      {

            // open the input stream
         RinexUPDXStream strm(filename.c_str());

         if(!strm.is_open()) 
         {
            Exception e("File " + filename + " could not be opened");
            GPSTK_THROW(e);
         }
         strm.exceptions(std::ios::failbit);
      // cout << "Opened file " << filename << endl;

            // declare header and data
         RinexUPDXHeader head;
         RinexUPDXData data;

            // read the RINEX clock header
         try 
         {
            strm >> head;
         }
         catch(Exception& e)
         {
            e.addText("Error reading header of file " + filename);
            GPSTK_RETHROW(e);
         }
      // cout << "Read header" << endl; head.dump();

            // save in FileStore
         clkFiles.addFile(filename, head);

            // read data
         try
         {
            while(strm >> data) 
            {

            ///data.dump(cout);

               if(data.datatype == std::string("AS")) 
               {
                  SatUPDX rec;

                  rec.updSatWL = data.updSatWL; 
                  rec.updSatLC = data.updSatLC; 
                  rec.updSatL1 = data.updSatL1; 
                  rec.updSatL2 = data.updSatL2;

                  addSatUPDX(data.sat, data.time, rec);
               }
            }
         }
         catch(Exception& e) 
         {
            e.addText("Error reading data of file " + filename);
            GPSTK_RETHROW(e);
         }

            // close
         strm.close();

      }
      catch(Exception& e) 
      { 
          GPSTK_RETHROW(e); 
      }

   }  // end RinexUPDXStore::loadFile()


}  // End of namespace gpstk
