#ifndef ELECTRICCAPACITANCE2_HPP_INCLUDED
#define ELECTRICCAPACITANCE2_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file ElectricCapacitance2.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Wed 24 Apr 2013 20:39:48
//! @brief Capacitance
//! @ingroup ElectricComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, HOPSAN++, CompgenModels}/ElectricComponents.nb*/

using namespace hopsan;

class ElectricCapacitance2 : public ComponentC
{
private:
     double Cc;
     double alpha;
     Port *mpPel1;
     Port *mpPel2;
     int mNstep;
     //Port Pel1 variable
     double uel1;
     double iel1;
     double cel1;
     double Zcel1;
     //Port Pel2 variable
     double uel2;
     double iel2;
     double cel2;
     double Zcel2;
//==This code has been autogenerated using Compgen==
     //inputVariables
     //outputVariables
     //InitialExpressions variables
     double cel1r;
     double cel2r;
     double cel1rf;
     double cel2rf;
     //LocalExpressions variables
     //Expressions variables
     //Port Pel1 pointer
     double *mpND_uel1;
     double *mpND_iel1;
     double *mpND_cel1;
     double *mpND_Zcel1;
     //Port Pel2 pointer
     double *mpND_uel2;
     double *mpND_iel2;
     double *mpND_cel2;
     double *mpND_Zcel2;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     //outputVariables pointers
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new ElectricCapacitance2();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;

        //Add ports to the component
        mpPel1=addPowerPort("Pel1","NodeElectric");
        mpPel2=addPowerPort("Pel2","NodeElectric");
        //Add inputVariables to the component

        //Add outputVariables to the component

//==This code has been autogenerated using Compgen==
        //Add constants/parameters
            addConstant("Cc", "Capacitance", "(A s)/V", 1.e-9,Cc);
            addConstant("alpha", "numerical damping", "", 0.1,alpha);
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port Pel1
        mpND_uel1=getSafeNodeDataPtr(mpPel1, NodeElectric::Voltage);
        mpND_iel1=getSafeNodeDataPtr(mpPel1, NodeElectric::Current);
        mpND_cel1=getSafeNodeDataPtr(mpPel1, NodeElectric::WaveVariable);
        mpND_Zcel1=getSafeNodeDataPtr(mpPel1, NodeElectric::CharImpedance);
        //Port Pel2
        mpND_uel2=getSafeNodeDataPtr(mpPel2, NodeElectric::Voltage);
        mpND_iel2=getSafeNodeDataPtr(mpPel2, NodeElectric::Current);
        mpND_cel2=getSafeNodeDataPtr(mpPel2, NodeElectric::WaveVariable);
        mpND_Zcel2=getSafeNodeDataPtr(mpPel2, NodeElectric::CharImpedance);

        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        cel1 = (*mpND_cel1);
        Zcel1 = (*mpND_Zcel1);
        //Port Pel2
        uel2 = (*mpND_uel2);
        iel2 = (*mpND_iel2);
        cel2 = (*mpND_cel2);
        Zcel2 = (*mpND_Zcel2);

        //Read inputVariables from nodes

        //Read outputVariables from nodes

//==This code has been autogenerated using Compgen==
        //InitialExpressions
        cel1r = uel1 + 2*iel1*Zcel1;
        cel2r = uel2 + 2*iel2*Zcel2;
        cel1rf = uel1;
        cel2rf = uel2;

        //LocalExpressions
        cel2r = cel2 + 2*iel2*Zcel2;
        cel1r = cel1 + 2*iel1*Zcel1;
        cel1rf = (1 - alpha)*cel1r + alpha*cel2;
        cel2rf = alpha*cel1 + (1 - alpha)*cel2r;

        //Initialize delays

     }
    void simulateOneTimestep()
     {
        //Read variables from nodes
        //Port Pel1
        uel1 = (*mpND_uel1);
        iel1 = (*mpND_iel1);
        //Port Pel2
        uel2 = (*mpND_uel2);
        iel2 = (*mpND_iel2);

        //Read inputVariables from nodes

        //LocalExpressions
        cel2r = cel2 + 2*iel2*Zcel2;
        cel1r = cel1 + 2*iel1*Zcel1;
        cel1rf = (1 - alpha)*cel1r + alpha*cel2;
        cel2rf = alpha*cel1 + (1 - alpha)*cel2r;

          //Expressions
          Zcel1 = mTimestep/((1 - alpha)*Cc);
          Zcel2 = mTimestep/((1 - alpha)*Cc);
          cel1 = cel2rf;
          cel2 = cel1rf;

        //Calculate the delayed parts


        //Write new values to nodes
        //Port Pel1
        (*mpND_cel1)=cel1;
        (*mpND_Zcel1)=Zcel1;
        //Port Pel2
        (*mpND_cel2)=cel2;
        (*mpND_Zcel2)=Zcel2;
        //outputVariables

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // ELECTRICCAPACITANCE2_HPP_INCLUDED
