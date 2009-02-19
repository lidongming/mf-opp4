/* -*- mode:c++ -*- ********************************************************
 * file:        SimTracer.cc
 *
 * author:      Jérôme Rousselot
 *
 * copyright:   (C) 2007-2008 CSEM SA, Neuchatel, Switzerland
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     Modifications to the MF Framework by CSEM
 ***************************************************************************/


#include "SimTracer.h"


Define_Module(SimTracer);

/*
 * Open some log files and write some static initializations stuff.
 */
void SimTracer::initialize(int stage)
{
  BasicModule::initialize(stage);
  if (stage == 0) {

	char treeName[250];
	int n;
	n = sprintf(treeName, "results/tree-%d.txt", ev.getConfig()->getActiveRunNumber());
    treeFile.open(treeName);
    if (!treeFile) {
      EV << "Error opening output stream for routing tree statistics."
          << endl;
    } else {
      treeFile << "graph aRoutingTree " << endl << "{" << endl;
    }


  } else if (stage == 1) {
    // const char *channelModulePath = "sim.channelcontrol";
    // cModule *modp = simulation.getModuleByPath(channelModulePath);
    cModule *modp = this->getParentModule()->getSubmodule("channelcontrol");
    ChannelControl *channelctrl = check_and_cast < ChannelControl * >(modp);
    stringstream toLog;

  }
}

/*
 * Close the nam log file.
 */
void SimTracer::finish()
{
  ofstream summaryPowerFile;

  summaryPowerFile.open("summary_power_radio.csv");
  map < unsigned long, double >::const_iterator iter =
    powerConsumptions.begin();
  for (; iter != powerConsumptions.end(); iter++) {
    summaryPowerFile << iter->first << "\t" << iter->second << endl;
  }
  summaryPowerFile.close();

  if (treeFile) {
    treeFile << "}" << endl;
    treeFile.close();

  }

}

/*
 * Record a line into the nam log file.
 */
void SimTracer::namLog(string namString)
{
  //Enter_Method_Silent();
  //namFile << namString << endl;
}

void SimTracer::radioEnergyLog(unsigned long mac, int state,
			       simtime_t duration, double power)
{
  Enter_Method_Silent();
  /*
  radioEnergyFile << mac << "\t" << state << "\t" << duration << "\t" << power
    << endl;
    */
  if (powerConsumptions.count(mac) == 0) {
    powerConsumptions[mac] = 0;
  }
  powerConsumptions[mac] = powerConsumptions[mac] + power * duration.dbl();
}



void SimTracer::logLink(int parent, int child)
{
  treeFile << "   " << parent << " -- " << child << " ;" << endl;
}

void SimTracer::logPosition(int node, double x, double y)
{
	treeFile << node << "[pos=\""<< x << ", " << y << "!\"];" << endl;
}
