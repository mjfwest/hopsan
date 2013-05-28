#ifndef HYDRAULICCENTRIFUGALPUMPJ_HPP_INCLUDED
#define HYDRAULICCENTRIFUGALPUMPJ_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file HydraulicCentrifugalPumpJ.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Mon 27 May 2013 12:24:32
//! @brief Centrifugal pump
//! @ingroup HydraulicComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, HOPSAN++, CompgenModels}/HydraulicComponents.nb*/

using namespace hopsan;

class HydraulicCentrifugalPumpJ : public ComponentQ
{
private:
     double kl;
     double b;
     double d;
     double beta2;
     double Ap;
     double rho;
     double Kcp;
     double Bp;
     double Jp;
     double pcav;
     Port *mpP1;
     Port *mpP2;
     Port *mpPmr1;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     double delayParts4[9];
     double delayParts5[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     double jsyseqnweight[4];
     int order[5];
     int mNstep;
     //Port P1 variable
     double p1;
     double q1;
     double T1;
     double dE1;
     double c1;
     double Zc1;
     //Port P2 variable
     double p2;
     double q2;
     double T2;
     double dE2;
     double c2;
     double Zc2;
     //Port Pmr1 variable
     double tormr1;
     double thetamr1;
     double wmr1;
     double cmr1;
     double Zcmr1;
     double eqInertiamr1;
//==This code has been autogenerated using Compgen==
     //inputVariables
     //outputVariables
     double q2e;
     double Pin;
     double Pout;
     //Expressions variables
     //Port P1 pointer
     double *mpND_p1;
     double *mpND_q1;
     double *mpND_T1;
     double *mpND_dE1;
     double *mpND_c1;
     double *mpND_Zc1;
     //Port P2 pointer
     double *mpND_p2;
     double *mpND_q2;
     double *mpND_T2;
     double *mpND_dE2;
     double *mpND_c2;
     double *mpND_Zc2;
     //Port Pmr1 pointer
     double *mpND_tormr1;
     double *mpND_thetamr1;
     double *mpND_wmr1;
     double *mpND_cmr1;
     double *mpND_Zcmr1;
     double *mpND_eqInertiamr1;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     //outputVariables pointers
     double *mpq2e;
     double *mpPin;
     double *mpPout;
     Delay mDelayedPart10;
     Delay mDelayedPart20;
     Delay mDelayedPart21;
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new HydraulicCentrifugalPumpJ();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;
        jacobianMatrix.create(5,5);
        systemEquations.create(5);
        delayedPart.create(6,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;


        //Add ports to the component
        mpP1=addPowerPort("P1","NodeHydraulic");
        mpP2=addPowerPort("P2","NodeHydraulic");
        mpPmr1=addPowerPort("Pmr1","NodeMechanicRotational");
        //Add inputVariables to the component

        //Add outputVariables to the component
            addOutputVariable("q2e","uncorrected flow","m3/s",0.,&mpq2e);
            addOutputVariable("Pin","input power","W",0.,&mpPin);
            addOutputVariable("Pout","output power","W",0.,&mpPout);

//==This code has been autogenerated using Compgen==
        //Add constants/parameters
            addConstant("kl", "Flow loss koeff.", "", 1.,kl);
            addConstant("b", "outlet axial width", "m", 0.02,b);
            addConstant("d", "Diameter", "m", 0.26,d);
            addConstant("beta2", "Outlet flow angle", "rad", 1.59,beta2);
            addConstant("Ap", "outlet flow area", "m2", 0.0004,Ap);
            addConstant("rho", "Fluid density", "kg/m2", 860,rho);
            addConstant("Kcp", "Leakage coeff", "m3/s/Pa", 1.e-9,Kcp);
            addConstant("Bp", "Visc friction coeff", "N/m/s", 1.,Bp);
            addConstant("Jp", "Visc friction coeff", "N/m/s", 0.1,Jp);
            addConstant("pcav", "cavitaion pressure", "Pa", 100.,pcav);
        mpSolver = new EquationSystemSolver(this,5);
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port P1
        mpND_p1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Pressure);
        mpND_q1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Flow);
        mpND_T1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Temperature);
        mpND_dE1=getSafeNodeDataPtr(mpP1, NodeHydraulic::HeatFlow);
        mpND_c1=getSafeNodeDataPtr(mpP1, NodeHydraulic::WaveVariable);
        mpND_Zc1=getSafeNodeDataPtr(mpP1, NodeHydraulic::CharImpedance);
        //Port P2
        mpND_p2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Pressure);
        mpND_q2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Flow);
        mpND_T2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Temperature);
        mpND_dE2=getSafeNodeDataPtr(mpP2, NodeHydraulic::HeatFlow);
        mpND_c2=getSafeNodeDataPtr(mpP2, NodeHydraulic::WaveVariable);
        mpND_Zc2=getSafeNodeDataPtr(mpP2, NodeHydraulic::CharImpedance);
        //Port Pmr1
        mpND_tormr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::Torque);
        mpND_thetamr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::Angle);
        mpND_wmr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::AngularVelocity);
        mpND_cmr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::WaveVariable);
        mpND_Zcmr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::CharImpedance);
        mpND_eqInertiamr1=getSafeNodeDataPtr(mpPmr1, \
NodeMechanicRotational::EquivalentInertia);

        //Read variables from nodes
        //Port P1
        p1 = (*mpND_p1);
        q1 = (*mpND_q1);
        T1 = (*mpND_T1);
        dE1 = (*mpND_dE1);
        c1 = (*mpND_c1);
        Zc1 = (*mpND_Zc1);
        //Port P2
        p2 = (*mpND_p2);
        q2 = (*mpND_q2);
        T2 = (*mpND_T2);
        dE2 = (*mpND_dE2);
        c2 = (*mpND_c2);
        Zc2 = (*mpND_Zc2);
        //Port Pmr1
        tormr1 = (*mpND_tormr1);
        thetamr1 = (*mpND_thetamr1);
        wmr1 = (*mpND_wmr1);
        cmr1 = (*mpND_cmr1);
        Zcmr1 = (*mpND_Zcmr1);
        eqInertiamr1 = (*mpND_eqInertiamr1);

        //Read inputVariables from nodes

        //Read outputVariables from nodes
        q2e = (*mpq2e);
        Pin = (*mpPin);
        Pout = (*mpPout);

