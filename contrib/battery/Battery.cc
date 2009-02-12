/*
 *  BatteryModule.cc
 *
 *  Created by Anna Foerster on 10/31/06.
 *  Copyright 2006-2009 Universita della Svizzera Italiana. All rights reserved.
 *
 *  Modifications:
 *  2009-02-11 Rudolf Hornig: ported to OMNeT++ 4 and refactored
 */


#include "Battery.h"

#define myId (getParentModule()->getId()-4)

Define_Module( Battery )

/**
 * Initialize the of the omnetpp.ini variables in stage 1. In stage
 * two subscribe to the RadioState.
 */
void Battery::initialize(int stage)
{
	BasicModule::initialize(stage);
	EV << "Stage is " << stage << endl;
	if (stage == 0)
	{
		batteryStateVector.setName("batteryState");

		maxBattery = par("maxBatteryCapacity");
        batteryTX = par("batteryTX");
        batteryRECV = par("batteryRECV");
		batterySLEEP = par("batterySLEEP");

		batteryState.setBatteryState(double(par("batteryCapacity")) / maxBattery);

		timeInSleep = 0;
		timeInTx = 0;
		timeInRecv = 0;

		updateInterval = par("updateInterval");
		if (updateInterval < 1)
			updateInterval = 1;

        radioState = RadioState::RECV;
		RadioState cs;
        radioStateCategory = -1;
        radioStateCategory = bb->subscribe(this, &cs, -1);
		EV << "Category is " << radioStateCategory << endl;
		lastTimestamp = 0;

		batteryStateCategory = bb->getCategory(&batteryState);

		updateStateMsg = NULL;
	}
	else if (stage == 1)
	{
		// publish the battery state
		bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

		// schedule the update timer
		updateStateMsg = new cMessage("BatteryUpdateTimer");
		scheduleAt(simTime() + updateInterval, updateStateMsg);
		EV << "Scheduling the timer.\n";
	}
}

/** @brief some finishing stuff - close the stats, etc. */
void Battery::finish()
{
	cancelAndDelete(updateStateMsg);
	//delete updateState;

	recordScalar("timeInSleep", timeInSleep);
	recordScalar("timeInTx", timeInTx);
	recordScalar("timeInRecv", timeInRecv);
}

/**
 * Update the internal copies of interesting BB variables
 *
 */
void Battery::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{
	Enter_Method_Silent();
	BasicModule::receiveBBItem(category, details, scopeModuleId);

	if (batteryState.getBatteryState() <= 0)
		return;

    if (category == radioStateCategory)
    {
        RadioState::States newRadioState = static_cast<const RadioState *>(details)->getState();
		EV << "New radio state. Current radio state: " << radioState << ", new state:" << newRadioState << endl;
		// if the last radioState was RECV and the new is TX, substract time*batteryRECV
		if (newRadioState == radioState)
		{
			EV << "No change in radio state. \n";
			return;
		}
		if ((newRadioState != RadioState::RECV) && (newRadioState != RadioState::SEND) && (newRadioState != RadioState::SLEEP))
		{
			EV << "New radio state different from RECV, SEND or SLEEP. Ignore and stay in the same state.\n";
			return;
		}

		updateBatteryState(radioState);

		radioState = newRadioState;

		bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

		batteryStateVector.record(batteryState.getBatteryState());

		EV << "New battery status: " << batteryState.getBatteryState() << endl;
		EV << "New radio state: " << radioState << endl;
    }
}

void Battery::handleMessage(cMessage *msg)
{
	// the module can receive only self messages
	if (msg != updateStateMsg)
		error("invalid message received.");

	// update the state
	updateBatteryState(radioState);

	// publish the new value.
	bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

	batteryStateVector.record(batteryState.getBatteryState());

	EV << "Current battery status: " << batteryState.getBatteryState() << endl;

	// reschedule or die.
	if (batteryState.getBatteryState() > 0)
	{
		EV << "Rescheduling the update timer.\n";
		scheduleAt(simTime() + updateInterval, updateStateMsg);
	}
	else
	{
		// TODO disable radio or take appropriate action
		EV << "Battery is empty. END. \n";
	}
}

void Battery::doUpdateBatteryState(simtime_t& timeInState, double currentDrawn)
{
	simtime_t deltaT = simTime() - lastTimestamp;
	timeInState += deltaT;
	batteryState.setBatteryState(batteryState.getBatteryState() - currentDrawn * SIMTIME_DBL(deltaT) / 3600 / maxBattery);
	lastTimestamp = simTime();
}

void Battery::updateBatteryState(RadioState::States radioState)
{
	if (radioState == RadioState::RECV)
		doUpdateBatteryState(timeInRecv, batteryRECV);
	else if (radioState == RadioState::SEND)
		doUpdateBatteryState(timeInTx, batteryTX);
	else if (radioState == RadioState::SLEEP)
		doUpdateBatteryState(timeInSleep, batterySLEEP);
}
