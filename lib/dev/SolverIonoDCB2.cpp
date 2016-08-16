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
//=============================================================================
//  Wei Wang, Wuhan University, 2016/03/08
//  Reference :Jin R,Jin S,Feng G (2012) M_DCB:Matlab code for estimating GNSS
//  satellite and receiver differential code biases. GPS Solu(16):541-548
//
//  Revision
//
//  2016/05/12, add the P1-C1 DCB for satellites and receivers
//
//=============================================================================
/**
 * @file SolverIonoDCB2.cpp
 */

#include "SolverIonoDCB2.hpp"
#include "MatrixFunctors.hpp"
#include "geometry.hpp"      // DEG_TO_RAD

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   //  Define some constants
   const double dcbFactor = 1e9/C_MPS;
 
     // Index initially assigned to this class
   int SolverIonoDCB2::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverIonoDCB2::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverIonoDCB2::getClassName() const
   { return "SolverIonoDCB2"; }


      // Compute the solution of the given equations set.
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
         InvalidSolver e("Unable to invert NormMatrix");
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
   gnssDataMap & SolverIonoDCB2::Process( gnssDataMap& gData,
                                          SourceIDSet& p1p2RecSet,
                                          SourceIDSet& c1p2RecSet,
                                          int interval )
   {
   

          // "currSatSet" stores satellites currently in view, and it is
          // related with the number of measurements.
          // Get a set with all satellites present in this GDS
      currSatSet = gData.getSatIDSet();
          // Get the number of satellites currently visible
      int numCurrentSV( currSatSet.size() );
	  // Get the number of receivers
      recSet = gData.getSourceIDSet();
      int numRec(recSet.size());
    
      gnssDataMap gMap = gData.extractSourceID(p1p2RecSet);
         // Firstly, estimate the p1-c1 dcb 
      if (!p1p2RecSet.empty())
      {
      	p1c1Estimate(gMap,p1p2RecSet);
      }
          // Number of SH coefficients
          // there are 13 sets of coefficients in a day,
          // correspnding to  0h, 2h, 4h...,24h respectively
      int numSH = (order+1) * (order+1);
      int numIonoCoef = numSH * (interval/2+1);

      numMeas = 0; //Reset numMeas
            
	    // Total number of unknowns:
            // P1-P2 DCB  for all receivers
            // P1-P2 DCB for satellites
            // ionospheric coefficients  
       numUnknowns = numRec + numCurrentSV + numIonoCoef;

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
              // resize the matrix  
         NormMatrix.resize(numUnknowns,numUnknowns,0.0);
         
         wVector.resize(numUnknowns,0.0);
     
         measVector.resize(numMeas,0.0);
         
         int count = 0;
            // fill the hMatrix and measVector according to the order of receivers
	 for (SourceIDSet::const_iterator itSour = recSet.begin();
	      itSour != recSet.end(); ++itSour)
	 {
	  SourceID rec(*(itSour));
          
	  bool isC1P2Rec(false);
              // distinguish the receiver type of this station
          SourceIDSet::iterator it = c1p2RecSet.find(rec);
          if ( it != c1p2RecSet.end() )
          {
             isC1P2Rec = true;
          }
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
		 itSat != tempSatSet.end(); ++itSat)
            {
               // record the position of element which is not zero
	      std::vector<int> index;  
          
              hVector.resize(numUnknowns,0.0);

              int satPos=std::distance(currSatSet.begin(),currSatSet.find(*itSat)); 
		// attention : PI is defined as P2-P1 , there we using -PI(P1-P2)
		// as measurements
              if ( isC1P2Rec )
              {
	         std::map<SatID,double>::iterator it = satP1C1DCB.find(*itSat);
                 if (it != satP1C1DCB.end())
                 {  
                   double p1c1dcb(it->second);
                    // there the P1-C1 DCB for satellite are eliminated from the observation
                   measVector(count) = (-piCom(seq)+p1c1dcb/dcbFactor)/mapFun(seq)*(-9.52437);
                 } 
		 else
		 {  
		     // if can not find the P1-C1 DCB, ignore it
	           measVector(count) = -piCom(seq)/mapFun(seq)*(-9.52437);	     
		 }
              }
              else
              {
		    // for P1P2RecSet
                 measVector(count) = -piCom(seq)/mapFun(seq)*(-9.52437); 
              }

              double coefDCB = 1.0/mapFun(seq)*(-9.52437);

              if (weightVector(seq)==0.0)
              {
                 weightVector(seq) = 1.0;
              }
             	  // the coefficient of P1-P2 DCB for receiver and satellite
              hVector(recPos) = hVector(satPos+numRec) = coefDCB ;
              index.push_back(recPos);
              index.push_back(satPos+numRec);
	      
		 // fill the SH coefficients	
               double Lat = DEG_TO_RAD*lat(seq);
                 // geomagnetic latitude
               Lat = std::asin(std::sin(DEG_TO_RAD*NGPLat)*std::sin(Lat)
                              + std::cos(DEG_TO_RAD*NGPLat)*std::cos(Lat)
                              * std::cos(DEG_TO_RAD*lon(seq)-DEG_TO_RAD*NGPLon));
	   
	        // transform geographic longitude into Sun-fixed longitude 
               double lonFixed = DEG_TO_RAD*lon(seq)+second*PI/43200.0 - PI;

	       double u = std::sin(Lat);
               int i = 0;
	      
	       double tp1 = (t2-t)/(t2-t1);
	       double tp2 = (t-t1)/(t2-t1);
               
	       int CoefStart(numRec+numCurrentSV+numSH*(t1-timeBegin));

	       for (int n=0;n<=order;n++)
	       {
		 for (int m=0;m<=n;m++)
                 {
	           double p = legendrePoly(n,m,u)*norm(n,m);
                   if (m==0)
		   {
                      hVector(CoefStart+i) = p*tp1 ;//An0
                      hVector(CoefStart+i+numSH) = p * tp2;// Next An0

                      index.push_back(CoefStart+i);
                      index.push_back(CoefStart+i+numSH);
                    
		   }
	           else   
                   {
                      hVector(CoefStart+i)= p*std::cos(m*lonFixed)*tp1;//Anm
                      hVector(CoefStart+i+numSH)=p*std::cos(m*lonFixed)*tp2;// Next Anm

                      index.push_back(CoefStart+i);
                      index.push_back(CoefStart+i+numSH);

                      i++;

                      hVector(CoefStart+i)= p*std::sin(m*lonFixed)*tp1;//Bnm
                      hVector(CoefStart+i+numSH)=p*std::sin(m*lonFixed)*tp2;//Next Bnm

                      index.push_back(CoefStart+i);
                      index.push_back(CoefStart+i+numSH);
                   }
                   i++;
		  }
	       }
		
		  //In order to reduce the memory consumption
		  //and speed up the process of matrix mutiplication
		for (std::vector<int>::iterator it = index.begin();
                     it != index.end(); it++)
                {
                 
                  wVector(*it) += hVector(*it) * weightVector(seq) * measVector(count);
                  
                  for (std::vector<int>::iterator it2 = index.begin();
                       it2 != index.end(); it2++) 

                  {    
                      
                    NormMatrix(*it,*it2) += hVector(*it) * weightVector(seq) * hVector(*it2);
                    
                  }

                }
	      seq++;
	      count++;
           }  // End of 'for (SatIDSet::...)'       

         }  // End of for '(gnssDataMap::...)'
     
  
     }  // End of for '(sourceMap::...)'

          // 1 constraint condition
      Matrix<double> consMatrix;
          // the sum of P1-P2 DCB for satellites is zero 
      consMatrix.resize(1,numUnknowns,0.0);
      for (int i = 0;i<numCurrentSV;i++)
      {
         consMatrix(0,i+numRec)= 1.0;	 
      }

        // update NormMatrix
      NormMatrix += transpose(consMatrix)*consMatrix;

  
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
         recState[*(it)] = sol(i)*dcbFactor;
         i++;
      }
      
      for (SatIDSet::const_iterator it2 = currSatSet.begin();
	   it2 != currSatSet.end(); it2++)
      {
         satState[*(it2)] = sol(i)*dcbFactor;
	 
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


   gnssDataMap SolverIonoDCB2::p1c1Estimate(gnssDataMap& gdsMap, 
                                            SourceIDSet& p1p2RecSet)
   throw (InvalidSolver)
   {
        // get the number of p1p2Rec;
      int numP1P2Rec(p1p2RecSet.size());
        // get the number of observed satellites
      SatIDSet satSet(gdsMap.getSatIDSet());

      int numSV(satSet.size());
        // the number of unknowns
      int numUnknowns = numP1P2Rec + numSV;

      Matrix<double> NMatrix;
      NMatrix.resize(numUnknowns,numUnknowns,0.0);

      Vector<double> WVector;
      WVector.resize(numUnknowns,0.0);

         // loop receivers
      for (SourceIDSet::const_iterator it = p1p2RecSet.begin();
           it != p1p2RecSet.end();++it)

      {
          SourceID rec(*(it));
              // get the sequence number of this receiver in the recSet
	  int recPos= std::distance(p1p2RecSet.begin(),p1p2RecSet.find(rec));

	    // extract the gnssDataMap of this receiver
          gnssDataMap tempMap(gdsMap.extractSourceID(rec));

	        // get the observed satellite of this receiver
	  SatIDSet tempSat(tempMap.getSatIDSet());

          int numSat(tempSat.size());

	  Matrix<double> aMatrix;
          aMatrix.resize(numSat,numUnknowns,0.0);
            // containers to store the mean C1-P1 DCB for this receiver
	    // if have P1, usually also have C1
	  Vector<double> mVector;
	  mVector.resize(numSat,0.0);
	  
          int i(0);

	  for (SatIDSet::const_iterator itSat = tempSat.begin();
	       itSat != tempSat.end();++itSat)
	  {   
              
             double sumInst(0.0);
             int numEpoch(0);

             int satPos=std::distance(satSet.begin(),satSet.find(*itSat));
	  
             aMatrix(i,recPos) = aMatrix(i,satPos+numP1P2Rec) = 1.0;

                // loop epochs
             for (gnssDataMap::const_iterator itEpoch = tempMap.begin();
	          itEpoch != tempMap.end(); ++itEpoch)
             {
	        CommonTime epoch(itEpoch->first);
                double c1(0.0),p1(0.0);
	        try
		{
		      // get C1 obervable
		  c1 = tempMap.getValue(epoch,rec,*itSat,TypeID::C1);
	              // get P1 observable
		  p1 = tempMap.getValue(epoch,rec,*itSat,TypeID::P1);
	        }
		     // if can not find value, skip this epoch
		catch (...)
	        { 
		  continue; 
	        }

		sumInst += (p1-c1);   
		 
		numEpoch++;

	      }

               // insert the mean value into map
             if (sumInst && numEpoch)
             {
	        mVector(i) = sumInst/numEpoch;
             }
              
             i++;
           }

	       // update NMatrix and WVector
	   NMatrix += transpose(aMatrix) * aMatrix;
	   WVector += transpose(aMatrix) * mVector;	      
	 
       } // End of 'for (SourceIDSet...)'

         // constraint condition
         // the sum of P1-C1 DCB for satellites is zero
      Matrix<double> consMatrix;
      consMatrix.resize(1,numUnknowns,0.0);
      for (int i = 0;i<numSV;i++)
      {
         consMatrix(0,i+numP1P2Rec)= 1.0;	 
      }

        // update NormMatrix
      NMatrix += transpose(consMatrix)*consMatrix;

	
        // estimate P1-C1 DCB using LS method
      Matrix<double> qMatrix;
    
      try
      {
         qMatrix = inverse( NMatrix );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert NormMatrix");
         GPSTK_THROW(e);
      }
        
         // Now, compute the Vector holding the solution...
      Vector<double> solution = qMatrix * WVector;

       // insert the solution into map
      recP1C1DCB.clear();
      satP1C1DCB.clear();
      
      int i(0);
      for (SourceIDSet::const_iterator it = p1p2RecSet.begin();
           it != p1p2RecSet.end();++it)
      {
         recP1C1DCB[*it] = solution(i)*dcbFactor;
         i++;
      }

      for (SatIDSet::const_iterator itSat = satSet.begin();
           itSat != satSet.end();++itSat)
      {
         satP1C1DCB[*itSat] = solution(i)*dcbFactor;
         i++;
      }
      

      return gdsMap;

   }  // End of 'SolverIonoDCB2::p1c1Estimate'

   double SolverIonoDCB2::getRecDCB( const SourceID& rec )
   {
     std::map<SourceID,double>::const_iterator it = recState.find(rec);
     if (it != recState.end())
     {
       return (*it).second ;	 
     }
     
     else 
     {
       cerr<<"The P1-P2 DCB for "<<asString(rec).substr(0,10)<<" is not found!"<<endl;	
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
       cerr<<"The P1-P2 DCB for "<<asString(sat)<<" is not found!"<<endl;	
       return 0.0;
     }
   }
    
   double SolverIonoDCB2::getSatP1C1DCB( const SatID& sat )
   {
     std::map<SatID,double>::const_iterator it = satP1C1DCB.find(sat);
     if (it != satP1C1DCB.end())
     {
       return (*it).second ;	 
     }
     
     else 
     {
       cerr<<"The P1-C1 DCB for "<<asString(sat)<<" is not found!"<<endl;	
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
         return ((2.0*n-1.0)*u*legendrePoly(n-1,m,u)-(n+m-1.0)
	                                      *legendrePoly(n-2,m,u))/(n-m);
      }
      
   }  // End of method 'SolverIonoDCB2::legendrePoly()'


}  // End of namespace gpstk

