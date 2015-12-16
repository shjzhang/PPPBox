#pragma ident "$Id: Vertex.hpp 1889 2012-06-15 15:47:23Z shjzhang $"

/**
 * @file Vertex.hpp 
 * Simple index to represent the vertex of the observation net.
 */

#ifndef GPSTK_VERTEX_HPP
#define GPSTK_VERTEX_HPP

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
//  Modifications
//  -------------
//
//  - Create this subroutine, 2012/06/15.
//
//  Copyright
//  ---------
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include "SatID.hpp"
#include "SourceID.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */

   class Vertex
   {
   public:

         /// The type of vertex.
      enum VertexType
      {
         Unknown,   ///< Unknown type
         Receiver,  ///< Receiver
         Satellite, ///< Satellite
      };


         /// empty constructor, creates an unknown source data object
      Vertex()
         : vertexType(Unknown), vertexName("")
      {};


         /// Explicit constructor
      Vertex(const SourceID& source )
      {
         vertexType = Receiver;
         vertexName = StringUtils::asString(source);
      };


         /// Explicit constructor
      Vertex(const SatID& sat )
      {
         vertexType = Satellite;
         vertexName = StringUtils::asString(sat);
      };


         /// Copy constructor
      Vertex(const Vertex& s)
         : vertexType(s.vertexType), vertexName(s.vertexName)
      {};


         /// Assignment operator
      Vertex& operator=(const Vertex& right)
      {
         if ( this == &right )
         {
            return (*this);
         }

         vertexType = right.vertexType;
         vertexName = right.vertexName;

         return *this;

      }  // End of 'Vertex::operator=()'


         /// Equality operator requires all fields to be the same.
      bool operator==(const Vertex& right) const
      {

         return (vertexType==right.vertexType && 
                 vertexName==right.vertexName);

      }  // End of 'Vertex::operator==()'


         /// Ordering is arbitrary but required to be able to use a Vertex
         /// as an index to a std::map. If an application needs
         /// some other ordering, inherit and override this function.
      bool operator<(const Vertex& right) const
      {

         if (vertexType == right.vertexType)
         {
            return vertexName < right.vertexName;
         }
         else
         {
            return vertexType < right.vertexType;
         }

      }  // End of 'Vertex::operator<()'


         /// Inequality operator
      bool operator!=(const Vertex& right) const
      { return !(operator==(right)); }


         /// 'Greater than' operator
      bool operator>(const Vertex& right) const
      {  return (!operator<(right) && !operator==(right)); }


         /// 'Less or equal than' operator
      bool operator<=(const Vertex& right) const
      { return (operator<(right) || operator==(right)); }


         /// 'Greater or equal than' operator
      bool operator>=(const Vertex& right) const
      { return !(operator<(right)); }


         /// Convenience method used by dump().
      static std::string convertVertexTypeToString(VertexType vt)
      {
         switch(vt)
         {
            case Receiver : return "Receiver";    break;
            case Satellite: return "Satellite";   break;
            case Unknown  : return "Unknown";     break;
            default       : return "??";          break;
         };
      }

         /// Convenience output method
      std::ostream& dump(std::ostream& s) const
      {
         s << convertVertexTypeToString(vertexType) << " " << vertexName;
         return s;
      };


         /// Destructor
      virtual ~Vertex() {};


   private:


         /// Type of the data source (GPS receiver, Inertial system, etc)
      VertexType  vertexType;

         /// Name of the data source
      std::string vertexName;


   }; // End of class 'Vertex'


   namespace StringUtils
   {

         // convert this object to a string representation
      inline std::string asString(const Vertex& p)
      {

         std::ostringstream oss;
         p.dump(oss);

         return oss.str();

      }  // End of function 'asString()'

   }  // End of namespace StringUtils



      // Stream output for Vertex
   inline std::ostream& operator<<( std::ostream& s,
                             const Vertex& p )
   {

      p.dump(s);

      return s;

   }  // End of 'operator<<'


   /// Handy type definition

      // Set holding the "Vertex"
   typedef std::set<Vertex> VertexSet;

      // Map holding values related to "Vertex"
   typedef std::map<Vertex, int> VertexValueMap;



}  // End of namespace gpstk
#endif   // GPSTK_VERTEX_HPP
