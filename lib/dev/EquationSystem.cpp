#pragma ident "$Id: EquationSystem.cpp 2939 2012-04-06 19:55:11Z shjzhang $"

/**
 * @file EquationSystem.cpp
 * Class to define and handle complex equation systems for solvers.
 */

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 
//  ---------
//
//  Shoujian Zhang, Wuhan University, 2015
//
//============================================================================
//
//  History 
//
//  2012/06/19
//  Modify of the Method "getPhiQ()". The gnssRinex is needed to 'Prepare' 
//  the stochastic model. When the variable is source-indexed, the gnssRinex 
//  can be given by finding the source in gdsMap, but when the variable is 
//  only satellite-indexed, the gnssRinex is the first gnssRinex which "SEE"
//  this satellite. (2012.06.19)
//
//  2012/09/01
//  Change 
//  "varUnknowns.insert( currentUnknowns.begin(), currentUnknowns.end() );"
//  to 
//  "varUnknowns = currentUnknowns;"
//
//  2015/07/05 
//  Re-design the whole equation system for SolverGeneral/SolverGeneral2
//
//============================================================================


#include "SystemTime.hpp"
#include "EquationSystem.hpp"
#include <iterator>
#include "Epoch.hpp"
#include "TimeString.hpp"

namespace gpstk
{



      // General white noise stochastic model
   WhiteNoiseModel EquationSystem::whiteNoiseModel;



