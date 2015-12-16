#pragma ident "$Id: Ambiguity.cpp 2596 2011-12-08 17:22:04Z shjzhang $"

/**
 * @file Ambiguity.cpp
 * Class which represent the ambiguity
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
//  Shoujian Zhang - Wuhan University, 2013
//
//============================================================================

#include <set>
#include <map>
#include <list>
#include <queue>
#include <vector>

#include "Ambiguity.hpp"

namespace gpstk
{

       /// Copy constructor.
    Ambiguity::Ambiguity(const Ambiguity &arc)
       throw()
    {
       arcSat = arc.arcSat;
       arcNum = arc.arcNum;
    }


       /// Assignment operator.
    Ambiguity& Ambiguity::operator=(const Ambiguity& right)
       throw()
    {
       arcSat = right.arcSat;
       arcNum = right.arcNum;
       return (*this);
    }


       /// This ordering is required to be able
       /// to use a Ambiguity as an index to a std::map, or as part of a
       /// std::set.
    bool Ambiguity::operator<(const Ambiguity& right) const
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

       /// Equality operator
    bool Ambiguity::operator==(const Ambiguity& right) const
    {
       return( ( arcSat    == right.getSatellite() ) &&
               ( arcNum    == right.getArcNumber() ) );
    }


}  // End of namespace gpstk
