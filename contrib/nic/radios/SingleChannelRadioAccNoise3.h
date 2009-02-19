/* -*- mode:c++ -*- ********************************************************
 * file:        SingleChannelRadioAccNoise3.h
 *
 * author:      Andreas Koepke, Jérôme Rousselot, Amre El-Hoiyid
 *
 * copyright:   (C) 2005 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 * 				(C) 2007 CSEM
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
 ***************************************************************************/

#ifndef SINGLECHANNELRADIOACCNOISE3_H
#define SINGLECHANNELRADIOACCNOISE3_H

#include <omnetpp.h>
#include <BasicModule.h>
#include <ModuleAccess.h>
#include <ActiveChannel.h>
#include <map>
#include <sstream>
#include "RadioAccNoise3State.h"
#include "Bitrate.h"
#include "ConstsAccNoise3.h"
#include "SimTracer.h"
#include "BasicIPMacLayer.h"

/**
 * @brief A simple radio being able to send / transmit on one channel.
 * The interface with the mac layer is simple: the MAC can asks the radio
 * one of three commands (ENTER_SLEEP, ENTER_RX and ENTER_TX).
 * Internally, the radio has 7 states, and three of them are steady.
 * So the MAC layer only cares about steady states and the internal
 * transient states are used to model power consumption.
 *
 * @ingroup radios
 * @author Andreas Koepke, Jérôme Rousselot, Amre El-Hoiydi
 **/
class SingleChannelRadioAccNoise3:public BasicModule
{


public:

	/** @brief The list of commands that can be sent by the MAC Layer. */
  enum commands
  {
    ENTER_SLEEP = 1,
    ENTER_RX,
    ENTER_TX,
  };

	/** @brief Data structure associating a power consumption to each radio state. */
   std::map < RadioAccNoise3State::States, double >powerConsumptions;

    /** @brief Initialization of the module and some variables*/
  virtual void initialize(int);
	/** @brief  Called at the end of the simulation for clean up. */
  virtual void finish();
    /** @brief Only called by the timers when they trigger. */
  virtual void handleMessage(cMessage *);

    /**
     * @brief Method to control state of this radio (e.g. by the MAC layer)
     */

  virtual void execute(commands command);

  /* @brief Method to get the current state of the radio */
   RadioAccNoise3State::States getState();

    /** @brief change the active channel, never fails (returns true). */
  bool setActiveChannel(int c);

    /** @brief change the bit rate, never fails (returns true) */
  bool setBitrate(double b);

  ~SingleChannelRadioAccNoise3();

protected:
    /** @brief hold radio state */
   RadioAccNoise3State state;
    /** @brief channel radio category */
  int stateCat;

    /**@brief Parameter enabling logging of energy consumption */
  bool doLogEnergy;
    /** @brief holds active channel, default channel is set by MAC protocol */
  ActiveChannel aChannel;
    /** @brief active channel category */
  int aChannelCat;

  /** @brief Keeps track of this radio power consumption. */
  double energyUsed;

    /** @brief current bit rate for this radio, default is set by MAC */
  Bitrate bitrate;
    /** @brief current bit rate and the category number */
  int bitrateCat;

	/** @brief cached value of the NIC mac address. */
  unsigned long macaddress;
    /** variables that hold the time the radio needs
     *	to stay in the transient states
     */
  simtime_t delay_setup_rx;
  simtime_t delay_setup_tx;
  simtime_t delay_switch_rx_tx;
  simtime_t delay_switch_tx_rx;

	/** @brief Message sent to self to simulate a timer. Used to leave
	transient states. */
  cMessage *timer;


    /** id of the surrounding nic module */
  int nicModuleId;

	/** @brief Called by the Finite State Machine when an unexpected transition
	is encountered while processing a command. */
  void fsmError(commands command);
	/** @brief Called to enter a new state. */
  void enterState(RadioAccNoise3State::States);
	/** @brief Called by the Finite State Machine when an unexpected transition
	is encountered while processing transient states. */
  void fsmError(RadioAccNoise3State::States);
	/** @brief Generate trace data to monitor node energy consumption. */
  void logEnergy();
    /**
     * perform switch if possible
     */
  //bool switchTo(RadioAccNoise3State::States, simtime_t delta);

	/** @brief Cancels the timer if it is currently scheduled, and reschedule
	it as requested. */
  void startTimer(simtime_t delay);

    /** @brief Pointer to tracer module */
  SimTracer *tracer;
};

class SingleChannelRadioAccNoise3Access:public ModuleAccess <
  SingleChannelRadioAccNoise3 >
{
public:
  SingleChannelRadioAccNoise3Access():ModuleAccess <
    SingleChannelRadioAccNoise3 > ("radio")
  {
  }
};

#endif
