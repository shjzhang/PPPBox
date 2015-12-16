#pragma ident "$Id: Arc.hpp  $"

/**
 * @file Arc.hpp
 * Class to define and handle the edge of the 'observed' network.
 */

#ifndef GPSTK_ARC_HPP
#define GPSTK_ARC_HPP

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
//
//  Copyright
//  ---------
//
//  Shoujian Zhang - School of Geodesy and Geomatics, Wuhan University, 2011
//
//============================================================================
//
//  Revision
//  --------
//
//  - Create this program, 2011/12/08
//  - Change the type of the arc source from "SourceID" to "Vertex",
//    Change the type of the arc satellite from "SatID" to "vertex".
//    It will be useful in the Kruskal algorithm to create minmum spanning tree.
//    2012/06/15
//
//============================================================================

#include <set>
#include <map>
#include <list>
#include <queue>
#include <vector>

#include "Exception.hpp"
#include "Vertex.hpp"
#include "SatID.hpp"
#include "SourceID.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{
      
      /**
       *  Class to store the arc information for the observed edge, including
       *  the receiver's Source name, Satellite, and arc number.
       */
   class Arc
   {
   public:

         /**
          * @ingroup exceptionclass
          * DayTime basic exception class.
          */
      NEW_EXCEPTION_CLASS(ArcException, gpstk::Exception);


         /// Default constructor for Arc
      Arc()
      {};


         /** Common constructor for Arc.
          *
          * @param source     Source this Arc is related to.
          * @param sat        Satellite this Arc is related to.
          * @param weight     Variable representing the independent term.
          */
      Arc( const SourceID& source,
           const SatID& sat, 
           const int& arcNumber)
          : arcSource(source), arcSat(sat), arcNum(arcNumber)
      {};

         /// Copy constructor.
      Arc(const Arc &arc)
         throw(ArcException);


         /// Assignment operator.
      Arc& operator=(const Arc& right)
         throw();


         /// Get source this variable is assigned to (if any).
      SourceID getSource() const
      { return arcSource; };


         /** Set source this variable is assigned to.
          *
          * @param source     Internal, specific SourceID of variable.
          */
      Arc& setSource(const SourceID& source)
      { arcSource = source; return (*this); };



         /// Get satellite this variable is assigned to (if any).
      SatID getSatellite() const
      { return arcSat; };


         /** Set satellite this variable is assigned to.
          *
          * @param sat   Internal, specific SatID of variable.
          */
      Arc& setSatellite(const SatID& sat)
      { arcSat= sat; return (*this); };


         /// Get satellite this variable is assigned to (if any).
      int getArcNumber() const
      { return arcNum; };


         /** Set satellite this variable is assigned to.
          *
          * @param satellite  Internal, specific SatID of variable.
          */
      Arc& setArcNumber(const double& arcNumber)
      { arcNum = arcNumber; return (*this); };


         /// This ordering is required to be able
         /// to use a Arc as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Arc& right) const;


         /// Equality operator
      virtual bool operator==(const Arc& right) const;


         /// Destructor
      virtual ~Arc() {};


   protected:


         /// satellite of this arc.
      SatID arcSat;


         /// source of this arc.
      SourceID arcSource;


         /// arc number of this edge .
      int arcNum;


   }; // End of class 'Arc'

   inline std::ostream& operator<<( std::ostream& s,
                             const Arc& arc )
   {
      s << arc.getSource() << "   "
        << arc.getSatellite() << "   "
        << arc.getArcNumber();

      return s;
   };

      /// Handy type definition

   typedef std::set<Arc> ArcSet;

   namespace StringUtils
   {
      inline std::string asString(const Arc& v)
      {
         std::ostringstream oss;
         oss << v.getSource() << "   "
             << v.getSatellite() << "   "
             << v.getArcNumber();

         return oss.str();
      }
   }
      //@}

}  // End of namespace gpstk
#endif   // GPSTK_ARC_HPP
