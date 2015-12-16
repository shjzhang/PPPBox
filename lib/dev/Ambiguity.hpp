#pragma ident "$Id: Ambiguity.hpp 2596 2011-12-08 17:22:04Z shjzhang $"

/**
 * @file Ambiguity.hpp
 * Class to express the ambiguity
 */

#ifndef GPSTK_AMBIGUITY_HPP
#define GPSTK_AMBIGUITY_HPP

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
//  - Create this program, 2013/07/20
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

#include "SatID.hpp"
#include "Exception.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{
      
      /**
       *  Class to express the ambiguity identity, which consists of
       *  the satellite id and arc number
       *
       */
   class Ambiguity
   {
   public:


         /// Default constructor for Ambiguity
      Ambiguity()
      {};


         /** Common constructor for Ambiguity.
          * @param sat        Satellite this Ambiguity is related to.
          * @param arcNumber  Satellite arc number
          */
      Ambiguity( const SatID& sat, 
                 const int& arcNumber)
          : arcSat(sat), arcNum(arcNumber)
      {};


         /// Copy constructor.
      Ambiguity(const Ambiguity &arc)
         throw();


         /// Assignment operator.
      Ambiguity& operator=(const Ambiguity& right)
         throw();



         /// Get satellite this variable is assigned to (if any).
      SatID getSatellite() const
      { return arcSat; };


         /** Set satellite this variable is assigned to.
          *
          * @param sat   Internal, specific SatID of variable.
          */
      Ambiguity& setSatellite(const SatID& sat)
      { arcSat= sat; return (*this); };


         /// Get satellite this variable is assigned to (if any).
      int getArcNumber() const
      { return arcNum; };


         /** Set satellite this variable is assigned to.
          *
          * @param satellite  Internal, specific SatID of variable.
          */
      Ambiguity& setAmbiguityNumber(const double& arcNumber)
      { arcNum = arcNumber; return (*this); };


         /// This ordering is required to be able
         /// to use a Ambiguity as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Ambiguity& right) const;


         /// Equality operator
      virtual bool operator==(const Ambiguity& right) const;


         /// Destructor
      virtual ~Ambiguity() {};


   protected:


         /// satellite of this arc.
      SatID arcSat;


         /// arc number of this edge .
      int arcNum;


   }; // End of class 'Ambiguity'


   inline std::ostream& operator<<( std::ostream& s,
                                    const Ambiguity& amb)
   {
      s << amb.getSatellite() << "   "
        << amb.getArcNumber();

      return s;
   };

      /// Struct for ambiguity data store in 'Kalman Filter' 
      
      /// A structure used to store the ambiguity data
   struct ambFilterData 
   {
         // Default constructor initializing the data in the structure
      ambFilterData() 
         : windowSize(0), ambiguity(0.0) 
      {};

      int windowSize;

      double ambiguity;                  ///< Ambiguity value.
      std::map<Ambiguity, double> aCovMap;  ///< Ambiguities covariance values.

   };


      /// Handy type definition

   typedef std::set<Ambiguity> AmbiguitySet;

   typedef std::map<Ambiguity, ambFilterData> AmbiguityDataMap;

   namespace StringUtils
   {
      inline std::string asString(const Ambiguity& v)
      {
         std::ostringstream oss;
         oss << v.getSatellite() << "   "
             << v.getArcNumber();

         return oss.str();
      }
   }
      //@}

}  // End of namespace gpstk
#endif   // GPSTK_AMBIGUITY_HPP
