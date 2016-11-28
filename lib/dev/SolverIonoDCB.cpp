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
//  
//  2016/4/11
//  fix bugs, adjust the sequence of parameters to be estimated.
//
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
	  
      pSatDCBStoModel = &constantModel;
         // for receiver DCBs
      pRecDCBStoModel = &constantModel;  

      CoefModel.setQprime(3e-6);
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
   gnssDataMap & SolverIonoDCB::Process(CommonTime& epoch,gnssDataMap& gData)
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
        currSatSet = gData.getSatIDSet();

            // Get the number of satellites currently visible
        int numCurrentSV( currSatSet.size() );
        cout<<"Current SVs " << numCurrentSV<<endl;    
	     // Get the number of receivers
	 	recSet = gData.getSourceIDSet();
          
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

              sourceDataMap tempSourMap(itEpoch->second);
			   // the SatIDSet corresponding to this receiver
	          SatIDSet tempSatSet = tempSourMap.getSatIDSet();
                
			  int seq = 0;
          
              satTypeValueMap dummy(tempSourMap[rec]);
            
               // extract the PI combination
			  Vector<double> piCom(dummy.getVectorOfTypeID(TypeID::PI));
               // extract the weight
			  Vector<double> wVec(dummy.getVectorOfTypeID(TypeID::weight));
             
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
				 
				 if (wVec(seq)==0.0)
                 {
                  rMatrix(count,count) = 1.0;
                 }
                  else
                 {
                  rMatrix(count,count) = wVec(seq);
                 }
				  
				   // fill the SH coefficients	
                 double Lat = DEG_TO_RAD*lat(seq);
                 Lat = std::asin(std::sin(DEG_TO_RAD*NGPLat)*std::sin(Lat)
                                +std::cos(DEG_TO_RAD*NGPLat)*std::cos(Lat)
                               *std::cos(DEG_TO_RAD*lon(seq)-DEG_TO_RAD*NGPLon));
				 if (lon(seq)>=180.0)
				 {
					 lon(seq)-=360.0;
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
		// consMatrix.resize(1,numUnknowns,0.0);
		 for (int i = 0;i<numCurrentSV;i++)
         {
		   hMatrix(numMeas-1,i+numRec+numIonoCoef) = 1.0;	 
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
 
		// the SH coefficients 
        pCoefStoModel->Prepare(dummySat,gRin);
		double phi = pCoefStoModel->getPhi();
		double q = pCoefStoModel->getQ();
        for (int i=0;i<numIonoCoef;i++)
		{	
         phiMatrix(i,i) = phi;
         qMatrix(i,i)   = q;
         }
            // the DCBs of receiver

		pRecDCBStoModel->Prepare(dummySat,gRin);
		double recPhi = pRecDCBStoModel->getPhi();
		double recQ= pRecDCBStoModel->getQ();
		for (int j=numIonoCoef;j<numIonoCoef+numRec;j++)
		{
         phiMatrix(j,j) = recPhi;
         qMatrix(j,j)   = recQ;
		 }
            // the DCBs of satellite
		pSatDCBStoModel->Prepare(dummySat,gRin);
		double satPhi = pSatDCBStoModel->getPhi();
		double satQ= pSatDCBStoModel->getQ();
		for (int s= numIonoCoef+numRec; s<numUnknowns;s++)
		{
		  phiMatrix(s,s) = satPhi;
		  qMatrix(s,s) = satQ;
         }
            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );


               // Fill the initialErrorCovariance matrix
               // the ionospheric coefficients
            for (int i=0;i<numIonoCoef;i++)
            { 
               initialErrorCovariance(i,i) = 10000.0;     // (100 m)**2; 
            } 
               // DCBs for reciever
			for( int i=numIonoCoef; i<numRec+numIonoCoef; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
            }

               // DCBs for satellite    
            for( int i=numRec+numIonoCoef; i<numUnknowns; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;     // (100 m)**2
            }
                 

               // Reset Kalman filter
            kFilter.Reset( initialState, initialErrorCovariance );
 
               // No longer first time
            firstTime = false;

         }
         else
         {
            // std::cout<<"Cov size "<<covMatrix.rows()<<std::endl;
            // std::cout<<std::endl<<covMatrix<<std::endl;
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);

               // Set first part of current state vector and covariance matrix
               // the numIonoCoef and numRec is fixed at different epochs
            for( int i=0; i<numIonoCoef;i++ )
            {
               currentState(i) = solution(i);
     		 
                  // This fills the upper left quadrant of covariance matrix
               for( int j=0; j<numIonoCoef;j++ )
               {
                  currentErrorCov(i,j) =  covMatrix(i,j);
               }
            }
            for( int i=numIonoCoef; i<numIonoCoef+numRec;i++ )
            {
               currentState(i) = solution(i);
     		 
                  // This fills the upper left quadrant of covariance matrix
               for( int j=numIonoCoef; j<numIonoCoef+numRec;j++ )
               {

                  currentErrorCov(i,j) =  covMatrix(i,j);
               }
            }
               // Fill in the rest of state vector and covariance matrix
               // These are values that depend on satellites being processed
            int c1(numIonoCoef+numRec);
            for( VariableSet::const_iterator itVar = satUnknowns.begin();
                 itVar != satUnknowns.end();
                 ++itVar )
            {
               currentState(c1) = satState[(*itVar)];
               ++c1;
            }
              // Fill the convariance matrix
			VariableSet tempSet(satUnknowns);
			c1 = numIonoCoef+numRec; //Reset 'c1'
            for( VariableSet::const_iterator itVar1 = satUnknowns.begin();
                 itVar1 != satUnknowns.end();
                 ++itVar1 )
            {
               if (covarianceMap.find(*itVar1)!= covarianceMap.end())
			   {
			    currentErrorCov(c1,c1)
				            =covarianceMap[(*itVar1)].satIndexedVarCov[(*itVar1)];	 	   
				 for (int i= 0; i < numIonoCoef; i++)

               {
			
                currentErrorCov(c1,i)=
				   currentErrorCov(i,c1) =
				                covarianceMap[(*itVar1)].ionoCoefVarCov(i);
          
			    } 

			   }
			  else if (satState.find(*itVar1)!=satState.end())
				{
		         currentErrorCov(c1,c1) = 0.01;			
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

              int c3 = numIonoCoef;
              for (VariableSet::const_iterator itVar3 = recUnknowns.begin();
			       itVar3 != recUnknowns.end();
				   ++itVar3) 

              {
			    currentErrorCov(c1,c3) = 
				     currentErrorCov(c3,c1) =
					   covarianceMap[(*itVar1)].recIndexedVarCov[(*itVar3)];
              
			    ++c3;	  
			   }
               
			
               
              ++c1;
            } // End of 'for (VariableSet::...)'

       
          
		   int k(numIonoCoef);
           for (VariableSet::const_iterator itVar4 = recUnknowns.begin();
		        itVar4 != recUnknowns.end();
				++itVar4)
            {
				
		    for (int i = 0;i<numIonoCoef;i++)	
				
				{
			      
                 currentErrorCov(k,i)=
				    currentErrorCov(i,k)=
					  covarianceMap[(*itVar4)].ionoCoefVarCov(i);
					
				}
		     ++k;		
			}
        
            // cout<<currentErrorCov.size()<<endl;
			// cout<<"currEV"<<std::endl<<currentErrorCov<<endl;

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
	     //satState.clear();
	     IonoCoef.resize(numIonoCoef,0.0);
		 covarianceMap.clear();
         int c1(numIonoCoef+numRec);
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
         
		 c1 = numIonoCoef+numRec;
        
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
            
		   int c3(numIonoCoef);
         
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

           covarianceMap[*(itVar1)].ionoCoefVarCov.resize(numIonoCoef,0.0);
		   for (int c4 = 0; c4 < numIonoCoef;++c4)
           {
			   //store the covariance matrix between satellite Dcbs
			   //and SH coefficients
		     covarianceMap[*(itVar1)].ionoCoefVarCov(c4)=covMatrix(c1,c4);  
		   
		   }

	      ++c1;		 
		 }
         
           // store the SH coefficients  
         for (int i = 0;i<numIonoCoef;i++)
           {
		     IonoCoef(i) = solution(i);	   
		   }
           // store the covariance matrix between receiver DCBs 
		   // and SH coefficents
        int k(numIonoCoef);
		for (VariableSet::const_iterator itVar4 = recUnknowns.begin();
		     itVar4 != recUnknowns.end();
			 ++itVar4)
         {
		    recState[(*itVar4)] = solution(k);
			covarianceMap[*(itVar4)].ionoCoefVarCov.resize(numIonoCoef,0.0);

		   for (int j= 0;j<numIonoCoef;j++)  
	    	 {
			  covarianceMap[(*itVar4)].ionoCoefVarCov(j) = covMatrix(k,j);
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

	SolverIonoDCB& SolverIonoDCB::getSolution(void)
   {
    for (VariableSet::const_iterator itVar = recUnknowns.begin();
		 itVar!= recUnknowns.end();
	     ++itVar)
    {
     cout<<asString(*itVar)<<" "<<recState[(*itVar)]*3.3356<<endl;

    }
  
    for (VariableDataMap::const_iterator itVar2 = satState.begin();
		 itVar2!= satState.end();
	     ++itVar2)
    {
     cout<<asString(itVar2->first)<<" "<<(itVar2->second)*3.3356<<endl;

    }
    cout<< "A00 : "<<IonoCoef(0)<<endl;
   }

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
