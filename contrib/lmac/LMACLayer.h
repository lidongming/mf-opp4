/*
 *  LMACLayer.cc
 *  LMAC for MF 2.02, omnetpp 3.4
 *
 *  Created by Anna Förster on 10/10/08.
 *  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
 *
 */

#ifndef LMAC_LAYER_H
#define LMAC_LAYER_H

#include <list>
#include <ActiveChannel.h>
#include <RadioState.h>
#include <RSSI.h>
#include <MediumIndication.h>
#include <Bitrate.h>
#include <DroppedPacket.h>
#include <BasicMacLayer.h>
#include <Blackboard.h>
#include "LMACPkt_m.h"
#include <SingleChannelRadio.h>
#include <SimpleAddress.h>
#include <MacControlInfo.h>

class  LMACLayer : public BasicMacLayer
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
    typedef std::list<LMACPkt*> MacQueue;
    
    /** @brief MAC states
     *
     *  The MAC states help to keep track what the MAC is actually
     *  trying to do -- this is esp. useful when radio switching takes
     *  some time.
	 *  SLEEP -- the node sleeps but accepts packets from the network layer
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
		LMAC_CONTROL=167,
		LMAC_TIMEOUT=168,
		LMAC_WAKEUP=169,
		LMAC_SEND_DATA=170,
		LMAC_SETUP_PHASE_END=171,
		LMAC_CHECK_CHANNEL=172,
		LMAC_SOMEBODY=173,
	};
	
	/** @brief dummy receiver address to indicate no pending packets in the control packet */
	static const int LMAC_NO_RECEIVER;
	
	/** @brief the setup phase is the beginning of the simulation, where only control packets at very small slot durations are exchanged. */
	bool SETUP_PHASE;
	
	/** @brief indicate how often the node needs to change its slot because of collisions */
	cOutVector* slotChange;
	
    /** @brief keep track of MAC state */
    States macState;
    
    /** @brief Current state of active channel (radio), set using radio, updated via BB */
    RadioState::States radioState;
	
    /** @brief category number given by bb for RadioState */
    int catRadioState;
	
	/** @brief track and publish current occupation state of medium */
    MediumIndication indication;
    int catIndication;


    /** @brief Duration of a slot */
    double slotDuration;
	/** @brief Duration of teh control time in each slot */
	double controlDuration;
	/** @brief my slot ID */
	int mySlot;
	/** @brief how many slots are there */
	int numSlots;
	/** @brief The current slot of the simulation */
	int currSlot;
	/** @brief Occupied slots from nodes, from which I hear directly */
	short occSlotsDirect[64];
	/** @brief Occupied slots of two-hop neighbors */
	short occSlotsAway[64];
	/** @brief The first couple of slots are reserved for nodes with special needs to avoid changing slots for them (mobile nodes) */
	int reservedMobileSlots;

    /** @brief cached pointer to radio module */
    SingleChannelRadio* radio;
    
    /** @brief A queue to store packets from upper layer in case another
    packet is still waiting for transmission..*/
    MacQueue macQueue;
    
    /** @brief length of the queue*/
    unsigned queueLength;
    
	cMessage* wakeup;
	cMessage* timeout;
	cMessage* sendData;
	cMessage* initChecker;
	cMessage* checkChannel;
    
    /** @brief the bit rate at which we transmit */
    double bitrate;

	/** @brief find a new slot */
	void findNewSlot();
	
    /** @brief Inspect reasons for dropped packets */
    DroppedPacket droppedPacket;
    
    /** @brief plus category from BB */
    int catDroppedPacket;
    
    /** @brief publish dropped packets nic wide */
    int nicId;
    

};

#endif