      /* Add a new equation to be managed.
       *
       * @param equation   Equation object to be added.
       *
       */
   EquationSystem& EquationSystem::addEquation( const Equation& equation )
   {

         // Add "equation" to "equDescriptionList"
      equDescriptionList.push_back(equation);

         // We must "Prepare()" this EquationSystem
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::addEquation()'



      /* Remove an Equation being managed. In this case the equation is
       * identified by its independent term.
       *
       * @param indterm  Variable object of the equation independent term
       *                 (measurement type).
       *
       * \warning All Equations with the same independent term will be
       *          erased.
       */
   EquationSystem& EquationSystem::removeEquation( const Variable& indterm )
   {

         // Create a backup list
      std::list<Equation> backupList;

         // Visit each "Equation" in "equDescriptionList"
      for( std::list<Equation>::const_iterator itEq =
                                                equDescriptionList.begin();
           itEq != equDescriptionList.end();
           ++itEq )
      {

            // If current equation has a different independent term, save it
         if ( (*itEq).getIndependentTerm() != indterm )
         {
            backupList.push_back(*itEq);
         }

      }

         // Clear the full contents of this object
      clearEquations();

         // Add each "Equation" in the backup equation list
      for( std::list<Equation>::const_iterator itEq = backupList.begin();
           itEq != backupList.end();
           ++itEq )
      {
         addEquation(*itEq);
      }

         // We must "Prepare()" this EquationSystem again
      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::removeEquation()'



      // Remove all Equation objects from this EquationSystem.
   EquationSystem& EquationSystem::clearEquations()
   {
         // First, clear the "equDescriptionList"
      equDescriptionList.clear();

      isPrepared = false;

      return (*this);

   }  // End of method 'EquationSystem::clearEquations()'



      /* Prepare this object to carry out its work.
       *
       * @param gData   GNSS data structure (GDS).
       *
       */
   EquationSystem& EquationSystem::Prepare( gnssRinex& gData )
   {

         // First, create a temporary gnssDataMap
      gnssDataMap myGDSMap;

         // Get gData into myGDSMap
      myGDSMap.addGnssRinex( gData );

         // Call the map-enabled method, and return the result
      return (Prepare(myGDSMap));

   }  // End of method 'EquationSystem::Prepare()'



      /* Prepare this object to carry out its work.
       *
       * @param gdsMap     Map of GNSS data structures (GDS), indexed
       *                   by SourceID.
       *
       */
   EquationSystem& EquationSystem::Prepare( gnssDataMap& gdsMap )
   {

         // Let's start storing 'current' unknowns set from 'previous' epoch
      oldUnknowns = currentUnknowns;

         // Former currentUnknowns will belong to global unknowns set
      varUnknowns = currentUnknowns;


         // Let's prepare current sources and satellites
      prepareCurrentSourceSat(gdsMap); 

         // Prepare set of current unknowns and list of current equations
      currentUnknowns = prepareCurrentUnknownsAndEquations(gdsMap);

         // Warning, here we only process the current unknowns
      varUnknowns = currentUnknowns;

         // Compute phiMatrix and qMatrix
      getPhiQ(gdsMap);

         // Build prefit residuals vector
      getPrefitGeometryWeights(gdsMap);

         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'EquationSystem::Prepare()'



      // Get current sources (SourceID's) and satellites (SatID's)
   void EquationSystem::prepareCurrentSourceSat( gnssDataMap& gdsMap )
   {

         // Clear "currentSatSet" and "currentSourceSet"
      currentSatSet.clear();
      currentSourceSet.clear();

         // Insert the corresponding SatID's in "currentSatSet"
      currentSatSet = gdsMap.getSatIDSet();

         // Insert the corresponding SourceID's in "currentSourceSet"
      currentSourceSet = gdsMap.getSourceIDSet();


         // Let's return
      return;

   }  // End of method 'EquationSystem::prepareCurrentSourceSat()'



      // Prepare set of current unknowns and list of current equations
   VariableSet EquationSystem::prepareCurrentUnknownsAndEquations(
                                                         gnssDataMap& gdsMap )
   {
         // Let's clear the current equations list
      currentEquationsList.clear();

         // Let's create 'currentUnkSet' set
      VariableSet currentUnkSet;

         // Let's retrieve the unknowns according to the 
         // equation descriptions and 'gdsMap'

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
            // Then, iterate through corresponding 'sourceDataMap'
         for( sourceDataMap::const_iterator sdmIter = (*it).second.begin();
              sdmIter != (*it).second.end();
              ++sdmIter )
         {
            
               // Visit each "Equation" in "equDescriptionList"
            for( std::list<Equation>::const_iterator itEq = equDescriptionList.begin();
                 itEq != equDescriptionList.end();
                 ++itEq )
            {

                  // Bool indicating whether current source is attributed to
                  // current equation.
               bool found(false);


                  // Check if current equation description is valid for all sources
               if ( (*itEq).getEquationSource() == Variable::allSources )
               {
                   found = true;
               }
                 // Check if equation description is valid for some sources
               else if ( (*itEq).getEquationSource() == Variable::someSources )
               {
                     // First, get the SourceID set for this equation description
                  SourceIDSet equSourceSet =  (*itEq).getSourceSet();

                     // Judge whether current source in inclued in 'equSourceSet'?
                  SourceIDSet::iterator itsrc=equSourceSet.find( (*sdmIter).first );
                  if(itsrc != equSourceSet.end() )
                  {
                     found = true;
                  }
               }
               else
               {
                     // In this case, we take directly the source as 'equSource' 
                  SourceID equSource = (*itEq).getEquationSource() ;
                  if( (equSource) == (*sdmIter).first )
                  {
                     found = true;
                  }

               } // End of 'if ( (*itEq).getEquationSource() == ...'

               if(found)
               {
                     // We need a copy of current Equation object description
                  Equation tempEquation( (*itEq) );

                     // Update equation independent term with SourceID information
                  tempEquation.header.equationSource = (*sdmIter).first;

                     // Iterate the satellite and create the equations
                  for( satTypeValueMap::const_iterator stvmIter =
                                                      (*sdmIter).second.begin();
                       stvmIter != (*sdmIter).second.end();
                       stvmIter++ )
                  {
                        // Set equation satellite
                     tempEquation.header.equationSat = (*stvmIter).first;

                        // Remove all the variables from this equation
                     tempEquation.clear();

                        // Set the type value data 
                     tempEquation.header.typeValueData = (*stvmIter).second;

                        // Now, let's visit all Variables and the corresponding 
                        // coefficient in this equation description
                     for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
                          vcmIter != (*itEq).body.end();
                          ++vcmIter )
                     {
                           // We will work with a copy of current Variable
                        Variable var( (*vcmIter).first );
                          
                           // Take out the coefficient information from equation
                           // description
                        Coefficient coef( (*vcmIter).second);

                           // Check what type of variable we are working on
                           
                           // If variable is source-indexed, set SourceID
                        if( var.getSourceIndexed() )
                        {
                           var.setSource( (*sdmIter).first );
                        }

                           // If variable is not satellite-indexed, we just need to
                           // add it to "currentUnkSet
                        if( !var.getSatIndexed() )
                        {
                              // Insert the result in "currentUnkSet" 
                           currentUnkSet.insert(var);

                              // Add this variable and related coefficient information to 
                              // current equation description. 
                           tempEquation.addVariable(var, coef);
                        }
                        else
                        {
                              // Set satellite
                           var.setSatellite( (*stvmIter).first );

                              // Insert the result in "currentUnkSet" 
                           currentUnkSet.insert(var);

                              // Add this variable and related coefficient information to 
                              // current equation description. 
                           tempEquation.addVariable(var, coef);

                        }  // End of 'if( !var.getSatIndexed() )...'

                     }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

                        // New equation is complete: Add it to 'currentEquationsList'
                     currentEquationsList.push_back( tempEquation );

                  }  // End of 'for( satTypeValueMap::const_iterator ...'

               }  // End of 'if(found)'

            }  // End of 'for(std::list<Equation>::const_iterator)'

         }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      }  // End of 'for( gnssDataMap::const_iterator it = ...'
        
         // Return set of current unknowns
      return currentUnkSet;

   }  // End of method 'EquationSystem::prepareCurrentUnknownsAndEquations()'



      // Compute PhiMatrix
   void EquationSystem::getPhiQ( const gnssDataMap& gdsMap )
   {
    
         // Set to store the variables that already prepared
      VariableSet varUnkPrepared;

      const int numVar( varUnknowns.size() );

         // Resize phiMatrix and qMatrix
      phiMatrix.resize( numVar, numVar, 0.0);
      qMatrix.resize( numVar, numVar, 0.0);

         // Let's get current time
      gnssDataMap::const_iterator it=gdsMap.begin();
      CommonTime epoch((*it).first);

         // Visit each Equation in "currentEquationsList"
      for( std::list<Equation>::const_iterator itEq =
                                                   currentEquationsList.begin();
           itEq != currentEquationsList.end();
           ++itEq )
      {
             // Now, let's visit all Variables and the corresponding 
             // coefficient in this equation description
          for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
               vcmIter != (*itEq).body.end();
               ++vcmIter )
          {
                // We will work with a copy of current Variable
             Variable var( (*vcmIter).first );

                // find this current varialbe in 'varUnkPrepared'
             VariableSet::const_iterator itVar=varUnkPrepared.find( var );

                // If not found, then you should 'prepare' the stochastic model
                // for this variable
             if( itVar == varUnkPrepared.end() )
             {
                SatID varSat(var.getSatellite());
                SourceID varSource(var.getSource());
                typeValueMap tData( (*itEq).header.typeValueData );

                   // Prepare variable's stochastic model
                var.getModel()->Prepare(epoch,
                                        varSource, 
                                        varSat, 
                                        tData);

                   // Now, Let's get the position of this variable in 
                   // 'varUnknowns'
                int i(0);
                VariableSet::const_iterator itVar1=varUnknowns.begin();
                while( (*itVar1) != var )
                {
                   i++;
                   itVar1++;
                }
                   // Now, check if this is an 'old' variable
                if( oldUnknowns.find( var ) != oldUnknowns.end() )
                {
                      // This variable is 'old'; compute its phi and q values
                   phiMatrix(i,i) = var.getModel()->getPhi();
                   qMatrix(i,i)   = var.getModel()->getQ();
                }
                else
                {
                      // This variable is 'new', so let's use its initial variance
                      // instead of its stochastic model
                   phiMatrix(i,i) = 0.0;
                   qMatrix(i,i)   = var.getInitialVariance();

                }

                   // At last, insert current variable into 'varUnkPrepared'
                varUnkPrepared.insert(var);

             }  // End of 'if(itVar==varUnkPrepared.end())'
               
          }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

      }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'


      return;

   }  // End of method 'EquationSystem::getPhiQ()'


