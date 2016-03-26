/* -----------------------------------------------------------------------------
 * File: UCCSDetector.hpp
 * This class is designed to detect cycle slip in a view of whole sats not the 
 * way of satellite by satellite
 * ***
 * Written by Lei Zhao, WHU, 2016/01/12 ~ 2016/01/... 
 * Directed by Shoujian Zhang, an associate professor in sgg of WHU
 * ***
 * Example of using this class
 * @code
 *		...
 *		while(rin >> gRin) 
 *		{
 *			gRin >> getLI >> basic >> computeTropo >> phaseAlign 
 *				  >> linear3 >> UCCSDetector >> ...;
 *    }
 * @endCode
 * ----------------------------------------------------------------------------*/

#include "UCCSDetector.hpp"

namespace gpstk
{
	// Returns a string identifying this object.
	std::string UCCSDetector::getClassName() const
	{ return "UCCSDetector"; }

	// Minimum buffer size. It is always set to 5
	const int UCCSDetector::minBufferSize = 5;
	
	const double UCCSDetector::miu1 = 1.0;
	const double UCCSDetector::miu2 = L1_FREQ_GPS*L1_FREQ_GPS/(L2_FREQ_GPS*L2_FREQ_GPS);

	// virtual method realization
	gnssRinex& UCCSDetector::Process(gnssRinex& gData)
		throw(ProcessingException)
	{
		try
		{
			Process(gData.header.epoch, gData.body, gData.header.epochFlag);
			return gData;
		}
		catch(Exception& u)
		{
			// Throw an exception if something unexpected happens
			 ProcessingException e( getClassName() + ":"
											+ u.what() );
			 GPSTK_THROW(e);
		}
	} // end of 'gnssRinex& UCCSDetector::Process(gnssRinex& gData)'

