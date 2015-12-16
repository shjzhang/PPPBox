#pragma ident "$Id: DatumPPP.hpp 2583 2012-06-09 01:26:05Z shjzhang $" 

/**
 * @file DatumPPP.hpp
 * Class to define and handle the ambiguity datum for PPP positioning.
 */

#ifndef GPSTK_DATUM_PPP_HPP
#define GPSTK_DATUM_PPP_HPP

//============================================================================
//
//  Function 
//
//  Class to define and choose the robust ambiguity datum for PPP positioning.
//
//  Copyright
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include <algorithm>

#include "Arc.hpp"
#include "ARRound.hpp"
#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Stats.hpp"


namespace gpstk
{

      /// Thrown when attempting to use an invalid DatumPPP
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidDatumPPP, gpstk::Exception);

      /** @addtogroup DataStructures */
      //@{


      /** This class defines and handles the ambiguity datum for the GNSS
       *  PPP positioning with ambiguity resolution. 
       *
       * In order to achieve this, the 'DatumPPP' class will start from a
       * list of 'ambiguity' and the currently available GNSS data.
       *
       * In this class, the deviation, variance and the elevation are 
       * applied together to select a reasonable ambiguity datum for 
       * PPP positioning.
       *
       * @sa SolverPPPAR.hpp
       *
       */
   class DatumPPP
   {
   public:

         /// Default constructor
      DatumPPP()
         : firstTime(true), isPrepared(false)
      {};


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          */
      virtual DatumPPP& Prepare(gnssRinex& gData);


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriAmbDataMap  Apriori state map.
          */
      virtual void setAmbMap( 
                   std::map<SatID,double>& apriAmbDataMap )
         throw(InvalidDatumPPP)
      {
         ambMap = apriAmbDataMap;
      };


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriAmbCovMap  Apriori covariance map.
          */
      virtual void setAmbCovMap( 
                   std::map<SatID, std::map<SatID, double> >& apriAmbCovMap )
         throw(InvalidDatumPPP)
      {
         ambCovMap = apriAmbCovMap;
      };


         /** Reset the ambiguity values and covariance map.
          *
          * @param apriAmbDataMap  Apriori ambiguity data map.
          * @param apriAmbCovMap    Apriori ambiguity covariance map.
          */
      virtual void Reset( 
                   std::map<SatID,double>& apriAmbDataMap ,
                   std::map<SatID, std::map<SatID, double> >& apriAmbCovMap )
         throw(InvalidDatumPPP)
      {
           // Reset the 'ambMap' and 'ambCovMap'
         ambMap = apriAmbDataMap;
         ambCovMap = apriAmbCovMap;
      };


         /** Get the ambiguity datum set which is fixed directly.
          */
      virtual std::map<SatID,double> getAmbFixedMap( void )
         throw(InvalidDatumPPP);


         /// Destructor
      virtual ~DatumPPP() {};


   private :


         /// Map holding the fixed ambiguity value
         /// Map holding the a priori unknowns values 
      std::map<SatID,double> ambFixedMap;


         /// Map holding the a priori unknowns values 
      std::map<SatID,double> ambMap;
         

         /// Map holding covariance information
      std::map<SatID, std::map<SatID, double> > ambCovMap;


         /// Current Independent Edge
      Arc oldArc;


         /// Current Independent Edge
      Arc newArc;


         /// Whether or not this DatumPPP is ready to be used
      bool firstTime;


         /// Whether or not this DatumPPP is ready to be used
      bool isPrepared;


         /// Current epoch
      CommonTime currentEpoch;


         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;


         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;


         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;


   }; // End of class 'DatumPPP'

      //@}

}  // End of namespace gpstk


#endif   // GPSTK_DATUM_PPP_HPP
