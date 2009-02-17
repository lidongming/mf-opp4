/*
 *  BMACLayer.cc
 *  BMAC for MF 2.02, omnetpp 3.4
 *
 *  Created by Anna Foerster on 10/10/08.
 *  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
 *
 *  Converted to OMNeT++ 4 by Rudolf Hornig
 */

#include "BMACLayer.h"

#include "NicControlType.h"
#include "FWMath.h"

Define_Module( BMACLayer )

#define myId (getParentModule()->getParentModule()->getId()-4)


/**
 * Initialize the of the omnetpp.ini variables in stage 1. In stage
 * two subscribe to the RadioState.
 */
void BMACLayer::initialize(int stage)
{
    BasicMacLayer::initialize(stage);

    if (stage == 0) {

        queueLength = par("queueLength");
		animation = par("animation");
        slotDuration = par("slotDuration");
        bitrate = par("bitrate");
		headerLength = par("headerLength");
		EV << "headerLength is: " << headerLength << endl;

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

		sendData = false;
    }

    else if(stage == 1) {
        int channel;
        myMacAddr = getParentModule()->getId();
        channel = par("defaultChannel");
        radio->setActiveChannel(channel);
        radio->setBitrate(bitrate);

        EV << "queueLength = " << queueLength
           << " slotDuration = " << slotDuration
           << " bitrate = " << bitrate << endl;

        //radio->switchToSleep();

		EV << "Scheduling the first wakeup at : " << slotDuration << endl;

		wakeup = new cMessage("wakeup");
		wakeup->setKind(BMAC_WAKEUP);
		scheduleAt(dblrand()*((double)slotDuration), wakeup);

		timeoutData = new cMessage("timeoutData");
		timeoutData->setKind(BMAC_TIMEOUT_DATA);

		sendPreambles = new cMessage("sendPreambles");
		sendPreambles->setKind(BMAC_SEND_PREAMBLES);

		checkChannel = new cMessage("checkChannel");
		checkChannel->setKind(BMAC_CHECK_CHANNEL);

		changeDisplayColor("black");

    }
}

void BMACLayer::finish() {
    MacQueue::iterator it;
    if (!wakeup->isScheduled()) delete wakeup;
	if (!checkChannel->isScheduled()) delete checkChannel;
	if (!timeoutData->isScheduled()) delete timeoutData;
	if (!sendPreambles->isScheduled()) delete sendPreambles;

    for(it = macQueue.begin(); it != macQueue.end(); ++it) {
        delete (*it);
    }
    macQueue.clear();
}

/**
 * Check whether the queue is not full: if yes, print a warning and drop the packet.
 * Then initiate sending of the packet, if the node is sleeping. Do nothing, if node is working.
 */
void BMACLayer::handleUpperMsg(cMessage *msg)
{
    BMACPkt *mac = static_cast<BMACPkt *>(encapsMsg(msg));

    // message has to be queued if another message is waiting to be send
    // or if we are already trying to send another message

    if (macQueue.size() <= queueLength) {
        macQueue.push_back(mac);
		EV << "packet putt in queue\n  queue size: " << macQueue.size() << " macState: " << macState << endl;
    }
    else {
        // queue is full, message has to be deleted
        EV << "New packet arrived, but queue is FULL, so new packet is deleted\n";
        mac->setName("MAC ERROR");
        mac->setKind(NicControlType::PACKET_DROPPED);
        sendControlUp(mac);
        droppedPacket.setReason(DroppedPacket::QUEUE);
        bb->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
		EV <<  "WARNING! Queue is full, forced to delete.\n";
    }
	// force wakeup now
	if (wakeup->isScheduled() && (macState == SLEEP))
	{
		cancelEvent(wakeup);
		scheduleAt(simTime() + dblrand()*0.1f, wakeup);
	}
}

/**
 * Send one short preamble packet immediately.
 */
