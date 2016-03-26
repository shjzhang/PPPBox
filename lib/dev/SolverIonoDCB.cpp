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
 * @file SolverIonoDCB.cpp
 */

#include "SolverIonoDCB.hpp"
#include "MatrixFunctors.hpp"
#include "geometry.hpp"      // DEG_TO_RAD
#include <cmath> 

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverIonoDCB::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverIonoDCB::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverIonoDCB::getClassName() const
   { return "SolverIonoDCB"; }


      // Common constructor.
   SolverIonoDCB::SolverIonoDCB(int SHOrder)
      :order(SHOrder),firstTime(true)
   {

         // Set the equation system structure
      prepare();

         // Call initializing method
      Init();

   }  // End of 'SolverIonoDCB::SolverIonoDCB()'



      // Initializing method.
   void SolverIonoDCB::Init(void)
   {

         // Pointer to default stochastic model for satellite DCBs
	  
	  satDCBModel.setQprime(3e-16);
      pSatDCBStoModel = &satDCBModel;
         // for receiver DCBs
	  recDCBModel.setQprime(3e-16);
      pRecDCBStoModel = &recDCBModel;  

      CoefModel.setQprime(3e-4);

	  pCoefStoModel = &CoefModel;
       
   }  // End of method 'SolverIonoDCB::Init()'


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
   int SolverIonoDCB::Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& weightMatrix)
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("Weight matrix is not square");
         GPSTK_THROW(e);
      }

      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension of \
