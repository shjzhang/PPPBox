#pragma ident "$Id$"
/**
 * @file GetIPP.hpp
 * This is a class to compute the position of ionosphereic pierce point
 * and the ionosphere mapping function(MSLM).
 */

#ifndef GPSTK_GetIPP_HPP
#define GPSTK_GetIPP_HPP

#include "Position.hpp"
#include "ProcessingClass.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
     
   class GetIPP : public ProcessingClass
   {
   public:


         /// Default constructor.
      GetIPP();


         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
      GetIPP(const Position& RxCoordinates)
         throw(Exception);


         /** Explicit constructor, taking as input reference station
          *  coordinates and ionex maps (Ionex Store object) to be used.
          *
          * @param RxCoordinates    Receiver coordinates.
          * @param ionoMap          Type of ionosphere mapping function (string)
     
          */
      GetIPP( const Position& RxCoordinates,
              const std::string& ionoMap )
         throw(Exception);


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(Exception);


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(Exception)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(Exception)
      { Process(gData.header.epoch, gData.body); return gData; };



         /** Method to set the initial (a priori) position of receiver.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int setInitialRxPosition(const Position& RxCoordinates)
         throw(GeometryException);


         /// Method to set the initial (a priori) position of receiver.
      virtual int setInitialRxPosition(void)
         throw(GeometryException);


         /// Method to get the default ionosphere mapping function type.
      virtual std::string getIonoMapType() const
      { return ionoMapType; };


         /** Method to set the default ionosphere mapping function type. 
          *
          * @param ionoMapType      Type of ionosphere mapping function (string)
          *                        
          *
          * @warning No implementation for JPL's mapping function.
          */
      virtual GetIPP& setIonoMapType(const std::string& ionoMap);
         
       /** Ionosphere mapping function
       *
       * @param elevation     Elevation of satellite as seen at receiver
       *                      (degrees).
       * @param ionoMapType   Type of ionosphere mapping function (string)
       *                      (0) NONE no mapping function is applied
       *                      (1) SLM  Single Layer Model (IGS)
       *                      (2) MSLM Modified Single Layer Model (CODE)
       *                      (3) ESM  Extended Slab Model (JLP)
       *
       * Details at: www.aiub.unibe.ch/download/users/schaer/igsiono/doc/mslm.pdf
       *
       * @warning No implementation for JPL's mapping function.
       */
      double getIonoMappingFunction(const double& elevation,
                                         const std::string& ionoMapType);

         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~GetIPP() {};


   protected:


         /// Either estimated or "a priori" position of receiver
      Position rxPos;

         /// Type of ionosphere mapping function 
      std::string ionoMapType;

         /// the mean value for the height of the ionosphere for which 
         /// the TEC values are extracted.
      double ionoHeight;


   }; // End of class 'GetIPP'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_GetIPP_HPP