void BMACLayer::sendPreamble()
{
	BMACPkt* preamble = new BMACPkt();
	preamble->setSrcAddr(myMacAddr);
	preamble->setDestAddr(L2BROADCAST);
	preamble->setKind(BMAC_CONTROL);
	preamble->setBitLength(headerLength);
	sendDown(preamble);
}

/**
 * Handle own messages:
 * BMAC_WAKEUP: wake up the node, check the channel for some time.
 * BMAC_CHECK_CHANNEL: if the channel is free, check whether there is something in the queue and switch teh radio to TX.
 * when switched to TX, (in receiveBBItem), the node will start sending preambles for a full slot duration.
 * if the channel is busy, stay awake to receive message. Schedule a timeout to handle false alarms.
 * BMAC_SEND_PREAMBLES: sending of preambles over. Next time the data packet will be send out (single one).
 * BMAC_TIMEOUT_DATA: timeout the node after a false busy channel alarm. Go back to sleep.
 */
void BMACLayer::handleSelfMsg(cMessage *msg)
{
	EV << "received self message\n";
	if(msg->getKind() == BMAC_WAKEUP)
	{
		EV << "Waking up. Let's see whether something is going on on the channel\n";
		macState = RX;
		radio->switchToRecv();

		// schedule a small check channel period
		scheduleAt(simTime() + 0.01f, checkChannel);
		changeDisplayColor("yellow");
	}
	if (msg->getKind() == BMAC_CHECK_CHANNEL)
	{
		EV << "Medium is free :" << (indication.getState() == MediumIndication::IDLE) << endl;
		if (indication.getState() == MediumIndication::IDLE)
		{
			EV << "Channel is free, let's see whether we have something waiting.\n";
			if (macQueue.size() > 0)
			{
				macState = TX;
				radio->switchToSend();
				changeDisplayColor("green");
				scheduleAt(simTime() + slotDuration, sendPreambles);
			}
			else
			{
				EV << "Channel is free and queue is empty. Go back to sleep and wake up again after a full slot\n";
				macState = SLEEP;
				radio->switchToSleep();
				changeDisplayColor("black");
				scheduleAt(simTime() + slotDuration, wakeup);
			}
		}
		// if the channel is busy, wait for incoming packets
		else
		{
			// schedule a timeout for receiving a big packet (256 bytes) and cancel it again when a new control packet arrives.
			scheduleAt(simTime() + 0.1f, timeoutData);
		}
	}
	if(msg->getKind() == BMAC_SEND_PREAMBLES)
	{
		EV << "Stop sending preambles. Next time send the data packet\n";
		sendData = true;
	}

	if(msg->getKind() == BMAC_TIMEOUT_DATA)
	{
		EV << "Nothing received for a long time. Go back to sleep and re-schedule the next wakeup\n";
		macState = SLEEP;
		radio->switchToSleep();
		changeDisplayColor("black");
		if (macQueue.size() > 0)
		{
			EV << "Something in the queue. Waking up soon again\n";
			scheduleAt(simTime() + dblrand()*0.1f, wakeup);
		}
		else
		{
			EV << "Nothing in the queue. Waking up after a full slot\n";
			scheduleAt(simTime() + slotDuration, wakeup);
		}
	}
}


/**
 * Handle BMAC preambles and received data packets.
 */
