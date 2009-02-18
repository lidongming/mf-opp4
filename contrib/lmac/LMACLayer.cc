/*
 *  LMACLayer.cc
 *  LMAC for MF 2.02, omnetpp 4
 *
 *  Created by Anna Foerster on 10/10/08.
 *  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
 *
 *  Converted to OMNeT++ 4 by Rudolf Hornig
 */

#include "LMACLayer.h"

#include "NicControlType.h"
#include "FWMath.h"

Define_Module( LMACLayer )

#define myId (getParentModule()->getParentModule()->getId()-4)


const int LMACLayer::LMAC_NO_RECEIVER = -2;
/**
 * Initialize the of the omnetpp.ini variables in stage 1. In stage
 * two subscribe to the RadioState.
 */
void LMACLayer::initialize(int stage)
{
    BasicMacLayer::initialize(stage);

    if (stage == 0) {

        queueLength = par("queueLength");
        slotDuration = par("slotDuration");
        bitrate = par("bitrate");
		headerLength = par("headerLength");
		EV << "headerLength is: " << headerLength << endl;
        numSlots = par("numSlots");
		// the first N slots are reserved for mobile nodes to be able to function normally
		reservedMobileSlots = par("reservedMobileSlots");
		radioState = RadioState::SLEEP;
        droppedPacket.setReason(DroppedPacket::NONE);
        nicId = getParentModule()->getId();
        RadioState cs;
        MediumIndication mi;

		indication = MediumIndication::IDLE;
		catIndication = bb->subscribe(this, &mi, getParentModule()->getId());

        catRadioState = bb->subscribe(this, &cs, getParentModule()->getId());

        catDroppedPacket = bb->getCategory(&droppedPacket);

        // get handle to radio
        radio = SingleChannelRadioAccess().get();

        macState = SLEEP;

		slotChange = new cOutVector("slotChange");

		// how long does it take to send/receive a control packet
		controlDuration = ((double)headerLength + (double)numSlots + 16) / (double)bitrate;
		EV << "Controil packets take : " << controlDuration << " seconds to transmit\n";
    }

    else if(stage == 1) {
        int channel;
        myMacAddr = getParentModule()->getId();
        channel = hasPar("defaultChannel") ? par("defaultChannel") : 0;
        radio->setActiveChannel(channel);
        radio->setBitrate(bitrate);

        EV << "queueLength = " << queueLength
           << " slotDuration = " << slotDuration
		   << " controlDuration = " << controlDuration
		   << " numSlots = " << numSlots
           << " bitrate = " << bitrate << endl;

        //radio->switchToSleep();

		EV << "Scheduling the first wakeup at : " << slotDuration << endl;

		timeout = new cMessage("timeout");
		timeout->setKind(LMAC_TIMEOUT);

		sendData = new cMessage("sendData");
		sendData->setKind(LMAC_SEND_DATA);

		wakeup = new cMessage("wakeup");
		wakeup->setKind(LMAC_WAKEUP);

		initChecker = new cMessage("setup phase");
		initChecker->setKind(LMAC_SETUP_PHASE_END);

		checkChannel = new cMessage("checkchannel");
		checkChannel->setKind(LMAC_CHECK_CHANNEL);

		// the first 5 full slots we will be waking up every controlDuration to setup the network first
		// normal pavkets will be queued, but will be send only after the setup phase
		scheduleAt(slotDuration*5*numSlots, initChecker);

		scheduleAt(slotDuration, wakeup);

		SETUP_PHASE = true;

		for (int i = 0; i < numSlots; i++)
		{
			occSlotsDirect[i] = -1;
			occSlotsAway[i] = -1;
		}

		if (myId >= reservedMobileSlots)
			mySlot = ((int) getParentModule()->getParentModule()->getId() )% (numSlots - reservedMobileSlots);
		else
			mySlot = myId;
		//occSlotsDirect[mySlot] = myMacAddr;
		//occSlotsAway[mySlot] = myMacAddr;
		currSlot = 0;

		EV << "ID: " << getParentModule()->getParentModule()->getId() << ". Picked random slot: " << mySlot << endl;
    }
}


void LMACLayer::finish() {
    MacQueue::iterator it;
    if (!timeout->isScheduled()) delete timeout;
    if (!wakeup->isScheduled()) delete wakeup;
	if (!checkChannel->isScheduled()) delete checkChannel;

	if (!sendData->isScheduled()) delete sendData;
	if (!initChecker->isScheduled()) delete initChecker;



    for(it = macQueue.begin(); it != macQueue.end(); ++it) {
        delete (*it);
    }
    macQueue.clear();
}

