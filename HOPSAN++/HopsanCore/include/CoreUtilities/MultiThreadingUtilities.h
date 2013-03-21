#ifndef MULTITHREADINGUTILITIES_H
#define MULTITHREADINGUTILITIES_H

#include <sstream>
#include <cassert>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#ifdef USETBB
#include "mutex.h"
#include "atomic.h"
#include "tick_count.h"
#include "task_group.h"
#endif

#include "Component.h"
#include "ComponentSystem.h"

using namespace std;
using namespace hopsan;

size_t determineActualNumberOfThreads(const size_t nDesiredThreads);


template <class T>
class TaskPool
{
public:
    TaskPool(std::vector<T*> data, int nThreads)
    {
        mData = data;
        mStartIdx=mData.size()-1;
        std::vector<T*> tempData;
        for(size_t i=0; i<mData.size(); ++i)
        {
            tempData.push_back(mData[i]);
            tempData.push_back(0);
            tempData.push_back(0);
            tempData.push_back(0);
        }
        mData = tempData;
        mnThreads=nThreads;
        reset();
//#ifdef USETBB
//        mpGetMutex = new tbb::mutex();
//#else
//        mpGetMutex = 0;
//#endif
    }

    inline void reset()
    {
        mIdx = mStartIdx;
    }


    inline bool isFinished()
    {
        return mIdx <= -mnThreads;
    }


    inline int getAndDecrementIndex()
    {
#ifdef USETBB
        return mIdx.fetch_and_store(mIdx-1);
#else
        return mIdx--;
#endif
    }


    inline T* getDataPtr(int idx)
    {
        return mData[idx*4];
    }

private:
    int mStartIdx;
    std::vector<T*> mData;
    //tbb::mutex *mpGetMutex;
#ifdef USETBB
    tbb::atomic<int> mIdx;
#else
    int mIdx;
#endif
    int mnThreads;
};



//! @brief Class for barrier locks in multi-threaded simulations.
class BarrierLock
{
public:
    //! @brief Constructor.
    //! @note Number of threads must be correct! Wrong value will result in either deadlocks or threads or non-synchronized threads.
    //! @param nThreads Number of threads to by synchronized.
    BarrierLock(size_t nThreads)
    {
        mnThreads=nThreads;
        mCounter = 0;
        mLock = true;
    }

    //! @brief Locks the barrier.
    inline void lock() { mCounter=0; mLock=true; }

    //! @brief Unlocks the barrier.
    inline void unlock() { mLock=false; }

    //! @brief Returns whether or not the barrier is locked.
    inline bool isLocked() { return mLock; }

    //! @brief Increments barrier counter by one.
    inline void increment() { ++mCounter; }

    //! @brief Returns whether or not all threads have incremented the barrier.
    inline bool allArrived() { return (mCounter == (mnThreads-1)); }      //One less due to master thread

private:
    int mnThreads;
#ifdef USETBB
    tbb::atomic<int> mCounter;
    tbb::atomic<bool> mLock;
#else
    int mCounter;
    bool mLock;
#endif
};




//! @brief Class for slave simulation threads, which must be syncronized from a master simulation thread
class taskSimOneComponent
{
public:
    taskSimOneComponent(Component* pComponent, double time)
    {
        mpComponent = pComponent;
        mTime = time;
    }

    void operator() ()
    {
        mpComponent->simulate(mTime);
    }
private:
    Component *mpComponent;
    double mTime;
};




//! @brief Class for slave simulation threads, which must be syncronized from a master simulation thread
class taskSimPool
{
public:
    taskSimPool(TaskPool<Component> *sPool, TaskPool<Component> *qPool, TaskPool<Component> *cPool, TaskPool<Node> *nPool,
                double startTime, double timeStep, size_t numSimSteps, int threadId, ComponentSystem* pSystem)
    {
        mTime = startTime;
        mNumSimSteps = numSimSteps;
        mTimeStep = timeStep;
        msPool = sPool;
        mqPool = qPool;
        mcPool = cPool;
        mnPool = nPool;
        mThreadId = threadId;
        mpSystem = pSystem;
    }

    void operator() ()
    {
        for(size_t i=0; i<mNumSimSteps; ++i)
        {
            int idx;

            mTime += mTimeStep;

            //! Signal Components !//

            idx = msPool->getAndDecrementIndex();
            while(idx>=0)
            {
                Component *pComponent = msPool->getDataPtr(idx);
                pComponent->simulate(mTime);
                idx = msPool->getAndDecrementIndex();
            }
            while(!msPool->isFinished()) {}
            if(mThreadId == 0) { mnPool->reset(); }

            //! C Components !//

            idx = mcPool->getAndDecrementIndex();
            while(idx>=0)
            {
                Component *pComponent = mcPool->getDataPtr(idx);
                pComponent->simulate(mTime);
                idx = mcPool->getAndDecrementIndex();
            }
            while(!mcPool->isFinished()) {}
            if(mThreadId == 0) { mqPool->reset(); }

            idx = mnPool->getAndDecrementIndex();
            while(idx>=0)
            {
                idx = mnPool->getAndDecrementIndex();
            }
            while(!mnPool->isFinished()) {}
            if(mThreadId == 0) { msPool->reset(); }

            //! Q Components !//

            idx = mqPool->getAndDecrementIndex();
            while(idx>=0)
            {
                Component *pComponent = mqPool->getDataPtr(idx);
                pComponent->simulate(mTime);
                idx = mqPool->getAndDecrementIndex();
            }

            //! Log Nodes !//

            if(mThreadId == 0)      //Hack because of Peters log data changes, must be fixed
            {
                mpSystem->logTimeAndNodes(i);
            }

            while(!mqPool->isFinished()) {}
            if(mThreadId == 0) { mcPool->reset(); }

        }
    }
private:
    size_t mNumSimSteps;
    double mTimeStep;
    double mTime;
    TaskPool<Component> *msPool;
    TaskPool<Component> *mqPool;
    TaskPool<Component> *mcPool;
    TaskPool<Node> *mnPool;
    int mThreadId;
    ComponentSystem *mpSystem;
};




