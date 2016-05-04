//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//
//  Wei Wang, Wuhan University, 2016/03/08
//  Reference :Jin R,Jin S,Feng G (2012) M_DCB:Matlab code for estimating GNSS
//  satellite and receiver differential code biases. GPS Solu(16):541-548
//=============================================================================

/**
 * @file SolverIonoDCB2.cpp
 */

#include "SolverIonoDCB2.hpp"
#include "MatrixFunctors.hpp"
#include "geometry.hpp"      // DEG_TO_RAD
#include <cmath> 
#include <time.h>



using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverIonoDCB2::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverIonoDCB2::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverIonoDCB2::getClassName() const
   { return "SolverIonoDCB2"; }


      // Common constructor.
   SolverIonoDCB2::SolverIonoDCB2(int SHOrder)
      :order(SHOrder)
   {

         // Set the equation system structure
      prepare();

   }



      // Compute the solution of the given equations set.
      //
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      // @param weightMatrix      Matrix of weights
      //
      // \warning A typical Kalman filter works with the measurements noise
      // covariance matrix, instead of the matrix of weights. Beware of this
      // detail, because this method uses the later.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SolverIonoDCB2::Compute( const Matrix<double>& normMatrix,
                                const Vector<double>& wVector )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;
      
      Matrix<double> qMatrix;

      // Let's try to invert AT*A   matrix
      try
      {
         qMatrix = inverse( normMatrix );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert matrix normMatrix");
         GPSTK_THROW(e);
      }
        
         // Now, compute the Vector holding the solution...
      sol = qMatrix * wVector;
        
         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverIonoDCB2::Compute()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssDataMap & SolverIonoDCB2::Process( gnssDataMap& gData, int interval )
    {


          // Please note that there are two different sets being defined:
          //
          // - "currSatSet" stores satellites currently in view, and it is
          //   related with the number of measurements.
          //
          // - "satSet" stores satellites being processed; this set is
          //   related with the number of unknowns.
          //
          // Get a set with all satellites present in this GDS
      currSatSet = gData.getSatIDSet();
          // Get the number of satellites currently visible
      int numCurrentSV( currSatSet.size() );
	  // Get the number of receivers
      recSet = gData.getSourceIDSet();
          
      int numRec(recSet.size());
	  // Number of SH coefficients
          // there are 13 sets of coefficients in a day,
          // correspnding to the 0h, 2h, 4h...,24h respectively
      int numIonoCoef = (order+1)*(order+1)*(interval/2+1);

      numMeas = 0; //Reset numMeas

	   // create variables for the program
      recUnknowns.clear();
      satUnknowns.clear();

     	  // let's create the receiver-related unkonwns
      for (TypeIDSet::const_iterator it = recIndexedTypes.begin();
	   it != recIndexedTypes.end(); it++)
      {
		 
	Variable var((*it));

       for (SourceIDSet::const_iterator itRec = recSet.begin();
	    itRec != recSet.end(); itRec++)
        {
         	 // set satellite			 
	   var.setSource((*itRec));
	     // insert in 'varUnknowns'
	   recUnknowns.insert(var);
	 }  
		
       }
        // let's create the satellite-related unkonwns
       for (TypeIDSet::const_iterator it = satIndexedTypes.begin();		     
            it != satIndexedTypes.end(); it++)
        {
		 
	 Variable var((*it));
	 for (SatIDSet::const_iterator itSat = currSatSet.begin();
	      itSat != currSatSet.end(); itSat++)
 	 {
         	 // set satellite			 
	   var.setSatellite((*itSat));
	     // insert in 'varUnknowns'
	   satUnknowns.insert(var);
	  }  

     	}
		 
            // Total number of unknowns 
         numUnknowns = numIonoCoef + numCurrentSV + numRec;
             
           //get the number of measurements
       for (gnssDataMap::const_iterator itEpoch = gData.begin();
	    itEpoch != gData.end(); ++itEpoch)
	{
 
	 for (sourceDataMap::const_iterator itSour = (itEpoch->second).begin();
	      itSour != (itEpoch->second).end(); ++itSour)
	{
          numMeas += (itSour->second).numSats();
	
	}

        }
	 
	 CommonTime epochBegin( gData.begin()->first );
	 double secondBegin = epochBegin.getSecondOfDay();
         int timeBegin = int(secondBegin/7200.0);
         
         NormMatrix.resize(numUnknowns,numUnknowns,0.0);
         
         wVector.resize(numUnknowns,0.0);
     
         measVector.resize(numMeas,0.0);
         
         int count = 0;
            // fill the hMatrix and measVector according to the order of receivers
	 for (SourceIDSet::const_iterator itSour = recSet.begin();
	      itSour != recSet.end(); ++itSour)
	 {
	  SourceID rec(*(itSour));
              // get the sequence number of this receiver in the recSet
	  int recPos= std::distance(recSet.begin(),recSet.find(rec));
              // extract the gnssDataMap of this receiver
          gnssDataMap tempMap(gData.extractSourceID(rec));
              // loop epochs
          for (gnssDataMap::const_iterator itEpoch = tempMap.begin();
	         itEpoch != tempMap.end(); ++itEpoch)
	  {
	    CommonTime epoch(itEpoch->first);
	         // get the second of this epoch , will be used to transform geographic
	         // longitude into Sun-fixed longitude
	    double second = epoch.getSecondOfDay();
            
            double t = second/7200.0;
            int t1 = int(t);
            int t2 = t1 + 1;
            
            sourceDataMap tempSourMap(itEpoch->second);
		 // the SatIDSet corresponding to this receiver
	    SatIDSet tempSatSet = tempSourMap.getSatIDSet();
   
	    int seq = 0;
            satTypeValueMap dummy(tempSourMap[rec]);
                  // extract the PI combination
	    Vector<double> piCom(dummy.getVectorOfTypeID(TypeID::PI));

		 // the latitude of ionospheric pierce points
	    Vector<double> lat(dummy.getVectorOfTypeID(TypeID::LatIPP));

		 // the longitude of ionospheric pierce points
	    Vector<double> lon(dummy.getVectorOfTypeID(TypeID::LonIPP));
             
                 // the weight of PI
	    Vector<double> weightVector(dummy.getVectorOfTypeID(TypeID::weight));
	         // the mapping function value
	    Vector<double> mapFun(dummy.getVectorOfTypeID(TypeID::ionoMap));

               // loop in the SatIDSet
	   for (SatIDSet::const_iterator itSat = tempSatSet.begin();
		itSat != tempSatSet.end(); ++ itSat)
            {
               // record the position of element which is not zero
	      std::vector<int> index;  
          
              hVector.resize(numUnknowns,0.0);
              
              int satPos=std::distance(currSatSet.begin(),currSatSet.find(*itSat)); 
		// attention : PI is defined as P2-P1 , there we using -PI(P1-P2)
		// as measVector
	      measVector(count) = -piCom(seq)/mapFun(seq)*(-9.52437); 
              if (weightVector(seq)==0.0)
              {
                 weightVector(seq) = 1.0;
              }
              
		// the coefficient of DCB for receiver
	      hVector(recPos)=1.0/mapFun(seq)*(-9.52437);
              index.push_back(recPos);
                 // the coefficient of DCB for satellite
	      hVector(satPos+numRec)= 1.0/mapFun(seq)*(-9.52437);
              index.push_back(satPos+numRec);

		 // fill the SH coefficients	
              double Lat = DEG_TO_RAD*lat(seq);
                   //geomagnetic latitude
              Lat = std::asin(std::sin(DEG_TO_RAD*NGPLat)*std::sin(Lat)
                                 +std::cos(DEG_TO_RAD*NGPLat)*std::cos(Lat)
                                 *std::cos(DEG_TO_RAD*lon(seq)-DEG_TO_RAD*NGPLon));
	   
	        // transform geographic longitude into Sun-fixed longitude 
               double SunFixedLon = DEG_TO_RAD*lon(seq)+second*M_PI/43200.0 - M_PI;

	       double u = std::sin(Lat);
               int i = 0;
	         // the start position of ionosphereic coefficient
               int CoefStart = numCurrentSV+numRec+(order+1)*(order+1)*(t1-timeBegin);
	       for (int n=0;n<=order;n++)
	       for (int m=0;m<=n;m++)
               {
	        
                 if (m==0)
		 {
                    hVector(CoefStart+i)=
                                  legendrePoly(n,m,u)*norm(n,m)*(t2-t)/(t2-t1);//An0
                    hVector(CoefStart+i+(order+1)*(order+1))=
                                  legendrePoly(n,m,u)*norm(n,m)*(t-t1)/(t2-t1);//An0

                   index.push_back(CoefStart+i);
                   index.push_back(CoefStart+i+(order+1)*(order+1));
                    
		 }
	         else   
                 {
                   hVector(CoefStart+i)=
		     legendrePoly(n,m,u)*norm(n,m)*std::cos(m*SunFixedLon)*(t2-t)/(t2-t1);//Anm
                   hVector(CoefStart+i+(order+1)*(order+1))=
		     legendrePoly(n,m,u)*norm(n,m)*std::cos(m*SunFixedLon)*(t-t1)/(t2-t1);//Anm

                   index.push_back(CoefStart+i);
                   index.push_back(CoefStart+i+(order+1)*(order+1));

                   i++;

                   hVector(CoefStart+i)=
		     legendrePoly(n,m,u)*norm(n,m)*std::sin(m*SunFixedLon)*(t2-t)/(t2-t1);//Bnm
                   hVector(CoefStart+i+(order+1)*(order+1))=
		     legendrePoly(n,m,u)*norm(n,m)*std::sin(m*SunFixedLon)*(t-t1)/(t2-t1);//Bnm

                   index.push_back(CoefStart+i);
                   index.push_back(CoefStart+i+(order+1)*(order+1));
                 }

                  i++;
     		
		 }
		  //in order to reduce the memory consumption
		  //and speed up the process of matrix mutiplication
		for (std::vector<int>::iterator it = index.begin();
                     it != index.end(); it++)
                {
                 
                  wVector(*it) += hVector(*it) * weightVector(seq) * measVector(count);
                  
                  for (std::vector<int>::iterator it2 = it;
                       it2 != index.end(); it2++) 

                  {
                    NormMatrix(*it,*it2)=NormMatrix(*it2,*it) 
                                  += hVector(*it) * weightVector(seq) * hVector(*it2);
                    
                  }

                }
	      seq++;
	      count++;
           }  // End of 'for (SatIDSet::...)'       

         }  // End of for '(sourceMap::...)'
     
     }  // End of for '(gnssDataMap::...)'
          // Finally, only one constraint condition: 
	  // the sum of satellite DCBs is zero
      Matrix<double> consMatrix;
      consMatrix.resize(1,numUnknowns,0.0);
      for (int i = 0;i<numCurrentSV;i++)
      {
        consMatrix(0,i+numRec) = 1.0;	 
      }
        // update NormMatrix
      NormMatrix+=transpose(consMatrix)*consMatrix;	
 
         // Call the Compute() method with the defined equation model.
         // This equation model MUST HAS BEEN previously set, usually when
         // creating the SolverIonoDCB2 object with the appropriate
         // constructor.
      Compute(NormMatrix,wVector);

        // insert the result into corrsponding containers
      int i = 0;
      for (SourceIDSet::const_iterator it = recSet.begin();
	   it != recSet.end(); it++)
      {
	    // Now, the unit of DCBs is ns
         recState[*(it)] = sol(i)*3.3356;
         i++;
      }
      for (SatIDSet::const_iterator it2 = currSatSet.begin();
	   it2 != currSatSet.end(); it2++)
      {
         satState[*(it2)] = sol(i)*3.3356;
	 i++;
      }
      
      IonoCoef.resize(numIonoCoef,0.0);
      for (int j = 0; j< numIonoCoef;j++)
      {
       IonoCoef(j) = sol(i);	  
       i++;
      }
     
   
     return gData;


   }  // End of method 'SolverIonoDCB2::Process()'



   SolverIonoDCB2& SolverIonoDCB2::prepare(void)
   {

         // First, let's clear all the set that storing the variable
      recIndexedTypes.clear();
      satIndexedTypes.clear();
	 // fill the types that are source-indexed
      recIndexedTypes.insert(TypeID::recP1P2DCB);
	 // fill the types that are satellite-indexded
      satIndexedTypes.insert(TypeID::satP1P2DCB);
    
      return (*this);

   }  // End of method 'SolverIonoDCB2::prepare()'

   double SolverIonoDCB2::getRecDCB( const SourceID& rec )
   {
     std::map<SourceID,double>::const_iterator it = recState.find(rec);
     if (it != recState.end())
     {
       return (*it).second ;	 
     }
     
     else 
     {
       cerr<<asString(rec).substr(0,10)<<"is not found!";	
       return 0.0;
     }

   }
   
   double SolverIonoDCB2::getSatDCB( const SatID& sat )
   {
     std::map<SatID,double>::const_iterator it = satState.find(sat);
     if (it != satState.end())
     {
       return (*it).second ;	 
     }
     
     else 
     {
       cerr<<asString(sat)<<"is not found!";	
       return 0.0;
     }
   }
  

    double SolverIonoDCB2::norm( int n, int m ) 
    {
      // The input should be n >= m >= 0
      double fac(1.0);
      for(int i = (n-m+1); i <= (n+m); i++)
      {
         fac = fac * double(i);
      }

      double delta  = (m == 0) ? 1.0 : 0.0;

      double num = (2.0 * n + 1.0) * (2.0 - delta);

      // We should make sure fac!=0, but it won't happen on the case,
      // so we just skip handling it
      double out = std::sqrt(num/fac);                  
      
      return out;

   }  // End of method 'SolverIonoDCB2::norm'
   
  
      //  Legendre polynomial
   double SolverIonoDCB2::legendrePoly(int n, int m, double u)
   {
      if(0==n && 0==m)
      {
         return 1.0;
      }
      else if(m==n)
      {
         return (2.0*m-1.0)*std::sqrt(1.0-u*u)*legendrePoly(n-1,m-1,u);
      }
      else if(n==m+1)
      {
         return (2.0*m+1)*u*legendrePoly(m,m,u);
      }
      else
      {
         return ((2.0*n-1.0)*u*legendrePoly(n-1,m,u)-(n+m-1.0)*legendrePoly(n-2,m,u))/(n-m);
      }
      
   }  // End of method 'SolverIonoDCB2::legendrePoly()'


}  // End of namespace gpstk
