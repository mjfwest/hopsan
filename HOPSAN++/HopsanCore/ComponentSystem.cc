/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   ComponentSystem.cc
//! @author FluMeS
//! @date   2009-12-20
//!
//! @brief Contains Component base classes as well as Component Parameter class
//!
//$Id$

#include <sstream>
#include <cassert>
#include <limits>
#include <cmath>

#include "ComponentSystem.h"
#include "CoreUtilities/HopsanCoreMessageHandler.h"
#include "CoreUtilities/FindUniqueName.h"
#include "HopsanEssentials.h"

#ifdef USETBB
#include "mutex.h"
#include "atomic.h"
#include "tick_count.h"
#include "task_group.h"
#endif

using namespace std;
using namespace hopsan;



//Constructor
ComponentSystem::ComponentSystem(string name) : Component(name)
{
    mTypeName = "ComponentSystem";
    mIsComponentSystem = true;
    mDesiredTimestep = 0.001;
    #ifdef USETBB
    mpStopMutex = new tbb::mutex();
    #endif
}

double ComponentSystem::getDesiredTimeStep()
{
    return mDesiredTimestep;
}


//! Sets a bool which is looked at in initialization and simulation loops.
//! This method can be used by users e.g. GUIs to stop an started initializatiion/simulation process
void ComponentSystem::stop()
{
    mStop = true;
}


Parameters &ComponentSystem::getSystemParameters()
{
    return *mParameters;
}


//<<<<<<< .mine
//=======

////! @brief Adds a new System parameter to the system or change the vaÃ¶lue of an existing one
////!
////! A parameter is added but has no ponters to values.
////! Or if there exist a parameter with this name the value is changed and the ponters are unchanged.
////! The parameter is then mapped to one or more doubles with the mapParameter method.
////! @param[in] sysParName is the name of the new System parameter.
////! @param[in] value is the value of the System parameter.
////! @see mapParameter(std::string sysParName, double *mappedValue)
////! @return true if a it went OK, false otherwise
//bool SystemParameters::add(std::string sysParName, double value)
//{
//    //This is a QD for handling negative values...
//    if(sysParName.at(0) == '-')
//        return false;
//    //This is a QD for handling negative values...

//    if(mSystemParameters.count(sysParName) > 0)
//    //sysParName is already present, change its value
//    {
//        mSystemParameters[sysParName].first = value;
//        update(sysParName);

//        //This is a QD for handling negative values...
//        std::ostringstream oss;
//        oss << "-" << sysParName;
//        std::string negSysParName = oss.str();
//        mSystemParameters[negSysParName].first = -value;
//        update(negSysParName);
//        //This is a QD for handling negative values...
//    }
//    else
//    //sysParName is not present, create a new one
//    {
//        SystemParameter sysPar;
//        sysPar.first = value;
//        mSystemParameters[sysParName] = sysPar;

//        //This is a QD for handling negative values...
//        std::ostringstream oss;
//        oss << "-" << sysParName;
//        std::string negSysParName = oss.str();
//        SystemParameter negSysPar;
//        negSysPar.first = -value;
//        mSystemParameters[negSysParName] = negSysPar;
//        //This is a QD for handling negative values...
//    }
//    return true;
//}

////! @brief Read the value of System parameter
////!
////! @param[in] sysParName is the name of the new System parameter.
////! @param[out] value is the value of the System parameter.
////! @return true if a post with sysParName exsited, false otherwise
//bool SystemParameters::getValue(std::string sysParName, double &value)
//{
////std::map<std::string, SystemParameter>::iterator it;
////for(it = mSystemParameters.begin(); it != mSystemParameters.end(); ++it)
////{
////    std::cout << it->first << mSystemParameters.count(sysParName) << std::endl;
////}
//    if(mSystemParameters.count(sysParName))
//    {
//        value = mSystemParameters[sysParName].first;
//        return true;
//    }
//    else
//        return false;
//}

////! @brief Get a map with System parameter names as keys and their values as values
////!
////! @return a map with keys: System parameter names, values: System parameter values
//std::map<std::string, double> SystemParameters::getSystemParameterMap()
//{
//    std::map<std::string, double> sysPar;
//    std::map<std::string, SystemParameter>::iterator map_it;
//    for(map_it = mSystemParameters.begin(); map_it != mSystemParameters.end(); ++map_it)
//    {
//        //Create a new map with only the name and value (no pointers)
//        //This is a QD for handling negative values...
//        //Do not count if it start with '-' because then there is this special hack!
//        if(map_it->first.at(0) != '-')
//        //This is a QD for handling negative values...
//        {
//            sysPar[map_it->first] = map_it->second.first;
//        }
//    }
//    return sysPar;
//}

////! @brief Finds out if a double has a reference in the System parameters
////!
////! @param[in] mappedValue is a pointer to a double.
////! @return the name of the System parameter which is mapped to the input, an empty std::string if its not.
//std::string SystemParameters::findOccurrence(double *mappedValue)
//{
//    std::string sysParName ="";
//    std::list<double*>::iterator list_it;
//    std::map<std::string, SystemParameter>::iterator map_it;
//    for(map_it = mSystemParameters.begin(); map_it != mSystemParameters.end(); ++map_it)
//    {
//        //Go through all pointers to see if mappedValue are present somewhere
//        for(list_it = map_it->second.second.begin(); list_it != map_it->second.second.end(); ++list_it)
//        {
//            if(*list_it == mappedValue)
//            {
//                sysParName = map_it->first;
//            }
//        }
//    }
//    return sysParName;
//}

////! @brief Delete a System parameter
////!
////! @param[in] sysParName the System parameter to be deleted.
//void SystemParameters::erase(std::string sysParName)
//{
//    mSystemParameters.erase(sysParName);

//    //This is a QD for handling negative values...
//    std::ostringstream oss;
//    oss << "-" << sysParName;
//    std::string negSysParName = oss.str();
//    mSystemParameters.erase(negSysParName);
//    //This is a QD for handling negative values...
//}

////! @brief Maps a double to a System parameter
////!
////! After this method has been ran the SystemParameter object have a pointer
////! stored to the double and can write the System parameter value to it directly
////! without the knoledge of the "double owner".
////! If the sysParName does not exist in the SystemParameters nothing will happen.
////!
////! @param[in] sysParName is the name of the System parameter which should point to the double.
////! @param[in] mappedValue is a pointer to a double.
////! @return true if a it went OK, false otherwise
////! @see unMapParameter(std::string sysParName, double *mappedValue)
//bool SystemParameters::mapParameter(std::string sysParName, double *mappedValue)
//{
//    bool success = false;
//    //If mappedValue is in the map somwhere else it is removed first
//    unMapParameter(mappedValue);

//    std::map<std::string, SystemParameter>::iterator it;
//    it = mSystemParameters.find(sysParName);
//    //If the sysParName exists in the mappedValue is added, if not nothing happens
//    if(it != mSystemParameters.end())
//    {
//        it->second.second.push_back(mappedValue);
//        //the System parameter value is written to the mappedValue
//        *mappedValue = it->second.first;
//        success = true;
//    }
//    return success;
//}

////! @brief Unmaps a double from a System parameter
////!
////! This method removes the pointer to the mappedValue from the SystemParameters.
////! After this is ran the double is free from the SystemParameters
////!
////! @param[in] sysParName is the name of the System parameter which should point to the double.
////! @param[in] mappedValue is a pointer to a double.
////! @see MapParameter(std::string sysParName, double *mappedValue)
//void SystemParameters::unMapParameter(std::string sysParName, double *mappedValue)
//{
//    std::list<double*>::iterator list_it, remove_it;
//    bool found = false;
//    //Go through all mapped values for the System parameter sysParName
//    for(list_it = mSystemParameters[sysParName].second.begin(); list_it !=mSystemParameters[sysParName].second.end(); ++list_it)
//    {
//        //If it is found it is saved to be removed
//        if(*list_it == mappedValue)
//        {
//            remove_it = list_it;
//            found = true;
//        }
//    }
//    if((mSystemParameters.count(sysParName)) && found)
//    {
//        //remove the occurance of the mappedValue
//        mSystemParameters[sysParName].second.erase(remove_it);
//    }
//}

////! @brief Unmaps a double from a System parameter
////!
////! This method removes the pointer to the mappedValue from the SystemParameters.
////! After this is ran the double is free from the SystemParameters
////!
////! @param[in] mappedValue is a pointer to a double.
////! @see MapParameter(std::string sysParName, double *mappedValue)
//void SystemParameters::unMapParameter(double *mappedValue)
//{
//    std::map<std::string, SystemParameter>::iterator map_it;
//    //Go through all mapped values for all the System parameters
//    for(map_it = mSystemParameters.begin(); map_it != mSystemParameters.end(); ++map_it)
//    {
//        //remove the pointer for the mappedValue in System parameters
//        unMapParameter(map_it->first, mappedValue);
//    }
//}

////! @brief Write the all System parameters values to the doubles that they points to.
//void SystemParameters::update()
//{
//    std::map<std::string, SystemParameter>::iterator map_it;
//    for(map_it = mSystemParameters.begin(); map_it != mSystemParameters.end(); ++map_it)
//    {
//        //Write the System parameter value to all pointer addresses
//        update(map_it->first);
//    }
//}

////! @brief Write the System parameter value to the doubles that sysParName points to.
////!
////! @param[in] sysParName the System parameter to update
////! @return true if a it went OK, false otherwise
//bool SystemParameters::update(std::string sysParName)
//{
//    bool success = false;
//    std::list<double*>::iterator list_it;
//    if(mSystemParameters.count(sysParName) > 0)
//    {
//        for(list_it = mSystemParameters[sysParName].second.begin(); list_it != mSystemParameters[sysParName].second.end(); ++list_it)
//        {
//            //Write the System parameter value to all pointer addresses for sysParName
//            *(*list_it) = mSystemParameters[sysParName].first;
//        }
//        success = true;
//    }
//    return success;
//}


////! @brief Get a reference to the System parameters
////!
////! Use this method to manipulate the System parameters, e.g. getSystemParameters().add("myNewSysPar", 42.0);
////!
////! @returns A reference to the System parameters
//SystemParameters &ComponentSystem::getSystemParameters()
//{
//    return this->mSystemParameters;
//}


//>>>>>>> .r2999
void ComponentSystem::addComponents(vector<Component*> components)
{
    std::vector<Component *>::iterator itx;
    for(itx = components.begin(); itx != components.end(); ++itx)
    {
        addComponent((*itx));
    }
}


void ComponentSystem::addComponent(Component *pComponent)
{
    //First check if the name already exists, in that case change the suffix
    string modname = this->determineUniqueComponentName(pComponent->getName());
    pComponent->setName(modname);

    //Add to the cqs component vectors
    addSubComponentPtrToStorage(pComponent);

    pComponent->setSystemParent(this);
    pComponent->mModelHierarchyDepth = this->mModelHierarchyDepth+1; //Set the ModelHierarchyDepth counter
}


//! Rename a sub component and automatically fix unique names
void ComponentSystem::renameSubComponent(string oldname, string newname)
{
    //cout << "Trying to rename: " << old_name << " to " << new_name << endl;
    //First find the post in the map where the old name resides, copy the data stored there
    SubComponentMapT::iterator it = mSubComponentMap.find(oldname);
    Component* temp_comp_ptr;
    if (it != mSubComponentMap.end())
    {
        //If found erase old record
        temp_comp_ptr = it->second;
        mSubComponentMap.erase(it);

        //insert new (with new name)
        string mod_new_name = this->determineUniqueComponentName(newname);

        //cout << "new name is: " << mod_name << endl;
        mSubComponentMap.insert(pair<string, Component*>(mod_new_name, temp_comp_ptr));

        //Now change the actual component name, without trying to do rename (we are in rename now, would cause infinite loop)
        temp_comp_ptr->setName(mod_new_name, true);
    }
    else
    {
        cout << "Error no component with old_name: " << oldname << " found!" << endl;
        assert(false);
    }
}


//! Remove a dub component from a system, can also be used to actually delete the component
//! @param[in] name The name of the component to remove from the system
//! @param[in] doDelete Set this to true if the component should be deleted after removal
void ComponentSystem::removeSubComponent(string name, bool doDelete)
{
    Component* pComp = getSubComponent(name);
    removeSubComponent(pComp, doDelete);
}