/**
 * Check whether the queue is not full: if yes, print a warning and drop the packet.
 * Sending of messages is automatic.
 */
void LMACLayer::handleUpperMsg(cMessage *msg)
{
    LMACPkt *mac = static_cast<LMACPkt *>(encapsMsg(msg));

    // message has to be queued if another message is waiting to be send
    // or if we are already trying to send another message

    if (macQueue.size() <= queueLength) {
        macQueue.push_back(mac);
	EV << "packet putt in queue\n  queue size: " << macQueue.size() << " macState: " << macState
	    << "; mySlot is " << mySlot << "; current slot is " << currSlot << endl;;

    }
    else {
        // queue is full, message has to be deleted
        EV << "New packet arrived, but queue is FULL, so new packet is deleted\n";
        mac->setName("MAC ERROR");
        mac->setKind(NicControlType::PACKET_DROPPED);
        sendControlUp(mac);
        droppedPacket.setReason(DroppedPacket::QUEUE);
        bb->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
		EV <<  "ERROR: Queue is full, forced to delete.\n";
    }
}

/**
 * Handle self messages:
 * LMAC_SETUP_PHASE_END: end of setup phase. Change slot duration to normal and start sending data packets. The slots of the nodes should be stable now.
 * LMAC_SEND_DATA: send the data packet.
 * LMAC_CHECK_CHANNEL: check the channel in own slot. If busy, change the slot. If not, send a control packet.
 * LMAC_WAKEUP: wake up the node and either check the channel before sending a control packet or wait for control packets.
 * LMAC_TIMEOUT: go back to sleep after nothing happened.
 */
void LMACLayer::handleSelfMsg(cMessage *msg)
{
	EV << "received self message\n";
	if(msg->getKind() == LMAC_SETUP_PHASE_END)
	{
		EV << "Setup phase end. Start normal work at the next slot.\n";
		if (wakeup->isScheduled())
			cancelEvent(wakeup);

		scheduleAt(simTime()+slotDuration, wakeup);

		SETUP_PHASE = false;
	}

	if(msg->getKind() == LMAC_SEND_DATA)
	{
		// we should be in our own slot and the control packet should be already sent. receiving neighbors should wait for the data now.
		if (currSlot != mySlot)
		{
			EV << "ERROR: Send data message received, but we are not in our slot!!! Repair.\n";
			radio->switchToSleep();
			if (timeout->isScheduled())
				cancelEvent(timeout);
			return;
		}
		LMACPkt* data = macQueue.front();
		data->setMySlot(mySlot);
		data->setOccupiedSlotsArraySize(numSlots);
		for (int i = 0; i < numSlots; i++)
			data->setOccupiedSlots(i, occSlotsDirect[i]);

		sendDown(data);
		macQueue.pop_front();
	}

	if(msg->getKind() == LMAC_CHECK_CHANNEL)
	{
		// if the channel is clear, get ready for sending the control packet
		EV << "Medium is free :" << (indication.getState() == MediumIndication::IDLE) << endl;
		if (indication.getState() == MediumIndication::IDLE)
		{
			EV << "Channel is free, so let's prepare for sending.\n";
			macState = TX;
			radio->switchToSend();
		}
		// there is a collision! somebody is sending in MY slot!
		else
		{
			EV << "Somebody is using my slot, try to find another one.\n";
			findNewSlot();
			// remain in recv in this slot.
			if (!timeout->isScheduled())
				scheduleAt(simTime()+controlDuration, timeout);
		}
	}

	if(msg->getKind() == LMAC_WAKEUP)
	{
		currSlot++;
		currSlot %= numSlots;
		EV << "New slot starting - No. " << currSlot << ", my slot is " << mySlot << endl;
		// check whether I am not sending something still..
		if (macState == TX)
			error("I am still sending a message, while a new slot is starting!\n");

		if (mySlot == currSlot)
		{
			EV << "Waking up in my slot. Switch to RECV first to check the channel.\n";
			radio->switchToRecv();
			macState = RX;
			double small_delay = controlDuration*dblrand();
			scheduleAt(simTime()+small_delay, checkChannel);
			EV << "Checking for channel for " << small_delay << " time.\n";
		}
		else
		{
			macState = RX;
			radio->switchToRecv();
			if (!SETUP_PHASE)
				scheduleAt(simTime()+controlDuration, timeout);
			EV << "Waking up in a foreigh slot. Ready to receive control packet.\n";
		}
		if (SETUP_PHASE)
			scheduleAt(simTime()+2.f*controlDuration, wakeup);
		else
			scheduleAt(simTime()+slotDuration, wakeup);
	}

	if(msg->getKind() == LMAC_TIMEOUT)
	{
		EV << "Control timeout.\n";
		if (macState != RX)
			error("I am in the wrong state! Current state is : %d ", macState);

		EV << "Medium is free :" << (indication.getState() == MediumIndication::IDLE) << endl;
		if (indication.getState() == MediumIndication::IDLE)
		{
			EV << "Nothing happens. Go back to sleep.\n";
			macState = SLEEP;
			radio->switchToSleep();
		}
		// something is coming, let's wait for another 0.2f*controlDuration
		else
		{
			EV << "Something is coming in. Let's wait a little bit longer.\n";
			scheduleAt(simTime()+controlDuration, timeout);
		}


	}
}

