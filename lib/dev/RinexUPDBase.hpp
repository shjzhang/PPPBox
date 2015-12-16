#pragma ident "$Id: RinexUPDBase.hpp 2635 2012-08-11 12:56:47Z shjzhang $"

/**
 * @file RinexUPDBase.hpp
 * Base class for satellite bias file data
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
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================
//

#ifndef GPSTK_RINEX_UPD_BASE_INCLUDE
#define GPSTK_RINEX_UPD_BASE_INCLUDE

#include "FFData.hpp"

namespace gpstk
{
   /** @defgroup RinexUPD Satellite UPD format file I/O */
   //@{

      /// This class is here to make readable inheritance diagrams.
   class RinexUPDBase : public FFData
   {
   public:

         /// Destructor per the coding standards
      virtual ~RinexUPDBase() {}

   };

   //@}

}  // namespace

#endif   // GPSTK_RINEX_UPD_BASE_INCLUDE