//! Remove a sub component from a system, can also be used to actually delete the component
//! @param[in] c_ptr A pointer to the component to remove
//! @param[in] doDelete Set this to true if the component should be deleted after removal
void ComponentSystem::removeSubComponent(Component* pComponent, bool doDelete)
{
    std::string compName = pComponent->getName();

    //Disconnect all ports before erase from system
    PortPtrMapT::iterator ports_it;
    vector<Port*>::iterator conn_ports_it;
    for (ports_it = pComponent->mPortPtrMap.begin(); ports_it != pComponent->mPortPtrMap.end(); ++ports_it)
    {
        //! @todo what about multiports here
        vector<Port*> connected_ports = ports_it->second->getConnectedPorts(); //Get a copy of the connected ports ptr vector
        //We can not use an iterator directly connected to the vector inside the port as this will be changed by the disconnect calls
        for (conn_ports_it = connected_ports.begin(); conn_ports_it != connected_ports.end(); ++conn_ports_it)
        {
            disconnect(ports_it->second, *conn_ports_it);
        }
    }

    //Remove from storage
    removeSubComponentPtrFromStorage(pComponent);

    //Shall we also delete the component completely
    if (doDelete)
    {
        delete pComponent; //! @todo can I really delete here or do I need to use the factory for external components
    }

    gCoreMessageHandler.addDebugMessage("Removed component: \"" + compName + "\" from system: \"" + this->getName() + "\"", "removedcomponent");
}

string ComponentSystem::reserveUniqueName(string desiredName)
{
    string newname = this->determineUniqueComponentName(desiredName);
    mReservedNames.insert(std::pair<std::string, int>(newname,0)); //The inte 0 is a dummy value that is never used
    return newname;
}

void ComponentSystem::unReserveUniqueName(string name)
{
    cout << "unReserveUniqueName: " << name;
    cout << " count before: " << mReservedNames.count(name);
    mReservedNames.erase(name);
    cout << " count after: " << mReservedNames.count(name) << std::endl;
}

void ComponentSystem::addSubComponentPtrToStorage(Component* pComponent)
{
    switch (pComponent->getTypeCQS())
    {
    case Component::C :
        mComponentCptrs.push_back(pComponent);
        break;
    case Component::Q :
        mComponentQptrs.push_back(pComponent);
        break;
    case Component::S :
        mComponentSignalptrs.push_back(pComponent);
        break;
    case Component::UNDEFINEDCQSTYPE :
        mComponentUndefinedptrs.push_back(pComponent);
        break;
    default :
            gCoreMessageHandler.addErrorMessage("Trying to add module with unspecified CQS type: " + pComponent->getTypeCQSString()  + ", (Not added)");
        return;
    }

    mSubComponentMap.insert(pair<string, Component*>(pComponent->getName(), pComponent));
}

void ComponentSystem::removeSubComponentPtrFromStorage(Component* pComponent)
{
    SubComponentMapT::iterator it = mSubComponentMap.find(pComponent->getName());
    if (it != mSubComponentMap.end())
    {
        vector<Component*>::iterator cit; //Component iterator
        switch (it->second->getTypeCQS())
        {
        case Component::C :
            for (cit = mComponentCptrs.begin(); cit != mComponentCptrs.end(); ++cit)
            {
                if ( *cit == pComponent )
                {
                    mComponentCptrs.erase(cit);
                    break;
                }
            }
            break;
        case Component::Q :
            for (cit = mComponentQptrs.begin(); cit != mComponentQptrs.end(); ++cit)
            {
                if ( *cit == pComponent )
                {
                    mComponentQptrs.erase(cit);
                    break;
                }
            }
            break;
        case Component::S :
            for (cit = mComponentSignalptrs.begin(); cit != mComponentSignalptrs.end(); ++cit)
            {
                if ( *cit == pComponent )
                {
                    mComponentSignalptrs.erase(cit);
                    break;
                }
            }
            break;
        case Component::UNDEFINEDCQSTYPE :
            for (cit = mComponentUndefinedptrs.begin(); cit != mComponentUndefinedptrs.end(); ++cit)
            {
                if ( *cit == pComponent )
                {
                    mComponentUndefinedptrs.erase(cit);
                    break;
                }
            }
            break;
        default :
            cout << "This should not happen neither C Q or S type" << endl;
            assert(false);
        }

        mSubComponentMap.erase(it);
    }
    else
    {
        gCoreMessageHandler.addErrorMessage("The component you are trying to remove: " + pComponent->getName() + " does not exist (Does Nothing)");
    }
}


//! @brief Sorts the signal component vector
//! Components are sorted so that they are always simulated after the components they receive signals from. Algebraic loops can be detected, in that case this function does nothing.
void ComponentSystem::sortSignalComponentVector(std::vector<Component*> &rOldSignalVector)
{
    std::vector<Component*> newSignalVector;

    bool didSomething = true;
    while(didSomething)
    {
        didSomething = false;
        std::vector<Component*>::iterator it;
        for(it=rOldSignalVector.begin(); it!=rOldSignalVector.end(); ++it)  //Loop through the unsorted signal component vector
        {
            if(!componentVectorContains(newSignalVector, (*it)))    //Ignore components that are already added to the new vector
            {
                bool readyToAdd=true;
                std::vector<Port*>::iterator itp;
                std::vector<Port*> portVector = (*it)->getPortPtrVector();
                for(itp=portVector.begin(); itp!=portVector.end(); ++itp) //Ask each port for its node, then ask the node for its write port component
                {
                    if(((*itp)->getPortType() == READPORT) &&
                       ((*itp)->isConnected()) &&
                       ((!componentVectorContains(newSignalVector, (*itp)->getNodePtr()->getWritePortComponentPtr())) && (*itp)->getNodePtr()->getWritePortComponentPtr() != 0 &&(*itp)->getNodePtr()->getWritePortComponentPtr()->getTypeCQS() == Component::S) &&
                       ((*itp)->getNodePtr()->getWritePortComponentPtr() != 0) &&
                       ((*itp)->getNodePtr()->getWritePortComponentPtr()->mpSystemParent == this))
                    {
                        readyToAdd=false;   //Flag false if required component is not yet added to signal vector, in case node has a write port
                    }
                }
                if(readyToAdd)  //Add the component if all required write port components was already added
                {
                    newSignalVector.push_back((*it));
                    didSomething = true;
                }
            }
        }
    }

    if(newSignalVector.size() == rOldSignalVector.size())   //All components moved to new vector = success!
    {
        rOldSignalVector = newSignalVector;
        stringstream ss;
        std::vector<Component*>::iterator it;
//        for(it=newSignalVector.begin(); it!=newSignalVector.end(); ++it)
//            ss << (*it)->getName() << "\n";                                                                                               //DEBUG
//        gCoreMessageHandler.addDebugMessage("Sorted signal components:\n" + ss.str());
    }
    else    //Something went wrong, all components were not moved. This is likely due to an algebraic loop.
    {
        gCoreMessageHandler.addWarningMessage("Found algebraic loop in signal components. Sorting not possible.");
    }
}


//! @brief Figures out whether or not a component vector contains a certain component
bool ComponentSystem::componentVectorContains(std::vector<Component*> vector, Component *pComp)
{
    std::vector<Component*>::iterator it;
    for(it=vector.begin(); it!=vector.end(); ++it)
    {
        if((*it) == pComp)
        {
            return true;
        }
    }
    return false;
}


//! @brief Overloaded function that behaves slightly different when determining unique port names
//! In systemcomponents we must make sure that systemports and subcomponents have unique names, this simplifies things in the GUI later on
//! It is VERY important that systemports dont have the same name as a subcomponent
std::string ComponentSystem::determineUniquePortName(std::string portname)
{
    return findUniqueName<PortPtrMapT, SubComponentMapT, ReservedNamesT>(mPortPtrMap,  mSubComponentMap, mReservedNames, portname);
}

//! @brief Overloaded function that behaves slightly different when determining unique component names
//! In systemcomponents we must make sure that systemports and subcomponents have unique names, this simplifies things in the GUI later on
//! It is VERY important that systemports dont have the same name as a subcomponent
//! @todo the determineUniquePortNAme and ComponentName looks VERY similar maybe we could use the same function for both
std::string ComponentSystem::determineUniqueComponentName(std::string name)
{
    return findUniqueName<SubComponentMapT, PortPtrMapT, ReservedNamesT>(mSubComponentMap, mPortPtrMap, mReservedNames, name);
}


//! @brief Get a Component ptr to the component with supplied name, can also return a ptr to self if no subcomponent found but systemport with name found
//! For this to work we need to make sure that the sub components and systemports have unique names
Component* ComponentSystem::getComponent(string name)
{
//    cout << "getComponent: " << name << " in: " << mName << endl;
    //First try to find among subcomponents
    Component *tmp = getSubComponent(name);
    if (tmp == 0)
    {
        //Now try to find among systemports
        Port* pPort = this->getPort(name);
        if (pPort != 0)
        {
            if (pPort->getPortType() == SYSTEMPORT)
            {
                //Return the systemports owner (the system component)
                tmp = pPort->getComponent();
                //cout << "Found systemport with name: " << name << " returning parent: " << tmp->getName() << endl;
            }
        }
    }
    return tmp;
}


Component* ComponentSystem::getSubComponent(string name)
{
//    cout << "getSubComponent: " << name << " in " <<  this->mName << endl;
    SubComponentMapT::iterator it = mSubComponentMap.find(name);
    if (it != mSubComponentMap.end())
    {
        return it->second;
    }
    else
    {
        //cout << "getSubComponent: The component you requested: " << name << " does not exist in: " << this->mName << endl;
        return 0;
    }
}


ComponentSystem* ComponentSystem::getSubComponentSystem(string name)
{
    Component* temp_component_ptr = getSubComponent(name);
    ComponentSystem* temp_compsys_ptr = dynamic_cast<ComponentSystem*>(temp_component_ptr);

    if (temp_compsys_ptr == NULL)
    {
        cout << "dynamic cast failed, maybe " << name << " is not a component system" << endl;
        assert(false);
    }

    return temp_compsys_ptr;
}


vector<string> ComponentSystem::getSubComponentNames()
{
    //! @todo for now create a vector of the component names, later maybe we should return a pointer to the real internal map
    vector<string> names;
    SubComponentMapT::iterator it;
    for (it = mSubComponentMap.begin(); it != mSubComponentMap.end(); ++it)
    {
        names.push_back(it->first);
    }

    return names;
}


