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

#ifndef GPSTK_UCCSDETECTOR_H_H_H
#define GPSTK_UCCSDETECTOR_H_H_H

#include <deque>
#include "ProcessingClass.hpp"
#include "SolverLMS.hpp"

namespace gpstk 
{
	class UCCSDetector : public ProcessingClass
	{
	public:
		// Default constructor
		UCCSDetector() : staticReceiver(true),
							  obsType(TypeID::LI), 
							  obsType1(TypeID::prefitP1),obsType2(TypeID::prefitP2),
							  obsType3(TypeID::prefitL1),obsType4(TypeID::prefitL2),
							  lliType1(TypeID::LLI1),lliType2(TypeID::LLI2),
							  resultType1(TypeID::CSL1),resultType2(TypeID::CSL2),
							  deltaTMax(61.0), minThreshold(0.04), LIDrift(0.002), 
							  useLLI(true),maxBufferSize(12)
		{};

		virtual UCCSDetector& useC1(void)
		{ obsType1 = TypeID::prefitC; return (*this); };
		
		// virtual function in the abstract base class
		virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
			throw(ProcessingException)
		{Process(gData.header.epoch, gData.body); return gData;};
		virtual gnssRinex& Process(gnssRinex& gData)
			throw(ProcessingException);

		// core method of this class
		virtual satTypeValueMap& Process( const CommonTime& epoch,
													 satTypeValueMap& gData,
													  const short& epochflag = 0 )
			throw(ProcessingException);

		virtual std::string getClassName(void) const;

		// Destructor 
		virtual ~UCCSDetector() {};

	private:

		// receiver state
		bool staticReceiver;

		// obs type for LI or PI.
		TypeID obsType;

		// obs type1 ~ 4 for prefit P and L
		TypeID obsType1;
		TypeID obsType2;
		TypeID obsType3;
		TypeID obsType4;
		
		// Type of LLI1 record.
		TypeID lliType1;

		// Type of LLI2 record.
		TypeID lliType2;

		// Type of result #1.
		TypeID resultType1;

		// Type of result #2.
		TypeID resultType2;

		// MaXimum interval of time allowed betwen two successive epochs,
		// in seconds.
		double deltaTMax;
		
		// Minimum threshold to declare cycle slip, in meters.
		double minThreshold;

		// LI combination limit drift, in meters/second.
		double LIDrift;

		// This field tells whether to use or ignore the LLI indexes as an aid.
		bool useLLI;

		// Maximum buffer size.
		int maxBufferSize;

		// Maximum size of buffer. It is always set to 5.
		static const int minBufferSize;
		static const double miu1;
		static const double miu2;

		// A structure used to store filter data for a SV.
		struct filterData
		{
			// Default constructor initializing the data in the structure.
			filterData() : formerPrefitP1(0.0), formerPrefitP2(0.0),
								formerPrefitL1(0.0), formerPrefitL2(0.0),
								controlWindowSize(0), formerLI(0.0),  
								formerEpoch(CommonTime::BEGINNING_OF_TIME)
			{};
			int controlWindowSize;
			std::deque<CommonTime> epochDeque;
			std::deque<double> deltaLIBuffer;
			std::deque<double> deltaPIBuffer;
			CommonTime formerEpoch;
			double formerPrefitP1;
			double formerPrefitP2;
			double formerPrefitL1;
			double formerPrefitL2;
			double formerLI;
		}; // end of 'struct filterData'

		// This struct is designed for the convenience
		struct obs
		{
			obs() : deltaPrefitP1(0.0), deltaPrefitP2(0.0),
					  deltaPrefitL1(0.0), deltaPrefitL2(0.0),
					  deltaIon(0.0)		
			{};

			double deltaPrefitP1;
			double deltaPrefitP2;
			double deltaPrefitL1;
			double deltaPrefitL2;
			double deltaIon;
		};

		// Map holding the information regarding every satellite.
		std::map<SatID, filterData> UCData;

		// Map holding the obs regarding every ssatellite. 
		std::map<SatID,obs> obsData;

		// Set holding all the sats needing CS detection 
		SatIDSet unsureSatSet;

		// SolerLMS obj
		SolverLMS solver;

		// 
		virtual bool getFilterData( const CommonTime& epoch,
											 const SatID& sat,
											 typeValueMap& tvMap,
											 const short& epochflag,
											 const double& li,
											 const double& pfp1,
											 const double& pfp2,
											 const double& pfl1,
											 const double& pfl2,
											 const double& lli1,
											 const double& lli2 );

	}; // end of class declaration

} // end of namespace 


#endif