void BMACLayer::handleLowerMsg(cMessage *msg)
{
    BMACPkt *mac = static_cast<BMACPkt *>(msg);
	int dest = mac->getDestAddr();

	if (mac->getKind() == BMAC_CONTROL)
	{
		EV << " I have received a control packet from src " << mac->getSrcAddr() << " and dest " << dest << ".\n";

		// if we are listening to the channel and receive a preamble, wait for the data
		if (checkChannel->isScheduled())
		{
			EV << "Wait for data\n";
			cancelEvent(checkChannel);
			scheduleAt(simTime() + 0.1f, timeoutData);
		}
		// reschedule the timeout data
		else if (timeoutData->isScheduled())
		{
			cancelEvent(timeoutData);
			scheduleAt(simTime() + 0.1f, timeoutData);
		}
		delete mac;
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
		changeDisplayColor("black");
		if (timeoutData->isScheduled())
			cancelEvent(timeoutData);
		if (checkChannel->isScheduled())
			cancelEvent(checkChannel);
		//re-schedule the wakeup
		if (macQueue.size() > 0)
		{
			EV << "Something in the queue. Waking up soon again\n";
			scheduleAt(simTime() + dblrand()*0.1f, wakeup);
		}
		else
		{
			EV << "Nothing in the queue. Waking up after a full slot\n";
			scheduleAt(simTime() + slotDuration, wakeup);
		}
	}

}

/**
 * Handle transmission over messages: either send another preambles or the data packet itself.
 */
void BMACLayer::handleLowerControl(cMessage *msg)
{
	if(msg->getKind() == NicControlType::TRANSMISSION_OVER)
	{
		// if data is scheduled for transfer, don;t do anything.
		if (sendPreambles->isScheduled())
		{
			EV << "send another preamble." << endl;
			sendPreamble();
		}
		else if (sendData)
		{
			EV << " stop sending preambles. send data." << endl;
			BMACPkt* data = macQueue.front();
			sendDown(data);
			macQueue.pop_front();
			sendData = false;
		}
		else
		{
			EV << "Transmission of data over, go back to sleep.\n";
			macState = SLEEP;
			radio->switchToSleep();
			changeDisplayColor("black");
			if (macQueue.size() > 0)
			{
				EV << "Something in the queue. Waking up soon again\n";
				scheduleAt(simTime() + dblrand()*0.1f, wakeup);
			}
			else
			{
				EV << "Nothing in the queue. Waking up after a full slot\n";
				scheduleAt(simTime() + slotDuration, wakeup);
			}
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
void BMACLayer::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{
    Enter_Method_Silent();
    BasicMacLayer::receiveBBItem(category, details, scopeModuleId);

    if(category == catRadioState) {
        radioState = static_cast<const RadioState *>(details)->getState();
        if((macState == TX) && (radioState == RadioState::SEND)) {
            EV << " radio in SEND state" << endl;

			// send first a control message, so that non-receiving nodes can switch off.
			EV << "Sending the first preamble packet.\n";
			sendPreamble();
        }
    }
	else if (category == catIndication)
	{
		indication = static_cast<const MediumIndication *>(details)->getState();
		EV << "The new state of the medium is free: " << indication.getState() << endl;
	}
}

/**
 * Encapsulates the received network-layer packet into a MacPkt and set all needed
 * header fields.
 */
MacPkt * BMACLayer::encapsMsg(cMessage * msg)
{

    BMACPkt *pkt = new BMACPkt(msg->getName(), msg->getKind());
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
    pkt->encapsulate(check_and_cast<cPacket*>(msg));
    EV <<"pkt encapsulated\n";

    return pkt;

}

cMessage * BMACLayer::decapsMsg(MacPkt *msg) {
    cMessage *m = msg->decapsulate();
    m->setControlInfo(new MacControlInfo(msg->getSrcAddr()));
    // delete the macPkt
    delete msg;
    EV << " message decapsulated " << endl;
    return m;
}

/**
 * Change the color of the node for animation purposes.
 * black: SLEEP
 * yellow: RECV
 * green: TX
 */
void BMACLayer::changeDisplayColor(char *color)
{
	if (!animation)
		return;

	cDisplayString dispStr = getParentModule()->getParentModule()->getDisplayString();
	dispStr.setTagArg("i",0,"misc/node2_vs");
	dispStr.setTagArg("i", 1, color);
	dispStr.setTagArg("i", 2, "100");
	EV << "Changing my color\n";
	getParentModule()->getParentModule()->setDisplayString(dispStr.str());
}
