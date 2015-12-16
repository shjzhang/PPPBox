#pragma ident "$Id: Edge.cpp 2596 2011-12-08 17:22:04Z shjzhang $"

/**
 * @file Edge.cpp
 * Class to define and handle the edge of the 'observed' network.
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
//  Shoujian Zhang - School of Geodesy and Geomatics, Wuhan University, 2011
//
//============================================================================

#include <set>
#include <map>
#include <list>
#include <queue>
#include <vector>

#include "Edge.hpp"

namespace gpstk
{

       /// Copy constructor.
    Edge::Edge(const Edge &edge)
       throw()
    {
       arcSource  = edge.arcSource;
       arcSat     = edge.arcSat;
       arcNum     = edge.arcNum;
       apriVar    = edge.apriVar;
       apriWeight = edge.apriWeight;
       elevVar    = edge.elevVar;
       elev       = edge.elev;
    }


       /// Assignment operator.
    Edge& Edge::operator=(const Edge& right)
       throw()
    {
       if ( this == &right )
       {
          return (*this);
       }

       arcSource  = right.arcSource;
       arcSat     = right.arcSat;
       arcNum     = right.arcNum;
       apriVar    = right.apriVar;
       apriWeight = right.apriWeight;
       elevVar    = right.elevVar;
       elev       = right.elev;

       return (*this);
    }

       /// This ordering is required to be able
       /// to use a Edge as an index to a std::map, or as part of a
       /// std::set. The edges are ordered by its varation firstly, 
       /// then the arc
    bool Edge::operator<(const Edge& right) const
    { 

       if( apriVar == right.getApriVariance() )
       {

           if( elevVar == right.getElevVariance() )
           {

               if( arcSource == right.getSource() )
               {

                   if( arcSat == right.getSatellite() )
                   {
                      return ( arcNum  < right.getArcNumber() ); 
                   }
                   else
                   {
                      return ( arcSat < right.getSatellite() );
                   }
               }
               else
               {
                  return ( arcSource < right.getSource() );
               }
           }
           else
           {
              return( elevVar < right.getElevVariance() );
           }
       }
       else
       {
          return ( apriVar < right.getApriVariance() );
       }

    };

}  // End of namespace gpstk
