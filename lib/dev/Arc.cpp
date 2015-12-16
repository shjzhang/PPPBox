#pragma ident "$Id: Arc.cpp 2596 2011-12-08 17:22:04Z shjzhang $"

/**
 * @file Arc.cpp
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
//  Copyright
//  ---------
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

namespace gpstk
{

       /// Copy constructor.
    Arc::Arc(const Arc &arc)
       throw(Arc::ArcException)
    {
       arcSource = arc.arcSource;
       arcSat = arc.arcSat;
       arcNum = arc.arcNum;
    }


       /// Assignment operator.
    Arc& Arc::operator=(const Arc& right)
       throw()
    {
       arcSource = right.arcSource;
       arcSat = right.arcSat;
       arcNum = right.arcNum;
       return (*this);
    }


       /// This ordering is required to be able
       /// to use a Arc as an index to a std::map, or as part of a
       /// std::set.
    bool Arc::operator<(const Arc& right) const
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

       /// Equality operator
    bool Arc::operator==(const Arc& right) const
    {
       return( ( arcSource == right.getSource()    ) &&
               ( arcSat    == right.getSatellite() ) &&
               ( arcNum    == right.getArcNumber() ) );
    }


}  // End of namespace gpstk