	// core method of this class
	satTypeValueMap& UCCSDetector::Process(const CommonTime& epoch,
														satTypeValueMap& gData,
														const short& epochflag)
		throw(ProcessingException)
	{
		//std::cout << getClassName() << std::endl;
		try
		{ 
			
			bool reportCS(false);
			double value(0.0);
			double value1(0.0);
			double value2(0.0);
			double value3(0.0);
			double value4(0.0);

			double lli1(0.0);
			double lli2(0.0);

			SatIDSet satRejectedSet;

			 // Loop through all the satellites
			satTypeValueMap::iterator it;
			for (it = gData.begin(); it != gData.end(); ++it)
			{
				try
				{
					// Try to extract the values
					value = (*it).second(obsType);
					value1 = (*it).second(obsType1);
					value2 = (*it).second(obsType2);
					value3 = (*it).second(obsType3);
					value4 = (*it).second(obsType4);

				}
				catch(...)
				{
					// If some value is missing, then schedule this satellite
					// for removal
					satRejectedSet.insert( (*it).first );
					continue;
				}

				if (useLLI)
				{
					try
					{
						// Try to get the LLI1 index
						lli1  = (*it).second(lliType1);
					}
					catch(...)
					{
						// If LLI #1 is not found, set it to zero
						// You REALLY want to have BOTH LLI indexes properly set
						 lli1 = 0.0;
					}

					try
					{
						// Try to get the LLI2 index
						lli2  = (*it).second(lliType2);
					}
					catch(...)
					{
						// If LLI #2 is not found, set it to zero
						// You REALLY want to have BOTH LLI indexes properly set
						lli2 = 0.0;
					}
				} // end of 'if(useLLI)'

				reportCS = getFilterData( epoch,
												  (*it).first,
												  (*it).second,
												  epochflag,
												  value,
												  value1,
												  value2,
												  value3,
												  value4,
												  lli1,
												  lli2 );
				if(reportCS) // time gaps or obvious CS for this sats
				{
					// mark this sat with CS
					(*it).second[resultType1] = 1.0;
					(*it).second[resultType2] = 1.0;
				}
				else{ // sats needing more look

					unsureSatSet.insert((*it).first);
				} // end of 'if(reportCS)'
														
			} // end of ' for (it = gData.begin(); ...'
			
			
			size_t s(unsureSatSet.size()); // I have output this num and it's enough
												 // to solve defined equation system 
			if( s > 2 ) // solve the equation system
			{

				// For every sat, there are 5 types of obs, namely
				//		delataPrefitP1, delataPrefitP1,
				//		delataPrefitL1, delataPrefitL2,
				//		deltaIon
				size_t rows(5*s);

				// The columns should be equal to unknows
				//		delta receiver clock offset	1
				//		delta Ion delay on L1 of all sats	s
				//		cycle slip on 2 frequency of all sats		2*s
				size_t cols( 1 + s + 2*s );


				// Declare a Vector for obs
				Vector<double> y(rows, 0.0);


				// Declare a design Matrix
				Matrix<double> dMatrix(rows, cols, 0.0);

			
				// Different state corresponding to different unknowns/columns.
				if( !staticReceiver )
				{
					// 3 more coordinates parameter 
					cols = cols + 3; 
					dMatrix.resize(rows, cols, 0.0);

					// Feed 'y' and 'dMatrix' with data
					size_t i(0);
					std::map<SatID,obs>::const_iterator ite;
					for( ite = obsData.begin();
						  ite != obsData.end();
						  ++ite)
					{
						// delataPrefitP1
						y(i) = obsData[(*ite).first].deltaPrefitP1;

						// delataPrefitP2
						y( i + 1 ) = obsData[(*ite).first].deltaPrefitP2;
	
						// delataPrefitL1
						y( i + 2 ) = obsData[(*ite).first].deltaPrefitL1;
	
						// delataPrefitL2
						y( i + 3 ) = obsData[(*ite).first].deltaPrefitL2;
	
						// deltaIon
						y( i + 4 ) = obsData[(*ite).first].deltaIon;

							// dMatrix
						for(size_t k=i; k<i+5; k++)
						{
							// colums corresponding to coordinates
							dMatrix( k, 0 ) = gData((*ite).first)(TypeID::dx);
							dMatrix( k, 1 ) = gData((*ite).first)(TypeID::dy);
							dMatrix( k, 2 ) = gData((*ite).first)(TypeID::dz);
							
							// colums corresponding to receiver clock error dT
							dMatrix( k, 3 ) = 1;

							// colums corresponding to Iono
							if( (k-i) == 0 ) dMatrix( k, 4+i/5 ) = miu1;
							if( (k-i) == 1 ) dMatrix( k, 4+i/5 ) = miu2;
							if( (k-i) == 2 ) dMatrix( k, 4+i/5 ) = -miu1;
							if( (k-i) == 3 ) dMatrix( k, 4+i/5 ) = -miu2;

							// colums corresponding to cycle slip
							if( (k-i) == 2 ) dMatrix( k, 3+1+s+(i/5)*2) 
													= L1_WAVELENGTH_GPS;

							if( (k-i) == 3 ) dMatrix( k, 3+1+s+(i/5)*2+1) 
													= L2_WAVELENGTH_GPS;
							
							// the Iono row
							if( (k-i) == 4 ) dMatrix( k, 3+1+(i/5) ) = 1.0;

						} // end of ' for(size_t k=i ... '

						// preparation for next sat
						i = i + 5;

					} // end of 'for( ite = obsData.begin() ...'
				
					
				}
				else{ // static state
					
					// Feed 'y' and 'dMatrix' with data
					size_t i(0);
					std::map<SatID,obs>::const_iterator ite;
					for( ite = obsData.begin();
						  ite != obsData.end();
						  ++ite )
					{
						// delataPrefitP1
						y(i) = obsData[(*ite).first].deltaPrefitP1;

						// delataPrefitP2
						y( i + 1 ) = obsData[(*ite).first].deltaPrefitP2;
	
						// delataPrefitL1
						y( i + 2 ) = obsData[(*ite).first].deltaPrefitL1;
	
						// delataPrefitL2
						y( i + 3 ) = obsData[(*ite).first].deltaPrefitL2;
	
						// deltaIon
						y( i + 4 ) = obsData[(*ite).first].deltaIon;

							// dMatrix
						for(size_t k=i; k<i+5; k++)
						{
							
							// colums corresponding to receiver clock error dT
							if( (k - i) != 4 ) dMatrix( k, 0 ) = 1;

							// colums corresponding to Iono
							if( (k-i) == 0 ) dMatrix( k, 1+i/5 ) = miu1;
							if( (k-i) == 1 ) dMatrix( k, 1+i/5 ) = miu2;
							if( (k-i) == 2 ) dMatrix( k, 1+i/5 ) = -miu1;
							if( (k-i) == 3 ) dMatrix( k, 1+i/5 ) = -miu2;

							// colums corresponding to cycle slip
							if( (k-i) == 2 ) dMatrix( k, 1+s+(i/5)*2) 
													= L1_WAVELENGTH_GPS;

							if( (k-i) == 3 ) dMatrix( k, 1+s+(i/5)*2+1) 
													= L2_WAVELENGTH_GPS;
							
							// the Iono row
							if( (k-i) == 4 ) dMatrix( k, 1+(i/5) ) = 1.0;

						} // end of ' for(size_t k=i ... '
						
						// preparation for next sat
						i = i + 5;

					} // end of 'for( ite = obsData.begin() ...'
				
				} // end of 'if( staticReceiver )'
			
				/*std::cout << std::fixed << std::setprecision(3) << std::endl;
				std::cout << "num of sats: " << s << std::endl;
				std::cout << "y: " << std::endl << y << std::endl;
				std::cout << "my dMatrix: " << std::endl << dMatrix << std::endl;
				*/
				
				// Use a SolveLMS obj to get the solution.
				solver.Compute( y, dMatrix);
				
				// get cycle slip vector

				// SPAR 

				// Judge CS   

			} // end of ' if( s > 2 ) '



			// clear the unsureSatSet and obsData for next epoch
			unsureSatSet.clear();
			obsData.clear();


			// Remove satellites with missing data
			gData.removeSatID(satRejectedSet);
			return gData;
		}
		catch(Exception& u)
		{
			 // Throw an exception if something unexpected happens
			ProcessingException e( getClassName() + ":"
										  + u.what() );
			GPSTK_THROW(e);
		}

	} // end of 'satTypeValueMap& UCCSDetector::Process(...'


