/* -*- mode:c++ -*- ********************************************************
 * file:        RadioAccNoise3State.h
 *
 * author:      Jérôme Rousselot
 *		Andreas Koepke
 *
 * copyright:   (C) 2007-2008 CSEM SA, Neuchatel, Switzerland.
 *		(C) 2004 Telecommunication Networks Group (TKN) at
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
 * part of:     Modifications to the MF Framework by CSEM
 **************************************************************************/

#ifndef RADIOACCNOISE3STATE_H
#define RADIOACCNOISE3STATE_H

#include <omnetpp.h>
#include <Blackboard.h>
#include <sstream>

/**
 * @brief Class to hold the channel state of a certain radio of a nic
 *
 *
 * @ingroup utils
 * @ingroup blackboard
 * @author Andreas K�pke
 * @sa Blackboard
 */
class RadioAccNoise3State:public BBItem
{
  BBITEM_METAINFO(BBItem);

public:
    /** @brief possible states of this radio*/
  enum States
  {
    NONE = 0,
    SLEEP,			// avoid counting from zero
    SETUP_RX,
    RX,
    SETUP_TX,
    TX,
    SWITCH_RX_TX,
    SWITCH_TX_RX,
  };


protected:
    /** @brief holds the state for this channel */
   States state;
  simtime_t since;

public:

    /** @brief function to get the state*/
   States getState() const
  {
    return state;
  }
    /** @brief function to get the time at which the current state was entered */
  simtime_t getEntranceTime() const
  {
    return since;
  }
    /** @brief set the state of the radio*/
  void setState(States s, simtime_t now)
  {
    state = s;
    since = now;
  }

    /** @brief Constructor*/
RadioAccNoise3State(States s = SLEEP):BBItem(), state(s) {
  };

    /** @brief Enables inspection */
  std::string info()const
  {
    std::ostringstream ost;
    ost << " RadioAccNoise3State is ";
    switch (state) {
      case SLEEP:
	ost << "SLEEP";
	break;
	case SETUP_RX:ost << "SETUP RX";
	break;
	case SETUP_TX:ost << "SETUP TX";
	break;
	case RX:ost << "RX";
	break;
	case TX:ost << "TX";
	break;
	case SWITCH_RX_TX:ost << "SWITCH_RX_TX";
	break;
	case SWITCH_TX_RX:ost << "SWITCH_TX_RX";
	break;
	default:ost << "Error! Radio is in an unknown state:" << state;
	break;
    }
    return ost.str();
  }
};

#endif
