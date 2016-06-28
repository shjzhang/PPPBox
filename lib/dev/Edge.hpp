#pragma ident "$Id: Edge.hpp 2596 2011-12-08 17:22:04Z shjzhang $"

/**
 * @file Edge.hpp
 * Class to define and handle the edge of the 'observed' network.
 */

#ifndef GPSTK_EDGE_HPP
#define GPSTK_EDGE_HPP

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
//  Shoujian Zhang - School of Geodesy and Geomatics, Wuhan University, 2011
//
//============================================================================

#include <set>
#include <map>
#include <list>
#include <queue>
#include <vector>

#include "Arc.hpp"
#include "Variable.hpp"
#include "DataStructures.hpp"
#include "GNSSconstants.hpp" // DEG_TO_RAD

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /// Class to define and handle edge of the observed network.
   class Edge : public Arc
   {
   public:

         /// Default constructor for Edge
      Edge() {};


         /** Common constructor for Edge.
          *
          * @param source     Source this Edge is related to.
          * @param sat        Satellite this Edge is related to.
          * @param weight     Variable representing the independent term.
          */
      Edge( const SourceID& source,
            const SatID&    sat, 
            const double&   arcNum,
            const double&   aprioriWeight,
            const double&   elevation )
          : Arc(source, sat, arcNum), 
            apriWeight(aprioriWeight),
            elev(elevation)
      {
         apriVar = 1.0/apriWeight;
         elevVar = 1.0/std::sin(elev*DEG_TO_RAD);
      };

         /// Copy constructor.
      Edge(const Edge &edge)
         throw();


         /// Assignment operator.
      Edge& operator=(const Edge& right)
         throw();


         /// Get apriori weight this variable is assigned to (if any).
      double getApriWeight() const
      { return apriWeight; };


         /// Get apriori variance this variable is assigned to (if any).
      double getApriVariance() const
      { return apriVar; };


         /** Set apriori weight this variable is assigned to.
          *
          * @param weight, weight of the variable relating to  this edge.
          *
          * @warning the 'weight' here is the weight of the related variable
          * in the observation equation, so the larger of the 'weight', the
          * smaller of the variance. And the variance is chosen as
          * the graph weight.
          */
      Edge& setApriWeight(const double& weight)
      { 
         apriWeight = weight; 
         apriVar = 1.0/apriWeight;
         return (*this); 
      };


         /// Get elevation this variable is assigned to (if any).
      double getElevation() const
      { return elev; };


         /// Get apriori variance this variable is assigned to (if any).
      double getElevVariance() const
      { return elevVar; };


         /** Set elevation this variable is assigned to.
          *
          * @param elevVar, elevVar for this edge.
          */
      Edge& setElevation(const double& elevation)
      { 
          elev = elevation; 
          elevVar = 1.0/std::sin(elev*DEG_TO_RAD);
          return (*this); 
      };


         /// This ordering is required to be able
         /// to use a Edge as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Edge& right) const;


         /// Destructor
      virtual ~Edge() {};


   private:

         /// Apriori variance of this edge .
      double apriVar;

         /// Elevation of this edge .
      double apriWeight;

         /// Elevation variance of this edge .
      double elevVar;

         /// Elevation of this edge .
      double elev;

   }; // End of class 'Edge'


      /// Handy type definition
   typedef std::set<Edge> EdgeSet;

      /// Output
   namespace StringUtils
   {
      inline std::string asString(const Edge& v)
      {
         std::ostringstream oss;
//       oss << v.getApriVariance() << " "
//           << v.getApriWeight()   << " "
//           << v.getElevVariance() << " "
//           << v.getElevation()    << " "
         oss << v.getSource()       << " "
             << v.getSatellite()    << " "
             << v.getArcNumber()    << " "
             << v.getApriVariance() << " "
             << v.getApriWeight()   << " "
             << v.getElevVariance() << " "
             << v.getElevation()  ;  

         return oss.str();
      }
   }
      //@}

}  // End of namespace gpstk
#endif   // GPSTK_EDGE_HPP
