/* -*- mode:c++ -*- ********************************************************
 * file:        BasicModule.cc
 *
 * author:      Steffen Sroka
 *              Andreas Koepke
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 **************************************************************************/

#include "BasicModule.h"

/**
 * Subscription to Blackboard should be in stage==0, and firing
 * notifications in stage==1 or later.
 *
 * NOTE: You have to call this in the initialize() function of the
 * inherited class!
 */
void BasicModule::initialize(int stage)
{    
    if (stage == 0) {        
        hasPar("debug") ? debug = par("debug").boolValue() : debug = false;
        // get a pointer to the Blackboard module
        bb = BlackboardAccess().get();
    }
}

cModule *BasicModule::findHost(void) const 
{
    cModule *mod;
    for (mod = getParentModule(); mod != 0; mod = mod->getParentModule())
    {
        if (strstr(mod->getName(), "host") != NULL || strstr(mod->getName(), "Host") != NULL)
            break;
    }
    if (!mod)
        error("findHost: no host module found!");

    return mod;
}

/**
 * This function returns the logging name of the module with the
 * specified id. It can be used for logging messages to simplify
 * debugging in TKEnv.
 *
 * Only supports ids from simple module derived from the BasicModule
 * or the nic compound module id.
 *
 * @param id Id of the module for the desired logging name
 * @return logging name of module id or NULL if not found
 * @sa logName
 */
std::string BasicModule::getLogName(int id)
{
    BasicModule *mod;
    std::string lname;
    mod = check_and_cast<BasicModule *>(simulation.getModule(id));
    if (mod->isSimple()) {
        lname = mod->logName();
    }
    else if(mod->getSubmodule("snrEval")) {
        lname = check_and_cast<BasicModule *>(mod->getSubmodule("snrEval"))->logName();
    }
    else if(mod->getSubmodule("phy")) {
        lname = check_and_cast<BasicModule *>(mod->getSubmodule("phy"))->logName();
    }
    return lname;
};
