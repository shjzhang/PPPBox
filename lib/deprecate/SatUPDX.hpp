#pragma ident "$Id: SatUPDX.hpp 2963 2012-08-12 15:07:30Z shjzhang $"

/**
 * @file SatUPDX.hpp
 * Class to store the satellite upd data.
 */

#ifndef GPSTK_SAT_UPDX_HPP
#define GPSTK_SAT_UPDX_HPP

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include <iostream>

namespace gpstk
{
   /** @addtogroup geodeticgroup */
   //@{

      /// This class will store the satellite updes.
   class SatUPDX
   {
   public:

         /// Default constructor
      SatUPDX()  
      {};

         /// Destructor.
      virtual ~SatUPDX() 
      {};

         /// member data

         ///< upd for WL
      double updSatWL;  

         ///< upd for WL
      double updSatLC;  

         ///< upd for L1
      double updSatL1;  

         ///< upd for L2
      double updSatL2;  


   }; // end class SatUPDX


      /** Output operator for SatUPDX
       *
       *  @param s     output stream 
       *  @param upd   SatUPDX that is sent to \c os
       */
   inline std::ostream& operator<<(std::ostream& os, const SatUPDX& upd) 
       throw()
   {
      os << ", l1     upd:" << upd.updSatL1
         << ", l2     upd:" << upd.updSatL2
         << std::endl;;

      return os;
   }

   //@}

}  // end namespace gpstk


#endif // GPSTK_SAT_UPD_HPP
