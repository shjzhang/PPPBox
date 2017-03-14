#include "PPPTask.hpp"
using namespace StringUtils;

// Constructor
PPPTask::PPPTask()
{
    m_bRealTime = true;
    m_sPPPConfFile = "./ppprt.conf";
    m_sEopFileListName = "/home/qi/program/PPPBox/workplace/ntrip/erplist";
    m_sCorrMount = "IGS03";
    m_dCorrWaitTime = 5;
}

// Destructor
PPPTask::~PPPTask()
{
//    while(!m_staObsQueue.empty())
//    {
//        delete m_staObsQueue.front();
//        m_staObsQueue.pop_front();
//    }
}

bool PPPTask::run()
{
    try
    {
        if(m_bRealTime)
        {
            spinUp();
            process();
            return true;
        }
        else
        {
            processFiles();
        }
    }
    catch(...)
    {

    }
    return false;
}

bool PPPTask::waitForCorr(const CommonTime &epoTime) const
{
    if(!m_bRealTime || m_sCorrMount.empty())
    {
        return false;
    }
    // Verify the validity of m_lastClkCorrTime
    // For the first time, we will wait for the clkcorr
    else if(m_lastClkCorrTime.getDays() == 0.0||
            m_lastClkCorrTime.getSecondOfDay() == 0.0)
    {
        return true;
    }
    else
    {
        double dt = epoTime - m_lastClkCorrTime;
        if(dt > 1.0 && dt < m_dCorrWaitTime)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}


// Method to print solution values
void PPPTask::printSolution(  ofstream& outfile,
                            const SolverLMS& solver,
                            const CommonTime& time,
                            const ComputeDOP& cDOP,
                            int   numSats,
                            double dryTropo,
                            Position& pos,
                            const string format,
                            int   precision)
{

      // Prepare for printing
   outfile << fixed;

      // Print results
   outfile << static_cast<YDSTime>(time).year;                                // Year           - #1
   outfile << setw(8) << static_cast<YDSTime>(time).doy;                      // DayOfYear      - #2
   outfile << setprecision(3)<< setw(12)<< static_cast<YDSTime>(time).sod;    // SecondsOfDay   - #3

   double recX(solver.getSolution(TypeID::recX));  // ECEF X
   double recY(solver.getSolution(TypeID::recY));  // ECEF Y
   double recZ(solver.getSolution(TypeID::recZ));  // ECEF Z

   double dx(recX - pos[0]);
   double dy(recY - pos[1]);
   double dz(recZ - pos[2]);

   Triple dxyzTriple(dx,dy,dz);

   Position recPos(recX,recY,recZ);
   double lat(recPos.getGeodeticLatitude());   // Geodetic latitude
   double lon(recPos.getLongitude());          // Geodetic longitude
   double height(recPos.getHeight());          // Geodetic Height

   if ( format == "BLH" )
   {
       outfile << setprecision(9)         << setw(16) << lat;         // Lat        - #4
       outfile << setprecision(9)         << setw(16) << lon;         // Lon        - #5
       outfile << setprecision(precision) << setw(14) << height;      // Height     - #6
   }
   else if ( format == "XYZ")
   {
       outfile << setw(14) << recX;           // recX       - #4
       outfile << setw(14) << recY;           // recY       - #5
       outfile << setw(14) << recZ;           // recZ       - #6
   }
   else if ( format == "dxyz")
   {
       outfile << setw(14) << dx;             // dx       - #4
       outfile << setw(14) << dy;             // dy       - #5
       outfile << setw(14) << dz;             // dz       - #6
   }
   else if ( format == "dneu")
   {
       ENUUtil enu(lat,lon);
       Triple denu(enu.convertToENU(dxyzTriple));
       outfile << setw(14) << denu[1];        // dn        - #4
       outfile << setw(14) << denu[0];        // de        - #5
       outfile << setw(14) << denu[2];        // du        - #6
   }
         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // ztd - #7
   outfile << setw(10) << solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo;

   outfile << setw(5)  << numSats;           // Number of satellites - #12

   outfile << setw(8)  << cDOP.getGDOP();    // GDOP - #13
   outfile << setw(8)  << cDOP.getPDOP();    // PDOP - #14

      // Add end-of-line
   outfile << endl;


   return;


}  // End of method 'ppprt::printSolution()'



// Method to print model values
void PPPTask::printModel( ofstream& modelfile, const gnssRinex& gData,
                          int   precision)
{
    // Prepare for printing
    modelfile << fixed << setprecision( precision );

    // Get epoch out of GDS
    CommonTime time(gData.header.epoch);

    // Iterate through the GNSS Data Structure
    for ( satTypeValueMap::const_iterator it = gData.body.begin();
          it!= gData.body.end();
          it++ )
    {

        // Print epoch
        modelfile << static_cast<YDSTime>(time).year << "  ";  // Year         #1
        modelfile << static_cast<YDSTime>(time).doy  << "  ";  // DayOfYear    #2
        modelfile << static_cast<YDSTime>(time).sod  << "  ";  // SecondsOfDay #3

        // Print satellite information (Satellite system and ID number)
        modelfile << (*it).first << " ";         // System         #4
                                                 // ID number      #5

       // Print model values
        for( typeValueMap::const_iterator itObs  = (*it).second.begin();
             itObs != (*it).second.end();
             itObs++ )
        {
            // Print type names and values
            modelfile << (*itObs).first << " ";
            modelfile << (*itObs).second << " ";

        }  // End of 'for( typeValueMap::const_iterator itObs = ...'

        modelfile << endl;

    }  // End for (it = gData.body.begin(); ... )
    return;
}

void PPPTask::spinUp()
{
    m_sPPPConfFile = SIG_CENTER->m_sPPPConfile;
    // Check if the user provided a configuration file name
    if(m_sPPPConfFile.size()==0)
    {
        m_sPPPConfFile = "ppprt.conf";
    }
    else
    {
    }
    // Enable exceptions
    m_confReader.exceptions(ios::failbit);

    try
    {
        // Try to open the provided configuration file
        m_confReader.open(m_sPPPConfFile);
    }
    catch(...)
    {
        cerr << "Problem opening file " << m_sPPPConfFile << endl;
        cerr << "Maybe it doesn't exist or you don't have proper "
             << "read permissions." << endl;

        exit (-1);
    } // End of 'try-catch' block

    // If a given variable is not found in the provided section, then
    // 'confReader' will look for it in the 'DEFAULT' section.
    m_confReader.setFallback2Default(true);
}


// Method that will really process information
void PPPTask::process()
{
    RealTimeEphStore ephStore;

    //******************************************
    // Let's read ocean loading BLQ data files
    //******************************************

    // BLQ data store object
    BLQDataReader blqStore;

    // Read BLQ file name from the configure file
    string blqFile = m_confReader.getValue( "oceanLoadingFile", "DEFAULT");

    try
    {
        blqStore.open( blqFile );
    }
    catch (FileMissingException& e)
    {
           // If file doesn't exist, issue a warning
        cerr << "BLQ file '" << blqFile << "' doesn't exist or you don't "
             << "have permission to read it. Skipping it." << endl;
        exit(-1);
    }

    //***********************
    // Let's read eop files
    //***********************

    // Declare a "EOPDataStore" object to handle earth rotation parameter file
    EOPDataStore eopStore;

    ifstream eopFileListStream;
    eopFileListStream.open(m_sEopFileListName.c_str(), ios::in);

    if(!eopFileListStream)
    {
          // If file doesn't exist, issue a warning
       cerr << "erp file List Name'" << m_sEopFileListName << "' doesn't exist or you don't "
            << "have permission to read it. Skipping it." << endl;

       exit(-1);
    }
    string eopFile;
    while( eopFileListStream >> eopFile )
    {
       try
       {
          eopStore.loadIGSFile( eopFile );
       }
       catch (FileMissingException& e)
       {
             // If file doesn't exist, issue a warning
          cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
               << "have permission to read it. Skipping it." << endl;
          continue;
       }
    }
       // Close file
    eopFileListStream.close();

    // Create a 'ProcessingList' object where we'll store
    // the processing objects in order
    ProcessingList preprocessList;
    ProcessingList predictList;
    ProcessingList correctList;

    // First time for this rinex file
    CommonTime initialTime;

    // This object will check that all required observables are present
    RequireObservables requireObs;
    requireObs.addRequiredType(TypeID::P1);
    requireObs.addRequiredType(TypeID::P2);
    requireObs.addRequiredType(TypeID::L1);
    requireObs.addRequiredType(TypeID::L2);

    // Add 'requireObs' to processing list (it is the first)
    preprocessList.push_back(requireObs);

    // This object will check that code observations are within
    // reasonable limits
    SimpleFilter pObsFilter;
    pObsFilter.setFilteredType(TypeID::P2);
    pObsFilter.addFilteredType(TypeID::P1);

    bool filterCode( m_confReader.getValueAsBoolean( "filterCode" ) );

      // Check if we are going to use this "SimpleFilter" object or not
    if( filterCode )
    {
        preprocessList.push_back(pObsFilter);       // Add to processing list
    }

    // This object defines several handy linear combinations
    LinearCombinations comb;

    // Object to compute linear combinations for cycle slip detection
    ComputeLinear linear1;

    linear1.addLinear(comb.pdeltaCombination);
    linear1.addLinear(comb.mwubbenaCombination);
    linear1.addLinear(comb.ldeltaCombination);
    linear1.addLinear(comb.liCombination);
    preprocessList.push_back(linear1);       // Add to processing list

    // Objects to mark cycle slips
    LICSDetector markCSLI;                   // Checks LI cycle slips
    preprocessList.push_back(markCSLI);      // Add to processing list
    MWCSDetector markCSMW;                   // Checks Merbourne-Wubbena cycle slips
    preprocessList.push_back(markCSMW);      // Add to processing list


    // Object to keep track of satellite arcs
    SatArcMarker2 markArc;
    markArc.setDeleteUnstableSats(false);
    markArc.setUnstablePeriod(1.0);
    preprocessList.push_back(markArc);       // Add to processing list


//    // Object to decimate data
//    Decimate decimateData(
//             m_confReader.getValueAsDouble( "decimationInterval"),
//             m_confReader.getValueAsDouble( "decimationTolerance"),
//             initialTime );
//    preprocessList.push_back(decimateData);  // Add to processing list

    // class to store the information of EKF
    PPPExtendedKalmanFilter pppEKF;

    // Declare a basic modeler
    BasicModel basic(pppEKF, ephStore);

    basic.setMinElev(m_confReader.getValueAsDouble("cutOffElevation"));
       // If we are going to use P1 instead of C1, we must reconfigure 'basic'
    basic.setDefaultObservable(TypeID::P1);
       // Add to processing list
    predictList.push_back(basic);
    correctList.push_back(basic);

       // Object to remove eclipsed satellites
    EclipsedSatFilter eclipsedSV;
    predictList.push_back(eclipsedSV);       // Add to processing list

       // Object to compute weights based on elevation
    ComputeElevWeights elevWeights;
    correctList.push_back(elevWeights);      // Add to processing list


       // Object to compute gravitational delay effects
    GravitationalDelay grDelay(pppEKF);
    correctList.push_back(grDelay);          // Add to processing list


       // Vector from monument to antenna ARP [UEN], in meters
    //Triple offsetARP( roh.antennaOffset );
    Triple offsetARP( 0.0, 0.0, 0.0 );

       // Declare some antenna-related variables
    Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
    AntexReader antexReader;
    Antenna receiverAntenna;

       // Check if we want to use Antex information
    bool useantex( m_confReader.getValueAsBoolean( "useAntex") );
    string antennaModel;
    if( useantex )
    {
          // Feed Antex reader object with Antex file
        antexReader.open( m_confReader.getValue( "antexFile" ) );

          // Antenna model
        antennaModel = "TRM59800.00";

          // Get receiver antenna parameters
          // Warning: If no corrections are not found for one specific
          //          radome, then the antenna with radome NONE are used.
       try
       {
           receiverAntenna = antexReader.getAntenna( antennaModel );
       }
       catch(ObjectNotFound& notFound)
       {
             // new antenna model
           antennaModel.replace(16,4,"NONE");
             // new receiver antenna with new antenna model
           receiverAntenna = antexReader.getAntenna( antennaModel );
       }

    }

    // Object to compute satellite antenna phase center effect
    /// Because of using brdc ephemeris, we deedn't this correction.////
//    ComputeSatPCenter svPcenter(pppEKF);
//    if( useantex )
//    {
//          // Feed 'ComputeSatPCenter' object with 'AntexReader' object
//        svPcenter.setAntexReader( antexReader );
//    }

//    correctList.push_back(svPcenter);       // Add to processing list


       // Declare an object to correct observables to monument
    CorrectObservables corr(ephStore,pppEKF);
    corr.setMonument( offsetARP );

       // Check if we want to use Antex patterns
    bool usepatterns(m_confReader.getValueAsBoolean("usePCPatterns" ));
    if( useantex && usepatterns )
    {
        corr.setAntenna( receiverAntenna );

           // Should we use elevation/azimuth patterns or just elevation?
        corr.setUseAzimuth(m_confReader.getValueAsBoolean("useAzim" ));
    }
    else
    {
          // Fill vector from antenna ARP to L1 phase center [UEN], in meters
        offsetL1[0] = m_confReader.fetchListValueAsDouble("offsetL1");
        offsetL1[1] = m_confReader.fetchListValueAsDouble("offsetL1");
        offsetL1[2] = m_confReader.fetchListValueAsDouble("offsetL1");

           // Vector from antenna ARP to L2 phase center [UEN], in meters
        offsetL2[0] = m_confReader.fetchListValueAsDouble("offsetL2");
        offsetL2[1] = m_confReader.fetchListValueAsDouble("offsetL2");
        offsetL2[2] = m_confReader.fetchListValueAsDouble("offsetL2");

        corr.setL1pc( offsetL1 );
        corr.setL2pc( offsetL2 );

    }

    correctList.push_back(corr);       // Add to processing list


       // Object to compute wind-up effect
    ComputeWindUp windup( ephStore ,pppEKF);
    if( useantex )
    {
          // Feed 'ComputeSatPCenter' object with 'AntexReader' object
        windup.setAntexReader( antexReader );
    }

    correctList.push_back(windup);       // Add to processing list


       // Declare a NeillTropModel object
    NeillTropModel neillTM;

       // Object to compute the tropospheric data
    ComputeTropModel computeTropo(neillTM);
    correctList.push_back(computeTropo);       // Add to processing list


       // Object to compute code combination with minus ionospheric delays
       // for L1/L2 calibration
    ComputeLinear linear2;

    linear2.addLinear(comb.q1Combination);
    linear2.addLinear(comb.q2Combination);
    correctList.push_back(linear2);       // Add to processing list


       // Object to align phase with code measurements
    PhaseCodeAlignment phaseAlignL1;
    phaseAlignL1.setCodeType(TypeID::Q1);
    phaseAlignL1.setPhaseType(TypeID::L1);
    phaseAlignL1.setPhaseWavelength(0.190293672798);
    //phaseAlignL1.setUseSatArc(true);
    correctList.push_back(phaseAlignL1);       // Add to processing list

       // Object to align phase with code measurements
    PhaseCodeAlignment phaseAlignL2;
    phaseAlignL2.setCodeType(TypeID::Q2);
    phaseAlignL2.setPhaseType(TypeID::L2);
    phaseAlignL2.setPhaseWavelength(0.244210213425);
    //phaseAlignL2.setUseSatArc(true);
    correctList.push_back(phaseAlignL2);       // Add to processing list


       // Object to compute ionosphere-free combinations to be used
       // as observables in the PPP processing
    ComputeLinear linear3;
    linear3.addLinear(comb.pcCombination);
    linear3.addLinear(comb.lcCombination);
    correctList.push_back(linear3);       // Add to processing list


       // Declare a simple filter object to screen PC
    SimpleFilter pcFilter;
    pcFilter.setFilteredType(TypeID::PC);

       // IMPORTANT NOTE:
       // Like in the "filterCode" case, the "filterPC" option allows you to
       // deactivate the "SimpleFilter" object that filters out PC, in case
       // you need to.
    bool filterPC( m_confReader.getValueAsBoolean( "filterPC") );

       // Check if we are going to use this "SimpleFilter" object or not
    if( filterPC )
    {
        correctList.push_back(pcFilter);       // Add to processing list
    }


       // Object to compute prefit-residuals
    ComputeLinear linear4(comb.pcPrefit);
    linear4.addLinear(comb.lcPrefit);
    correctList.push_back(linear4);       // Add to processing list



       // Object to compute DOP values
    ComputeDOP cDOP;
    correctList.push_back(cDOP);       // Add to processing list

       // Get the obsInterval
    double decimateInterval(m_confReader.getValueAsDouble( "decimationInterval"));

       // Declare solver objects
    SolverPPPPredict pppPredictSolver(pppEKF,decimateInterval);

    SolverPPPCorrect pppCorrectSolver(pppEKF);

       // Get if we want 'forwards-backwards' or 'forwards' processing only
    int cycles( m_confReader.getValueAsInt("filterCycles") );

       // Get if kinematic mode is on.
    bool kinematic( m_confReader.getValueAsBoolean( "KinematicMode") );
    double accSigma(m_confReader.getValueAsDouble("AccelerationSigma"));
    string result_format(m_confReader.getValue("resultFormat"));

          // Check about coordinates as white noise
    if ( kinematic )
    {
             // Reconfigure solver
         pppPredictSolver.setKinematic();
         pppPredictSolver.setAccSigma(accSigma);

    }

       // Add solver to processing list
    predictList.push_back(pppPredictSolver);
    correctList.push_back(pppCorrectSolver);

       // Object to compute tidal effects
    SolidTides solid;


       // Configure ocean loading model
    OceanLoading ocean(blqStore);

       // Object to model pole tides
    PoleTides pole(eopStore);


    // Prepare for printing
    int precision( m_confReader.getValueAsInt( "precision" ) );


    string outputFileName = "ppp_albh0.out";
    ofstream outfile;
    outfile.open( outputFileName.c_str(), ios::out );

      // Print Header
    outfile << "% Program : PPPBox\n";
    outfile << "% Positioning Mode : ";
    if (kinematic)
    {
        outfile << "kinematic\n";
    }
    else
    {
        outfile << "static\n";
    }
    outfile << "% Year"
            << setw(6) << "Doy"
            << setw(12)<< "Second";
    if (kinematic)
    {
        outfile << "kinematic\n";
    }
    else
    {
        outfile << "static\n";
    }
    outfile << "% Year"
            << setw(6) << "Doy"
            << setw(12)<< "Second";

    if ( result_format == "BLH")
    {
        outfile << setw(16) << "Lat(deg)"
                << setw(16) << "Lon(deg)"
                << setw(14) << "Height(m)";
    }
    else if ( result_format == "XYZ")
    {
        outfile << setw(14) <<  "X(m)"
                << setw(14) <<  "Y(m)"
                << setw(14) <<  "Z(m)";
    }
    else if ( result_format == "dxyz")
    {
        outfile << setw(14) <<  "dx(m)"
                << setw(14) <<  "dy(m)"
                << setw(14) <<  "dz(m)";
    }
    else if ( result_format == "dneu")
    {
        outfile << setw(14) <<  "dn(m)"
                << setw(14) <<  "de(m)"
                << setw(14) <<  "du(m)";
    }
    else
    {
        cout << "Unknown Result Format !" << endl;
        exit(-1);
    }

    outfile << setw(10) << "ZTD(m)"
            << setw(6)  << "nSat"
            << setw(7)  << "GDOP"
            << setw(8)  << "PDOP" << endl;

       // Let's check if we are going to print the model
    bool printmodel( m_confReader.getValueAsBoolean( "printModel" ) );

    string modelName;
    ofstream modelfile;

    double drytropo(0.0);
    bool firstTime = true;

    gnssRinex gRin;
    while(1)
    {
        // Wait for observation data's comming
        unique_lock<mutex> lock(SIG_CENTER->m_allObsMutex);
        SIG_CENTER->m_condObsReady.wait(lock);

        // Put data into the queue
        if(m_staObsMap.size() == 0)
        {
            continue;
        }
        m_staObsQueue.push_back(m_staObsMap);

        // Process the oldest epochs
        while(m_staObsQueue.size() != 0)
        {
            StaObsMap staObsMap = m_staObsQueue.front();
            StaObsMap::iterator itm = staObsMap.begin();
            list<t_satObs>::iterator itl = (itm->second).begin();
            CommonTime& epoTime =  itl->_time;
            if(waitForCorr(epoTime))
            {
                break;
            }

            double dt = epoTime - m_lastClkCorrTime;
            string utcTime = CivilTime(epoTime).printf("%02H:%02M:%02S");
            cout << "epotime2: " << utcTime <<", dt: " << dt << endl;

            SIG_CENTER->m_gpsEphMutex.lock();
            ephStore = *(SIG_CENTER->m_ephStore);
            SIG_CENTER->m_gpsEphMutex.unlock();
            ephStore.usingCorrection(true);
            basic.setDefaultEphemeris(ephStore);
            corr.setEphemeris(ephStore);
            windup.setEphemeris(ephStore);

            // ***********************
            // Loop all stations' observation data at one epoch
            // ***********************
            while(itm!=staObsMap.end())
            {
                list<t_satObs> obsList = itm->second;
                string station = (obsList.begin())->_staID;

                // Four character station
                string staID4 = station.substr(0, 4);

                // Let's check the ocean loading data for current station before
                // the real data processing.
                if( ! blqStore.isValid(staID4) )
                {
                    cout << "There is no BLQ data for current station:" << station << endl;
                    cout << "Current staion will be not processed !!!!" << endl;
                    continue;
                }

                cout << "Process for station ==>" << station << endl;
                try
                {
                    Rinex3ObsHeader header = SIG_CENTER->m_obsStream->m_header;
                    gRin =  obsList2gnssRinex(obsList,header);

                       // Preprocess
                    //cout << gRin.body;
                    gRin >> preprocessList;
                    if(firstTime)
                    {
                        cout << "Starting processing for station: '" << station << "'." << endl;

                        Bancroft bancroft;

                        SatIDSet currSatSet(gRin.body.getSatID());

                        int numSats(currSatSet.size());

                        Matrix<double> data(numSats,4,0.0);

                        Vector<double> solution;

                        int i(0);
                            // loop in currSatSet
                        for(SatIDSet::const_iterator it = currSatSet.begin();
                            it != currSatSet.end();
                            it++)
                         {
                               // get the postion and clock bias of given satellite
                             Xvt svPosVel(ephStore.getXvt(*it,epoTime));

                             data(i,0) = svPosVel.x[0];         // X
                             data(i,1) = svPosVel.x[1];         // Y
                             data(i,2) = svPosVel.x[2];         // Z
                             data(i,3) = gRin.body[*it][TypeID::P1]+C_MPS*svPosVel.clkbias;
                             i++;
                         }

                         int ret = bancroft.Compute(data,solution);
                         double x,y,z;
                         x = solution[0];
                         y = solution[1];
                         z = solution[2];
                         Position recPos(solution[0],solution[1],solution[2]);
                         pppEKF.setRxPosition(recPos);

                           // no more first time
                         firstTime = false;
                    } // End of 'if (firstTime)'

                    gRin >> predictList;       // TimeUpdate

                    Position tempPos(pppEKF.getRxPosition());
                    cout << "predPOS: " << tempPos << endl;
                      // Compute solid, oceanic and pole tides effects at this epoch
                    Triple tides( solid.getSolidTide( epoTime, tempPos)  +
                                 ocean.getOceanLoading( staID4, epoTime )  +
                                 pole.getPoleTide( epoTime, tempPos) );

                      // Update observable correction object with tides information
                    corr.setExtraBiases(tides);

                      // reset the receiver position
                    neillTM.setAllParameters(initialTime,tempPos);

                    gRin >> correctList;       // MeasUpdate
                    cout << "corrPOS: " << pppEKF.getRxPosition() << endl;
                      // Get the dry ZTD
                    drytropo = neillTM.dry_zenith_delay();
                }
                catch(DecimateEpoch& d)
                {
                      // If we catch a DecimateEpoch exception, just continue.
                   continue;
                }
                catch(SVNumException& s)
                {
                      // If we catch a SVNumException, just continue.
                   cerr << "SVNumException '" << station <<
                            "' at epoch: " << epoTime << "; " << s << endl;
                   continue;
                }
                catch(Exception& e)
                {
                   cerr << "Exception for receiver '" << station <<
                           "' at epoch: " << epoTime << "; " << e << endl;
                   continue;
                }
                catch(...)
                {
                   cerr << "Unknown exception for receiver '" << station <<
                           " at epoch: " << epoTime << endl;
                   continue;
                }


                   // Ask if we are going to print the model
                if ( printmodel )
                {
                   printModel( modelfile, gRin, precision );

                }

                   // Check what type of solver we are using
                if ( cycles < 1 )
                {

                      // This is a 'forwards-only' filter. Let's print to output
                      // file the results of this epoch
                    Position precisePos(-2341333.077, -3539049.529, 4745791.267);
                    printSolution( outfile,
                                   pppCorrectSolver,
                                   epoTime,
                                   cDOP,
                                   gRin.numSats(),
                                   drytropo,
                                   precisePos,
                                   result_format,
                                   precision );

                }  // End of 'if ( cycles < 1 )'

                ++itm;
            } // End of 'while(itm!=staObsMap.end())'

            if(m_staObsQueue.size()!=0)
            {
                m_staObsQueue.pop_front();
            }

        } // End of 'while(m_staObsQueue.size() != 0 && !waitForCorr(eTime))'

    }  // End of 'while(1)'


    //***********************************************
    //
    // At last, Let's clear the content of EOP object
    //
    //***********************************************
    eopStore.clear();
}

void PPPTask::processFiles()
{
    string obsFile = "/home/qi/program/PPPBox/workplace/ppprt/onsa0190.15o";
    string erpFileList = "/home/qi/program/PPPBox/workplace/ppprt/erplist";
    string ephFileList = "/home/qi/program/PPPBox/workplace/ppprt/ephlist";
    string clkFileList = "/home/qi/program/PPPBox/workplace/ppprt/clklist";
    string mscFileName = "/home/qi/program/PPPBox/workplace/ppprt/msc.txt";

    spinUp();

    SP3EphemerisStore SP3EphList;

       // Set flags to reject satellites with bad or absent positional
       // values or clocks
    SP3EphList.rejectBadPositions(true);
    SP3EphList.rejectBadClocks(true);

       // Now read sp3 files from 'sp3FileList'
    ifstream sp3FileListStream;

    sp3FileListStream.open(ephFileList.c_str(), ios::in);
    if(!sp3FileListStream)
    {
          // If file doesn't exist, issue a warning
       cerr << "SP3 file List Name'" << ephFileList << "' doesn't exist or you don't "
            << "have permission to read it. Skipping it." << endl;

       exit(-1);
    }

    string sp3File;
    while( sp3FileListStream >> sp3File )
    {
       try
       {
          SP3EphList.loadFile( sp3File );
       }
       catch (FileMissingException& e)
       {
             // If file doesn't exist, issue a warning
          cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
               << "have permission to read it. Skipping it." << endl;
          continue;
       }
    }
       // Close file
    sp3FileListStream.close();

       //***********************
       // Let's read clock files
       //***********************

       // If rinex clock file list is given, then use rinex clock
          // Now read clk files from 'clkFileList'
       ifstream clkFileListStream;

          // Open clkFileList File
       clkFileListStream.open(clkFileList.c_str(), ios::in);
       if(!clkFileListStream)
       {
             // If file doesn't exist, issue a warning
          cerr << "clock file List Name'" << clkFileList << "' doesn't exist or you don't "
               << "have permission to read it. Skipping it." << endl;

          exit(-1);
       }

       string clkFile;
       while( clkFileListStream >> clkFile )
       {
          try
          {
             SP3EphList.loadRinexClockFile( clkFile );
          }
          catch (FileMissingException& e)
          {
                // If file doesn't exist, issue a warning
             cerr << "rinex CLK file '" << clkFile << "' doesn't exist or you don't "
                  << "have permission to read it. Skipping it." << endl;
             continue;
          }
       }

          // Close file
       clkFileListStream.close();


       // Let's read ocean loading BLQ data files
       //***********************

       // BLQ data store object
    BLQDataReader blqStore;

       // Read BLQ file name from the configure file
    string blqFile = m_confReader.getValue( "oceanLoadingFile", "DEFAULT");

    try
    {
       blqStore.open( blqFile );
    }
    catch (FileMissingException& e)
    {
          // If file doesn't exist, issue a warning
       cerr << "BLQ file '" << blqFile << "' doesn't exist or you don't "
            << "have permission to read it. Skipping it." << endl;
       exit(-1);
    }

       //***********************
       // Let's read eop files
       //***********************

       // Declare a "EOPDataStore" object to handle earth rotation parameter file
    EOPDataStore eopStore;

       // Now read eop files from 'eopFileList'
    ifstream eopFileListStream;

       // Open eopFileList File
    eopFileListStream.open(erpFileList.c_str(), ios::in);
    if(!eopFileListStream)
    {
          // If file doesn't exist, issue a warning
       cerr << "erp file List Name'" << erpFileList << "' doesn't exist or you don't "
            << "have permission to read it. Skipping it." << endl;

       exit(-1);
    }

    string eopFile;
    while( eopFileListStream >> eopFile )
    {
       try
       {
          eopStore.loadIGSFile( eopFile );
       }
       catch (FileMissingException& e)
       {
             // If file doesn't exist, issue a warning
          cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
               << "have permission to read it. Skipping it." << endl;
          continue;
       }
    }
       // Close file
    eopFileListStream.close();

       //**********************************************
       // Now, Let's read MSC data
       //**********************************************

       // Declare a "MSCStore" object to handle msc file
    MSCStore mscStore;

    try
    {
       mscStore.loadFile( mscFileName );
    }
    catch (gpstk::FFStreamError& e)
    {
          // If file doesn't exist, issue a warning
       cerr << e << endl;
       cerr << "MSC file '" << mscFileName << "' Format is not supported!!!"
            << "stop." << endl;
       exit(-1);
    }
    catch (FileMissingException& e)
    {
          // If file doesn't exist, issue a warning
       cerr << "MSC file '" << mscFileName << "' doesn't exist or you don't "
            << "have permission to read it." << endl;
       exit(-1);
    }

       //**********************************************************
       // Now, Let's perform the PPP for each rinex files
       //**********************************************************


    string rnxFile = obsFile;
          // ===================
          // Let's read rinex file list !!!!
          // ===================


          // Create input observation file stream
       RinexObsStream rin;
       rin.exceptions(ios::failbit); // Enable exceptions

          // Try to open Rinex observations file
       try
       {
          rin.open( rnxFile, std::ios::in );
       }
       catch(...)
       {
          cerr << "Problem opening file '"
               << rnxFile
               << "'." <<endl;
          cerr << "Maybe it doesn't exist or you don't have "
               << "proper read permissions."
               << endl;

          cerr << "Skipping receiver '" << rnxFile << "'."
               << endl;

             // Close current Rinex observation stream
          rin.close();

       }  // End of 'try-catch' block


          // ===================
          // Let's read the header firstly!!!!
          // ===================
       RinexObsHeader roh;
       try
       {
          rin >> roh;
       }
       catch(FFStreamError& e)
       {
          cerr << "Problem in reading file '"
               << rnxFile
               << "'." << endl;

          cerr << "Skipping receiver '" << rnxFile << "'."
               << endl;

             // Close current Rinex observation stream
          rin.close();

       }

          // Get the station name for current rinex file
       string station = roh.markerName.substr(0,4);

          // First time for this rinex file
       CommonTime initialTime( roh.firstObs );
       initialTime.setTimeSystem(TimeSystem::GPS);

          // MSC data for this station
       MSCData mscData;
       try
       {
          mscData = mscStore.findMSC( station, initialTime );
       }
       catch (InvalidRequest& ie)
       {
             // If file doesn't exist, issue a warning
          cerr << "The station " << station
               << " isn't included in MSC file." << endl;
       }
          // get the precise position of this station( X,Y,Z in ITRF )
       Position precisePos(mscData.coordinates);

          // Show a message indicating that we are starting with this station
       cout << "Starting processing for station : '" << station << "'." << endl;

       if ( !blqStore.isValid(station) )
       {
           cerr << "Can't find this station in BLQ Store,"
                << "So the ocean tide effect won't be corrected!"
                << endl;
       }

          // Create a 'ProcessingList' object where we'll store
          // the processing objects in order
       ProcessingList preprocessList;
       ProcessingList predictList;
       ProcessingList correctList;

          // This object will check that all required observables are present
       RequireObservables requireObs;
       requireObs.addRequiredType(TypeID::P1);
       requireObs.addRequiredType(TypeID::P2);
       requireObs.addRequiredType(TypeID::L1);
       requireObs.addRequiredType(TypeID::L2);

        // Add 'requireObs' to processing list (it is the first)
       preprocessList.push_back(requireObs);
          // This object will check that code observations are within
          // reasonable limits
       SimpleFilter pObsFilter;
       pObsFilter.setFilteredType(TypeID::P2);
       pObsFilter.addFilteredType(TypeID::P1);
          // IMPORTANT NOTE:
          // It turns out that some receivers don't correct their clocks
          // from drift.
          // When this happens, their code observations may drift well beyond
          // what it is usually expected from a pseudorange. In turn, this
          // effect causes that "SimpleFilter" objects start to reject a lot of
          // satellites.
          // Thence, the "filterCode" option allows you to deactivate the
          // "SimpleFilter" object that filters out C1, P1 and P2, in case you
          // need to.
       bool filterCode( m_confReader.getValueAsBoolean( "filterCode" ) );

          // Check if we are going to use this "SimpleFilter" object or not
       if( filterCode )
       {
          preprocessList.push_back(pObsFilter);       // Add to processing list
       }


          // This object defines several handy linear combinations
       LinearCombinations comb;


          // Object to compute linear combinations for cycle slip detection
       ComputeLinear linear1;

       linear1.addLinear(comb.pdeltaCombination);
       linear1.addLinear(comb.mwubbenaCombination);
       linear1.addLinear(comb.ldeltaCombination);
       linear1.addLinear(comb.liCombination);
       preprocessList.push_back(linear1);       // Add to processing list

          // Objects to mark cycle slips
       LICSDetector markCSLI;                   // Checks LI cycle slips
       preprocessList.push_back(markCSLI);      // Add to processing list
       MWCSDetector markCSMW;                   // Checks Merbourne-Wubbena cycle slips
       preprocessList.push_back(markCSMW);      // Add to processing list


          // Object to keep track of satellite arcs
       SatArcMarker2 markArc;
       markArc.setDeleteUnstableSats(false);
       markArc.setUnstablePeriod(151.0);
       preprocessList.push_back(markArc);       // Add to processing list


          // Object to decimate data
       Decimate decimateData(
                m_confReader.getValueAsDouble( "decimationInterval"),
                m_confReader.getValueAsDouble( "decimationTolerance"),
                initialTime );
       preprocessList.push_back(decimateData);  // Add to processing list

          // class to store the information of EKF
       PPPExtendedKalmanFilter pppEKF;

          // Declare a basic modeler
       BasicModel basic(pppEKF, SP3EphList);
          // Set the minimum elevation
       basic.setMinElev(m_confReader.getValueAsDouble("cutOffElevation"));
          // If we are going to use P1 instead of C1, we must reconfigure 'basic'
       basic.setDefaultObservable(TypeID::P1);
          // Add to processing list
       predictList.push_back(basic);
       correctList.push_back(basic);

          // Object to remove eclipsed satellites
       EclipsedSatFilter eclipsedSV;
       predictList.push_back(eclipsedSV);       // Add to processing list

          // Object to compute weights based on elevation
       ComputeElevWeights elevWeights;
       correctList.push_back(elevWeights);      // Add to processing list


          // Object to compute gravitational delay effects
       GravitationalDelay grDelay(pppEKF);
       correctList.push_back(grDelay);          // Add to processing list


          // Vector from monument to antenna ARP [UEN], in meters
       Triple offsetARP( roh.antennaOffset );

          // Declare some antenna-related variables
       Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
       AntexReader antexReader;
       Antenna receiverAntenna;

          // Check if we want to use Antex information
       bool useantex( m_confReader.getValueAsBoolean( "useAntex") );
       string antennaModel;
       if( useantex )
       {
             // Feed Antex reader object with Antex file
          antexReader.open( m_confReader.getValue( "antexFile" ) );

             // Antenna model
          antennaModel = roh.antType;

             // Get receiver antenna parameters
             // Warning: If no corrections are not found for one specific
             //          radome, then the antenna with radome NONE are used.
          try
          {
             receiverAntenna = antexReader.getAntenna( antennaModel );
          }
          catch(ObjectNotFound& notFound)
          {
                // new antenna model
             antennaModel.replace(16,4,"NONE");
                // new receiver antenna with new antenna model
             receiverAntenna = antexReader.getAntenna( antennaModel );
          }

       }

          // Object to compute satellite antenna phase center effect
       ComputeSatPCenter svPcenter(pppEKF);
       if( useantex )
       {
             // Feed 'ComputeSatPCenter' object with 'AntexReader' object
          svPcenter.setAntexReader( antexReader );
       }

       correctList.push_back(svPcenter);       // Add to processing list


          // Declare an object to correct observables to monument
       CorrectObservables corr(SP3EphList,pppEKF);
       corr.setMonument( offsetARP );

          // Check if we want to use Antex patterns
       bool usepatterns(m_confReader.getValueAsBoolean("usePCPatterns" ));
       if( useantex && usepatterns )
       {
          corr.setAntenna( receiverAntenna );

             // Should we use elevation/azimuth patterns or just elevation?
          corr.setUseAzimuth(m_confReader.getValueAsBoolean("useAzim" ));
       }
       else
       {
             // Fill vector from antenna ARP to L1 phase center [UEN], in meters
          offsetL1[0] = m_confReader.fetchListValueAsDouble("offsetL1");
          offsetL1[1] = m_confReader.fetchListValueAsDouble("offsetL1");
          offsetL1[2] = m_confReader.fetchListValueAsDouble("offsetL1");

             // Vector from antenna ARP to L2 phase center [UEN], in meters
          offsetL2[0] = m_confReader.fetchListValueAsDouble("offsetL2");
          offsetL2[1] = m_confReader.fetchListValueAsDouble("offsetL2");
          offsetL2[2] = m_confReader.fetchListValueAsDouble("offsetL2");

          corr.setL1pc( offsetL1 );
          corr.setL2pc( offsetL2 );

       }

       correctList.push_back(corr);       // Add to processing list


          // Object to compute wind-up effect
       ComputeWindUp windup( SP3EphList,pppEKF);
       if( useantex )
       {
             // Feed 'ComputeSatPCenter' object with 'AntexReader' object
          windup.setAntexReader( antexReader );
       }

       correctList.push_back(windup);       // Add to processing list


          // Declare a NeillTropModel object
       NeillTropModel neillTM;

          // Object to compute the tropospheric data
       ComputeTropModel computeTropo(neillTM);
       correctList.push_back(computeTropo);       // Add to processing list


          // Object to compute code combination with minus ionospheric delays
          // for L1/L2 calibration
       ComputeLinear linear2;

       linear2.addLinear(comb.q1Combination);
       linear2.addLinear(comb.q2Combination);
       correctList.push_back(linear2);       // Add to processing list


          // Object to align phase with code measurements
       PhaseCodeAlignment phaseAlignL1;
       phaseAlignL1.setCodeType(TypeID::Q1);
       phaseAlignL1.setPhaseType(TypeID::L1);
       phaseAlignL1.setPhaseWavelength(0.190293672798);

       correctList.push_back(phaseAlignL1);       // Add to processing list

          // Object to align phase with code measurements
       PhaseCodeAlignment phaseAlignL2;
       phaseAlignL2.setCodeType(TypeID::Q2);
       phaseAlignL2.setPhaseType(TypeID::L2);
       phaseAlignL2.setPhaseWavelength(0.244210213425);
       correctList.push_back(phaseAlignL2);       // Add to processing list


          // Object to compute ionosphere-free combinations to be used
          // as observables in the PPP processing
       ComputeLinear linear3;
       linear3.addLinear(comb.pcCombination);
       linear3.addLinear(comb.lcCombination);
       correctList.push_back(linear3);       // Add to processing list


          // Declare a simple filter object to screen PC
       SimpleFilter pcFilter;
       pcFilter.setFilteredType(TypeID::PC);

          // IMPORTANT NOTE:
          // Like in the "filterCode" case, the "filterPC" option allows you to
          // deactivate the "SimpleFilter" object that filters out PC, in case
          // you need to.
       bool filterPC( m_confReader.getValueAsBoolean( "filterPC") );

          // Check if we are going to use this "SimpleFilter" object or not
       if( filterPC )
       {
          correctList.push_back(pcFilter);       // Add to processing list
       }


          // Object to compute prefit-residuals
       ComputeLinear linear4(comb.pcPrefit);
       linear4.addLinear(comb.lcPrefit);
       correctList.push_back(linear4);       // Add to processing list


          // Object to compute DOP values
       ComputeDOP cDOP;
       correctList.push_back(cDOP);       // Add to processing list


          // Get the obsInterval
       double decimateInterval(m_confReader.getValueAsDouble( "decimationInterval"));
          // Declare solver objects
       SolverPPPPredict pppPredictSolver(pppEKF,decimateInterval);

       SolverPPPCorrect pppCorrectSolver(pppEKF);

          // Get if we want 'forwards-backwards' or 'forwards' processing only
       int cycles( m_confReader.getValueAsInt("filterCycles") );

          // Get if kinematic mode is on.
       bool kinematic( m_confReader.getValueAsBoolean( "KinematicMode") );
       double accSigma(m_confReader.getValueAsDouble("AccelerationSigma"));
       string result_format(m_confReader.getValue("resultFormat"));

             // Check about coordinates as white noise
       if ( kinematic )
       {
                // Reconfigure solver
            pppPredictSolver.setKinematic();
            pppPredictSolver.setAccSigma(accSigma);

       }

          // Add solver to processing list
       predictList.push_back(pppPredictSolver);
       correctList.push_back(pppCorrectSolver);

          // Object to compute tidal effects
       SolidTides solid;


          // Configure ocean loading model
       OceanLoading ocean(blqStore);

          // Object to model pole tides
       PoleTides pole(eopStore);


          // This is the GNSS data structure that will hold all the
          // GNSS-related information
       gnssRinex gRin;


          // Prepare for printing
       int precision( m_confReader.getValueAsInt( "precision" ) );


       string outputFileName;

       outputFileName = ( rnxFile + ".out3" );

       ofstream outfile;
       outfile.open( outputFileName.c_str(), ios::out );

         // Print Header
       outfile << "% Program : PPPBox\n";
       outfile << "% Positioning Mode : ";
       if (kinematic)
       {
           outfile << "kinematic\n";
       }
       else
       {
           outfile << "static\n";
       }
       outfile << "% Year"
               << setw(6) << "Doy"
               << setw(12)<< "Second";

       if ( result_format == "BLH")
       {
           outfile << setw(16) << "Lat(deg)"
                   << setw(16) << "Lon(deg)"
                   << setw(14) << "Height(m)";
       }
       else if ( result_format == "XYZ")
       {
           outfile << setw(14) <<  "X(m)"
                   << setw(14) <<  "Y(m)"
                   << setw(14) <<  "Z(m)";
       }
       else if ( result_format == "dxyz")
       {
           outfile << setw(14) <<  "dx(m)"
                   << setw(14) <<  "dy(m)"
                   << setw(14) <<  "dz(m)";
       }
       else if ( result_format == "dneu")
       {
           outfile << setw(14) <<  "dn(m)"
                   << setw(14) <<  "de(m)"
                   << setw(14) <<  "du(m)";
       }
       else
       {
           cout << "Unknown Result Format !" << endl;
           exit(-1);
       }

       outfile << setw(10) << "ZTD(m)"
               << setw(6)  << "nSat"
               << setw(7)  << "GDOP"
               << setw(8)  << "PDOP" << endl;

          // Let's check if we are going to print the model
       bool printmodel( m_confReader.getValueAsBoolean( "printModel" ) );

       string modelName;
       ofstream modelfile;

          // Prepare for model printing
       if( printmodel )
       {
          modelName = rnxFile + ".model";
          modelfile.open( modelName.c_str(), ios::out );
       }


       double drytropo(0.0);
          //// *** Now comes the REAL forwards processing part *** ////
       bool firstTime(true);
          // Loop over all data epochs
       while(rin >> gRin)
       {

             // Store current epoch
          CommonTime time(gRin.header.epoch);

          try
          {
                 // Preprocess
              gRin >> preprocessList;
                 // get a initial receiver position using bancroft method
              if (firstTime)
              {
                 Bancroft bancroft;

                 SatIDSet currSatSet(gRin.body.getSatID());

                 int numSats(currSatSet.size());

                 Matrix<double> data(numSats,4,0.0);

                 Vector<double> solution;

                 int i(0);
                     // loop in currSatSet
                 for(SatIDSet::const_iterator it = currSatSet.begin();
                     it != currSatSet.end();
                     it++)
                  {
                        // get the postion and clock bias of given satellite
                      Xvt svPosVel(SP3EphList.getXvt(*it,time));

                      data(i,0) = svPosVel.x[0];         // X
                      data(i,1) = svPosVel.x[1];         // Y
                      data(i,2) = svPosVel.x[2];         // Z
                      data(i,3) = gRin.body[*it][TypeID::P1]+C_MPS*svPosVel.clkbias;

                      i++;
                  }

                  bancroft.Compute(data,solution);

                  Position recPos(solution[0],solution[1],solution[2]);
                  pppEKF.setRxPosition(recPos);

                    // no more first time
                  firstTime = false;

              }  // End of 'if (firstTime)'

              gRin >> predictList;       // TimeUpdate

              Position tempPos(pppEKF.getRxPosition());
                // Compute solid, oceanic and pole tides effects at this epoch
              Triple oceanTide(0.0,0.0,0.0);
              if ( blqStore.isValid(station) )
              {
                  oceanTide = ocean.getOceanLoading( station, time );
              }

              Triple tides( solid.getSolidTide( time, tempPos) + oceanTide
                            + pole.getPoleTide( time, tempPos) );

                // Update observable correction object with tides information
              corr.setExtraBiases(tides);

                // reset the receiver position
              neillTM.setAllParameters(initialTime,tempPos);

              gRin >> correctList;       // MeasUpdate

                // Get the dry ZTD
              drytropo = neillTM.dry_zenith_delay();

          }
          catch(DecimateEpoch& d)
          {
                // If we catch a DecimateEpoch exception, just continue.
             continue;
          }
          catch(SVNumException& s)
          {
                // If we catch a SVNumException, just continue.
             continue;
          }
          catch(Exception& e)
          {
             cerr << "Exception for receiver '" << station <<
                     "' at epoch: " << time << "; " << e << endl;
             continue;
          }
          catch(...)
          {
             cerr << "Unknown exception for receiver '" << station <<
                     " at epoch: " << time << endl;
             continue;
          }


             // Ask if we are going to print the model
          if ( printmodel )
          {
             printModel( modelfile,
                         gRin,
                         precision );

          }

             // Check what type of solver we are using
          if ( cycles < 1 )
          {

                // This is a 'forwards-only' filter. Let's print to output
                // file the results of this epoch
             printSolution( outfile,
                            pppCorrectSolver,
                            time,
                            cDOP,
                            gRin.numSats(),
                            drytropo,
                            precisePos,
                            result_format,
                            precision );

          }  // End of 'if ( cycles < 1 )'

          // The given epoch hass been processed. Let's get the next one

       }  // End of 'while(rin >> gRin)'


          // Close current Rinex observation stream
       rin.close();

          // If we printed the model, we must close the file
       if ( printmodel )
       {
             // Close model file for this station
          modelfile.close();
       }

          //// *** Forwards processing part is over *** ////



          // Now decide what to do: If solver was a 'forwards-only' version,
          // then we are done and should continue with next station.
       if ( cycles < 1 )
       {
             // Close output file for this station
          outfile.close();

             // We are done with this station. Let's show a message
          cout << "Processing finished for station: '" << station
               << "'. Results in file: '" << outputFileName << "'." << endl;


       }

       //***********************************************
       //
       // At last, Let's clear the content of SP3/EOP/MSC object
       //
       //***********************************************
    SP3EphList.clear();
    eopStore.clear();


    return;
 }  // End of 'ppprt::processFile()'
