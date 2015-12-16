#pragma ident "$Id: DatumPPPNL.hpp 2583 2012-06-09 01:26:05Z shjzhang $" 

/**
 * @file DatumPPPNL.hpp
 * Class to define and handle the ambiguity datum for receiver positioning.
 */

#ifndef GPSTK_DATUMPPPNL_HPP
#define GPSTK_DATUMPPPNL_HPP

//============================================================================
//
//  Function 
//  --------
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  Copyright
//  ---------
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
//  Author
//  ------
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include <algorithm>

#include "Arc.hpp"
#include "ARRound.hpp"
#include "Edge.hpp"
#include "Vertex.hpp"
#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Stats.hpp"


namespace gpstk
{

      /// Thrown when attempting to use an invalid DatumPPPNL
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidDatumPPPNL, gpstk::Exception);


      /** @addtogroup DataStructures */
      //@{


      /** This class defines and handles the ambiguity datum for the GNSS
       *  single rover stations in PPP processing. 
       *
       * In order to achieve this, the 'DatumPPPNL' class will start from a
       * list of 'ambiguity' and the currently available GNSS data.
       * From there, it will deduce the ambiguity unknowns, observed edges
       * independent ambiguity unknowns from the above information.
       *
       * A typical way to use this class follows, showing how to set up an
       * object to perform "NeuNet" (PPP):
       *
       * @code
       *
       *
       * @endcode
       *
       * In this way, rather complex processing strategies may be set up in a
       * handy and flexible way.
       *
       * \warning Please be aware that this class requires a significant amount
       * of overhead. Therefore, if your priority is execution speed you should
       * either use the already provided 'purpose-specific' solvers (like
       * 'SolverPPP' or hand-code your own class. For new processing strategies
       * you should balance the importance of machine time (extra overhead)
       * versus researcher time (writing a new solver).
       *
       * @sa Variable.hpp.
       *
       */
   class DatumPPPNL
   {
   public:

         /// Default constructor
      DatumPPPNL()
         : firstTime(true), isPrepared(false)
      {};


         /** Prepare this object to carry out its work.
          *
          * @param gData   GNSS data structure (GDS).
          */
      virtual DatumPPPNL& Prepare(gnssRinex& gData);


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriAmbDataMap  Apriori state map.
          */
      virtual void setAmbMap( 
                   std::map<SatID,double>& apriAmbDataMap )
         throw(InvalidDatumPPPNL)
      {
         ambMap = apriAmbDataMap;
      };


         /** Set the a priori ambiguity unknowns, their values .
          *
          * @param apriAmbCovMap  Apriori covariance map.
          */
      virtual void setAmbCovMap( 
                   std::map<SatID, std::map<SatID, double> >& apriAmbCovMap )
         throw(InvalidDatumPPPNL)
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
         throw(InvalidDatumPPPNL)
      {
           // Reset the 'ambMap' and 'ambCovMap'
         ambMap = apriAmbDataMap;
         ambCovMap = apriAmbCovMap;
      };


         /** Get the ambiguity datum set which is fixed directly.
          */
      virtual std::map<SatID,double> getAmbFixedMap( void )
         throw(InvalidDatumPPPNL);


         /// Destructor
      virtual ~DatumPPPNL() {};


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


         /// Whether or not this DatumPPPNL is ready to be used
      bool firstTime;


         /// Whether or not this DatumPPPNL is ready to be used
      bool isPrepared;


         /// Current epoch
      CommonTime currentEpoch;


         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;


         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;


         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;


   }; // End of class 'DatumPPPNL'

      //@}

}  // End of namespace gpstk


#endif   // GPSTK_DATUMPPPNL_HPP