//==This code has been autogenerated using Compgen==


        //Initialize delays
        delayParts2[1] = (1.*Power(Ap,2)*Kcp*mTimestep*Power(p1,2) - \
2.*Power(Ap,2)*Kcp*mTimestep*p1*p2 + 1.*Power(Ap,2)*Kcp*mTimestep*Power(p2,2) \
- 1.*Power(Ap,2)*mTimestep*p1*q2e + 1.*Power(Ap,2)*mTimestep*p2*q2e - \
0.5*Kcp*kl*mTimestep*p1*Power(q2,2)*rho + \
0.5*Kcp*kl*mTimestep*p2*Power(q2,2)*rho + \
0.5*kl*mTimestep*Power(q2,2)*q2e*rho - 0.001*Power(Ap,2)*mTimestep*tormr1 + \
0.002*Power(Ap,2)*Jp*wmr1 - 0.001*Power(Ap,2)*Bp*mTimestep*wmr1 - \
1.*Power(Ap,2)*mTimestep*tormr1*Abs(wmr1) + 2.*Power(Ap,2)*Jp*wmr1*Abs(wmr1) \
- 1.*Power(Ap,2)*Bp*mTimestep*wmr1*Abs(wmr1))/(-0.002*Power(Ap,2)*Jp - \
0.001*Power(Ap,2)*Bp*mTimestep - 2.*Power(Ap,2)*Jp*Abs(wmr1) - \
1.*Power(Ap,2)*Bp*mTimestep*Abs(wmr1));
        mDelayedPart21.initialize(mNstep,delayParts2[1]);

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];
        delayedPart[5][1] = delayParts5[1];
     }
    void simulateOneTimestep()
     {
        Vec stateVar(5);
        Vec stateVark(5);
        Vec deltaStateVar(5);

        //Read variables from nodes
        //Port P1
        T1 = (*mpND_T1);
        c1 = (*mpND_c1);
        Zc1 = (*mpND_Zc1);
        //Port P2
        T2 = (*mpND_T2);
        c2 = (*mpND_c2);
        Zc2 = (*mpND_Zc2);
        //Port Pmr1
        cmr1 = (*mpND_cmr1);
        Zcmr1 = (*mpND_Zcmr1);

        //Read inputVariables from nodes

        //LocalExpressions

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = q2e;
        stateVark[1] = wmr1;
        stateVark[2] = p1;
        stateVark[3] = p2;
        stateVark[4] = tormr1;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //CentrifugalPumpJ
         //Differential-algebraic system of equation parts

          //Assemble differential-algebraic equations
          systemEquations[0] =Kcp*(-p1 + p2) + q2e - Ap*signedSquareL((2*(p1 \
- p2 + (rho*wmr1*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b))/(kl*rho),10.);
          systemEquations[1] =wmr1 + (mTimestep*(kl*Power(q2,2)*(-0.5*Kcp*p1 \
+ 0.5*Kcp*p2 + 0.5*q2e)*rho + Power(Ap,2)*(1.*Kcp*Power(p1,2) - 2.*Kcp*p1*p2 \
+ 1.*Kcp*Power(p2,2) - 1.*p1*q2e + 1.*p2*q2e - 0.001*tormr1) - \
1.*Power(Ap,2)*tormr1*Abs(wmr1)))/(Power(Ap,2)*(-0.002*Jp - \
0.001*Bp*mTimestep - 2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1))) + \
delayedPart[2][1];
          systemEquations[2] =p1 - lowLimit(c1 - q2*Zc1,0);
          systemEquations[3] =p2 - lowLimit(c2 + q2*Zc2,0);
          systemEquations[4] =-cmr1 + tormr1 - wmr1*Zcmr1;

          //Jacobian matrix
          jacobianMatrix[0][0] = 1 - \
(0.31831*Ap*rho*wmr1*Cot(beta2)*dxSignedSquareL((2*(p1 - p2 + \
(rho*wmr1*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b))/(kl*rho),10.))/(b*kl);
          jacobianMatrix[0][1] = (-2*Ap*(0.25*Power(d,2)*rho*wmr1 + \
(rho*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b)*dxSignedSquareL((2*(p1 - p2 + \
(rho*wmr1*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b))/(kl*rho),10.))/(kl*rho);
          jacobianMatrix[0][2] = -Kcp - (2*Ap*dxSignedSquareL((2*(p1 - p2 + \
(rho*wmr1*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b))/(kl*rho),10.))/(kl*rho);
          jacobianMatrix[0][3] = Kcp + (2*Ap*dxSignedSquareL((2*(p1 - p2 + \
(rho*wmr1*(0.25*b*Power(d,2)*wmr1 + \
0.159155*q2e*rho*Cot(beta2)))/b))/(kl*rho),10.))/(kl*rho);
          jacobianMatrix[0][4] = 0;
          jacobianMatrix[1][0] = (mTimestep*(Power(Ap,2)*(0. - 1.*p1 + 1.*p2) \
+ 0.5*kl*Power(q2,2)*rho))/(Power(Ap,2)*(-0.002*Jp - 0.001*Bp*mTimestep - \
2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1)));
          jacobianMatrix[1][1] = 1 - (mTimestep*(kl*Power(q2,2)*(-0.5*Kcp*p1 \
+ 0.5*Kcp*p2 + 0.5*q2e)*rho + Power(Ap,2)*(1.*Kcp*Power(p1,2) - 2.*Kcp*p1*p2 \
+ 1.*Kcp*Power(p2,2) - 1.*p1*q2e + 1.*p2*q2e - 0.001*tormr1) - \
1.*Power(Ap,2)*tormr1*Abs(wmr1))*(0. - 2.*Jp*dxAbs(wmr1) - \
1.*Bp*mTimestep*dxAbs(wmr1)))/(Power(Ap,2)*Power(-0.002*Jp - \
0.001*Bp*mTimestep - 2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1),2)) + \
(mTimestep*(0. - 1.*Power(Ap,2)*tormr1*dxAbs(wmr1)))/(Power(Ap,2)*(-0.002*Jp \
- 0.001*Bp*mTimestep - 2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1)));
          jacobianMatrix[1][2] = 0. + (mTimestep*(Power(Ap,2)*(0. + 2.*Kcp*p1 \
- 2.*Kcp*p2 - 1.*q2e) + (0. - \
0.5*Kcp)*kl*Power(q2,2)*rho))/(Power(Ap,2)*(-0.002*Jp - 0.001*Bp*mTimestep - \
2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1)));
          jacobianMatrix[1][3] = (mTimestep*(Power(Ap,2)*(0. - 2.*Kcp*p1 + \
2.*Kcp*p2 + 1.*q2e) + 0.5*Kcp*kl*Power(q2,2)*rho))/(Power(Ap,2)*(-0.002*Jp - \
0.001*Bp*mTimestep - 2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1)));
          jacobianMatrix[1][4] = 0. + (mTimestep*(0. - 0.001*Power(Ap,2) - \
1.*Power(Ap,2)*Abs(wmr1)))/(Power(Ap,2)*(-0.002*Jp - 0.001*Bp*mTimestep - \
2.*Jp*Abs(wmr1) - 1.*Bp*mTimestep*Abs(wmr1)));
          jacobianMatrix[2][0] = 0;
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;
          jacobianMatrix[2][3] = 0;
          jacobianMatrix[2][4] = 0;
          jacobianMatrix[3][0] = 0;
          jacobianMatrix[3][1] = 0;
          jacobianMatrix[3][2] = 0;
          jacobianMatrix[3][3] = 1;
          jacobianMatrix[3][4] = 0;
          jacobianMatrix[4][0] = 0;
          jacobianMatrix[4][1] = -Zcmr1;
          jacobianMatrix[4][2] = 0;
          jacobianMatrix[4][3] = 0;
          jacobianMatrix[4][4] = 1;
