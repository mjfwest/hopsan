#ifndef SIGNALWAYPOINT_HPP_INCLUDED
#define SIGNALWAYPOINT_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file SignalWaypoint.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Wed 3 Jul 2013 09:57:22
//! @brief Waypoint Controller with way points
//! @ingroup SignalComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, HOPSAN++, CompgenModels}/SignalControlAero.nb*/

using namespace hopsan;

class SignalWaypoint : public ComponentSignal
{
private:
     double R;
     double alt1;
     double v1;
     double wptol;
     double xcWp1;
     double ycWp1;
     int mNstep;
//==This code has been autogenerated using Compgen==
     //inputVariables
     double s1;
     double xc;
     double yc;
     //outputVariables
     double set1;
     double altitudeRef;
     double headingRef;
     double velocityRef;
     double distanceWp;
     //Expressions variables
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     double *mps1;
     double *mpxc;
     double *mpyc;
     //inputParameters pointers
     double *mpR;
     double *mpalt1;
     double *mpv1;
     double *mpwptol;
     double *mpxcWp1;
     double *mpycWp1;
     //outputVariables pointers
     double *mpset1;
     double *mpaltitudeRef;
     double *mpheadingRef;
     double *mpvelocityRef;
     double *mpdistanceWp;
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new SignalWaypoint();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;

        //Add ports to the component
        //Add inputVariables to the component
            addInputVariable("s1","state 1","",0.,&mps1);
            addInputVariable("xc","x-position","m",0.,&mpxc);
            addInputVariable("yc","y-position","m",0.,&mpyc);

        //Add inputParammeters to the component
            addInputVariable("R", "Earth radius", "m", 6.3675e6,&mpR);
            addInputVariable("alt1", "altitude 1", "m", 0,&mpalt1);
            addInputVariable("v1", "ref. velocity 1", "m/s", 200,&mpv1);
            addInputVariable("wptol", "waypoint tolerance", "m", \
500,&mpwptol);
            addInputVariable("xcWp1", "waypoint longitude", "deg", \
15.1669,&mpxcWp1);
            addInputVariable("ycWp1", "waypoint lattitude", "deg", \
58.3812,&mpycWp1);
        //Add outputVariables to the component
            addOutputVariable("set1","set state","",0.,&mpset1);
            addOutputVariable("altitudeRef","reference \
altitude","m",0.,&mpaltitudeRef);
            addOutputVariable("headingRef","reference \
heading","rad",0.,&mpheadingRef);
            addOutputVariable("velocityRef","reference \
velocity","m",0.,&mpvelocityRef);
            addOutputVariable("distanceWp","distance to \
wp","m",0.,&mpdistanceWp);

//==This code has been autogenerated using Compgen==
        //Add constantParameters
     }

    void initialize()
     {
        //Read port variable pointers from nodes

        //Read variables from nodes

        //Read inputVariables from nodes
        s1 = (*mps1);
        xc = (*mpxc);
        yc = (*mpyc);

        //Read inputParameters from nodes
        R = (*mpR);
        alt1 = (*mpalt1);
        v1 = (*mpv1);
        wptol = (*mpwptol);
        xcWp1 = (*mpxcWp1);
        ycWp1 = (*mpycWp1);

        //Read outputVariables from nodes
        set1 = (*mpset1);
        altitudeRef = (*mpaltitudeRef);
        headingRef = (*mpheadingRef);
        velocityRef = (*mpvelocityRef);
        distanceWp = (*mpdistanceWp);

//==This code has been autogenerated using Compgen==


        //Initialize delays

     }
    void simulateOneTimestep()
     {
        //Read variables from nodes

        //Read inputVariables from nodes
        s1 = (*mps1);
        xc = (*mpxc);
        yc = (*mpyc);

        //LocalExpressions

          //Expressions
          set1 = s1*onPositive(wptol - 0.0174533*Sqrt(Power(R,2)*Power(yc - \
ycWp1,2) + Power(R,2)*Power(xc - xcWp1,2)*Power(Cos(0.0174533*yc),2)));
          altitudeRef = alt1*s1;
          headingRef = s1*Atan2L(R*(-xc + xcWp1)*Cos(0.0174533*yc),R*(-yc + \
ycWp1));
          velocityRef = s1*v1;
          distanceWp = 0.0174533*Sqrt(Power(R,2)*Power(yc - ycWp1,2) + \
Power(R,2)*Power(xc - xcWp1,2)*Power(Cos(0.0174533*yc),2));

        //Calculate the delayed parts


        //Write new values to nodes
        //outputVariables
        (*mpset1)=set1;
        (*mpaltitudeRef)=altitudeRef;
        (*mpheadingRef)=headingRef;
        (*mpvelocityRef)=velocityRef;
        (*mpdistanceWp)=distanceWp;

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // SIGNALWAYPOINT_HPP_INCLUDED