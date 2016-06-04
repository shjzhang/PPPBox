/**
 * @file GetIPP.cpp
 * This is a class to compute the position of ionosphereic pierce point
 * and the ionosphere mapping function(MSLM).
 */

#include "GetIPP.hpp"
#include "GNSSconstants.hpp" // C_MPS
#include "geometry.hpp"      // DEG_TO_RAD

namespace gpstk
{

   using namespace std;

      // Returns a string identifying this object.
   std::string GetIPP::getClassName() const
   { return "GetIPP"; }



      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   GetIPP::GetIPP(const Position& RxCoordinates)
      throw(Exception)
   {
      setIonoMapType("MSLM");
      setInitialRxPosition(RxCoordinates);

   }  // End of constructor 'GetIPP::GetIPP()'



      /** Explicit constructor, taking as input reference station
       * coordinates and ionex maps (Ionex Store object) to be used.
       *
       * @param RxCoordinates    Receiver coordinates.
       * @param dObservable      Observable type to be used by default.
       * @param ionoMapType      Type of ionosphere mapping function (string)
       *                       
       */
   GetIPP::GetIPP( const Position& RxCoordinates,
                   const std::string& ionoMap)
         throw(Exception)
      {

         setInitialRxPosition(RxCoordinates);
  
         setIonoMapType(ionoMap);

      }  // End of constructor 'GetIPP::GetIPP()'



      /** Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& GetIPP::Process( const CommonTime& time,
                                     satTypeValueMap& gData )
      throw(Exception)
   {

      SatIDSet satRejectedSet;

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv)
         {

               // If elevation or azimuth is missing, then remove satellite
            if( stv->second.find(TypeID::elevation) == stv->second.end() ||
                stv->second.find(TypeID::azimuth)   == stv->second.end() )
            {

               satRejectedSet.insert( stv->first );

               continue;

            }
            else
            {

                  // Scalars to hold satellite elevation, azimuth, ionospheric
                  // map and ionospheric slant delays
               double elevation( stv->second(TypeID::elevation) );
               double azimuth(   stv->second(TypeID::azimuth)   );
               double ionoMap(0.0);
              

                  // calculate the position of the ionospheric pierce-point
                  // corresponding to the receiver-satellite ray
               Position IPP = rxPos.getIonosphericPiercePoint( elevation,
                                                               azimuth,
                                                               ionoHeight);

                  // TODO
                  // Checking the collinearity of rxPos, IPP and SV

                  // Convert coordinate system
               Position pos(IPP);
               pos.transformTo(Position::Geocentric);
                 
                  // Now, Let's compute the difference of the latitude between 
                  // the IPP and 
               double latIPP = IPP.getGeocentricLatitude(); 
              
                  // Now, compute the difference of the longitude between
                  // the IPP 
               double lonIPP = pos.getLongitude(); 
               if (lonIPP >= 180.0)
	       {
	         lonIPP -= 360.0;	   
	       }
           
               (*stv).second[TypeID::LatIPP]  = latIPP;
               (*stv).second[TypeID::LonIPP]  = lonIPP;
             

               try
               {

                  ionoMap = getIonoMappingFunction( elevation,ionoMapType);
               }
               catch(InvalidRequest)
               {

                     // If some problem appears, then schedule this
                     // satellite for removal
                  satRejectedSet.insert( stv->first );

                  continue;    // Skip this SV if problems arise

               }

                  // Now we have to add the new values (i.e., ionosphere delays)
                  // to the data structure
               
               (*stv).second[TypeID::ionoMap] = ionoMap;
                       

            }  // End of 'if( stv->second.find(TypeID::elevation) == ... '

         }  // End of loop 'for(stv = gData.begin()...'

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& e)
      {

         GPSTK_RETHROW(e);

      }

   }  // End of method 'GetIPP::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int GetIPP::setInitialRxPosition(const Position& RxCoordinates)
      throw(GeometryException)
   {

      try
      {

         rxPos = RxCoordinates;

         return 0;

      }
      catch(GeometryException)
      {
         return -1;
      }

   }  // End of method 'GetIPP::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int GetIPP::setInitialRxPosition(void)
      throw(GeometryException)
   {

      try
      {

         Position rxpos(0.0, 0.0, 0.0, Position::Cartesian, NULL);

         setInitialRxPosition(rxpos);

         return 0;

      }
      catch(GeometryException)
      {
         return -1;
      }

   }  // End of method 'GetIPP::setInitialRxPosition()'



      /** Method to set the default ionosphere mapping function type.
       *  If no valid type than NONE is set.
       *
       * @param ionoMapType  Type of ionosphere mapping function (string)
       *                   
       *
       * @warning No implementation for JPL's mapping function.
       */
   GetIPP& GetIPP::setIonoMapType(const std::string& ionoMap)
   {

         // here we set the type
      ionoMapType = ( ionoMap != "NONE" && ionoMap != "SLM" &&
                      ionoMap != "MSLM" && ionoMap != "ESM") ? "NONE" :
                                                               ionoMap;

         // and here the ionosphere height, in meters
      ionoHeight = (ionoMap == "MSLM") ? 506700.0 : 450000.0;

      return (*this);

   }

   double GetIPP::getIonoMappingFunction(const double& elevation,
                                         const std::string& ionoMapType)
   {
        // map
      double imap(1.0);
         // Earth's radius in KM
      double Re = 6371.0;
         // zenith angle
      double z0 = 90.0 - elevation;

      if( ionoMapType == "SLM" )
      {

            // As explained in: Hofmann-Wellenhof et al. (2004) - GPS Theory and
            // practice, 5th edition, SpringerWienNewYork, Chapter 6.3, pg. 102

            // ionosphere height in KM
         double ionoHeight = 450.0;
            // zenith angle of the ionospheric pierce point (IPP)
         double sinzipp  = Re / (Re + ionoHeight) * std::sin(z0*DEG_TO_RAD);
         double zipprad  = std::asin(sinzipp);

         imap = 1.0/std::cos(zipprad);

      }
      else if( ionoMapType == "MSLM" )
      {
         // maximum zenith distance is 80 degrees
         if( z0 <= 80.0 )
         {
           // ionosphere height in KM
           double ionoHeight = 506.7;
           double alfa       = 0.9782;
           // zenith angle of the ionospheric pierce point (IPP)
           double sinzipp = Re / (Re + ionoHeight)
               * std::sin(alfa * z0 * DEG_TO_RAD);
           double zipprad = std::asin(sinzipp);

           imap = 1.0/std::cos(zipprad);
         }

      }
      else if( ionoMapType == "ESM" )
      {
         //TODO
      }
      else  // that means ionoMapType == "NONE"
      {
         // TODO
      }


      return imap;

   }  // End of method 'IonexStore::iono_mapping_function()'


}  // End of namespace gpstk