bool  ComponentSystem::haveSubComponent(string name)
{
    if (mSubComponentMap.count(name) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//! Adds a node as subnode in the system, if the node is already owned by someone else, trasfere owneship to this system
void ComponentSystem::addSubNode(Node* node_ptr)
{
    if (node_ptr->getOwnerSystem() != 0)
    {
        node_ptr->getOwnerSystem()->removeSubNode(node_ptr);
    }
    mSubNodePtrs.push_back(node_ptr);
    node_ptr->mpOwnerSystem = this;
}


//! Removes a previously added node
void ComponentSystem::removeSubNode(Node* node_ptr)
{
    vector<Node*>::iterator it;
    for (it=mSubNodePtrs.begin(); it!=mSubNodePtrs.end(); ++it)
    {
        if (*it == node_ptr)
        {
            node_ptr->mpOwnerSystem = 0;
            mSubNodePtrs.erase(it);
            break;
        }
    }
    //! @todo some notification if you try to remove something that does not exist (can not check it==mSubNodePtrs.end() ) this check can be OK after an successfull erase
}


//! preAllocates log space (to speed up later access for log writing)
void ComponentSystem::preAllocateLogSpace(const double startT, const double stopT, const size_t nSamples)
{
    cout << "stopT = " << stopT << ", startT = " << startT << ", mTimestep = " << mTimestep << endl;

    //Allocate memory for subnodes
    vector<Node*>::iterator it;
    for (it=mSubNodePtrs.begin(); it!=mSubNodePtrs.end(); ++it)
    {
        if (mStop)
            break;

        //! @todo this is an ugly quit hack test
        (*it)->setLogSettingsNSamples(nSamples, startT, stopT, mTimestep);
        //(*it)->setLogSettingsSkipFactor(1, startT, stopT, mTimestep);
        //(*it)->preAllocateLogSpace(needed_slots);
        (*it)->preAllocateLogSpace();
    }
}


//! Tells all subnodes contained within a system to store current data in log
void ComponentSystem::logAllNodes(const double time)
{
    vector<Node*>::iterator it;
    for (it=mSubNodePtrs.begin(); it!=mSubNodePtrs.end(); ++it)
    {
        (*it)->logData(time);
    }
}


//! Adds a transparent SubSystemPort
Port* ComponentSystem::addSystemPort(string portname)
{
    if (portname.empty())
    {
        //Force default portname p, if nothing else specified
        portname = "p";
    }

    //! @todo not hardcode, "undefined_nodetype" maybe define or something, it is used elsevere also
    return addPort(portname, SYSTEMPORT, "undefined_nodetype", Port::REQUIRED);
}


//! Rename system port
string ComponentSystem::renameSystemPort(const string oldname, const string newname)
{
    return renamePort(oldname,newname);
}


//! delete System prot
void ComponentSystem::deleteSystemPort(const string name)
{
    deletePort(name);
}


//! Set the type C, Q, or S of the subsystem
void ComponentSystem::setTypeCQS(typeCQS cqs_type, bool doOnlyLocalSet)
{
    //! @todo should really try to figure out a better way to do this
    //! @todo need to do erro checking, and make sure that the specified type really is valid, first and last component should be of this type (i think)

    //If type same as before do nothing
    if (cqs_type !=  mTypeCQS)
    {
        //Do we have a system parent
        if ( (mpSystemParent != 0) && (!doOnlyLocalSet) )
        {
            //Request change by our parent (som parent cahnges are neeeded)
            mpSystemParent->changeTypeCQS(mName, cqs_type);
        }
        else
        {
            switch (cqs_type)
            {
            case Component::C :
                mTypeCQS = Component::C;
                break;

            case Component::Q :
                mTypeCQS = Component::Q;
                break;

            case Component::S :
                mTypeCQS = Component::S;
                break;

            case Component::UNDEFINEDCQSTYPE :
                mTypeCQS = Component::UNDEFINEDCQSTYPE;
                break;

            default :
                cout << "Error: Specified type _" << getTypeCQSString() << "_ does not exist!" << endl;
                gCoreMessageHandler.addWarningMessage("Specified type: " + getTypeCQSString() + " does not exist!, System CQStype unchanged");
            }
        }
    }
}

//! @brief Change the cqs type of a stored subsystem component
bool ComponentSystem::changeTypeCQS(const string name, const typeCQS newType)
{
    //First get the component ptr and check if we are requesting new type
    Component* tmpptr = getSubComponent(name);
    if (newType != tmpptr->getTypeCQS())
    {
        //check that it is a system component, in that case change the cqs type
        if ( tmpptr->isComponentSystem() )
        {
            //Cast to system ptr
            //! @todo should have a member function that return systemcomponent ptrs
            ComponentSystem* tmpsysptr = dynamic_cast<ComponentSystem*>(tmpptr);

            //Remove old version
            this->removeSubComponentPtrFromStorage(tmpsysptr);

            //Change cqsType localy in the subcomponent, make sure to set true to avoid looping back to this rename
            tmpsysptr->setTypeCQS(newType, true);

            //readd to system
            this->addSubComponentPtrToStorage(tmpsysptr);
        }
        else
        {
            return false;
        }
    }
    return true;
}

//! @brief This function automatically determines the CQS type depending on the what has been connected to the systemports
//! @todo This function will go through all conected ports every time it is run, maybe a quicker version would only be run on the port beeing connected or disconnectd, in the connect and disconnect function
void ComponentSystem::determineCQSType()
{
    PortPtrMapT::iterator ppmit;

    size_t c_ctr=0;
    size_t q_ctr=0;
    size_t s_ctr=0;

    for (ppmit=mPortPtrMap.begin(); ppmit!=mPortPtrMap.end(); ++ppmit)
    {
        //all ports should be system ports in a subsystem
        assert((*ppmit).second->getPortType() == SYSTEMPORT);

        //! @todo I dont think that I really need to ask for ALL connected subports here, as it is actually only the component that is directly connected to the system port that is interesting
        //! @todo this means that I will be able to UNDO the Port getConnectedPorts madness, maybe, if wedont want ot in some other place
        vector<Port*> connectedPorts = (*ppmit).second->getConnectedPorts(-1); //Make a copy of connected ports
        vector<Port*>::iterator cpit;
        for (cpit=connectedPorts.begin(); cpit!=connectedPorts.end(); ++cpit)
        {
            if ( (*cpit)->getComponent()->getSystemParent() == this )
            {
                switch ((*cpit)->getComponent()->getTypeCQS())
                {
                case C :
                    ++c_ctr;
                    break;
                case Q :
                    ++q_ctr;
                    break;
                case S :
                    ++s_ctr;
                    break;
                default :
                    ;
                    //Do nothing, (connecting a port from a system with no cqs type set yet)
                }
            }
        }
    }

    //Ok now lets determine if we have a valid CQS type or not
    if ( (c_ctr > 0) && (q_ctr == 0) )
    {
        this->setTypeCQS(C);
    }
    else if ( (q_ctr > 0) && (c_ctr == 0) )
    {
        this->setTypeCQS(Q);
    }
    else if ( (s_ctr > 0) && (c_ctr==0) && (q_ctr==0) )
    {
        this->setTypeCQS(S);
    }
    else
    {
        //If we swap from valid type then give warning
        if (this->getTypeCQS() != UNDEFINEDCQSTYPE)
        {
            gCoreMessageHandler.addWarningMessage(string("Your action has caused the CQS type to become invalid in system: ")+this->getName());
        }
        //! @todo maybe we should let the GUI display ??? in port overlays instead of sending warning messages

        this->setTypeCQS(UNDEFINEDCQSTYPE);
    }
}


//! Connect two commponents string version
bool ComponentSystem::connect(string compname1, string portname1, string compname2, string portname2)
{
    Port* pPort1;
    Port* pPort2;

    //First some error checking
    stringstream ss; //Error string stream

    //Check if the components exist (and can be found)
    Component* pComp1 = getComponent(compname1);
    Component* pComp2 = getComponent(compname2);

    if (pComp1 == 0)
    {
        ss << "Component1: '"<< compname1 << "' can not be found when atempting connect";
        gCoreMessageHandler.addErrorMessage(ss.str(), "connectwithoutcomponent");
        return false;
    }

    if (pComp2 == 0)
    {
        ss << "Component2: '"<< compname2 << "' can not be found when atempting connect";
        gCoreMessageHandler.addErrorMessage(ss.str(), "connectwithoutcomponent");
        return false;
    }

    //Check if commponents have specified ports
    if (!pComp1->getPort(portname1, pPort1))
    {
        ss << "Component: '"<< pComp1->getName() << "' does not have a port named '" << portname1 << "'";
        gCoreMessageHandler.addErrorMessage(ss.str(), "portdoesnotexist");
        return false;
    }

    if (!pComp2->getPort(portname2, pPort2)) //Not else if because pPort2 has to be set in getPort
    {
        //raise Exception('type of port does not exist')
        ss << "Component: '"<< pComp2->getName() << "' does not have a port named '" << portname2 << "'";
        gCoreMessageHandler.addErrorMessage(ss.str(), "portdoesnotexist");
        return false;
    }

    //Ok components and ports exist, lets atempt the connect
    return connect( pPort1, pPort2 );
}

bool ConnectionAssistant::ensureSameNodeType(Port *pPort1, Port *pPort2)
{
    //Check if both ports have the same node type specified
    if (pPort1->getNodeType() != pPort2->getNodeType())
    {
        stringstream ss;
        ss << "You can not connect a {" << pPort1->getNodeType() << "} port to a {" << pPort2->getNodeType()  << "} port." <<
              " When connecting: {" << pPort1->getComponent()->getName() << "::" << pPort1->getPortName() << "} to {" << pPort2->getComponent()->getName() << "::" << pPort2->getPortName() << "}";
        gCoreMessageHandler.addErrorMessage(ss.str());
        return false;
    }
    return true;
}

//! Assumes that nodetype is set in both nodes
bool ConnectionAssistant::createNewNodeConnection(Port *pPort1, Port *pPort2, Node *&rpCreatedNode)
{
    std::cout << "-----------------------------createNewNodeConnection" << std::endl;
    if (!ensureSameNodeType(pPort1, pPort2))
    {
        return false;
    }

    //Create an instance of the node specified in nodespecifications
    //Node* pNode = gCoreNodeFactory.createInstance(pPort1->getNodeType());
    Node* pNode = HopsanEssentials::getInstance()->createNode(pPort1->getNodeType());

    // Check so the ports can be connected
    if (ensureConnectionOK(pNode, pPort1, pPort2))
    {
        //Set node in both components ports and add it to the parent system component
        pPort1->setNode(pNode);
        pPort2->setNode(pNode);

        //Add port pointers to node
        pNode->setPort(pPort1);
        pNode->setPort(pPort2);

        //let the ports know about each other
        pPort1->addConnectedPort(pPort2);
        pPort2->addConnectedPort(pPort1);

        //Return the created node
        rpCreatedNode = pNode;
        return true;
    }
    else
    {
        stringstream ss;
        ss << "Problem occured at connection" << pPort1->getComponentName() << " and " << pPort2->getComponentName();
        gCoreMessageHandler.addErrorMessage(ss.str());
        delete pNode;
        rpCreatedNode = 0;
        return false;
    }
}


bool ConnectionAssistant::mergeOrJoinNodeConnection(Port *pPort1, Port *pPort2, Node *&rpCreatedNode)
{
    std::cout << "-----------------------------mergeOrJoinNodeConnection" << std::endl;
    //! @todo no isok check is performed (not checks at all are performed)
    Port *pMergeFrom, *pMergeTo;
    assert(pPort1->isConnected() || pPort2->isConnected());

    if (!ensureSameNodeType(pPort1, pPort2))
    {
        return false;
    }

    //Ok, should we merge or join node connection
    //lets allways merge, but if node is missing in one port than the "merge" is actually a join
    if (!pPort1->isConnected())
    {
        pMergeFrom = pPort1;
        pMergeTo = pPort2;
    }
    else if (!pPort2->isConnected())
    {
        pMergeFrom = pPort2;
        pMergeTo = pPort1;
    }
    else
    {
        //! @todo maybe we should selcet in some smart way, for now lets merge from port1
        pMergeFrom = pPort1;
        pMergeTo = pPort2;
    }

    //lets keep the node in merge to port
    Node *pKeepNode = pMergeTo->getNodePtr();
    Node *pDiscardNode = pMergeFrom->getNodePtr();

    //Check for very rare occurance, (Looping a subsystem, and connecting a out port to a in port hwo are actually directly connected to each other)
    //assert(pKeepNode != pDiscardNode);
    if (pKeepNode == pDiscardNode)
    {
        //! @todo dont know if this error message is clear, but this should rarely happen
        gCoreMessageHandler.addErrorMessage("This connection would mean that a node is joined with it self, this does not make any sense and is not allowed");
        return false;
    }


    //set the new node recursively in the other port
    recursivelySetNode(pMergeFrom,0, pKeepNode);

    //let the ports know about each other
    pMergeFrom->addConnectedPort(pMergeTo);
    pMergeTo->addConnectedPort(pMergeFrom);

    if (pDiscardNode != 0)
    {
//        std::cout << "node2 ports size: " <<  pDiscardNode->mPortPtrs.size() << std::endl;
//        assert(pDiscardNode->mPortPtrs.size() == 0);
        //! @todo Right now we dont empty the node to be discarded, we just delete it, this should be OK, but if we implement a recursivelyUnSetNode function we could do this is empty check agian, the advantage of this check is to make sure that we are not doing any mistakes in the code
        pDiscardNode->getOwnerSystem()->removeSubNode(pDiscardNode);
        delete pDiscardNode;
    }


    if (ensureConnectionOK(pKeepNode, pMergeFrom, pMergeTo))
    {
        rpCreatedNode = pKeepNode;
        return true;
    }
    else
    {
        splitNodeConnection(pMergeFrom, pMergeTo); //Undo connection
        rpCreatedNode = 0;
        return false;
    }
}

void ConnectionAssistant::determineWhereToStoreNodeAndStoreIt(Node* pNode)
{
    //node ptr should not be zero
    assert(pNode != 0);

    vector<Port*>::iterator pit;
    Component *pMinLevelComp=0;
    //size_t min = std::numeric_limits<size_t>::max();
    size_t min = (size_t)-1;
    for (pit=pNode->mPortPtrs.begin(); pit!=pNode->mPortPtrs.end(); ++pit)
    {
        if ((*pit)->getComponent()->getModelHierarchyDepth() < min)
        {
            min = (*pit)->getComponent()->getModelHierarchyDepth();
            pMinLevelComp = (*pit)->getComponent();
        }
    }

    //Now add the node at the minimum level, if minimum is a system (we are connecting to our system parant) then dyncast the pointer
    //! @todo what if we are connecting only subsystems within the same lavel AND they have different timesteps
    if (pMinLevelComp->isComponentSystem())
    {
        ComponentSystem *pRootSys = dynamic_cast<ComponentSystem*>(pMinLevelComp);
        pRootSys->addSubNode(pNode);
    }
    else
    {
        pMinLevelComp->getSystemParent()->addSubNode(pNode);
    }
}

bool ConnectionAssistant::deleteNodeConnection(Port *pPort1, Port *pPort2)
{
    stringstream ss;
    assert(pPort1->getNodePtr() == pPort2->getNodePtr());
    Node* node_ptr = pPort1->getNodePtr();
    cout << "nPorts in node: " << node_ptr->mPortPtrs.size() << endl;

    //Make the ports forget about each other
    pPort1->eraseConnectedPort(pPort2);
    pPort2->eraseConnectedPort(pPort1);

    //Make the node forget about the ports
    node_ptr->removePort(pPort1);
    node_ptr->removePort(pPort2);

    //If no more connections exist, remove the entier node and free the memory
    if (node_ptr->mPortPtrs.size() == 0)
    {
        cout << "No more connections to the node exists, deleteing the node" << endl;
        node_ptr->getOwnerSystem()->removeSubNode(node_ptr);
        delete node_ptr;
        //! @todo maybe need to let the factory remove it insted of manually, in case of user supplied external nodes
    }

    return true;
}

void ConnectionAssistant::recursivelySetNode(Port *pPort, Port *pParentPort, Node *pNode)
{
    pPort->setNode(pNode);
    pNode->setPort(pPort);
    vector<Port*>::iterator pit;
    for (pit=pPort->getConnectedPorts().begin(); pit!=pPort->getConnectedPorts().end(); ++pit)
    {
        //dont recures back to parent will get stuck in infinate recursion
        if (*pit == pParentPort)
        {
            continue;
        }
        recursivelySetNode(*pit, pPort, pNode);
    }
}

Port* ConnectionAssistant::findMultiportSubportFromOtherPort(const Port *pMultiPort, Port *pOtherPort)
{
    assert(pOtherPort->getPortType() < MULTIPORT); //Make sure other is not a multiport
    std::vector<Port*> otherConnPorts = pOtherPort->getConnectedPorts();
    for (size_t i=0; i<otherConnPorts.size(); ++i)
    {
        //We assuyme that a port can not be connected multiple times to the same multiport
        if (otherConnPorts[i]->mpParentPort == pMultiPort)
        {
            return otherConnPorts[i];
        }
    }
    return 0;
}

bool ConnectionAssistant::splitNodeConnection(Port *pPort1, Port *pPort2)
{
    Port *pPortToBecomeEmpty=0, *pPortToKeep=0;

    //make sure not both ports will become empty this is handled by other code
    assert( !((pPort1->getConnectedPorts().size() < 2) && (pPort2->getConnectedPorts().size() < 2))  );

    if (pPort1->getConnectedPorts().size() < 2)
    {
        pPortToBecomeEmpty = pPort1;
        pPortToKeep = pPort2;
    }
    else if (pPort2->getConnectedPorts().size() < 2)
    {
        pPortToBecomeEmpty = pPort2;
        pPortToKeep = pPort1;
    }
    else
    {
        //unmerge
        //Handled by if below
    }

    //Check if we are unjoining
    if (pPortToBecomeEmpty !=0)
    {
        Node* pKeepNode = pPortToKeep->getNodePtr();

        //Make node forget the port to be disconnected
        pKeepNode->removePort(pPortToBecomeEmpty);

        //Make the ports forget each other (the disconnected port will also forget node)
        pPortToBecomeEmpty->eraseConnectedPort(pPortToKeep);
        pPortToKeep->eraseConnectedPort(pPortToBecomeEmpty);

        determineWhereToStoreNodeAndStoreIt(pKeepNode); //Relocaate the node if necessary
    }
    //Else we seems to be unmerging, create a new node for the "other side" of the broken connection
    else
    {
        //! @todo maybe make sure that the ports are really systemports to avoid code misstakes
        //Lets keep the node from port1 and create a copy for port two
        Node* pNode1 = pPort1->getNodePtr();
        //Node* pNode2 = gCoreNodeFactory.createInstance(pNode1->getNodeType());
        Node* pNode2 = HopsanEssentials::getInstance()->createNode(pNode1->getNodeType());

        pNode1->mPortPtrs.clear(); //Clear all port knowledge from the port, we will reset it bellow

        //Make the ports forget about each other
        pPort1->eraseConnectedPort(pPort2);
        pPort2->eraseConnectedPort(pPort1);

        //Recursively set the node in the port, directly connected ports and infinitely recursive connected ports
        recursivelySetNode(pPort1,0,pNode1);
        recursivelySetNode(pPort2,0,pNode2);

        //Now determine what system should own the node
        determineWhereToStoreNodeAndStoreIt(pNode1);
        determineWhereToStoreNodeAndStoreIt(pNode2);

    }

    return true;
}


//! Helpfunction that clears the nodetype in empty systemports, It will not clear the type if the port is not empty or if the port is not a systemport
void ConnectionAssistant::clearSysPortNodeTypeIfEmpty(Port *pPort)
{
    if ( (pPort->getPortType() == SYSTEMPORT) && (!pPort->isConnected()) )
    {
        pPort->mNodeType = "";
    }
}

//! Connect two components with specified ports to each other, reference version
bool ComponentSystem::connect(Port *pPort1, Port *pPort2)
{
    ConnectionAssistant connAssist;
    Component* pComp1 = pPort1->getComponent();
    Component* pComp2 = pPort2->getComponent();
    bool sucess=false;

    //First some error checking
    stringstream ss; //Message string stream

    //Prevent connection with self
    if (pPort1 == pPort2)
    {
        ss << "You can not connect a port to it self";
        gCoreMessageHandler.addErrorMessage(ss.str(), "selfconnection");
        return false;
    }

    //Prevent connection between two multiports
    //! @todo we might want to allow this in the future, right now disconnecting two multiports is also not implemented
    if ( pPort1->isMultiPort() && pPort2->isMultiPort() )
    {
        gCoreMessageHandler.addErrorMessage("You are not allowed to connect two MultiPorts to each other, (this may be allowed in the future)");
        return false;
    }


    //Prevent connection if porst are already connected to each other
    //! @todo What will happend with multiports
    if (pPort1->isConnectedTo(pPort2))
    {
        ss << "These two ports are already connected to each other";
        gCoreMessageHandler.addErrorMessage(ss.str(), "allreadyconnected");
        return false;
    }

    if (!connAssist.ensureNotCrossConnecting(pPort1, pPort2))
    {
        ss << "You can not cross-connect between systems";
        gCoreMessageHandler.addErrorMessage(ss.str(), "crossconnection");
        return false;
    }

    //Prevent connection of two blank systemports
    if ( (pPort1->getPortType() == SYSTEMPORT) && (pPort2->getPortType() == SYSTEMPORT) )
    {
        if ( (!pPort1->isConnected()) && (!pPort2->isConnected()) )
        {
            ss << "You are not allowed to connect two blank systemports to each other";
            gCoreMessageHandler.addErrorMessage(ss.str());
            return false;
        }
    }

    Node *pResultingNode = 0;
    //Now lets find out if one of the ports is a blank systemport
    //! @todo better way to find out if systemports are blank might give more clear code
    if ( ( (pPort1->getPortType() == SYSTEMPORT) && (!pPort1->isConnected()) ) || ( (pPort2->getPortType() == SYSTEMPORT) && (!pPort2->isConnected()) ) )
    {
        //Now lets find out wich of the ports that is a blank systemport
        Port *pBlankSysPort;
        Port *pOtherPort;

        //! @todo write help function
        if ( (pPort1->getPortType() == SYSTEMPORT) && (!pPort1->isConnected()) )
        {
            pBlankSysPort = pPort1;
            pOtherPort = pPort2;
        }
        else if ( (pPort2->getPortType() == SYSTEMPORT) && (!pPort2->isConnected()) )
        {
            pBlankSysPort = pPort2;
            pOtherPort = pPort1;
        }
        else
        {
            //this should not happen, assert is making sure we dont code wrong
            assert(false);
        }

        pBlankSysPort->mNodeType = pOtherPort->getNodeType(); //set the nodetype in the sysport

        //Check if we are connecting multiports, in that case add new subport, remember original portPointer though so that we can clean up if failure
        Port *pOtherMultiPort=0;
        connAssist.ifMultiportAddSubportAndSwapPtr(pOtherPort, pOtherMultiPort);

        if (!pOtherPort->isConnected())
        {
            sucess = connAssist.createNewNodeConnection(pBlankSysPort, pOtherPort, pResultingNode);
        }
        else
        {
            sucess = connAssist.mergeOrJoinNodeConnection(pBlankSysPort, pOtherPort, pResultingNode);
        }

        //Handle multiport connection sucess or failure
        connAssist.ifMultiportCleanupAfterConnect(pOtherPort, pOtherMultiPort, sucess);
    }
    //Non of the ports  are blank systemports
    else
    {
        //Check if we are connecting multiports, in that case add new subport, remember original portPointer though so that we can clean up if failure
        Port *pMultiPort1=0, *pMultiPort2=0;
        connAssist.ifMultiportAddSubportAndSwapPtr(pPort1, pMultiPort1);
        connAssist.ifMultiportAddSubportAndSwapPtr(pPort2, pMultiPort2);

        if (!pPort1->isConnected() && !pPort2->isConnected())
        {
            sucess = connAssist.createNewNodeConnection(pPort1, pPort2, pResultingNode);
        }
        else
        {
            sucess = connAssist.mergeOrJoinNodeConnection(pPort1, pPort2, pResultingNode);
        }

        //Handle multiport connection sucess or failure
        connAssist.ifMultiportCleanupAfterConnect(pPort1, pMultiPort1, sucess);
        connAssist.ifMultiportCleanupAfterConnect(pPort2, pMultiPort2, sucess);
    }

    //Abbort conenction if there was a connect failure
    if (!sucess)
    {
        return false;
    }

    //Update the CQS type
    this->determineCQSType();

    //Update parent cqs-type
    //! @todo we should only do this if we are actually connected directly to our parent, but I dont know what will take the most time, to ckeach if we are connected to parent or to just allways refresh parent
    if (mpSystemParent != 0)
    {
        this->mpSystemParent->determineCQSType();
    }

    //Update the node placement
    connAssist.determineWhereToStoreNodeAndStoreIt(pResultingNode);

    ss << "Connected: {" << pComp1->getName() << "::" << pPort1->getPortName() << "} and {" << pComp2->getName() << "::" << pPort2->getPortName() << "}";
    gCoreMessageHandler.addDebugMessage(ss.str(), "succesfulconnect");
    return true;
}



bool ConnectionAssistant::ensureConnectionOK(Node *pNode, Port *pPort1, Port *pPort2)
{
    size_t n_ReadPorts = 0;
    size_t n_WritePorts = 0;
    size_t n_PowerPorts = 0;
    size_t n_SystemPorts = 0;
    //size_t n_MultiPorts = 0;

    size_t n_Ccomponents = 0;
    size_t n_Qcomponents = 0;
    size_t n_SYScomponentCs = 0;
    size_t n_SYScomponentQs = 0;

    //Count the different kind of ports and C,Q components in the node
    vector<Port*>::iterator it;
    for (it=(*pNode).mPortPtrs.begin(); it!=(*pNode).mPortPtrs.end(); ++it)
    {
        if ((*it)->getPortType() == READPORT)
        {
            n_ReadPorts += 1;
        }
        else if ((*it)->getPortType() == WRITEPORT)
        {
            n_WritePorts += 1;
        }
        else if ((*it)->getPortType() == POWERPORT)
        {
            n_PowerPorts += 1;
        }
        else if ((*it)->getPortType() == SYSTEMPORT)
        {
            n_SystemPorts += 1;
        }
//        else if((*it)->getPortType() > MULTIPORT)
//        {
//            n_MultiPorts += 1;
//        }

        if ((*it)->getComponent()->isComponentC())
        {
            n_Ccomponents += 1;
            if ((*it)->getComponent()->isComponentSystem())
            {
                n_SYScomponentCs += 1;
            }
        }
        else if ((*it)->getComponent()->isComponentQ())
        {
            n_Qcomponents += 1;
            if ((*it)->getComponent()->isComponentSystem())
            {
                n_SYScomponentQs += 1;
            }
        }

    }

    //Check the kind of ports in the components subjected for connection
    //Dont count port if it is already conected to node as it was counted in the code above (avoids double counting)
    if ( !pNode->isConnectedToPort(pPort1) )
    {
        if ( pPort1->getPortType() == READPORT )
        {
            n_ReadPorts += 1;
        }
        if ( pPort1->getPortType() == WRITEPORT )
        {
            n_WritePorts += 1;
        }
        if ( pPort1->getPortType() == POWERPORT )
        {
            n_PowerPorts += 1;
        }
        if ( pPort1->getPortType() == SYSTEMPORT )
        {
            n_SystemPorts += 1;
        }
//        if( pPort1->getPortType() > MULTIPORT)
//        {
//            n_MultiPorts += 1;
//        }
        if ( pPort1->getComponent()->isComponentC() )
        {
            n_Ccomponents += 1;
            if ( pPort1->getComponent()->isComponentSystem() )
            {
                n_SYScomponentCs += 1;
            }
        }
        if ( pPort1->getComponent()->isComponentQ() )
        {
            n_Qcomponents += 1;
            if ( pPort1->getComponent()->isComponentSystem() )
            {
                n_SYScomponentQs += 1;
            }
        }
    }

    //Dont count port if it is already conected to node as it was counted in the code above (avoids double counting)
    if ( !pNode->isConnectedToPort(pPort2) )
    {
        if ( pPort2->getPortType() == READPORT )
        {
            n_ReadPorts += 1;
        }
        if ( pPort2->getPortType() == WRITEPORT )
        {
            n_WritePorts += 1;
        }
        if ( pPort2->getPortType() == POWERPORT )
        {
            n_PowerPorts += 1;
        }
        if ( pPort2->getPortType() == SYSTEMPORT )
        {
            n_SystemPorts += 1;
        }
        if ( pPort2->getComponent()->isComponentC() )
        {
            n_Ccomponents += 1;
            if ( pPort2->getComponent()->isComponentSystem() )
            {
                n_SYScomponentCs += 1;
            }
        }
        if ( pPort2->getComponent()->isComponentQ() )
        {
            n_Qcomponents += 1;
            if ( pPort2->getComponent()->isComponentSystem() )
            {
                n_SYScomponentQs += 1;
            }
        }
    }

    //Check if there are some problems with the connection
//    if(n_MultiPorts > 1)
//    {
//        gCoreMessageHandler.addErrorMessage("Trying to connect two MultiPorts to each other");
//        return false;
//    }
    if (n_PowerPorts > 2)
    {
        gCoreMessageHandler.addErrorMessage("Trying to connect more than two PowerPorts to same node");
        return false;
    }
    if (n_WritePorts > 1)
    {
        gCoreMessageHandler.addErrorMessage("Trying to connect more than one WritePort to same node");
        return false;
    }
    if ((n_PowerPorts > 0) && (n_WritePorts > 0))
    {
        gCoreMessageHandler.addErrorMessage("Trying to connect WritePort and PowerPort to same node");
        return false;
    }
    if ((n_PowerPorts == 0) && (n_WritePorts == 0) && (n_SystemPorts == 0))
    {
        cout << "Trying to connect only ReadPorts" << endl;
        gCoreMessageHandler.addErrorMessage("Trying to connect only ReadPorts");
        return false;
    }

    //cout << "nQ: " << n_Qcomponents << " nC: " << n_Ccomponents << endl;

    //Normaly we want at most one c and one q component but if there happen to be a subsystem in the picture allow one extra
    //This is only true if at least one powerport is connected - signal connecetions can be between any types of components
    //! @todo not 100% sure that this will work allways. Only work if we assume that the subsystem has the correct cqs type when connecting
    if ((n_Ccomponents > 1+n_SYScomponentCs) && (n_PowerPorts > 0))
    {
        gCoreMessageHandler.addErrorMessage("You can not connect two C-Component power ports to each other");
        return false;
    }
    if ((n_Qcomponents > 1+n_SYScomponentQs) && (n_PowerPorts > 0))
    {
        gCoreMessageHandler.addErrorMessage("You can not connect two Q-Component power ports to each other");
        return false;
    }
//    if ((pPort1->getPortType() == Port::READPORT) &&  (pPort2->getPortType() == Port::READPORT))
//    {
//        gCoreMessageHandler.addErrorMessage("Trying to connect ReadPort to ReadPort");
//        return false;
//    }
//    if( ((pPort1->getPortType() == Port::READPORT) && pPort2->getPortType() == Port::POWERPORT && n_PowerPorts > 1) or
//        ((pPort2->getPortType() == Port::READPORT) && pPort1->getPortType() == Port::POWERPORT && n_PowerPorts > 1) )
//    {
//        gCoreMessageHandler.addErrorMessage("Trying to connect one ReadPort to more than one PowerPort");
//        return false;
//    }

    //It seems to be OK!
    return true;
}

bool ConnectionAssistant::ensureNotCrossConnecting(Port *pPort1, Port *pPort2)
{
    //Check so that both components to connect have been added to the same system (or we are connecting to parent system)
    if ( (pPort1->getComponent()->getSystemParent() != pPort2->getComponent()->getSystemParent()) )
    {
        if ( (pPort1->getComponent()->getSystemParent() != pPort2->getComponent()) && (pPort2->getComponent()->getSystemParent() != pPort1->getComponent()) )
        {
            stringstream ss;
            ss << "The components, {"<< pPort1->getComponentName() << "} and {" << pPort2->getComponentName() << "}, "<< "must belong to the same subsystem";
            gCoreMessageHandler.addErrorMessage(ss.str());
            return false;
        }
    }
    return true;
}

//! @brief Detects if a port is a multiport, adds a subport and swaps the pointer, storing original port in argument two ptr
//! @param [in out] rpPort A refrence to a pointer to the port, will be swapped to new subport if multiport
//! @param [in out] rpOriginalPort A refrence to a pointer to the original multiport, will be 0 if not a multiport, will point to the multiport otherwise
void ConnectionAssistant::ifMultiportAddSubportAndSwapPtr(Port *&rpPort, Port *&rpOriginalPort)
{
    rpOriginalPort = 0; //Make sure null if not multiport
    if (rpPort->getPortType() >= MULTIPORT)
    {
        rpOriginalPort = rpPort;
        rpPort = rpPort->addSubPort();
    }
}

void ConnectionAssistant::ifMultiportCleanupAfterConnect(Port *pSubPort, Port *pMultiPort, const bool wasSucess)
{
    if (pMultiPort != 0)
    {
        if (wasSucess)
        {
            //! @todo What do we need to do to handle sucess
        }
        else
        {
            //We need to remove the last created subport
            pMultiPort->removeSubPort(pSubPort);
        }
    }
}

void ConnectionAssistant::ifMultiportCleanupAfterDissconnect(Port *&rpSubPort, Port *pMultiPort, const bool wasSucess)
{
    if (pMultiPort != 0)
    {
        if (wasSucess)
        {
            //If sucessful we should remove the empty port
            pMultiPort->removeSubPort(rpSubPort); //! @todo maybe should set the pointer to 0 inside when deleted, need ref to ptr or ptr ptr
            rpSubPort = pMultiPort; //We copy the multiport pointer back to the support pointer to make sure that it is still working
        }
        else
        {
            //! @todo What do we need to do to handle failure, nothing maybe
        }
    }
}

//! @brief Prepares port pointers for multiport disconnections,
void ConnectionAssistant::ifMultiportPrepareForDissconnect(Port *&rpPort1, Port *&rpPort2, Port *&rpMultiPort1, Port *&rpMultiPort2)
{
    //First make usre that multiport pointers are zero if no multiports are beeing connected
    rpMultiPort1=0;
    rpMultiPort2=0;

    // Port 1 is a multiport, but not port2
    if (rpPort1->getPortType() >= MULTIPORT && rpPort2->getPortType() < MULTIPORT )
    {
        rpMultiPort1 = rpPort1;
//        assert(rpPort2->getConnectedPorts().size() == 1); //Make sure that we dont atempt tu run this code on ports that are not to become empty
//        rpPort1 = rpPort2->getConnectedPorts()[0];
        rpPort1 = findMultiportSubportFromOtherPort(rpMultiPort1, rpPort2);
        assert(rpPort1 != 0);

    }
    // Port 2 is a multiport, but not port1
    else if (rpPort1->getPortType() < MULTIPORT && rpPort2->getPortType() >= MULTIPORT )
    {
        rpMultiPort2 = rpPort2;
//        assert(rpPort1->getConnectedPorts().size() == 1); //Make sure that we dont atempt tu run this code on ports that are not to become empty
//        rpPort2 = rpPort1->getConnectedPorts()[0];
        rpPort2 = findMultiportSubportFromOtherPort(rpMultiPort2, rpPort1);
        assert(rpPort2 != 0);
    }
    // both ports are multiports
    else if (rpPort1->getPortType() >= MULTIPORT && rpPort2->getPortType() >= MULTIPORT )
    {
        assert("Multiport <-> Multiport disconnection has not been implemented yet Aborting!" == 0);
        //! @todo need to search around to find correct subports
    }
}


//! @brief Disconnect two ports, string version
//! @todo need to make sure that components and prots given by name exist here
bool ComponentSystem::disconnect(string compname1, string portname1, string compname2, string portname2)
{
    Component *pComp1, *pComp2;
    Port *pPort1, *pPort2;

    pComp1 = getComponent(compname1);
    pComp2 = getComponent(compname2);

    if ( (pComp1!=0) && (pComp2!=0) )
    {
        pPort1 = pComp1->getPort(portname1);
        pPort2 = pComp2->getPort(portname2);

        if ( (pComp1!=0) && (pComp2!=0) )
        {
            return disconnect(pPort1, pPort2);
        }
    }

    stringstream ss;
    ss << "Disconnect: Could not find either " << compname1 << "->" << portname1 << " or " << compname2 << "->" << portname2 << endl;
    gCoreMessageHandler.addDebugMessage(ss.str());
    return false;
}

//! @brief Disconnects two ports and remove node if no one is using it any more.
//! @param pPort1 Pointer to first port
//! @param pPort2 Pointer to second port
//! @todo whay about system ports they are somewaht speciall
bool ComponentSystem::disconnect(Port *pPort1, Port *pPort2)
{
    cout << "disconnecting " << pPort1->getComponentName() << " " << pPort1->getPortName() << "  and  " << pPort2->getComponentName() << " " << pPort2->getPortName() << endl;

    ConnectionAssistant disconnAssistant;
    stringstream ss;
    bool success = false;
    //! @todo some more advanced error handling (are the ports really connected to each other and such)

    if (pPort1->isConnected() && pPort2->isConnected())
    {

        // If BOTH ports will NOT become empty, and if non of them are multiports
        if ( ((pPort1->getConnectedPorts().size() > 1) || (pPort2->getConnectedPorts().size() > 1)) &&
             !pPort1->isMultiPort() && !pPort2->isMultiPort() )
        {
            //! @todo what happens if we dissconnect a multiport from a port with multiple connections (can that even happen)
            success = disconnAssistant.splitNodeConnection(pPort1, pPort2);
        }
        // If BOTH ports will NOT become empty, and if the one becoming empty is a multiport
        //! @todo this check is incomplete becouse it collides with the creapy multiport getConnectedPorts madness
        else if ( ( pPort1->isMultiPort() || pPort2->isMultiPort() ) &&
                  ( ( (pPort1->getConnectedPorts().size() > 1) && !pPort1->isMultiPort() ) ||
                    ( (pPort2->getConnectedPorts().size() > 1) && !pPort2->isMultiPort() ) ) )
        {
            assert( pPort1->isMultiPort() || pPort2->isMultiPort() );

            //=========
            //! @todo these lineas are degugging checks, can mayb be removed later
            if (pPort1->isMultiPort())
            {
                assert(!pPort2->isMultiPort());
                assert(pPort2->getConnectedPorts().size() > 1);

            }
            if (pPort2->isMultiPort())
            {
                assert(!pPort1->isMultiPort());
                assert(pPort1->getConnectedPorts().size() > 1);

            }
            //==========

            //Handle multiports
            Port* pOriginalPort1=0, *pOriginalPort2=0;
            disconnAssistant.ifMultiportPrepareForDissconnect(pPort1, pPort2, pOriginalPort1, pOriginalPort2);

            success = disconnAssistant.splitNodeConnection(pPort1, pPort2);

            //Handle multiport connection sucess or failure
            disconnAssistant.ifMultiportCleanupAfterDissconnect(pPort1, pOriginalPort1, success);
            disconnAssistant.ifMultiportCleanupAfterDissconnect(pPort2, pOriginalPort2, success);

        }
        // If both ports will become empty, and if one or both is a multiport
        else
        {
            //Handle multiports
            Port* pOriginalPort1=0, *pOriginalPort2=0;
            disconnAssistant.ifMultiportPrepareForDissconnect(pPort1, pPort2, pOriginalPort1, pOriginalPort2);

            success = disconnAssistant.deleteNodeConnection(pPort1, pPort2);

            //Handle multiport connection sucess or failure
            disconnAssistant.ifMultiportCleanupAfterDissconnect(pPort1, pOriginalPort1, success);
            disconnAssistant.ifMultiportCleanupAfterDissconnect(pPort2, pOriginalPort2, success);
        }

        disconnAssistant.clearSysPortNodeTypeIfEmpty(pPort1);
        disconnAssistant.clearSysPortNodeTypeIfEmpty(pPort2);
        //! @todo maybe incorporate the clear checks into delete node and unmerge

    }
    else
    {
        gCoreMessageHandler.addWarningMessage("In disconnect: At least one of the ports do not seem to be connected, (does nothing)");
    }

    //Update the CQS type
    this->determineCQSType();

    //Update parent cqs-type
    //! @todo we should only do this if we are actually connected directly to our parent, but I dont know what will take the most time, to ckeach if we are connected to parent or to just allways refresh parent
    if (mpSystemParent != 0)
    {
        this->mpSystemParent->determineCQSType();
    }

    ss << "Disconnected: {"<< pPort1->getComponent()->getName() << "::" << pPort1->getPortName() << "} and {" << pPort2->getComponent()->getName() << "::" << pPort2->getPortName() << "}";
    cout << ss.str() << endl;
    gCoreMessageHandler.addDebugMessage(ss.str(), "succesfuldisconnect");

    return success;
}


//! @brief Sets the desired time step in a component system.
//! @brief param timestep New desired time step
void ComponentSystem::setDesiredTimestep(const double timestep)
{
    mDesiredTimestep = timestep;
    setTimestep(timestep);
}


void ComponentSystem::setInheritTimestep(const bool inherit)
{
    mInheritTimestep = inherit;
}


bool ComponentSystem::doesInheritTimestep()
{
    return mInheritTimestep;
}


//void ComponentSystem::setTimestep(const double timestep)
//{
//    mTimestep = timestep;
//
//    for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
//    {
//        if (!(mComponentSignalptrs[s]->isComponentSystem()))
//        {
//            mComponentSignalptrs[s]->setTimestep(timestep);
//        }
//    }
//
//    //C components
//    for (size_t c=0; c < mComponentCptrs.size(); ++c)
//    {
//        if (!(mComponentCptrs[c]->isComponentSystem()))
//        {
//            mComponentCptrs[c]->setTimestep(timestep);
//        }
//    }
//
//    //Q components
//    for (size_t q=0; q < mComponentQptrs.size(); ++q)
//    {
//        if (!(mComponentQptrs[q]->isComponentSystem()))
//        {
//            mComponentQptrs[q]->setTimestep(timestep);
//        }
//    }
//}


//! @brief Sets the time step in a component system.
//! Also propagates it to all contained components and systems.
//! @brief param timestep New time step
void ComponentSystem::setTimestep(const double timestep)
{
    mTimestep = timestep;

    for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
    {
        if (!(mComponentSignalptrs[s]->isComponentSystem()))
        {
            mComponentSignalptrs[s]->setTimestep(timestep);
        }
    }

    //C components
    for (size_t c=0; c < mComponentCptrs.size(); ++c)
    {
        if (!(mComponentCptrs[c]->isComponentSystem()))
        {
            mComponentCptrs[c]->setTimestep(timestep);
        }
    }

    //Q components
    for (size_t q=0; q < mComponentQptrs.size(); ++q)
    {
        if (!(mComponentQptrs[q]->isComponentSystem()))
        {
            mComponentQptrs[q]->setTimestep(timestep);
        }
    }
}


//! @brief Figure out which timestep to use for all sub systems
//! @param componentPtrs Vector with pointers to all sub components
void ComponentSystem::adjustTimestep(vector<Component*> componentPtrs)
{
    for (size_t c=0; c < componentPtrs.size(); ++c)
    {
        if (componentPtrs[c]->isComponentSystem())
        {
            if(componentPtrs[c]->doesInheritTimestep()) //Inherit timestep from parent system
            {
                componentPtrs[c]->setTimestep(mTimestep);
            }
            else    //Use desired timestep, and adjust it if necessary
            {
                double subTs = componentPtrs[c]->mDesiredTimestep;

                //If a subsystem's timestep is larger than this sytem's
                //timestep change it to this system's timestep
                if ((subTs > mTimestep) || (subTs < -0.0))
                {
                    subTs = mTimestep;
                }
                //Check that subRs is a multiple of timestep
                else// if ((timestep/subTs - floor(timestep/subTs)) > 0.00001*subTs)
                {
                    //subTs should get the nearest multiple of timestep as possible,
                    subTs = mTimestep/floor(mTimestep/subTs+0.5);
                }
                componentPtrs[c]->setTimestep(subTs);
            }
        }
        else
        {
            componentPtrs[c]->setTimestep(mTimestep);
        }
    }
}


//! @brief Checks that everything is OK before simulation
bool ComponentSystem::isSimulationOk()
{
    //Make sure that there are no components or systems with an undefined cqs_type present
    if (mComponentUndefinedptrs.size() > 0)
    {
        for (size_t i=0; i<mComponentUndefinedptrs.size(); ++i)
        {
            gCoreMessageHandler.addErrorMessage(string("The component {") + mComponentUndefinedptrs[i]->getName() + string("} does not have a valid CQS type."));
        }
        return false;
    }

    //Check the this systems own SystemPorts, are they connected (they must be)
    vector<Port*> ports = getPortPtrVector();
    for (size_t i=0; i<ports.size(); ++i)
    {
        if ( ports[i]->isConnectionRequired() && !ports[i]->isConnected() )
        {
            gCoreMessageHandler.addErrorMessage("Port " + ports[i]->getPortName() + " in " + getName() + " is not connected!");
            return false;
        }
        else if( ports[i]->isConnected() )
        {
            if(ports[i]->getNodePtr()->getNumberOfPortsByType(POWERPORT) == 1)
            {
                gCoreMessageHandler.addErrorMessage("Port " + ports[i]->getPortName() + " in " + getName() + " is connected to a node with only one attached power port!");
                return false;
            }
        }
    }

    //Check all subcomponents to make sure that all requirements for simulation are met
    //scmit = The subcomponent map iterator
    SubComponentMapT::iterator scmit = mSubComponentMap.begin();
    for ( ; scmit!=mSubComponentMap.end(); ++scmit)
    {
        Component* pComp = scmit->second; //Component pointer

        //Check that ALL ports that MUST be connected are connected
        vector<Port*> ports = pComp->getPortPtrVector();
        for (size_t i=0; i<ports.size(); ++i)
        {
            if ( ports[i]->isConnectionRequired() && !ports[i]->isConnected() )
            {
                gCoreMessageHandler.addErrorMessage("Port " + ports[i]->getPortName() + " on " + pComp->getName() + " is not connected!");
                return false;
            }
            else if( ports[i]->isConnected() )
            {
                if(ports[i]->getNodePtr()->getNumberOfPortsByType(POWERPORT) == 1)
                {
                    gCoreMessageHandler.addErrorMessage("Port " + ports[i]->getPortName() + " in " + getName() + " is connected to a node with only one power port!");
                    return false;
                }
            }

            //Check parameters in subcomponents
            std::string errParName;
            if(!(pComp->checkParameters(errParName)))
            {
                gCoreMessageHandler.addErrorMessage("The parameter " + errParName + " in system " + getName() + " and component " + pComp->getName() + " can not be evaluated, a system parameter has maybe been deleted or re-typed.");
                return false;
            }
        }

        //Check parameters in system
        std::string errParName;
        if(!(checkParameters(errParName)))
        {
            gCoreMessageHandler.addErrorMessage("The system parameter " + errParName + " in system " + getName() + " can not be evaluated, it maybe depend on a deleted system parameter.");
            return false;
        }

        //Recures testing into subsystems
        if (pComp->isComponentSystem())
        {
            if (!pComp->isSimulationOk())
            {
                return false;
            }
        }

        //! @todo check that all C-component required ports are connected to Q-component ports

        //! @todo check more stuff
    }

    return true;
}

//! @brief Load start values by telling each component to load their start values
void ComponentSystem::loadStartValues()
{
    std::vector<Component*>::iterator compIt;
    for(compIt = mComponentSignalptrs.begin(); compIt != mComponentSignalptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValues();
    }
    for(compIt = mComponentCptrs.begin(); compIt != mComponentCptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValues();
    }
    for(compIt = mComponentQptrs.begin(); compIt != mComponentQptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValues();
    }
}


//! @brief Load start values from last simulation to start value container
void ComponentSystem::loadStartValuesFromSimulation()
{
    std::vector<Component*>::iterator compIt;
    for(compIt = mComponentSignalptrs.begin(); compIt != mComponentSignalptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValuesFromSimulation();
    }
    for(compIt = mComponentCptrs.begin(); compIt != mComponentCptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValuesFromSimulation();
    }
    for(compIt = mComponentQptrs.begin(); compIt != mComponentQptrs.end(); ++compIt)
    {
        (*compIt)->loadStartValuesFromSimulation();
    }
}


//! @brief Initializes a system and all its contained components before a simulation.
//! Also allocates log data memory.
//! @param startT Start time of simlation
//! @param stopT Stop time of simlation
//! @param nSamples Number of log samples
void ComponentSystem::initialize(const double startT, const double stopT, const size_t nSamples)
{
    cout << "Initializing SubSystem: " << this->mName << endl;
    mStop = false; //This variable cannot be written on below, then problem might occur with thread safety, it's a bit ugly to write on it on this row.

    //preAllocate local logspace
    this->preAllocateLogSpace(startT, stopT, nSamples);

    adjustTimestep(mComponentSignalptrs);
    adjustTimestep(mComponentCptrs);
    adjustTimestep(mComponentQptrs);

    this->sortSignalComponentVector(mComponentSignalptrs);

    loadStartValues();

    //Init
    updateParameters();
    //Signal components
    for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
    {
        if (mStop)
            break;
        mComponentSignalptrs[s]->updateParameters();

        if (mComponentSignalptrs[s]->isComponentSystem())
        {
            mComponentSignalptrs[s]->initialize(startT, stopT, nSamples);
        }
        else
        {
            mComponentSignalptrs[s]->initialize();
        }
    }

    //C components
    for (size_t c=0; c < mComponentCptrs.size(); ++c)
    {
        if (mStop)
            break;
        mComponentCptrs[c]->updateParameters();

        if (mComponentCptrs[c]->isComponentSystem())
        {
            mComponentCptrs[c]->initialize(startT, stopT, nSamples);
        }
        else
        {
            mComponentCptrs[c]->initialize();
        }
    }

    //Q components
    for (size_t q=0; q < mComponentQptrs.size(); ++q)
    {
        if (mStop)
            break;
        mComponentQptrs[q]->updateParameters();

        if (mComponentQptrs[q]->isComponentSystem())
        {
            mComponentQptrs[q]->initialize(startT, stopT, nSamples);
        }
        else
        {
            mComponentQptrs[q]->initialize();
        }
    }
}


//! @brief Does all initialization except log space allocation.
//! Used in export functions to other environment. For some reason this is necessary.
//! @todo Find a better solution
void ComponentSystem::initializeComponentsOnly()
{
    cout << "Initializing SubSystem: " << this->mName << endl;
    mStop = false; //This variable cannot be written on below, then problem might occur with thread safety, it's a bit ugly to write on it on this row.

    adjustTimestep(mComponentSignalptrs);
    adjustTimestep(mComponentCptrs);
    adjustTimestep(mComponentQptrs);

    this->sortSignalComponentVector(mComponentSignalptrs);

    loadStartValues();

    //Init
    //Signal components
    for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
    {
        if (mStop)
            break;

        if (mComponentSignalptrs[s]->isComponentSystem())
        {
            mComponentSignalptrs[s]->initializeComponentsOnly();
        }
        else
        {
            mComponentSignalptrs[s]->initialize();
        }
    }

    //C components
    for (size_t c=0; c < mComponentCptrs.size(); ++c)
    {
        if (mStop)
            break;

        if (mComponentCptrs[c]->isComponentSystem())
        {
            mComponentCptrs[c]->initializeComponentsOnly();
        }
        else
        {
            mComponentCptrs[c]->initialize();
        }
    }

    //Q components
    for (size_t q=0; q < mComponentQptrs.size(); ++q)
    {
        if (mStop)
            break;

        if (mComponentQptrs[q]->isComponentSystem())
        {
            mComponentQptrs[q]->initializeComponentsOnly();
        }
        else
        {
            mComponentQptrs[q]->initialize();
        }
    }
}


#ifdef USETBB


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
    tbb::atomic<int> mCounter;
    tbb::atomic<bool> mLock;
};


//! @brief Class for slave simlation threads, which must be syncronized from a master simulation thread
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
    taskSimSlave(vector<Component*> sVector, vector<Component*> cVector, vector<Component*> qVector, vector<Node*> nVector,
                 double startTime, double timeStep, double stopTime, size_t nThreads, size_t threadID,
                 BarrierLock *pBarrier_S, BarrierLock *pBarrier_C, BarrierLock *pBarrier_Q, BarrierLock *pBarrier_N)
    {
        mVectorS = sVector;
        mVectorC = cVector;
        mVectorQ = qVector;
        mVectorN = nVector;
        mTime = startTime;
        mStopTime = stopTime;
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
        while(mTime < mStopTime)
        {

            //! Signal Components !//

            mpBarrier_S->increment();
            while(mpBarrier_S->isLocked()){}                         //Wait at S barrier

            for(size_t i=0; i<mVectorS.size(); ++i)
            {
                mVectorS[i]->simulate(mTime, mTime+mTimeStep);
            }


            //! C Components !//

            mpBarrier_C->increment();
            while(mpBarrier_C->isLocked()){}                         //Wait at C barrier

            for(size_t i=0; i<mVectorC.size(); ++i)
            {
                mVectorC[i]->simulate(mTime, mTime+mTimeStep);
            }

            //! Log Nodes !//

            mpBarrier_N->increment();
            while(mpBarrier_N->isLocked()){}                         //Wait at N barrier

            for(size_t i=0; i<mVectorN.size(); ++i)
            {
                mVectorN[i]->logData(mTime);
            }

            //! Q Components !//

            mpBarrier_Q->increment();
            while(mpBarrier_Q->isLocked()){}                         //Wait at Q barrier

            for(size_t i=0; i<mVectorQ.size(); ++i)
            {
                mVectorQ[i]->simulate(mTime, mTime+mTimeStep);
            }




            mTime += mTimeStep;
        }
    }
private:
    vector<Component*> mVectorS;
    vector<Component*> mVectorC;
    vector<Component*> mVectorQ;
    vector<Node*> mVectorN;
    double mStopTime;
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
    taskSimMaster(vector<Component*> sVector, vector<Component*> cVector, vector<Component*> qVector, vector<Node*> nVector, double *pSimTime,
                  double startTime, double timeStep, double stopTime, size_t nThreads, size_t threadID,
                  BarrierLock *pBarrier_S, BarrierLock *pBarrier_C, BarrierLock *pBarrier_Q, BarrierLock *pBarrier_N)
    {
        mVectorS = sVector;
        mVectorC = cVector;
        mVectorQ = qVector;
        mVectorN = nVector;
        mpSimTime = pSimTime;
        mTime = startTime;
        mStopTime = stopTime;
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
        while(mTime < mStopTime)
        {

            //! Signal Components !//

            while(!mpBarrier_S->allArrived()) {}    //Wait for all other threads to arrive at signal barrier
            mpBarrier_C->lock();                    //Lock next barrier (must be done before unlocking this one, to prevnet deadlocks)
            mpBarrier_S->unlock();                  //Unlock signal barrier


            for(size_t i=0; i<mVectorS.size(); ++i)
            {
                mVectorS[i]->simulate(mTime, mTime+mTimeStep);
            }

            //! C Components !//

            while(!mpBarrier_C->allArrived()) {}    //C barrier
            mpBarrier_N->lock();
            mpBarrier_C->unlock();

            for(size_t i=0; i<mVectorC.size(); ++i)
            {
                mVectorC[i]->simulate(mTime, mTime+mTimeStep);
            }

            //! Log Nodes !//

            while(!mpBarrier_N->allArrived()) {}    //N barrier
            mpBarrier_Q->lock();
            mpBarrier_N->unlock();

            for(size_t i=0; i<mVectorN.size(); ++i)
            {
                mVectorN[i]->logData(mTime);
            }

            //! Q Components !//

            while(!mpBarrier_Q->allArrived()) {}    //Q barrier
            mpBarrier_S->lock();
            mpBarrier_Q->unlock();

            for(size_t i=0; i<mVectorQ.size(); ++i)
            {
                mVectorQ[i]->simulate(mTime, mTime+mTimeStep);
            }

            *mpSimTime = mTime;     //Update time in component system, so that progress bar can use it

            mTime += mTimeStep;
        }
    }
private:
    vector<Component*> mVectorS;
    vector<Component*> mVectorC;
    vector<Component*> mVectorQ;
    vector<Node*> mVectorN;
    double mStopTime;
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


//! @brief Simulate function for multi-threaded simulations.
//! @param startT Start time of simulation
//! @param stopT Stop time of simulation
//! @param nDesiredThreads Desired amount of simulation threads
void ComponentSystem::simulateMultiThreaded(const double startT, const double stopT, const size_t nDesiredThreads)
{
    mStop = false;
    mTime = startT;
    double stopTsafe = stopT - mTimestep/2.0;                   //Calculate the "actual" stop time
                                                                //Minus half a timestep is here to ensure that no numerical issues occur

    logAllNodes(mTime);                                         //Log the first time step

    size_t nThreads = getNumberOfThreads(nDesiredThreads);      //Calculate how many threads to actually use
    simulateAndMeasureTime(5);                                  //Measure time
    sortComponentVectorsByMeasuredTime();                       //Sort component vectors

    vector< vector<Component*> > splitCVector;                  //Create split vectors
    vector< vector<Component*> > splitQVector;
    vector< vector<Component*> > splitSignalVector;
    vector< vector<Node*> > splitNodeVector;

    distributeCcomponents(splitCVector, nThreads);              //Distribute Q-type components
    distributeQcomponents(splitQVector, nThreads);              //Distribute C-type components
    distributeSignalcomponents(splitSignalVector, nThreads);    //"Distribute" signal components
    distributeNodePointers(splitNodeVector, nThreads);          //Distribute node pointers

    tbb::task_group *simTasks;                                  //Initialize TBB routines for parallel  simulation
    simTasks = new tbb::task_group;

    BarrierLock *pBarrierLock_S = new BarrierLock(nThreads);    //Create synchronization barriers
    BarrierLock *pBarrierLock_C = new BarrierLock(nThreads);
    BarrierLock *pBarrierLock_Q = new BarrierLock(nThreads);
    BarrierLock *pBarrierLock_N = new BarrierLock(nThreads);

    //Execute simulation
    simTasks->run(taskSimMaster(splitSignalVector[0], splitCVector[0], splitQVector[0],             //Create master thread
                                splitNodeVector[0], &mTime, mTime, mTimestep, stopTsafe, nThreads, 0,
                                pBarrierLock_S, pBarrierLock_C, pBarrierLock_Q, pBarrierLock_N));

    for(size_t t=1; t < nThreads; ++t)
    {
        simTasks->run(taskSimSlave(splitSignalVector[t], splitCVector[t], splitQVector[t],          //Create slave threads
                                   splitNodeVector[t], mTime, mTimestep, stopTsafe, nThreads, t,
                                   pBarrierLock_S, pBarrierLock_C, pBarrierLock_Q, pBarrierLock_N));
    }
    simTasks->wait();                                           //Wait for all tasks to finish
    delete(simTasks);                                           //Delete the tasks
}


//! @brief Helper function that simulates all components and measure their average time requirements.
//! @param steps How many steps to simulate
void ComponentSystem::simulateAndMeasureTime(size_t steps)
{
        //Simulate S, C and Q components one time step on single core and meassure the required time
    double time = 0;

    //Reset all measured times first
    for(size_t s=0; s<mComponentSignalptrs.size(); ++s)
        mComponentSignalptrs[s]->setMeasuredTime(0);
    for(size_t c=0; c<mComponentCptrs.size(); ++c)
        mComponentCptrs[c]->setMeasuredTime(0);
    for(size_t q=0; q<mComponentQptrs.size(); ++q)
        mComponentQptrs[q]->setMeasuredTime(0);

    //Measure time for each component during specified amount of steps
    for(size_t t=0; t<steps; ++t)
    {
        for(size_t s=0; s<mComponentSignalptrs.size(); ++s)
        {
            tbb::tick_count comp_start = tbb::tick_count::now();
            mComponentSignalptrs[s]->simulate(mTime, mTime+mTimestep);
            tbb::tick_count comp_end = tbb::tick_count::now();
            time += double((comp_end-comp_start).seconds());
            mComponentSignalptrs[s]->setMeasuredTime(mComponentSignalptrs[s]->getMeasuredTime()+time);
        }


        for(size_t c=0; c<mComponentCptrs.size(); ++c)
        {
            tbb::tick_count comp_start = tbb::tick_count::now();
            mComponentCptrs[c]->simulate(mTime, mTime+mTimestep);
            tbb::tick_count comp_end = tbb::tick_count::now();
            time += double((comp_end-comp_start).seconds());
            mComponentCptrs[c]->setMeasuredTime(mComponentCptrs[c]->getMeasuredTime()+time);
        }

        for(size_t q=0; q<mComponentQptrs.size(); ++q)
        {
            tbb::tick_count comp_start = tbb::tick_count::now();
            mComponentQptrs[q]->simulate(mTime, mTime+mTimestep);
            tbb::tick_count comp_end = tbb::tick_count::now();
            time += double((comp_end-comp_start).seconds());
            mComponentQptrs[q]->setMeasuredTime(mComponentQptrs[q]->getMeasuredTime()+time);
        }

        logAllNodes(mTime);
        mTime += mTimestep;
    }

    //Divide measured times with number of steps, to get the average
    for(size_t s=0; s<mComponentSignalptrs.size(); ++s)
        mComponentSignalptrs[s]->setMeasuredTime(mComponentSignalptrs[s]->getMeasuredTime()/steps);
    for(size_t c=0; c<mComponentCptrs.size(); ++c)
        mComponentCptrs[c]->setMeasuredTime(mComponentCptrs[c]->getMeasuredTime()/steps);
    for(size_t q=0; q<mComponentQptrs.size(); ++q)
        mComponentQptrs[q]->setMeasuredTime(mComponentQptrs[q]->getMeasuredTime()/steps);
}


//! @brief Helper function that sorts C- and Q- component vectors by simulation time for each component.
//! @todo This function uses bubblesort. Maybe change to something faster.
void ComponentSystem::sortComponentVectorsByMeasuredTime()
{
        //Sort the components from longest to shortest time requirement
    size_t i, j;
    bool flag = true;
    Component *tempC;
    for(i = 1; (i < mComponentCptrs.size()) && flag; ++i)
    {
        flag = false;
        for (j=0; j < (mComponentCptrs.size()-1); ++j)
        {
            if (mComponentCptrs[j+1]->getMeasuredTime() > mComponentCptrs[j]->getMeasuredTime())
            {
                tempC = mComponentCptrs[j];             //Swap elements
                mComponentCptrs[j] = mComponentCptrs[j+1];
                mComponentCptrs[j+1] = tempC;
                flag = true;               //Indicates that a swap occurred
            }
        }
    }
    flag = true;
    Component *tempQ;
    for(i = 1; (i < mComponentQptrs.size()) && flag; ++i)
    {
        flag = false;
        for (j=0; j < (mComponentQptrs.size()-1); ++j)
        {
            if (mComponentQptrs[j+1]->getMeasuredTime() > mComponentQptrs[j]->getMeasuredTime())
            {
                tempQ = mComponentQptrs[j];             //Swap elements
                mComponentQptrs[j] = mComponentQptrs[j+1];
                mComponentQptrs[j+1] = tempQ;
                flag = true;               //Indicates that a swap occurred
            }
        }
    }
}


//! @brief Helper function that decides how many thread to use.
//! User specifies desired amount, but it is limited by how many cores the processor has.
//! @param nDesiredThreads How many threads the user wants
int ComponentSystem::getNumberOfThreads(size_t nDesiredThreads)
{
        //Obtain number of processor cores from environment variable, or use user specified value if not zero
    size_t nThreads;
    size_t nCores;
    if(getenv("NUMBER_OF_PROCESSORS") != 0)
    {
        string temp = getenv("NUMBER_OF_PROCESSORS");   //! @todo This appears to be a Windows only environment variable. Figure out how to do it on Unix (and Mac OS)
        nCores = atoi(temp.c_str());
    }
    else
    {
        nCores = 1;               //If non-Windows system, make sure there is at least one thread
    }
    if(nDesiredThreads != 0)
    {
        nThreads = nDesiredThreads;              //If user specifides a number of threads, attempt to use this number
        if(nThreads > nCores)       //But limit number of threads to the number of system cores
        {
            nThreads = nCores;
        }
    }
    else
    {
        nThreads = nCores;          //User specified nothing, so use one thread per core
    }

    return nThreads;
}


//! @brief Helper function that distributes C componente equally over one vector per thread
//! @param rSplitCVector Reference to vector with vectors of components (one vector per thread)
//! @param nThreads Number of simulation threads
void ComponentSystem::distributeCcomponents(vector< vector<Component*> > &rSplitCVector, size_t nThreads)
{
    vector<double> timeVector;
    timeVector.resize(nThreads);
    for(size_t i=0; i<nThreads; ++i)
    {
        timeVector[i] = 0;
    }

    rSplitCVector.resize(nThreads);
    size_t cCompNum=0;
    while(true)
    {
        for(size_t thread=0; thread<nThreads; ++thread)
        {
            if(cCompNum == mComponentCptrs.size())
                break;
            rSplitCVector[thread].push_back(mComponentCptrs[cCompNum]);
            timeVector[thread] += mComponentCptrs[cCompNum]->getMeasuredTime();
            ++cCompNum;
        }
        if(cCompNum == mComponentCptrs.size())
            break;
    }

    for(size_t i=0; i<nThreads; ++i)
    {
        stringstream ss;
        ss << timeVector[i]*1000;
        gCoreMessageHandler.addDebugMessage("Creating C-type thread vector, measured time = " + ss.str() + " ms", "cvector");
    }
}


//! @brief Helper function that distributes Q componente equally over one vector per thread
//! @param rSplitQVector Reference to vector with vectors of components (one vector per thread)
//! @param nThreads Number of simulation threads
void ComponentSystem::distributeQcomponents(vector< vector<Component*> > &rSplitQVector, size_t nThreads)
{
    vector<double> timeVector;
    timeVector.resize(nThreads);
    for(size_t i=0; i<nThreads; ++i)
    {
        timeVector[i] = 0;
    }

    rSplitQVector.resize(nThreads);
    size_t qCompNum=0;
    while(true)
    {
        for(size_t thread=0; thread<nThreads; ++thread)
        {
            if(qCompNum == mComponentQptrs.size())
                break;
            rSplitQVector[thread].push_back(mComponentQptrs[qCompNum]);
            timeVector[thread] += mComponentQptrs[qCompNum]->getMeasuredTime();
            ++qCompNum;
        }
        if(qCompNum == mComponentQptrs.size())
            break;
    }

    for(size_t i=0; i<nThreads; ++i)
    {
        stringstream ss;
        ss << timeVector[i]*1000;
        gCoreMessageHandler.addDebugMessage("Creating Q-type thread vector, measured time = " + ss.str() + " ms", "qvector");
    }
}


//! @brief Helper function that distributes signal components over one vector per thread.
//! @param rSplitSignalVector Reference to vector with vectors of components (one vector per thread)
//! @param nThreads Number of simulation threads
void ComponentSystem::distributeSignalcomponents(vector< vector<Component*> > &rSplitSignalVector, size_t nThreads)
{

        //First we want to divide the components into groups,
        //depending on who they are connected to.

    std::map<Component *, size_t> groupMap;     //Maps each component to a group number
    size_t curMax = 0;                          //Highest used group number

    //Loop through all signal components
    for(size_t s=0; s<mComponentSignalptrs.size(); ++s)
    {
        //Loop through all ports in each signal component
        for(size_t p=0; p<mComponentSignalptrs[s]->getPortPtrVector().size(); ++p)
        {
            //Loop through all connected ports to each port in each signal component
            for(size_t c=0; c<mComponentSignalptrs[s]->getPortPtrVector()[p]->getConnectedPorts().size(); ++c)
            {
                //Compare group number between current signal component and each connected component
                Component *A = mComponentSignalptrs[s];
                Component *B = mComponentSignalptrs[s]->getPortPtrVector()[p]->getConnectedPorts()[c]->getComponent();
                if(!groupMap.count(A) && !groupMap.count(B))        //Neither component has a number, so give current component a new number
                {
                    groupMap.insert(std::pair<Component *, size_t>(A, curMax));
                    ++curMax;
                }
                else if(!groupMap.count(A) && groupMap.count(B))    //Connected port has a number, so give current component same number
                {
                    groupMap.insert(std::pair<Component *, size_t>(A, groupMap.find(B)->second));
                }
                else if(groupMap.count(A) && groupMap.count(B))     //Both component have numbers, so merge current components group with the other one
                {
                    //Merge A's value with B's
                    size_t Aval = groupMap.find(A)->second;
                    size_t BVal = groupMap.find(B)->second;
                    std::map<Component *, size_t>::iterator it;
                    for(it=groupMap.begin(); it!=groupMap.end(); ++it)
                    {
                        if((*it).second == Aval)
                        {
                            (*it).second = BVal;
                        }
                    }
                }
            }
        }
    }


        //Now we assign each component to a simulation thread vector.
        //We keep grouped components together, and always fill thread
        //with least measured time first.

    rSplitSignalVector.resize(nThreads);
    size_t i=0;                                             //Group number
    size_t currentVector=0;                                 //The vector to which we are currently adding components
    size_t nAddedComponents=0;                              //Total amount of added components
    std::vector<double> vectorTime;                           //Contains total measured time in each vector
    for(size_t j=0; j<rSplitSignalVector.size(); ++j)
    {
        vectorTime.push_back(0.0);
    }

    while(nAddedComponents < groupMap.size())               //Loop while there are still components to add
    {
        std::map<Component *, size_t>::iterator it;
        for(it=groupMap.begin(); it!=groupMap.end(); ++it)
        {
            if((*it).second == i)                           //Add all components with group number i to current vector
            {
                rSplitSignalVector[currentVector].push_back((*it).first);
                vectorTime[currentVector] += (*it).first->getMeasuredTime();
                ++nAddedComponents;
            }
        }

        //Find vector with smallest amount of data, to use next loop
        for(size_t j=0; j<vectorTime.size(); ++j)
        {
            if(vectorTime[j] < vectorTime[currentVector])
                currentVector = j;
        }
        ++i;
    }

    // DEBUG
    for(size_t i=0; i<vectorTime.size(); ++i)
    {
        std::stringstream ss;
        ss << 1000*vectorTime[i];
        gCoreMessageHandler.addDebugMessage("Creating S-type thread vector, measured time = " + ss.str() + " ms", "svector");
    }
    // END DEBUG


        //Finally we sort each component vector, so that
        //signal components are simlated in correct order:

    for(size_t i=0; i<rSplitSignalVector.size(); ++i)
    {
        sortSignalComponentVector(rSplitSignalVector[i]);
    }
}


//! @brief Helper function that distributes node pointers equally over one vector per thread
//! @param rSplitNodeVector Reference to vector with vectors of node pointers (one vector per thread)
//! @param nThreads Number of simulation threads
void ComponentSystem::distributeNodePointers(vector< vector<Node*> > &rSplitNodeVector, size_t nThreads)
{
    for(size_t c=0; c<nThreads; ++c)
    {
        vector<Node*> tempVector;
        rSplitNodeVector.push_back(tempVector);
    }
    size_t thread = 0;
    for(size_t n=0; n<mSubNodePtrs.size(); ++n)
    {
        rSplitNodeVector.at(thread).push_back(mSubNodePtrs.at(n));
        ++thread;
        if(thread>nThreads-1)
            thread = 0;
    }
}


#else
    //This overrides the multi-threaded simulation call with a single-threaded simulation if TBB is not installed.
//! @brief Simulate function that overrides multi-threaded simulation call with a single-threaded call.
//! In case multi-threaded support is not available.
void ComponentSystem::simulateMultiThreaded(const double startT, const double stopT, const size_t /*nThreads*/)
{
    this->simulate(startT, stopT);
}
#endif


//! @brief Simulate function for single-threaded simulations.
//! @param startT Start time of simulation
//! @param stopT Stop time of simulation
void ComponentSystem::simulate(const double startT, const double stopT)
{
    mStop = false;

    mTime = startT;

    //Simulate
    double stopTsafe = stopT - mTimestep/2.0; //minus half a timestep is here to ensure that no numerical issues occure

    while ((mTime < stopTsafe) && (!mStop))
    {
        //! @todo maybe use iterators instead
        //Signal components
        for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
        {
            mComponentSignalptrs[s]->simulate(mTime, mTime+mTimestep);
        }

        //C components
        for (size_t c=0; c < mComponentCptrs.size(); ++c)
        {
            mComponentCptrs[c]->simulate(mTime, mTime+mTimestep);
        }
        logAllNodes(mTime); //MOVED HERE BECAUSE C-COMP ARE SETTING START TIMES

        //Q components
        for (size_t q=0; q < mComponentQptrs.size(); ++q)
        {
            mComponentQptrs[q]->simulate(mTime, mTime+mTimestep);
        }

        mTime += mTimestep;
    }
}


//! @brief Finalizes a system component and all its contained components after a simulation.
//! @param startT Start time of simulation
//! @param stopT Stop time of simulation
void ComponentSystem::finalize(const double startT, const double stopT)
{
    //! @todo take the final simulation step is suitable here

    //Finalize
    //Signal components
    for (size_t s=0; s < mComponentSignalptrs.size(); ++s)
    {
        if (mComponentSignalptrs[s]->isComponentSystem())
        {
            mComponentSignalptrs[s]->finalize(startT, stopT);
        }
        else
        {
            mComponentSignalptrs[s]->finalize();
        }
//        mComponentSignalptrs[s]->secretFinalize();
    }

    //C components
    for (size_t c=0; c < mComponentCptrs.size(); ++c)
    {
        if (mComponentCptrs[c]->isComponentSystem())
        {
            mComponentCptrs[c]->finalize(startT, stopT);
        }
        else
        {
            mComponentCptrs[c]->finalize();
        }
//        mComponentCptrs[c]->secretFinalize();
    }

    //Q components
    for (size_t q=0; q < mComponentQptrs.size(); ++q)
    {
        if (mComponentQptrs[q]->isComponentSystem())
        {
            mComponentQptrs[q]->finalize(startT,stopT);
        }
        else
        {
            mComponentQptrs[q]->finalize();
        }
//        mComponentQptrs[q]->secretFinalize();
    }

    //loadStartValuesFromSimulation();
}
