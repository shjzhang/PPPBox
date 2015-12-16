#pragma ident "$Id$"



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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================






/**
 * @file FFData.cpp
 * Formatted File Data base class
 */

#include "FFData.hpp"
#include "FFStream.hpp"

namespace gpstk
{
   void FFData::putRecord(FFStream& s) const 
      throw(FFStreamError, gpstk::StringUtils::StringException)
   { 
      try
      {
         s.tryFFStreamPut(*this); 
      }
      catch(FFStreamError& e)
      {
         GPSTK_RETHROW(e);
      }
      catch(gpstk::StringUtils::StringException& e)
      {
         GPSTK_RETHROW(e);
      }

   }
   
   void FFData::getRecord(FFStream& s)
      throw(FFStreamError, gpstk::StringUtils::StringException)
   { 
      try
      {
         s.tryFFStreamGet(*this); 
      }
      catch(FFStreamError& e)
      {
         GPSTK_RETHROW(e);
      }
      catch(gpstk::StringUtils::StringException& e)
      {
         GPSTK_RETHROW(e);
      }
   }
   
   std::ostream& operator<<(std::ostream& o, const FFData& f)
         throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      FFStream* ffs = dynamic_cast<FFStream*>(&o);
      if (ffs)
      {
         f.putRecord(*ffs);
         return o;
      }
      else
      {
         FFStreamError e("operator<< stream argument must be an FFStream");
         GPSTK_THROW(e);
      }

   }

   std::istream& operator>>(std::istream& i, FFData& f)
         throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      try
      {
         FFStream* ffs = dynamic_cast<FFStream*>(&i);
         if (ffs)
         {
            try
            {
              f.getRecord(*ffs);
              return i;
            }
            catch(FFStreamError& e)
            {
               GPSTK_RETHROW(e);
            }
            catch(gpstk::StringUtils::StringException& e)
            {
               GPSTK_RETHROW(e);
            }
         }
         else
         {
            FFStreamError e("operator<< stream argument must be an FFStream");
            GPSTK_THROW(e);
         }
      }
      catch(FFStreamError& e)
      {
         GPSTK_RETHROW(e);
      }
      catch(gpstk::StringUtils::StringException& e)
      {
         GPSTK_RETHROW(e);
      }
   }
}
