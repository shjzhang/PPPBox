#pragma ident "$Id$"

/**
 * @file Distance.hpp
 * Calcualte the distance given two position.
 */

#ifndef DISTANCE_HPP
#define DISTANCE_HPP

//============================================================================
//
//  This file is a part of ROCKET software
//
//  The ROCKET software is based on GPSTK, RTKLIB and some other open source
//  GPS data processing software. The aim of ROCKET software is to compute
//  real-time GNSS orbit, clock and positions using GNSS data. 
//
//  The ROCKET software is developed at School of Geodesy and Geomatics, 
//  Wuhan University. 
//
//  Shoujian Zhang - Wuhan University, 2012
//
//============================================================================
//
//  Revision
//
//  2015/12/28  create this subroutine
//          
//  Warning:
//  Not supported untial now!
//
//============================================================================

#include <cmath>
#include "GNSSconstants.hpp"
#include "Position.hpp"
#include "Exception.hpp"


namespace gpstk
{

      /** @addtogroup math */
      //@{


      /** computes geodesic arc length and azimuth assuming that the points lie on 
       *  the reference ellipsoid defined by the input ELLIPSOID.  
       *  ELLIPSOID is a reference ellipsoid (oblate spheroid) object, a reference 
       *  sphere object, or a vector of the form [semimajor_axis, eccentricity].  
       *
       *  The output, ARCLEN, is expressed in the same length units as the semimajor
       *  axis of the ellipsoid or sphere.
       *
       *  To be updated:
       *  The azimuth should also be computed and returned for same geodetic 
       *  applications.
       */
   double distance(const Position& A, 
                   const Position& B, 
                   const EllipsoidModel *ell = NULL );


      /** Compute the arc length between the points expressed by latitude and 
       *  longitude.
       *
       * @param lat1  the latitude for the first point
       * @param lon1  the longitude for the first point
       * @param lat2  the latitude for the first point
       * @param lon2  the longitude for the first point
       *
       */
   double distance(double lat1, 
                   double lon1, 
                   double lat2, 
                   double lon2, 
                   const EllipsoidModel *ell = NULL); 
      //@}

}  // End of namespace gpstk

#endif   // DISTANCE_HPP