	// 
	bool UCCSDetector::getFilterData( const CommonTime& epoch,
													const SatID& sat,
													typeValueMap& tvMap,
													const short& epochflag,
													const double& li,
													const double& pfp1,
													const double& pfp2,
													const double& pfl1,
													const double& pfl2,
													const double& lli1,
													const double& lli2 )
	{
		bool reportCS(false);

		// Different between current and former epochs, in sec.
		double currentDeltaT(0.0);
		double currentDeltaP1(0.0);
		double currentDeltaP2(0.0);
		double currentDeltaL1(0.0);
		double currentDeltaL2(0.0);
		double currentDeltaLI(0.0);

		double deltaLimit(0.0);
		double delta(0.0);

		double tempLLI1(0.0);
		double tempLLI2(0.0);


		// Get current buffer size
		size_t s( UCData[sat].epochDeque.size() );
		
		currentDeltaT = epoch - UCData[sat].formerEpoch;

		// Store current epoch as former epoch
		UCData[sat].formerEpoch = epoch;

		// Current value of LI difference
		currentDeltaLI = li - UCData[sat].formerLI;
		
		// Increment control window size
		++UCData[sat].controlWindowSize;


		// Check if receiver already declared cycle slip or too much time
		// has elapsed
		// Note: If tvMap(lliType1) or tvMap(lliType2) don't exist, then 0
		// will be returned and those tests will pass

		if ( (tvMap(lliType1)==1.0) ||
			  (tvMap(lliType1)==3.0) ||
			  (tvMap(lliType1)==5.0) ||
			  (tvMap(lliType1)==7.0) )
		{
			tempLLI1 = 1.0;
		}

		if ( (tvMap(lliType2)==1.0) ||
			  (tvMap(lliType2)==3.0) ||
			  (tvMap(lliType2)==5.0) ||
			  (tvMap(lliType2)==7.0) )
		{
			tempLLI2 = 1.0;
		}

		if ( (epochflag==1)  ||
			  (epochflag==6)  ||
			  (tempLLI1==1.0) ||
			  (tempLLI2==1.0) ||
			  (currentDeltaT > deltaTMax) )
		{
			// We reset the filter with this
			UCData[sat].controlWindowSize = 0;

			// We reset buffer with the following lines
			UCData[sat].epochDeque.clear();
			UCData[sat].deltaLIBuffer.clear();

			// current buffer size should be updated
			s = UCData[sat].epochDeque.size();

			// Report cycle slip
			reportCS = true;

		}
		

		if( UCData[sat].controlWindowSize > 0 ) // This means the 2nd epoch start.
															 // Beacause all delta terms should
															 // be computed from the 2nd epoch.
		{
			
			// Check if we have enough data to start processing.
			if (s >= (size_t)minBufferSize)
			{
				// Get delta obs
					
				// First, currentDeltaLI
				double formerBias(0.0);
				double formerDeltaT(0.0);

					// iterator for the last and the one next to the last
				std::deque<CommonTime>::const_iterator nextToLastIt;
				nextToLastIt = --(--(UCData[sat].epochDeque.end()));

					// formerBias and formerDeltaT
				formerBias = UCData[sat].deltaLIBuffer.back();
				formerDeltaT = UCData[sat].epochDeque.back() - *(nextToLastIt);

					// compute threshold
				deltaLimit = minThreshold + std::abs(LIDrift*currentDeltaT);
				delta = std::abs( currentDeltaLI -
										formerBias*currentDeltaT/formerDeltaT);

				if( delta > deltaLimit ) // do a fitting job for too big 
												 // currentDeltaLI
				{
					// Declare a Vector for LI measurements
					Vector<double> y(s, 0.0);

					// Declare a Matrix for epoch information
					Matrix<double> M(s, 3, 0.0);

					// We store here the OLDEST (or FIRST) epoch in buffer for future
					// reference. This is important because adjustment will be made
					// with respect to that first epoch
					CommonTime firstEpoch(UCData[sat].epochDeque.front());

					// Feed 'y' with data
					for(size_t i=0; i<s; i++)
					{
						// The newest goes first in 'y' vector
						y(i) = UCData[sat].deltaLIBuffer[s-1-i];
					}

					// Feed 'M' with data
					for(size_t i=0; i<s; i++)
					{
						// Compute epoch difference with respect to FIRST epoch
						double dT(UCData[sat].epochDeque[s-1-i] - firstEpoch);

						 M(i,0) = 1.0;
						 M(i,1) = dT;
						 M(i,2) = dT*dT;
					}

					// Now, proceed to find a 2nd order fiting curve using a least
					// mean squares (LMS) adjustment
					Matrix<double> MT(transpose(M));
					Matrix<double> covMatrix( MT * M );

					// Let's try to invert MT*M   matrix
					try{
						covMatrix = inverseChol( covMatrix );
					}
					catch(...)
					{
						// If covMatrix can't be inverted we have a serious problem
						// with data, so reset buffer and declare cycle slip
						UCData[sat].epochDeque.clear();
						UCData[sat].deltaLIBuffer.clear();

						UCData[sat].controlWindowSize = 0;

						reportCS = true;
					}

					// Now, compute the Vector holding the results of adjustment to
					// second order curve
					Vector<double> a(covMatrix * MT * y);

					// Compute a predicted deltaLI

						// Compute epoch difference with respect to FIRST epoch
					double deltaT( epoch - firstEpoch );
					
					// Compute current adjusted LI value
					double preCurrentDeltaLI( a(0) 
													  + a(1)*deltaT + a(2)*deltaT*deltaT );

					// new assignment
					currentDeltaLI = preCurrentDeltaLI;
				

				} // end of ' if( delta > deltaLimit ) '

					// get deltaIon
				obsData[sat].deltaIon = currentDeltaLI/(miu1 - miu2);

				// Second, get other delta obs terms

					// delta prefitP1
				obsData[sat].deltaPrefitP1 = pfp1 - UCData[sat].formerPrefitP1;

					// delta prefitP2
				obsData[sat].deltaPrefitP2 = pfp2 - UCData[sat].formerPrefitP2;

					// delta prefitL1
				obsData[sat].deltaPrefitL1 = pfl1 - UCData[sat].formerPrefitL1;

					// delta prefitL2
				obsData[sat].deltaPrefitL2 = pfl2 - UCData[sat].formerPrefitL2;
	
			}
			else
			{
				// If we don't have enough data, we report cycle slips
				reportCS = true;
			}
	
			// Let's prepare for the next epoch
	
			// Store current epoch at the end of deque
			UCData[sat].epochDeque.push_back(epoch);
	
			// Store current value of LI at the end of deque
			UCData[sat].deltaLIBuffer.push_back(currentDeltaLI);
	
			// Update current buffer size
			s = UCData[sat].epochDeque.size();
		
			// Check if we have exceeded maximum window size
			if(s > size_t(maxBufferSize))
			{
				// Get rid of oldest data, which is at the beginning of deque
				UCData[sat].epochDeque.pop_front();
				UCData[sat].deltaLIBuffer.pop_front();
			}

		} // end of ' if( UCData[sat].controlWindowSize > 0 )  '

		// Let's prepare for the next time
		UCData[sat].formerPrefitP1 = pfp1;
		UCData[sat].formerPrefitP2 = pfp2;
		UCData[sat].formerPrefitL1 = pfl1;
		UCData[sat].formerPrefitL2 = pfl2;
		UCData[sat].formerLI = li;
		UCData[sat].formerEpoch = epoch;
		

		return reportCS;

	} // end of 'bool UCCSDetector::getFilterData( const CommonTime& ...'


} // end of namespace
