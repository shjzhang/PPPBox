#pragma ident "$Id$"

/**
 * @file RinexClockFilterOperators.hpp
 * Operators for FileFilter using RINEX 2 & 3 Met file data.
 */

#ifndef GPSTK_RINEXCLOCKFILTEROPERATOR_HPP
#define GPSTK_RINEXCLOCKFILTEROPERATOR_HPP

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
//  Copyright
//
//  Shoujian Zhang, 2015, Wuhan University
//
//============================================================================

#include <set>

#include "CivilTime.hpp"
#include "FileFilter.hpp"
#include "RinexClockData.hpp"
#include "RinexClockHeader.hpp"

namespace gpstk
{
  /** @addtogroup RinexClock */
  //@{

  typedef std::unary_function<RinexClockHeader, bool> RinexClockDataUnaryOperator;
  typedef std::binary_function<RinexClockData, RinexClockData, bool> RinexClockDataBinaryOperator;

  /// This compares all elements of the RinexClockData with less than
  /// (only for those fields which the two obs data share).
  struct RinexClockDataOperatorLessThanFull : public RinexClockDataBinaryOperator
  {
  public:

    /// The set is a set of RinexClockType that the two files have in 
    /// common.  This is easily generated with the set_intersection
    /// STL function.  See difftools/rmwdiff.cpp for an example.
    RinexClockDataOperatorLessThanFull()
    {}

    bool operator()(const RinexClockData& l, const RinexClockData& r) const
    {
      // Compare the times, site, sat, bias, drift.

      if (l.time < r.time)
        return true;
      else if (l.time != r.time)
        return false;

         // Time is equal when come to this place

      // Then check the site 
      if (l.site < r.site)
         return true;
      else if ( l.site != r.site )
         return false;

         // site is equal when come to this place
         
      // Then check the sat
      if (l.sat < r.sat)
         return true;
      else if ( l.sat != r.sat)
         return false;

      // Then check the bias
      if (l.bias < r.bias)
         return true;
      else if ( l.bias != r.bias )
         return false;

      // Then check the drift 
      if (l.drift < r.drift )
         return true;
      else if ( l.drift != r.drift )
         return false;

      // the data is either == or > at this point
      return false;
    }

  };

  /// Compares only times.
  struct RinexClockDataOperatorLessThanSimple : public RinexClockDataBinaryOperator
  {
  public:

    bool operator()(const RinexClockData& l, const RinexClockData& r) const
    {
      if (l.time < r.time)
        return true;
      return false;
    }
  };

  /// Compares only times.
  struct RinexClockDataOperatorEqualsSimple : public RinexClockDataBinaryOperator
  {
  public:

    bool operator()(const RinexClockData& l, const RinexClockData& r) const
    {
      if (l.time == r.time)
        return true;
      return false;
    }
  };

  /// Combines RinexClockHeaders into a single header, combining comments and
  /// adding the appropriate RinexClockTypes.  This assumes that all the headers
  /// come from the same station for setting the other header fields.  After
  /// running touch() on a list of RinexClockHeader, the internal theHeader will
  /// be the merged header data for those files .
  struct RinexClockHeaderTouchHeaderMerge : public RinexClockDataUnaryOperator
  {
  public:

    RinexClockHeaderTouchHeaderMerge()
      : firstHeader(true)
    {}

    bool operator()(const RinexClockHeader& l)
    {
      if (firstHeader)
      {
        theHeader = l;
        firstHeader = false;
      }
      else
      {
        std::set<std::string> commentSet;

        // insert the comments to the set and let the set take care of uniqueness
        copy(theHeader.commentList.begin(),
             theHeader.commentList.end(),
             inserter(commentSet, commentSet.begin()));
        copy(l.commentList.begin(),
             l.commentList.end(),
             inserter(commentSet, commentSet.begin()));

        // then copy the comments back into theHeader
        theHeader.commentList.clear();
        copy(commentSet.begin(), commentSet.end(),
             inserter(theHeader.commentList,
                      theHeader.commentList.begin()));
      }

      return true;
    }

    bool firstHeader;
    RinexClockHeader theHeader;
  };

  /// This filter will return true for any data not within the specified time range.
  struct RinexClockDataFilterTime : public RinexClockDataUnaryOperator
  {

  public:

    RinexClockDataFilterTime(const CommonTime& startTime,
                             const CommonTime& endTime   )
      : start(startTime), end(endTime)
    {}

    bool operator() (const RinexClockData& l) const
    {
      if ( l.time < start || l.time >= end )
        return true;
      return false;
    }

  private:

    CommonTime start, end;

  };

  //@}

} // namespace gpstk

#endif