//! @brief Class for slave simulation threads, which must be syncronized from a master simulation thread
class taskSimSlave
{
public:
    //! @brief Constructor for simulation thread class.
    //! @param sVector Vector with signal components executed from this thread
    //! @param cVector Vector with C-type components executed from this thread
    //! @param qVector Vector with Q-type components executed from this thread
    //! @param nVector Vector with nodes which is logged from this thread
    //! @param startTime Start time of simulation
    //! @param timeStep Step time of simulation
    //! @param stopTime Stop Time of simulation
    //! @param nThreads Number of threads used in simulation
    //! @param threadID Number of this thread
    //! @param *pBarrier_S Pointer to barrier before signal components
    //! @param *pBarrier_C Pointer to barrier before C-type components
    //! @param *pBarrier_Q Pointer to barrier before Q-type components
    //! @param *pBarrier_N Pointer to barrier before node logging
    taskSimSlave(ComponentSystem *pSystem, vector<Component*> sVector, vector<Component*> cVector, vector<Component*> qVector, vector<Node*> nVector,
                 double startTime, double timeStep, size_t numSimSteps, size_t nThreads, size_t threadID,
                 BarrierLock *pBarrier_S, BarrierLock *pBarrier_C, BarrierLock *pBarrier_Q, BarrierLock *pBarrier_N)
    {
        mpSystem = pSystem;
        mVectorS = sVector;
        mVectorC = cVector;
        mVectorQ = qVector;
        mVectorN = nVector;
        mTime = startTime;
        mNumSimSteps = numSimSteps;
        mTimeStep = timeStep;
        mnThreads = nThreads;
        mThreadID = threadID;
        mpBarrier_S = pBarrier_S;
        mpBarrier_C = pBarrier_C;
        mpBarrier_Q = pBarrier_Q;
        mpBarrier_N = pBarrier_N;
    }

    //! @brief Executable code for slave simulation thread
    void operator() ()
    {
        for(size_t i=0; i<mNumSimSteps; ++i)
        {
            if(mpSystem->wasSimulationAborted()) break;

            mTime += mTimeStep;

            //! Signal Components !//

            mpBarrier_S->increment();
            while(mpBarrier_S->isLocked()){}                         //Wait at S barrier

            for(size_t i=0; i<mVectorS.size(); ++i)
            {
                mVectorS[i]->simulate(mTime);
            }


            //! C Components !//

            mpBarrier_C->increment();
            while(mpBarrier_C->isLocked()){}                         //Wait at C barrier

            for(size_t i=0; i<mVectorC.size(); ++i)
            {
                mVectorC[i]->simulate(mTime);
            }


            //! Q Components !//

            mpBarrier_Q->increment();
            while(mpBarrier_Q->isLocked()){}                         //Wait at Q barrier

            for(size_t i=0; i<mVectorQ.size(); ++i)
            {
                mVectorQ[i]->simulate(mTime);
            }

            //! Log Nodes !//

            mpBarrier_N->increment();
            while(mpBarrier_N->isLocked()){}                         //Wait at N barrier

            //! @todo Temporary hack by Peter, after rewriting how node data and time is logged this no longer works, now master thread loags all nodes, need to come up with somthing smart
//            for(size_t i=0; i<mVectorN.size(); ++i)
//            {
//                mVectorN[i]->logData(mTime);
//            }

        }
    }
private:
    ComponentSystem *mpSystem;
    vector<Component*> mVectorS;
    vector<Component*> mVectorC;
    vector<Component*> mVectorQ;
    vector<Node*> mVectorN;
    size_t mNumSimSteps;
    double mTimeStep;
    double mTime;
    double *mpSimTime;
    size_t mnThreads;
    size_t mThreadID;
    BarrierLock *mpBarrier_S;
    BarrierLock *mpBarrier_C;
    BarrierLock *mpBarrier_Q;
    BarrierLock *mpBarrier_N;
};


//! @brief Class for master simulation thread, that is responsible for syncronizing the simulation
class taskSimMaster
{
public:

    //! @brief Constructor for master simulation thead class.
    //! @param sVector Vector with signal components executed from this thread
    //! @param cVector Vector with C-type components executed from this thread
    //! @param qVector Vector with Q-type components executed from this thread
    //! @param nVector Vector with nodes which is logged from this thread
    //! @param *pSimtime Pointer to the simulation time in the component system
    //! @param startTime Start time of simulation
    //! @param timeStep Step time of simulation
    //! @param stopTime Stop Time of simulation
    //! @param nThreads Number of threads used in simulation
    //! @param threadID Number of this thread
    //! @param *pBarrier_S Pointer to barrier before signal components
    //! @param *pBarrier_C Pointer to barrier before C-type components
    //! @param *pBarrier_Q Pointer to barrier before Q-type components
    //! @param *pBarrier_N Pointer to barrier before node logging
    taskSimMaster(ComponentSystem *pSystem, vector<Component*> sVector, vector<Component*> cVector, vector<Component*> qVector, vector<Node*> nVector, vector<double *> pSimTimes,
                  double startTime, double timeStep, size_t numSimSteps, size_t nThreads, size_t threadID,
                  BarrierLock *pBarrier_S, BarrierLock *pBarrier_C, BarrierLock *pBarrier_Q, BarrierLock *pBarrier_N)
    {
        mpSystem = pSystem;
        mVectorS = sVector;
        mVectorC = cVector;
        mVectorQ = qVector;
        mVectorN = nVector;
        mpSimTimes = pSimTimes;
        mTime = startTime;
        mNumSimSteps = numSimSteps;
        mTimeStep = timeStep;
        mnThreads = nThreads;
        mThreadID = threadID;
        mpBarrier_S = pBarrier_S;
        mpBarrier_C = pBarrier_C;
        mpBarrier_Q = pBarrier_Q;
        mpBarrier_N = pBarrier_N;
    }

    //! @brief Executable code for master simulation thread
    void operator() ()
    {
        for(size_t s=0; s<mNumSimSteps; ++s)
        {
            if(mpSystem->wasSimulationAborted()) break;

            mTime += mTimeStep;

            //! Signal Components !//

            while(!mpBarrier_S->allArrived()) {}    //Wait for all other threads to arrive at signal barrier
            mpBarrier_C->lock();                    //Lock next barrier (must be done before unlocking this one, to prevnet deadlocks)
            mpBarrier_S->unlock();                  //Unlock signal barrier

            for(size_t i=0; i<mVectorS.size(); ++i)
            {
                mVectorS[i]->simulate(mTime);
            }

            //! C Components !//

            while(!mpBarrier_C->allArrived()) {}    //C barrier
            mpBarrier_Q->lock();
            mpBarrier_C->unlock();

            for(size_t i=0; i<mVectorC.size(); ++i)
            {
                mVectorC[i]->simulate(mTime);
            }

            //! Q Components !//

            while(!mpBarrier_Q->allArrived()) {}    //Q barrier
            mpBarrier_N->lock();
            mpBarrier_Q->unlock();

            for(size_t i=0; i<mVectorQ.size(); ++i)
            {
                mVectorQ[i]->simulate(mTime);
            }

            for(size_t i=0; i<mpSimTimes.size(); ++i)
                *mpSimTimes[i] = mTime;     //Update time in component system, so that progress bar can use it

            //! Log Nodes !//

            while(!mpBarrier_N->allArrived()) {}    //N barrier
            mpBarrier_S->lock();
            mpBarrier_N->unlock();

            //! @todo Temporary hack by Peter, after rewriting how node data and time is logged this no longer works, now master thread loags all nodes, need to come up with somthing smart
//            for(size_t i=0; i<mVectorN.size(); ++i)
//            {
//                mVectorN[i]->logData(mTime);
//            }
            mpSystem->logTimeAndNodes(s+1); //s+1 since at s=0 one simulation has been performed /Björn

        }
    }
private:
    ComponentSystem *mpSystem;
    vector<Component*> mVectorS;
    vector<Component*> mVectorC;
    vector<Component*> mVectorQ;
    vector<Node*> mVectorN;
    size_t mNumSimSteps;
    double mTimeStep;
    double mTime;
    vector<double *> mpSimTimes;
    size_t mnSystems;
    size_t mnThreads;
    size_t mThreadID;
    BarrierLock *mpBarrier_S;
    BarrierLock *mpBarrier_C;
    BarrierLock *mpBarrier_Q;
    BarrierLock *mpBarrier_N;
};



//! @brief Class for simulating whole systems multi threaded
class taskSimWholeSystems
{
public:

    //! @brief Constructor for master simulation thead class.
    //! @param pSystem Pointer to the system to simulate
    taskSimWholeSystems(vector<ComponentSystem *> systemPtrs, double stopTime)
    {
        mSystemPtrs = systemPtrs;
        mStopTime = stopTime;
    }

    //! @brief Executable code for master simulation thread
    void operator() ()
    {
        for(size_t i=0; i<mSystemPtrs.size(); ++i)
        {
            mSystemPtrs[i]->simulate(mStopTime);
        }
    }
private:
    vector<ComponentSystem *> mSystemPtrs;
    double mStopTime;
};


#endif // MULTITHREADINGUTILITIES_H
