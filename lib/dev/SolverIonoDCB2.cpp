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

         // Call initializing method
      Init();

   }  // End of 'SolverIonoDCB2::SolverIonoDCB2()'



      // Initializing method.
   void SolverIonoDCB2::Init(void)
   {

       
   }  // End of method 'SolverIonoDCB2::Init()'


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
   int SolverIonoDCB2::Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& consMatrix)
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      int gCol = static_cast<int>(designMatrix.cols());

      int gRow = static_cast<int>(designMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of designMatrix");
         GPSTK_THROW(e);
      }

      Matrix<double> AT = transpose(designMatrix);
      covMatrix.resize(gCol, gCol);
      solution.resize(gCol);

         // Temporary storage for covMatrix. It will be inverted later
      covMatrix = AT * designMatrix;
      // Let's try to invert AT*A   matrix
      try
      {
         covMatrix = inverseSVD( covMatrix );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert matrix covMatrix");
         GPSTK_THROW(e);
      }

         // Now, compute the Vector holding the solution...
      solution = covMatrix * AT * prefitResiduals;
    
	 Vector<double> temp= solution;
	 int i = 0;
	 for (SourceIDSet::const_iterator it = recSet.begin();
	       it != recSet.end(); it++)
      {
         cout<<*(it)<<" DCB is : "<<temp(i)*(3.3356)<<endl;
		 i++;
	  }
	 for (SatIDSet::const_iterator it2 = currSatSet.begin();
	       it2 != currSatSet.end(); it2++)
      {
         cout<<*(it2)<<" DCB is: "<<temp(i)*(3.3356)<<endl;
		 i++;
	  }

        // ... and the postfit residuals Vector
      postfitResiduals = prefitResiduals - designMatrix * solution;
 
         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverIonoDCB2::Compute()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssDataMap & SolverIonoDCB2::Process( gnssDataMap& gData, satValueMap& satMap )
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
		   // all the GPS satellites
		SatIDSet allSat;
		for (int i = 1;i<=32;i++)
       {
	     SatID sat(i,SatID::systemGPS);
		 allSat.insert(sat);
	    }
        
		 // find the satellites which are not present
		satNotInView.clear();
        for (SatIDSet::const_iterator it = allSat.begin();
		     it != allSat.end(); it++)
		
		{
		  if (currSatSet.find(*it) == currSatSet.end())	
			{
		   	  cout<<*(it)<<" is missed"<<endl;	
		      satNotInView.insert(*it);   	 	
				
			}
		
		}
		
         // get the sum of DCBs for satNotInView, will be as constraint 
		 // in DCB estimating, if all satellites are present , the sumDCB is zero
		double sumDCB(0.0);
        for (SatIDSet::const_iterator it2 = satNotInView.begin();
		     it2 != satNotInView.end(); it2++)
        {
	       satValueMap::iterator temp = satMap.find(*it2);
		   if (temp != satMap.end())
		   {
		     double tempDCB = temp->second;
			 sumDCB += tempDCB;
		   }
		}


            // Get the number of satellites currently visible
        int numCurrentSV( currSatSet.size() );
        //cout<<"Current SVs " << numCurrentSV<<endl;    
	     // Get the number of receivers
	 	recSet = gData.getSourceIDSet();
          
	 	int numRec(recSet.size());
		 // Number of SH coefficients
		int numIonoCoef = (order+1)*(order+1);

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
	//	for ( VariableSet::const_iterator itVar = satUnknowns.begin();
	//		  itVar != satUnknowns.end(); ++itVar)
	//	{
	//		cout<<asString(*itVar)<<endl;	
	//	}
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
         // a constraint condition is needed to separate the DCBs
		 // of satellite and receiver
		 numMeas+=1;
		 cout<<"The number of measurements : "<< numMeas << endl;
		 
            // Total number of unknowns 
         numUnknowns = numIonoCoef + numCurrentSV + numRec;
         cout<<"numUnknowns: "<<numUnknowns<<endl;
            

            // Build the vector of measurements (PI combination)
         measVector.resize(numMeas, 0.0);
        
            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns,0.0); 

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
            //   cout<< epoch<<endl;
			  // get the second of this epoch , will be used to transform geographic
			  // longitude into Sun-fixed longitude
			  double second = epoch.getSecondOfDay();

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

			   // the mapping function value
			  Vector<double> mapFun(dummy.getVectorOfTypeID(TypeID::ionoMap));

               // loop in the SatIDSet
			  for (SatIDSet::const_iterator itSat = tempSatSet.begin();
			       itSat != tempSatSet.end(); ++ itSat)
              {
				 int satPos=std::distance(currSatSet.begin(),currSatSet.find(*itSat)); 
					// attention : PI is defined as P2-P1 , there we using -PI(P1-P2)
					// as measVector
				 measVector(count) = -piCom(seq)/mapFun(seq)*(-9.52437); 
				// cout<<"P4 : "<<measVector(seq+count);
		
				   // fill the SH coefficients	
                 double Lat = DEG_TO_RAD*lat(seq);
              
				
				 if (lon(seq)>=180.0)
				 {
					 lon(seq)=lon(seq)-360.0;
				 }
                 double SunFixedLon = DEG_TO_RAD*lon(seq)+second*M_PI/43200.0 - M_PI;
	             double u = std::sin(Lat);
				 int i = 0;
			   for (int n=0;n<=order;n++)
				 for (int m=0;m<=n;m++)
                {
	        
                 if (m==0)
			    {
                 hMatrix(count,i)=
				               legendrePoly(n,m,u)*norm(n,m);//An0
			    }
	             else   
                 {
                  hMatrix(count,i)=
								legendrePoly(n,m,u)*norm(n,m)*std::cos(m*SunFixedLon);//Anm

                  i++;
                  hMatrix(count,i)=
								legendrePoly(n,m,u)*norm(n,m)*std::sin(m*SunFixedLon);//Bnm
                  }
                  i++;
     		
				}
                   // the coefficient of DCB for receiver
				 hMatrix(count,recPos+numIonoCoef)=1.0/mapFun(seq)*(-9.52437);
                   // the coefficient of DCB for satellite
				 hMatrix(count,satPos+numRec+numIonoCoef)= 1.0/mapFun(seq)*(-9.52437);
				 seq++; 
				 count++;
				 }  // End of 'for (SatIDSet::...)'       
             
			}  // End of for '(sourceMap::...)'

		 }  // End of for '(gnssDataMap::...)'
        
            // Finally, only one constraint condition: 
			// the sum of satellite DCBs is zero
		//	consMatrix.resize(1,numUnknowns,0.0);
		 for (int i = 0;i<numCurrentSV;i++)
         {
		  // consMatrix(0,i+numRec) = 1.0;	 
		   
		   hMatrix(numMeas-1,i+numRec) = 1.0;	 
		 }
         
		
        measVector(numMeas-1)= 0.0;
        
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverIonoDCB2 object with the appropriate
            // constructor.
         Compute( measVector,hMatrix,consMatrix);





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
      // reference:Satellite Orbits Montenbruck. P66
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