      // Compute prefit residuals vector
   void EquationSystem::getPrefitGeometryWeights( gnssDataMap& gdsMap )
   {

         // Declare temporal storage for values
      std::vector<double> tempPrefit;

         // Total number of the current equations
      int numEqu( currentEquationsList.size() );
      int numVar( varUnknowns.size() );

      if(numVar==0)
      {
         GPSTK_THROW(InvalidEquationSystem("varUnknowns is empty, you must set it first!"));
      }

         // Resize hMatrix and rMatrix
      hMatrix.resize( numEqu, numVar, 0.0);
      rMatrix.resize( numEqu, numEqu, 0.0);

         // We need an equation index
      int row(0);

         // Visit each Equation in "currentEquationsList"
      for( std::list<Equation>::const_iterator itEq =
                                                   currentEquationsList.begin();
           itEq != currentEquationsList.end();
           ++itEq )
      {
            // Get the type value data from the header of the equation
         typeValueMap tData( (*itEq).header.typeValueData );

            // Get the independent type of this equation
         TypeID indepType( (*itEq).header.indTerm.getType() );

//       please check the prefit in the gnssDataMap

            // Temp measurement
         double tempMeas(tData(indepType));

            // First, fill weights matrix
            // Check if current 'tData' has weight info. If you don't want those
            // weights to get into equations, please don't put them in GDS
         if( tData.find(TypeID::weight) != tData.end() )
         {
               // Weights matrix = Equation weight * observation weight
            rMatrix(row,row) = (*itEq).header.constWeight * tData(TypeID::weight);
         }
         else
         {
               // Weights matrix = Equation weight
            rMatrix(row,row) = (*itEq).header.constWeight;
         }

            // Second, fill geometry matrix: Look for equation coefficients
            
            // Now, let's visit all Variables and the corresponding 
            // coefficient in this equation description
         for( VarCoeffMap::const_iterator vcmIter = (*itEq).body.begin();
              vcmIter != (*itEq).body.end();
              ++vcmIter )
         {
               // We will work with a copy of current Variable
            Variable var( (*vcmIter).first );

               // Coefficient Struct
            Coefficient coef( (*vcmIter).second);

               // Coefficient values
            double tempCoef(0.0);

               // Check if '(*itCol)' unknown variable enforces a specific
               // coefficient, according the coefficient information from
               // the equation
            if( coef.forceDefault )
            {
                  // Use default coefficient
               tempCoef = coef.defaultCoefficient;
            }
            else
            {
                  // Look the coefficient in 'tdata'

                  // Get type of current unknown
               TypeID type( var.getType() );

                  // Check if this type has an entry in current GDS type set
               if( tData.find(type) != tData.end() )
               {
                     // If type was found, insert value into hMatrix
                  tempCoef = tData(type);
               }
               else
               {
                     // If value for current type is not in gdsMap, then
                     // insert default coefficient for this variable
                  tempCoef = coef.defaultCoefficient;
               }

            }  // End of 'if( (*itCol).isDefaultForced() ) ...'

               // Now, Let's get the position of this variable in 
               // 'varUnknowns'
            int col(0);
            VariableSet::const_iterator itVar2=varUnknowns.begin();
            while( (*itVar2) != var )
            {
               col++;
               itVar2++;
            }

               // Set the geometry matrix
            hMatrix(row, col) = tempCoef;
              
         }  // End of 'for( VarCoeffMap::const_iterator vcmIter = ...'

            // insert current 'measurment vector' into 'tempPrefit'
         tempPrefit.push_back(tempMeas);

            // Increment row number
         ++row;

      }  // End of 'for( std::list<Equation>::const_iterator itEq = ...'

         // Then, finally get prefit residuals into appropriate gpstk::Vector
      measVector = tempPrefit;


      return;

   }  // End of method 'EquationSystem::getPrefitGeometryWeights()'




