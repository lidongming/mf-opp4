/* -*- mode:c++ -*- ********************************************************
 * file:        SnrEvalRadioAccNoise3.h
 *
 * author:      Marc Loebbers, Amre El-Hoiydi, Jérôme Rousselot
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
 * Change log:  Modified SnrEval80211 to match 802.15.4
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 ***************************************************************************/

#ifndef SNR_EVAL_RADIOACCNOISE3_H
#define SNR_EVAL_RADIOACCNOISE3_H

#include <SnrEval.h>
#include <sstream>

#include "ChannelAccess.h"
#include "NicControlType.h"
#include "ActiveChannel.h"
#include "Bitrate.h"
#include "AirFrameRadioAccNoise3_m.h"
#include "RSSI.h"
#include <ChannelControl.h>
#include "RadioAccNoise3State.h"

#include <cassert>

using namespace std;

/**
 * @brief A SnrEval for low power narrow band radios
 *
 * Subclass of BasicSnrEval. Basically the same as SnrEval.
 *  This module forms a physical layer together with
 * the DeciderRadioAccNoise3 module. The resluting physical
 * layer is intended to be used together with the MacRadioAccNoise3 module.
 *
 * @author Marc L�bbers
 *
 * @ingroup snrEval
 */
class SnrEvalRadioAccNoise3:public ChannelAccess
{
public:


    /** @brief Some extra parameters have to be read in */
  virtual void initialize(int);

  virtual void finish();

    /** @brief Called every time a message arrives -- we need to call a different encaps msg*/
  //void handleMessage( cMessage* );

    /** @brief Called by the Blackboard whenever a change occurs we're interested in */
  virtual void receiveBBItem(int category, const BBItem * details,
			     int scopeModuleId);

  enum States
  {
    MONITOR = 1,
    SYNC,
    DECODING
  };

  enum Events
  {
    NEW_FRAME = 1,
    END_FRAME,
    RADIO_ENTER_RX,
    RADIO_LEAVE_RX
  };

protected:

    enum t_accnoise3_frame_types {
        AIRFRAME
	};
		/** @brief Currently active channel, set using radio, updated via BB */
   ActiveChannel channel;
    /** @brief category number given by bb for ActiveChannel */
  int catActiveChannel;

  bool stats, trace;
  int nbCollisions;
		    /** brief a parameter that has to be read in from omnetpp.ini*/
  int headerLength;

  cOutVector frameDurations;

    /**
     * @brief power used to transmit messages
     *
     * The transmission power is initially set to the maximal
     * transmission power possible defined in the ChannelControl
     * module.
     *
     * It can be changed by the user but NEVER to a bigger value than
     * defined in ChannelControl.
     **/
  double transmitterPower;

    /**
     * @brief carrier frequency used to send messages
     *
     * The carrier frequency is initially set to the minimum carrier
     * frequency defined in the ChannelControl module.
     *
     * It can be changed by the user but NEVER to a smaller value than
     * defined in ChannelControl.
     **/
  double carrierFrequency;

    /**
     * @brief path loss coefficient
     *
     * The path loss coefficient is initially set to the minimum path
     * loss coefficient possible defined in the ChannelControl module.
     *
     * It can be changed by the user but NEVER to a smaller value than
     * defined in ChannelControl.
     **/
  double alpha;

    /** @brief Speed of light */
  static const double speedOfLight;


    /** @brief gate id*/
  /*@{ */
  int uppergateOut;
  int uppergateIn;
  int upperControlOut;
  /*@} */

    /** @brief Timer to indicate the end of a transmission to a higher layer */
  cMessage *txOverTimer;

  enum BasicSnrMsgKinds
  {
    RECEPTION_COMPLETE = 23001
  };

    /**
     * @name Convenience Functions
     * @brief Functions for convenience - NOT to be modified
     *
     * These are functions taking care of message encapsulation and
     * message sending. Normally you should not need to alter these but
     * should use them to handle message encasulation and sending. They
     * will wirte all necessary information into packet headers and add
     * or strip the appropriate headers for each layer.
     *
     */
  /*@{ */

    /** @brief Buffers message for 'transmission time'*/
  void bufferMsg(AirFrameRadioAccNoise3 * frame);

    /** @brief Unbuffers a message after 'transmission time'*/
  AirFrameRadioAccNoise3 *unbufferMsg(cMessage * msg);

    /** @brief Sends a message to the upper layer*/
  void sendUp(AirFrameRadioAccNoise3 *, const SnrList &);

    /** @brief Sends a message to the channel*/
  void sendDown(AirFrameRadioAccNoise3 * msg);

    /** @brief Sends a control message to the upper layer*/
  void sendControlUp(cMessage *);

  void handleSelfMsg(cMessage * msg);

  States state;

  AirFrameRadioAccNoise3 *encapsMsg(cMessage * msg);