/**
 * Handle LMAC control packets and data packets. Recognize collisions, change own slot if necessary and remember who is using which slot.
 */
void LMACLayer::handleLowerMsg(cMessage *msg)
{
    LMACPkt *mac = static_cast<LMACPkt *>(msg);
	int dest = mac->getDestAddr();
	// if we are listening to the channel and receive anything, there is a collision in the slot.
	bool collision = false;
	if (checkChannel->isScheduled())
	{
		cancelEvent(checkChannel);
		collision = true;
	}

	if (mac->getKind() == LMAC_CONTROL)
	{
		EV << " I have received a control packet from src " << mac->getSrcAddr() << " and dest " << dest << ".\n";

		// check first the slot assignment
		// copy the current slot assignment

		for (int s = 0; s < numSlots; s++)
		{
			occSlotsAway[s] = mac->getOccupiedSlots(s);
			EV << "Occupied slot " << s << ": " << occSlotsAway[s] << endl;
			EV << "Occupied direct slot " << s << ": " << occSlotsDirect[s] << endl;
		}

		if (mac->getMySlot() >-1)
		{
			// check first whether this address didn't have another occupied slot and free it again
			for (int i=0; i < numSlots; i++)
			{
				if (occSlotsDirect[i] == mac->getSrcAddr())
					occSlotsDirect[i] = -1;
				if (occSlotsAway[i] == mac->getSrcAddr())
					occSlotsAway[i] = -1;
			}
			occSlotsAway[mac->getMySlot()] = mac->getSrcAddr();
			occSlotsDirect[mac->getMySlot()] = mac->getSrcAddr();
		}

		collision = collision || (mac->getMySlot() == mySlot);
		if (((mySlot > -1) && (mac->getOccupiedSlots(mySlot) > -1) && (mac->getOccupiedSlots(mySlot) != myMacAddr)) || collision)
		{
			EV << "My slot is taken by " << mac->getOccupiedSlots(mySlot) << ". I need to change it.\n";
			findNewSlot();
			EV << "My new slot is " << mySlot << endl;
		}
		if (mySlot < 0)
		{
			EV << "I don;t have a slot - try to find one.\n";
			findNewSlot();
		}

		if(dest == myMacAddr || dest == L2BROADCAST)
		{
			EV << "I need to stay awake.\n";
			if (timeout->isScheduled())
				cancelEvent(timeout);
		}
		else
		{
			EV << "Incoming data packet not for me. Going back to sleep.\n";
			macState = SLEEP;
			radio->switchToSleep();
			if (timeout->isScheduled())
				cancelEvent(timeout);
		}
		delete mac;
		return;
	}
	else
	{
		EV << " I have received a data packet.\n";
		if(dest == myMacAddr || dest == L2BROADCAST)
		{
			EV << "sending pkt to upper...\n";
			sendUp(decapsMsg(mac));
		}
		else {
			EV << "packet not for me, deleting...\n";
			delete mac;
		}
		// in any case, go back to sleep
		macState = SLEEP;
		radio->switchToSleep();
		if (timeout->isScheduled())
			cancelEvent(timeout);
	}

}

/**
 * Handle transmission over messages: send the data packet or don;t do anyhting.
 */
void LMACLayer::handleLowerControl(cMessage *msg)
{
	if(msg->getKind() == NicControlType::TRANSMISSION_OVER)
	{
		// if data is scheduled for transfer, don;t do anything.
		if (sendData->isScheduled())
		{
			EV << " transmission of control packet over. data transfer will start soon." << endl;
			delete msg;
			return;
		}
		else
		{
			EV << " transmission over. nothing else is scheduled, get back to sleep." << endl;
			macState = SLEEP;
			radio->switchToSleep();
			if (timeout->isScheduled())
				cancelEvent(timeout);
		}
    }
    else {
        EV << "control message with wrong kind -- deleting\n";
    }
    delete msg;

}