      /* Return the TOTAL number of variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getTotalNumVariables() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return varUnknowns.size();

   }  // End of method 'EquationSystem::getTotalNumVariables()'



      /* Return the set containing all variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   VariableSet EquationSystem::getVarUnknowns() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return varUnknowns;

   }  // End of method 'EquationSystem::getVarUnknowns()'



      /* Return the CURRENT number of variables, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumVariables() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentUnknowns.size();

   }  // End of method 'EquationSystem::getCurrentNumVariables()'



      /* Return the set containing variables being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   VariableSet EquationSystem::getCurrentUnknowns() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentUnknowns;

   }  // End of method 'EquationSystem::getCurrentUnknowns()'



      /* Return the CURRENT number of sources, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumSources() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSourceSet.size();

   }  // End of method 'EquationSystem::getCurrentNumSources()'



      /* Return the set containing sources being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   SourceIDSet EquationSystem::getCurrentSources() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSourceSet;

   }  // End of method 'EquationSystem::getCurrentSources()'



      /* Return the CURRENT number of satellites, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   int EquationSystem::getCurrentNumSats() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSatSet.size();

   }  // End of method 'EquationSystem::getCurrentNumSats()'



      /* Return the set containing satellites being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   SatIDSet EquationSystem::getCurrentSats() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return currentSatSet;

   }  // End of method 'EquationSystem::getCurrentSats()'



      /* Get prefit residuals GPSTk Vector, given the current equation
       *  system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Vector<double> EquationSystem::getPrefitsVector() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return measVector;

   }  // End of method 'EquationSystem::getPrefitsVector()'



      /* Get geometry matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getGeometryMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return hMatrix;

   }  // End of method 'EquationSystem::getGeometryMatrix()'



      /* Get weights matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getWeightsMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return rMatrix;

   }  // End of method 'EquationSystem::getWeightsMatrix()'


      /* Get the State Transition Matrix (PhiMatrix), given the current
       * equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getPhiMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return phiMatrix;

   }  // End of method 'EquationSystem::getPhiMatrix()'



      /* Get the Process Noise Covariance Matrix (QMatrix), given the
       * current equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquationSystem exception.
       */
   Matrix<double> EquationSystem::getQMatrix() const
      throw(InvalidEquationSystem)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquationSystem("EquationSystem is not prepared"));
      }

      return qMatrix;

   }  // End of method 'EquationSystem::getQMatrix()'



}  // End of namespace gpstk