//==This code has been autogenerated using Compgen==

          //Solving equation using LU-faktorisation
          mpSolver->solve(jacobianMatrix, systemEquations, stateVark, iter);
          q2e=stateVark[0];
          wmr1=stateVark[1];
          p1=stateVark[2];
          p2=stateVark[3];
          tormr1=stateVark[4];
          //Expressions
          q2 = q2e*(onNegative(q2e) + onPositive(p1));
          q1 = -q2;
          Pin = -(tormr1*wmr1);
          Pout = (-p1 + p2)*q2;
        }

        //Calculate the delayed parts
        delayParts2[1] = (1.*Power(Ap,2)*Kcp*mTimestep*Power(p1,2) - \
2.*Power(Ap,2)*Kcp*mTimestep*p1*p2 + 1.*Power(Ap,2)*Kcp*mTimestep*Power(p2,2) \
- 1.*Power(Ap,2)*mTimestep*p1*q2e + 1.*Power(Ap,2)*mTimestep*p2*q2e - \
0.5*Kcp*kl*mTimestep*p1*Power(q2,2)*rho + \
0.5*Kcp*kl*mTimestep*p2*Power(q2,2)*rho + \
0.5*kl*mTimestep*Power(q2,2)*q2e*rho - 0.001*Power(Ap,2)*mTimestep*tormr1 + \
0.002*Power(Ap,2)*Jp*wmr1 - 0.001*Power(Ap,2)*Bp*mTimestep*wmr1 - \
1.*Power(Ap,2)*mTimestep*tormr1*Abs(wmr1) + 2.*Power(Ap,2)*Jp*wmr1*Abs(wmr1) \
- 1.*Power(Ap,2)*Bp*mTimestep*wmr1*Abs(wmr1))/(-0.002*Power(Ap,2)*Jp - \
0.001*Power(Ap,2)*Bp*mTimestep - 2.*Power(Ap,2)*Jp*Abs(wmr1) - \
1.*Power(Ap,2)*Bp*mTimestep*Abs(wmr1));

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];
        delayedPart[5][1] = delayParts5[1];

        //Write new values to nodes
        //Port P1
        (*mpND_p1)=p1;
        (*mpND_q1)=q1;
        (*mpND_dE1)=dE1;
        //Port P2
        (*mpND_p2)=p2;
        (*mpND_q2)=q2;
        (*mpND_dE2)=dE2;
        //Port Pmr1
        (*mpND_tormr1)=tormr1;
        (*mpND_thetamr1)=thetamr1;
        (*mpND_wmr1)=wmr1;
        (*mpND_eqInertiamr1)=eqInertiamr1;
        //outputVariables
        (*mpq2e)=q2e;
        (*mpPin)=Pin;
        (*mpPout)=Pout;

        //Update the delayed variabels
        mDelayedPart21.update(delayParts2[1]);

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // HYDRAULICCENTRIFUGALPUMPJ_HPP_INCLUDED