    /** @brief Buffer the frame and update noise levels and snr information...*/
  virtual void handleLowerMsgStart(AirFrameRadioAccNoise3 *);

    /** @brief Unbuffer the frame and update noise levels and snr information*/
  virtual void handleLowerMsgEnd(AirFrameRadioAccNoise3 *);

  void handleMessage(cMessage * msg);
  void handleUpperMsg(AirFrameRadioAccNoise3 * frame);

  void execute(Events event, AirFrameRadioAccNoise3 * frame);
  bool syncOnSFD(AirFrameRadioAccNoise3 * frame);
  void enterState(States newState);

  double evalBER(AirFrameRadioAccNoise3 * frame);

    /** @brief Calculates the power with which a packet is received.
     * Overwrite this function to call all required loss elements, by
     * default only pathloss is calculated. Fading, shadowing, antenna
     * pattern are not evaluated in this class.
     */
  virtual double calcRcvdPower(AirFrameRadioAccNoise3 * frame);

    /** @brief Calculate the path loss.
     */
  double calcPathloss(AirFrameRadioAccNoise3 * frame);

    /** @brief updates the snr information of the relevant AirFrames
     *  called in handleLowerMsgStart(AirFrame*)
     */
  void addNewSnr();

  void addToNoiseLevel(AirFrameRadioAccNoise3 * frame);
  void removeFromNoiseLevel(AirFrameRadioAccNoise3 * frame);
  void fsmError(Events event);

  void handlePublish(RSSI * r);

    /** @brief updates the snr information of the relevant AirFrame
     *  called in handleLowerMsgEnd(AirFrame*)
     */
  void modifySnrList(SnrList & list);

    /** @brief Calculate duration of this message */
  virtual double calcDuration(cMessage * m)
  {
    assert(static_cast<cPacket*>(m));
    cPacket* pkt = static_cast<cPacket*>(m);
    return static_cast < double >(pkt->getBitLength()) / bitrate;
  }



protected:
    /**
     * @brief Struct to store a pointer to the mesage, rcvdPower AND a
     * SnrList, needed in SnrEval::addNewSnr
     **/
  struct SnrStruct
  {
	/** @brief Pointer to the message this information belongs to*/
    AirFrameRadioAccNoise3 *ptr;
	/** @brief Received power of the message*/
    double rcvdPower;
	/** @brief Snr list to store the SNR values*/
    SnrList sList;
  };

    /**
     * @brief SnrInfo stores the snrList and the the recvdPower for the
     * message currently beeing received together with a pointer to the
     * message.
     **/
  SnrStruct snrInfo;

    /**
     * @brief Typedef used to store received messages together with
     * receive power.
     **/
  typedef std::map < AirFrameRadioAccNoise3 *, double >cRecvBuff;

    /**
     * @brief A buffer to store a pointer to a message and the related
     * receive power.
     **/
  cRecvBuff recvBuff;

    /** @brief Current state of active channel (radio), set using radio, updated via BB */
  RadioAccNoise3State::States radioState;

    /** @brief category number given by bb for RadioState */
  int catRadioState;

    /** @brief Last RSSI level */
  RSSI rssi;

    /** @brief category number given by bb for RSSI */
  int catRSSI;

    /** @brief if false, the RSSI will not be published during the reception of a frame
     *
     *  Set it to false if you want a small speed up in the simulation.
     */
  bool publishRSSIAlways;

    /** @brief track and publish current occupation state of medium */
  MediumIndication indication;
  int catIndication;

    /** @brief Cache the module ID of the NIC */
  int nicModuleId;

    /** @brief The noise level of the channel.*/
  double noiseLevel;

    /** @brief Used to store the time a radio switched to recieve.*/
  simtime_t recvTime;

	/** @brief Start Frame Delimiter length in bits. */
  int sfdLength;

    /**
     * @brief The wavelength. Calculated from the carrier frequency specified in the omnetpp.ini
     * The carrier frequency used. Can be specified in the
     * omnetpp.ini file. If not present it is read from the ChannelControl
     * module.
     **/
  double waveLength;

    /**
     * @brief Thermal noise on the channel. Can be specified in
     * omnetpp.ini. Default: -100 dBm
     **/
  double thermalNoise;

    /**
     * @brief Path loss coefficient.
     *
     * Can be specified in omnetpp.ini. If not it is read from the
     * ChannelControl module. This value CANNOT be smaller than the
     * one specified in the ChannelControl module. The simulation will
     * exit with an error!  The stored value is smaller to enable
     * faster calculation using the square of the distance.
     *
     **/
  double pathLossAlphaHalf;


    /** @brief do we send on the surface of a torus */
  bool useTorus;

    /** @brief then we need to know the edges of the playground */
  Coord playground;

    /** @brief keep bitrate to calculate duration */
  double bitrate;

    /** @brief BB category of bitrate */
  int catBitrate;
};

#endif
