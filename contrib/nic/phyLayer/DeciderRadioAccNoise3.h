/* -*- mode:c++ -*- *******************************************************
 * file:        DeciderRadioAccNoise3.h
 *
 * authors:     David Raguin / Marc Loebbers
 *              Amre El-Hoiydi, Jérôme Rousselot
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *              (C) 2007 CSEM
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


#ifndef  DECIDER_RADIOACCNOISE3_H
#define  DECIDER_RADIOACCNOISE3_H

#include <omnetpp.h>

#include <BasicLayer.h>
#include <AirFrameRadioAccNoise3_m.h>
#include "ConstsAccNoise3.h"
#include "RadioAccNoise3PhyControlInfo.h"
#include <math.h>
/**
 * @brief Decider for the Radio AccNoise3 Model
 *
 * TODO: update comment below.
 * Depending on the minimum of the snr included in the PhySDU this
 * module computes a bit error probability. The header (1 Mbit/s) is
 * always modulated with DBQPSK. The PDU is normally modulated either
 * with DBPSK (1 and 2 Mbit/s) or CCK (5.5 and 11 Mbit/s). CCK is not
 * easy to model, therefore it is modeled as DQPSK with a 16-QAM for
 * 5.5 Mbit/s and a 256-QAM for 11 Mbit/s.
 *
 *
 * @ingroup decider
 * @author Marc L�bbers, David Raguin, Amre El-Hoiydi, Jerome Rousselot
 */
class DeciderRadioAccNoise3:public BasicLayer
{


public:
    /** @brief Initialization of the module and some variables*/
  virtual void initialize(int);

  virtual void finish();
protected:
    /**
     * @brief Check whether we received the packet correctly.
     * Also appends the PhyControlInfo
     */
   virtual void handleLowerMsg(cMessage * msg);

    /**
     * @brief This function should not be called.
     */
  virtual void handleUpperMsg(cMessage * msg)
  {
    error("DeciderRadioAccNoise3 does not handle messages from upper layers");
  }

  virtual void handleSelfMsg(cMessage * msg)
  {
    error("DeciderRadioAccNoise3 does not handle selfmessages");
  }

    /** @brief Handle control messages from lower layer
     *  Just cut them through
     */
  virtual void handleLowerControl(cMessage * msg)
  {
    sendControlUp(msg);
  };

  cMessage *decapsMsg(AirFrameRadioAccNoise3 * frame);

    /** @brief converts a dB value into a normal fraction*/
  double dB2fraction(double);

    /** @brief computes if packet is ok or has errors*/
  bool packetOk(double, int, double bitrate);

#ifdef _WIN32
    /**
     * @brief Implementation of the error function
     *
     * Unfortunately the windows math library does not provide an
     * implementation of the error function, so we use an own
     * implementation (Thanks to Jirka Klaue)
     *
     * @author Jirka Klaue
     */
  double erfc(double);
#endif

protected:
    /** @brief should be set in the omnetpp.ini*/
  double bitrate;

	/** @brief Minimum bit error rate. If SNIR is high, computed ber could be
		higher than maximum radio performance. This value is an upper bound to
		the performance. */
  double BER_LOWER_BOUND;


};
#endif