/**
 * Update the internal copies of interesting BB variables
 *
 */
void LMACLayer::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{
    Enter_Method_Silent();
    BasicMacLayer::receiveBBItem(category, details, scopeModuleId);

    if(category == catRadioState) {
        radioState = static_cast<const RadioState *>(details)->getState();
        if((macState == TX) && (radioState == RadioState::SEND)) {
            EV << " radio in SEND state" << endl;

			// send first a control message, so that non-receiving nodes can switch off.
			EV << "Sending a control packet.\n";
			LMACPkt* control = new LMACPkt();
			control->setKind(LMAC_CONTROL);
			if ((macQueue.size() > 0) && !SETUP_PHASE)
				control->setDestAddr((macQueue.front())->getDestAddr());
			else
				control->setDestAddr(LMAC_NO_RECEIVER);

			control->setSrcAddr(myMacAddr);
			control->setMySlot(mySlot);
			control->setBitLength(headerLength + numSlots);
			control->setOccupiedSlotsArraySize(numSlots);
			for (int i = 0; i < numSlots; i++)
				control->setOccupiedSlots(i, occSlotsDirect[i]);

			sendDown(control);
			if ((macQueue.size() > 0) && (!SETUP_PHASE))
				scheduleAt(simTime()+controlDuration, sendData);
        }
    }
	else if (category == catIndication)
	{
		indication = static_cast<const MediumIndication *>(details)->getState();
		EV << "The new state of the medium is free: " << indication.getState() << endl;
		if(indication.getState() == MediumIndication::BUSY) {
			if ((checkChannel->isScheduled()) || (currSlot == mySlot))
			{
				EV << "Somebody is sending in my slot. Handle collision\n";
				if (checkChannel->isScheduled())
					cancelEvent(checkChannel);
				if (!timeout->isScheduled())
					scheduleAt(simTime()+controlDuration, timeout);
				//radio->switchToRecv();
				//findNewSlot();
			}

		}
	}
}

/**
 * Try to find a new slot after collision. If not possible, set own slot to -1 (not able to send anything)
 */
void LMACLayer::findNewSlot()
{
	// pick a random slot at the beginning and schedule the next wakeup
	// free the old one first
	int counter = 0;

	mySlot = intrand((numSlots - reservedMobileSlots));
	while ((occSlotsAway[mySlot] != -1) && (counter < (numSlots - reservedMobileSlots)))
	{
		counter++;
		mySlot--;
		if (mySlot < 0)
			mySlot = (numSlots - reservedMobileSlots)-1;
	}
	if (occSlotsAway[mySlot] != -1)
	{
		EV << "ERROR: I cannot find a free slot. Cannot send data.\n";
		mySlot = -1;
	}
	else
	{
		EV << "ERROR: My new slot is : " << mySlot << endl;
	}
	EV << "ERROR: I needed to find new slot\n";
	slotChange->recordWithTimestamp(simTime(), getParentModule()->getParentModule()->getId()-4);
}

/**
 * Encapsulates the received network-layer packet into a MacPkt and set all needed
 * header fields.
 */
MacPkt *LMACLayer::encapsMsg(cMessage * msg)
{

    LMACPkt *pkt = new LMACPkt(msg->getName(), msg->getKind());
    pkt->setBitLength(headerLength);

    // copy dest address from the Control Info attached to the network
    // mesage by the network layer
    MacControlInfo* cInfo = static_cast<MacControlInfo*>(msg->removeControlInfo());

    EV <<"CInfo removed, mac addr="<< cInfo->getNextHopMac()<<endl;
    pkt->setDestAddr(cInfo->getNextHopMac());

    //delete the control info
    delete cInfo;

    //set the src address to own mac address (nic module getId())
    pkt->setSrcAddr(myMacAddr);

    //encapsulate the network packet
    pkt->encapsulate(check_and_cast<cPacket *>(msg));
    EV <<"pkt encapsulated\n";

    return pkt;

}

cMessage *LMACLayer::decapsMsg(MacPkt *msg) {
    cMessage *m = msg->decapsulate();
    m->setControlInfo(new MacControlInfo(msg->getSrcAddr()));
    // delete the macPkt
    delete msg;
    EV << " message decapsulated " << endl;
    return m;
}

