/*
 *  BMACLayer.cc
 *  BMAC for MF 2.02, omnetpp 3.4
 *
 *  Created by Anna Förster on 10/10/08.
 *  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
 *
 */

#ifndef BMAC_LAYER_H
#define BMAC_LAYER_H

#include <list>
#include <ActiveChannel.h>
#include <RadioState.h>
#include <RSSI.h>
#include <MediumIndication.h>
#include <Bitrate.h>
#include <DroppedPacket.h>
#include <BasicMacLayer.h>
#include <Blackboard.h>
#include "BMACPkt_m.h"
#include <SingleChannelRadio.h>
#include <SimpleAddress.h>
#include <MacControlInfo.h>

class  BMACLayer : public BasicMacLayer
{
  public:


    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);

    /** @brief Delete all dynamically allocated objects of the module*/
    virtual void finish();

    /** @brief Handle messages from lower layer */
    virtual void handleLowerMsg(cMessage*);

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage*);

    /** @brief Handle self messages such as timers */
    virtual void handleSelfMsg(cMessage*);

    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage *msg);

    /** @brief Called by the Blackboard whenever a change occurs we're interested in */
    virtual void receiveBBItem(int category, const BBItem *details, int scopeModuleId);
	
	/** @brief decapsulate the network message from the MacPkt */
    virtual cMessage* decapsMsg(MacPkt*);

    /** @brief Encapsulate the NetwPkt into an MacPkt */
    virtual MacPkt* encapsMsg(cMessage*);
	

  protected:
    typedef std::list<BMACPkt*> MacQueue;
    
    /** @brief MAC states
     *
     *  The MAC states help to keep track what the MAC is actually
     *  trying to do -- this is esp. useful when radio switching takes
     *  some time.
	 *  SLEEP -- node sleeps, but accepts packets from the network layer
     *  RX  -- MAC accepts packets from PHY layer
     *  TX  -- MAC transmits a packet
     *  CCA -- Clear Channel Assessment - MAC checks
     *         whether medium is busy
     */
    
    enum States {
		SLEEP,
        RX, 
        CCA,
        TX,
    };
	
	enum TYPES {
		BMAC_CONTROL = 191,
		BMAC_WAKEUP = 192,
		BMAC_SEND_DATA = 193,
		BMAC_CHECK_CHANNEL = 194,
		BMAC_TIMEOUT_DATA = 195,
		BMAC_SEND_PREAMBLES = 196
	};


    /** @brief keep track of MAC state */
    States macState;
    
    /** @brief Current state of active channel (radio), set using radio, updated via BB */
    RadioState::States radioState;
	
    /** @brief category number given by bb for RadioState */
    int catRadioState;
	
	/** @brief track and publish current occupation state of medium */
    MediumIndication indication;
    int catIndication;

    /** @brief Duration of a slot
     *
     *  The duty cycle of the node. Every slotDuration it wakes up and checks what's up.
     */
    double slotDuration;

    /** @brief cached pointer to radio module */
    SingleChannelRadio* radio;
    
    /** @brief A queue to store packets from upper layer in case another
    packet is still waiting for transmission..*/
    MacQueue macQueue;
    
    /** @brief length of the queue*/
    unsigned queueLength;
    
    /** @brief wake up and listen for control packets */
	cMessage* wakeup;		 
	/** @brief after wake up, check the channel for some time*/
	cMessage* checkChannel;	 
	/** @brief stop sending control messages, send one data packet*/
	bool sendData;		
	/** @brief timeout when false positive*/
	cMessage* timeoutData;	 
	/** @brief sending currently preambles*/
	cMessage* sendPreambles;
    
    /** @brief the bit rate at which we transmit */
    double bitrate;

    /** @brief Inspect reasons for dropped packets */
    DroppedPacket droppedPacket;
    
    /** @brief plus category from BB */
    int catDroppedPacket;
    
    /** @brief publish dropped packets nic wide */
    int nicId;
	
	/** @brief send a single preamble packet */
	void sendPreamble();
	
	/** @brief change the color of the node depending on its state*/
	void changeDisplayColor(char *color);
	
	/** @brief change the color of the node or not*/
	bool animation;

};

#endif