weightMatrix");
         GPSTK_THROW(e);
      }

      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of designMatrix");
         GPSTK_THROW(e);
      }

      if (!(phiMatrix.isSquare()))
      {
         InvalidSolver e("phiMatrix is not square");
         GPSTK_THROW(e);
      }

      int phiRow = static_cast<int>(phiMatrix.rows());
      if (!(phiRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of phiMatrix");
         GPSTK_THROW(e);
      }

      if (!(qMatrix.isSquare()))
      {
         InvalidSolver e("qMatrix is not square");
         GPSTK_THROW(e);
      }

      int qRow = static_cast<int>(qMatrix.rows());
      if (!(qRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of qMatrix");
         GPSTK_THROW(e);
      }

         // After checking sizes, let's invert the matrix of weights in order
         // to get the measurements noise covariance matrix, which is what we
         // use in the "SimpleKalmanFilter" class
      Matrix<double> measNoiseMatrix;

      try
      { 
         measNoiseMatrix = inverseChol(weightMatrix);
      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute measurements noise \
covariance matrix.");
         GPSTK_THROW(e);
      }


      try
      {

            // Call the Kalman filter object.
         kFilter.Compute( phiMatrix,
                          qMatrix,
                          prefitResiduals,
                          designMatrix,
                          measNoiseMatrix);

      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }

         // Store the solution
      solution = kFilter.xhat;

         // Store the covariance matrix of the solution
      covMatrix = kFilter.P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverIonoDCB::Compute()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssDataMap & SolverIonoDCB::Process(CommonTime& epoch, gnssDataMap& gData)
      throw(ProcessingException)
   {

      try
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
       // SatIDSet currSatSet( gData.getSatIDSet() );

         for (int i = 1;i<=32;i++)
       {
	     SatID sat(i,SatID::systemGPS);
		 currSatSet.insert(sat);
	    }


            // Get the number of satellites currently visible
        int numCurrentSV( currSatSet.size() );
        //cout<<"Current SVs " << numCurrentSV<<endl;    
	     // Get the number of receivers
	 	SourceIDSet recSet(gData.getSourceIDSet());
          
	 	int numRec(recSet.size());
		 // Number of SH coefficients
		int numIonoCoef = (order+1)*(order+1);

        numMeas  = 0; //Reset numMeas

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
            // State Transition Matrix (PhiMatrix)
         phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Noise covariance matrix (QMatrix)
         qMatrix.resize(numUnknowns, numUnknowns, 0.0);


            // Build the vector of measurements (PI combination)
         measVector.resize(numMeas, 0.0);
            // Weights matrix
         rMatrix.resize(numMeas, numMeas, 0.0);
            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns,0.0); 

         int count = 0;
        for (gnssDataMap::const_iterator itEpoch = gData.begin();
			  itEpoch != gData.end(); ++itEpoch)
		{
			CommonTime epoch(itEpoch->first);
			  // get the second of this epoch , will be used to transform geographic
			  // longitude into Sun-fixed longitude
			double second = epoch.getSecondOfDay();
			  // loop epochs
			for (sourceDataMap::const_iterator itSour = (itEpoch->second).begin();
			     itSour != (itEpoch->second).end(); ++itSour)
			{
			  SourceID rec(itSour->first);
			   // get the sequence number of this receiver in the recSet
			  int recPos= std::distance(recSet.begin(),recSet.find(rec));
		      satTypeValueMap dummy(itSour->second);
			   // the SatIDSet corresponding to this receiver
	          SatIDSet tempSatSet= dummy.getSatID();
			  int seq = 0;

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
				 measVector(count) = piCom(seq)/mapFun(seq)*(-9.52437); 
				// cout<<"P4 : "<<measVector(seq+count);
				 // the weight 
                 rMatrix(count,count) = 10000.0;
				   // the coefficient of DCB for receiver
				 hMatrix(count,recPos)= 1.0/mapFun(seq)*(-9.52437);
                   // the coefficient of DCB for satellite
				 hMatrix(count,satPos+numRec)= 1.0/mapFun(seq)*(-9.52437);
				   // fill the SH coefficients	
                 double Lat = DEG_TO_RAD*lat(seq);
                 double SunFixedLon = DEG_TO_RAD*lon(seq)+second*M_PI/43200.0 - M_PI;
				  
	             double u = std::sin(Lat);
				 int i = 0;
			   for (int n=0;n<=order;n++)
				 for (int m=0;m<=n;m++)
                {
	        
                 if (m==0)
			    {
                 hMatrix(count,numCurrentSV+numRec+i)=
				               legendrePoly(n,m,u)*norm(n,m);//An0
			    }
	             else   
                 {
                  hMatrix(count,numCurrentSV+numRec+i)=
								legendrePoly(n,m,u)*norm(n,m)*std::cos(m*SunFixedLon);//Anm

                  i++;
                  hMatrix(count,numCurrentSV+numRec+i)=
								legendrePoly(n,m,u)*norm(n,m)*std::sin(m*SunFixedLon);//Bnm
                  }
                  i++;
     		
				}
                 
				 seq++; 
				 count++;
				 }  // End of 'for (SatIDSet::...)'       
             
			}  // End of for '(sourceMap::...)'

		 }  // End of for '(gnssDataMap::...)'
        
            // Finally, only one constraint condition: 
			// the sum of satellite DCBs is zero
		// consMatrix.resize(1,numUnknowns,0.0);
		 for (int i = 0;i<numCurrentSV;i++)
         {
		   hMatrix(numMeas-1,i+numRec) = 1.0;	 
		 }
         
		//consVector.resize(1,0.0);
		rMatrix(numMeas-1,numMeas-1)= 10000.0;
        measVector(numMeas-1)= 0.0;

            // Now, let's fill the Phi and Q matrices
         SatID  dummySat;
		    // construct a gnssRinex of current epoch firstly
         gnssDataMap::iterator it = gData.find(epoch);
         gnssDataMap tempMap;
		 tempMap.insert(make_pair(it->first,it->second));
		 gnssRinex gRin = tempMap.getGnssRinex(*(recSet.begin()));

            // the DCBs of receiver

		pRecDCBStoModel->Prepare(dummySat,gRin);
		double recPhi = pRecDCBStoModel->getPhi();
		double recQ= pRecDCBStoModel->getQ();
        cout<<"rec Q:  "<<recQ<<endl;
		for (int i=0;i<numRec;i++)
		{	
         phiMatrix(i,i) = recPhi;
         qMatrix(i,i)   = recQ;
         }
            // the DCBs of satellite
		pSatDCBStoModel->Prepare(dummySat,gRin);
		double satPhi = pSatDCBStoModel->getPhi();
		double satQ= pSatDCBStoModel->getQ();
		cout<<"sat Q: "<<satQ<<endl;
		for (int j=0;j<numCurrentSV;j++)
		{
         phiMatrix(j+numRec,j+numRec) = satPhi;
         qMatrix(j+numRec,j+numRec)   = satQ;
		 }
         
		 // the SH coefficients 
        pCoefStoModel->Prepare(dummySat,gRin);
		double phi = pCoefStoModel->getPhi();
		double q = pCoefStoModel->getQ();
		cout<< "q iono : "<<q<<endl;
		for (int s= 0; s<numIonoCoef;s++)
		{
		  phiMatrix(s+numRec+numCurrentSV,s+numRec+numCurrentSV) = phi;
		  qMatrix(s+numRec+numCurrentSV,s+numRec+numCurrentSV) = q;
		}

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );


               // Fill the initialErrorCovariance matrix

               // DCBs for reciever
			for( int i=0; i<numRec; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
            }

               // DCBs for satellite    
            for( int i=numRec; i<numRec+numCurrentSV; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;     // (100 m)**2
            }
               // the ionospheric coefficients
             for (int i=numRec+numCurrentSV;i<numUnknowns;i++)
               initialErrorCovariance(i,i) = 10000.0;         // (100 m)**2;    

               // Reset Kalman filter
            kFilter.Reset( initialState, initialErrorCovariance );
 
               // No longer first time
            firstTime = false;

         }
         else
         {
             //std::cout<<"Cov size "<<covMatrix.rows()<<std::endl;
            // std::cout<<std::endl<<covMatrix<<std::endl;
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);
            

               // Set first part of current state vector and covariance matrix
            for( int i=0; i<numRec;i++ )
            {
               currentState(i) = solution(i);
     		   cout<<"rec dcb : "<<solution(i)*(-3.3356)<<endl;

                  // This fills the upper left quadrant of covariance matrix
               for( int j=0; j<numRec; j++ )
               {
                  currentErrorCov(i,j) =  covMatrix(i,j);
               }
            }

               // Fill in the rest of state vector and covariance matrix
               // These are values that depend on satellites being processed
            int c1(numRec);
            for( VariableSet::const_iterator itVar = satUnknowns.begin();
                 itVar != satUnknowns.end();
                 ++itVar )
            {

               currentState(c1) = satState[(*itVar)];
			   cout<<"sat DCB : "<<satState[(*itVar)]*(-3.3356)<<endl;
               ++c1;
            }
              // Fill the convariance matrix
			VariableSet tempSet(satUnknowns);
			c1 = numRec; //Reset 'c1'
            for( VariableSet::const_iterator itVar1 = satUnknowns.begin();
                 itVar1 != satUnknowns.end();
                 ++itVar1 )
            {
               if (covarianceMap.find(*itVar1)!= covarianceMap.end())
			   {
			    currentErrorCov(c1,c1)
				            =covarianceMap[(*itVar1)].satIndexedVarCov[(*itVar1)];	 	   
				   
			   }
			  
              else
				{
					// gives a initial variance
		         currentErrorCov(c1,c1) = 10000.0;			
		   			
				}

               int c2(c1+1);
			     // remove current variable from 'tempSet'
               tempSet.erase(*itVar1);
               for( VariableSet::const_iterator itVar2 = tempSet.begin();
                 itVar2 != tempSet.end();
                 ++itVar2 )
              {
                 currentErrorCov(c1,c2)=currentErrorCov(c2,c1)
				          = covarianceMap[(*itVar1)].satIndexedVarCov[(*itVar2)];
                 ++c2;
	   		  }

              int c3 = 0;
              for (VariableSet::const_iterator itVar3 = recUnknowns.begin();
			       itVar3 != recUnknowns.end();
				   ++itVar3) 

              {
			    currentErrorCov(c1,c3) = 
				     currentErrorCov(c3,c1) =
					   covarianceMap[(*itVar1)].recIndexedVarCov[(*itVar3)];
              
			    ++c3;	  
			   }
               
			  
			  for (int i= 0; i < numIonoCoef; i++)
              {
			    currentErrorCov(c1,i+numRec+numCurrentSV)=
				   currentErrorCov(i+numRec+numCurrentSV,c1) =
				                  covarianceMap[(*itVar1)].ionoCoefVarCov[i];
				  
			   }     

              ++c1;
            } // End of 'for (VariableSet::...)'

            for (int j = numRec+numCurrentSV;j<numUnknowns;j++)
			
			{
			 currentState(j)= IonoCoef[j-numRec-numCurrentSV];

			 for (int s = numRec+numCurrentSV;s<numUnknowns;s++)
             {
			   currentErrorCov(j,s)= covMatrix(j,s);
			
		     }
				
			}
          
		   int k(0);
           for (VariableSet::const_iterator itVar4 = recUnknowns.begin();
		        itVar4 != recUnknowns.end();
				++itVar4)
            {
				
		    for (int i = numRec+numCurrentSV;i<numUnknowns;i++)	
				
				{
			      currentErrorCov(k,i)=
				    currentErrorCov(i,k)=
					  covarianceMap[(*itVar4)].ionoCoefVarCov[i-numRec-numCurrentSV];
					
				}
		     ++k;		
			}
        
               // Reset Kalman filter to current state and covariance matrix
            kFilter.Reset( currentState, currentErrorCov );

         }  // End of 'if(firstTime)'



            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverIonoDCB object with the appropriate
            // constructor.
         Compute( measVector,hMatrix,rMatrix );



            // Store those values of current state and covariance matrix
            // that depend on satellites currently in view
		 satState.clear();
		 recState.clear();
		 IonoCoef.resize(numIonoCoef,0.0);
		 covarianceMap.clear();

         int c1(numRec);
         for(VariableSet::const_iterator itVar = satUnknowns.begin();
		     itVar != satUnknowns.end();
			 ++itVar)
         {
              // store DCBs for satellite 
          satState[(*itVar)] = solution(c1);
          ++c1;
         }
            // store convariance matrix
         VariableSet tempSet(satUnknowns);        
         
		 c1 = numRec;
        
         for (VariableSet::const_iterator itVar1 = satUnknowns.begin();
		      itVar1 != satUnknowns.end();
			  ++itVar1)
         {
			 
		   covarianceMap[(*itVar1)].satIndexedVarCov[(*itVar1)] 	 
			                                  = covMatrix(c1,c1);
			 
		   int c2(c1+1);

		   tempSet.erase(*itVar1);
		   
		   for (VariableSet::const_iterator itVar2 = tempSet.begin();
		        itVar2 != tempSet.end();
				++itVar2)
           {
		     covarianceMap[(*itVar1)].satIndexedVarCov[(*itVar2)]	   
			                                  = covMatrix(c1,c2);
		     ++c2;
		   }		
            
		   int c3(0);
         
           for (VariableSet::const_iterator itVar3 = recUnknowns.begin();
		        itVar3 != recUnknowns.end();
				++itVar3)
           {
			    // store the covariance matrix between satellite DCBs 
				// and receiver DCBs
		     covarianceMap[(*itVar1)].recIndexedVarCov[(*itVar3)]
			                                  = covMatrix(c1,c3);
		   	   
			 ++c3;
		   }

           
		   for (int c4= numRec+numCurrentSV; c4< numUnknowns;++c4)
           {
			   //store the covariance matrix between satellite Dcbs
			   //and SH coefficients
		     covarianceMap[*(itVar1)].ionoCoefVarCov.push_back(covMatrix(c1,c4));  
		   
		   }

	      ++c1;		 
		 }
         
           // store the SH coefficients  
         for (int i = numRec+numCurrentSV;i<numUnknowns;i++)
           {
		     IonoCoef(i-numRec-numCurrentSV) = solution(i);	   
		   }
           // store the covariance matrix between receiver DCBs 
		   // and SH coefficents
        int k(0);
		for (VariableSet::const_iterator itVar4 = recUnknowns.begin();
		     itVar4 != recUnknowns.end();
			 ++itVar4)
         {
		   for (int j= numRec+numCurrentSV;j<numUnknowns;j++)  
	    	 {
			  covarianceMap[(*itVar4)].ionoCoefVarCov.push_back(covMatrix(k,j));
			 }
		  ++k;	 
		 }


         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                 + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverIonoDCB::Process()'



   SolverIonoDCB& SolverIonoDCB::prepare(void)
   {

         // First, let's clear all the set that storing the variable
	recIndexedTypes.clear();
	satIndexedTypes.clear();
		 // fill the types that are source-indexed
	recIndexedTypes.insert(TypeID::recP1P2DCB);
	 // fill the types that are satellite-indexded
	satIndexedTypes.insert(TypeID::satP1P2DCB);
    
      return (*this);

   }  // End of method 'SolverIonoDCB::prepare()'

	double SolverIonoDCB::norm( int n, int m ) 
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

   }  // End of method 'SolverIonoDCB::norm'
   
  
      //  Legendre polynomial
   double SolverIonoDCB::legendrePoly(int n, int m, double u)
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
      
   }  // End of method 'SolverIonoDCB::legendrePoly()'

}  // End of namespace gpstk
