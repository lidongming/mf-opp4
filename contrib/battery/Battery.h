/*
 *  Battery.h
 *
 *  Created by Anna Foerster on 10/31/06.
 *  Copyright 2006-2009 Universita della Svizzera Italiana. All rights reserved.
 *
 *  Modifications:
 *  2009-02-11 Rudolf Hornig: ported to OMNeT++ 4 and refactored
 *
 */

#ifndef _BATTERY_
#define _BATTERY_

#include <BasicModule.h>
#include <vector>
#include <RadioState.h>
#include <Blackboard.h>
#include "BatteryState.h"

/** @brief A simple linear battery module.
*
* Tracks the RadioState of the getNode(RECV and TX) and the simTime and subtracts the used value every time the
* state has changed or every N seconds ( the updateTimer parameter of the module).
* The stats are written to a folder, which is a parameter of the host itself. Inside this folder create a "battery" folder,
* and each of the nodes will write its output to a separate file.
*/
class Battery : public BasicModule
{
	protected:
		// parameters
		double batteryRECV;
		double batteryTX;
		double batterySLEEP;
		double maxBattery;

		// statistics
		cOutVector batteryStateVector;

		// state
		cMessage* updateStateMsg;
		int updateInterval;

		BatteryState batteryState;
		int batteryStateCategory;
		int radioStateCategory;  // category number given by bb for RadioState
		RadioState::States radioState;  //  Tracking the RadioState

		simtime_t lastTimestamp;

		simtime_t timeInSleep;
		simtime_t timeInRecv;
		simtime_t timeInTx;

	protected:
		/** @brief Some initialization stuff */
		virtual void initialize(int);

		/** @brief some finishing stuff - close the stats, etc. */
		virtual void finish();

		/** @brief handle all messages for this module - self message only, for updating the batteryState. */
		virtual void handleMessage(cMessage *msg);

		/** @brief Called by the Blackboard whenever a change occurs we're interested in */
		virtual void receiveBBItem(int category, const BBItem *details, int scopeModuleId);

		// helper function
		void doUpdateBatteryState(simtime_t& timeInState, double currentDrawn);

		/** @brief updates the timeInSleep/timeInRecv/timeInTx variable and the battery state since the last update */
		void updateBatteryState(RadioState::States radioState);
};

#endif
