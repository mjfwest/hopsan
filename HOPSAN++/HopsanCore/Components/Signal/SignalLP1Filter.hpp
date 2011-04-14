//!
//! @file   SignalLP1Filter.hpp
//! @author Karl Pettersson <karl.pettersson@liu.se>
//! @date   2010-06-10
//!
//! @brief Contains a Signal First Order Low Pass Filter Component using CoreUtilities
//!
//$Id$

#ifndef SIGNALLP1FILTER_HPP_INCLUDED
#define SIGNALLP1FILTER_HPP_INCLUDED

#include "../../ComponentEssentials.h"
#include "../../ComponentUtilities.h"

namespace hopsan {

    //!
    //! @brief
    //! @ingroup SignalComponents
    //!
    class SignalLP1Filter : public ComponentSignal
    {

    private:
        FirstOrderFilter mFilter;
        double mW, mMin, mMax;
        double *mpND_in, *mpND_out;
        Port *mpIn, *mpOut;

    public:
        static Component *Creator()
        {
            return new SignalLP1Filter("Filter");
        }

        SignalLP1Filter(const std::string name) : ComponentSignal(name)
        {

            mMin = -1.5E+300;
            mMax = 1.5E+300;

            mW=1000.0;

            mpIn = addReadPort("in", "NodeSignal", Port::NOTREQUIRED);
            mpOut = addWritePort("out", "NodeSignal", Port::NOTREQUIRED);

            registerParameter("omega", "Break frequency", "[rad/s]", mW);
        }


        void initialize()
        {
            mpND_in = getSafeNodeDataPtr(mpIn, NodeSignal::VALUE, 0);
            mpND_out = getSafeNodeDataPtr(mpOut, NodeSignal::VALUE);

            double num[2];
            double den[2];

            num[0] = 0.0;
            num[1] = 1.0;
            den[0] = 1.0/mW;
            den[1] = 1.0;

            mFilter.initialize(mTimestep, num, den, (*mpND_in), (*mpND_in), mMin, mMax);

            //Writes out the value for time "zero"
            (*mpND_out) = (*mpND_in);
        }


        void simulateOneTimestep()
        {
            //Write new values to nodes
            (*mpND_out) = mFilter.update((*mpND_in));
        }
    };
}

#endif // SIGNALLP1FILTER_HPP_INCLUDED


