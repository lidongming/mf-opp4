/* -*- mode:c++ -*- ********************************************************
 * file:        csma.cc
 *
 * author:      Jerome Rousselot, Amre El-Hoiydi,
 * 		Marc Loebbers, Yosia Hadisusanto, Andreas Koepke
 *
 * copyright:   (C) 2007 CSEM SA
 *		(C) 2004,2005,2006
 *              Telecommunication Networks Group (TKN) at Technische
 *              Universitaet Berlin, Germany.
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
 ***************************************************************************/

#include "csma.h"
#include "NicControlType.h"
#include "FWMath.h"
#include <cassert>

Define_Module( csma );

/**
 * Initialize the of the omnetpp.ini variables in stage 1. In stage
 * two subscribe to the RadioState.
 */

void csma::initialize(int stage)
{
	BasicIPMacLayer::initialize(stage);

	if (stage == 0) {
		headerLength = par("headerLength");
		queueLength = par("queueLength");
		busyRSSI = par("busyRSSI");
		sifs = par("sifs");
		macMinBE = par("macMinBE");
		macMaxBE = par("macMaxBE");
		transmissionAttemptInterruptedByRx = false;
		BE = macMinBE;
		NB = 0;
		nbTxFrames = 0;
		nbRxFrames = 0;
		nbMissedAcks = 0;
		nbTxAcks = 0;
		nbRecvdAcks = 0;
		nbDroppedFrames = 0;
		nbDuplicates = 0;
		nbBackoffs = 0;
		backoffValues = 0;
		stats = par("stats");
		trace = par("trace");
		macMaxCSMABackoffs = par("macMaxCSMABackoffs");
		macMaxFrameRetries = par("macMaxFrameRetries");
		macAckWaitDuration = par("macAckWaitDuration").doubleValue();
		aUnitBackoffPeriod = par("aUnitBackoffPeriod");
		ccaDetectionTime = par("ccaDetectionTime").doubleValue();
		rxSetupTime = par("rxSetupTime").doubleValue();
		aTurnaroundTime = par("aTurnaroundTime").doubleValue();
		bitrate = par("bitrate");
		ackLength = par("ackLength");
		ackMessage = NULL;
		initialCW = hasPar("contentionWindow") ? par("contentionWindow") : 3;
		backoffWindow = initialCW;
		radioState = RadioAccNoise3State::SLEEP;
		rssi = 0;
		droppedPacket.setReason(DroppedPacket::NONE);
		nicId = getParentModule()->getId();
		RadioAccNoise3State cs;
		RSSI rs;
		const char *addressString = par("macaddress");
		const unsigned int ipvalue = par("ipaddress");
		setAddresses(addressString, ipvalue);
		EV << "Set mac address to " << macaddress.getAddress() << ", param was " << addressString << endl;
		EV << "Set IP address to " << ipaddress.str() << ", param was " << ipvalue << endl;
		catRadioState = bb->subscribe(this, &cs, getParentModule()->getId());
		catRSSI = bb->subscribe(this, &rs, getParentModule()->getId());

		catDroppedPacket = bb->getCategory(&droppedPacket);

		// get handle to radio
		radio = SingleChannelRadioAccNoise3Access().get();

		// initialize the timers
		backoffTimer = new cMessage("timer-backoff");
		ccaTimer = new cMessage("timer-cca");
		sifsTimer = new cMessage("timer-sifs");
		rxAckTimer = new cMessage("timer-rxAck");
		macState = IDLE_1;
		txAttempts = 0;

		// Choose one color for each Mac State. This will be used by nam visualisation.
		macStatesColors.push_back("black");  // unused state 0.
		macStatesColors.push_back("black");  // IDLE
		macStatesColors.push_back("blue");   // BACKOFF
		macStatesColors.push_back("violet"); // CCA
		macStatesColors.push_back("red");	// TRANSMITFRAME
		macStatesColors.push_back("yellow");	// WAITACK
		macStatesColors.push_back("VioletRed"); // WAITSIFS
		macStatesColors.push_back("orange");	// TRANSMITACK

	} else if(stage == 1) {
		int channel;
		channel = hasPar("defaultChannel") ? par("defaultChannel") : 0;
		radio->setActiveChannel(channel);
		radio->setBitrate(bitrate);
		radio->execute(SingleChannelRadioAccNoise3::ENTER_RX);
		EV << "queueLength = " << queueLength
		<< " busyRSSI = " << busyRSSI
		<< " bitrate = " << bitrate
		<< " contentionWindow = " << initialCW << endl;

		busyRSSI = FWMath::dBm2mW(busyRSSI);
		mobility = check_and_cast<BasicMobility*>(getParentModule()->getParentModule()->getSubmodule("mobility"));

		// Get a handle to the tracer module
		// const char *tracerModulePath = "sim.simTracer";
		// cModule *modp = simulation.getModuleByPath(tracerModulePath);
		cModule *modp = this->getParentModule()->getParentModule()->getParentModule()->getSubmodule("simTracer");
		tracer = check_and_cast<SimTracer *>(modp);

		namPrevColor = "black";
		toLog << "n -t * -s " << macaddress.getAddress() << " -x " << mobility->getX();
		toLog << " -y " << mobility->getY() << " -Z 0 -z 8 -v circle -c black";
	    // creates the node
		tracer->namLog(toLog.str());
		EV << "finished csma init stage 1." << endl;
	}
}


void csma::finish() {
	BasicIPMacLayer::finish();
	if (stats) {
		recordScalar("nbTxFrames", nbTxFrames);
		recordScalar("nbRxFrames", nbRxFrames);
		recordScalar("nbDroppedFrames", nbDroppedFrames);
		recordScalar("nbMissedAcks", nbMissedAcks);
		recordScalar("nbRecvdAcks", nbRecvdAcks);
		recordScalar("nbTxAcks", nbTxAcks);
		recordScalar("nbDuplicates", nbDuplicates);
		if(nbBackoffs > 0) {
			recordScalar("meanBackoff", backoffValues / nbBackoffs);
		} else {
			recordScalar("meanBackoff", 0);
		}
		recordScalar("nbBackoffs", nbBackoffs);
		recordScalar("backoffDurations", backoffValues);
	}
}

csma::~csma() {
	cancelAndDelete(backoffTimer);
	cancelAndDelete(ccaTimer);
	cancelAndDelete(sifsTimer);
	cancelAndDelete(rxAckTimer);
	if(ackMessage) delete ackMessage;
	MacQueue::iterator it;
	for(it = macQueue.begin(); it != macQueue.end(); ++it) {
		delete (*it);
	}
	macStatesColors.clear();
}

/**
 * Encapsulates the message to be transmitted and pass it on
 * to the FSM main method for further processing.
 */
void csma::handleUpperMsg(cMessage *msg)
{
	//MacPkt *macPkt = encapsMsg(msg);
	MacPkt *macPkt = new MacPkt(msg->getName());
	macPkt->setBitLength(headerLength);
	MacControlInfo* cInfo = static_cast<MacControlInfo*>(msg->removeControlInfo());
	EV <<"CSMA received a message from upper layer, name is " << msg->getName() <<", CInfo removed, mac addr="<< cInfo->getNextHopMac()<<endl;
	int dest = cInfo->getNextHopMac();
	macPkt->setDestAddr(dest);
	delete cInfo;
	macPkt->setSrcAddr(macaddress.getAddress());

	if(SeqNrParent.find(dest) == SeqNrParent.end()){
		//no record of current parent -> add next sequence number to map
		SeqNrParent[dest] = 1;
		macPkt->setSequenceId(0);
		EV << "Adding a new parent to the map of Sequence numbers:" << dest << endl;
	}
	else{
		macPkt->setSequenceId(SeqNrParent[dest]);
		EV << "Packet send with sequence number = " << SeqNrParent[dest] << endl;
		SeqNrParent[dest]++;
	}

	RadioAccNoise3PhyControlInfo *pco = new RadioAccNoise3PhyControlInfo(bitrate);
	macPkt->setControlInfo(pco);
	assert(static_cast<cPacket*>(msg));
	macPkt->encapsulate(static_cast<cPacket*>(msg));
	EV <<"pkt encapsulated, length: " << macPkt->getBitLength() << "\n";
	executeMac(EV_SEND_REQUEST, macPkt);
}

void csma::updateStatusIdle(t_mac_event event, cMessage *msg)
{
	switch(event) {
	case EV_SEND_REQUEST:
		if (macQueue.size() <= queueLength) {
			macQueue.push_back(static_cast<MacPkt *>(msg));
			EV << "(1) FSM State IDLE_1, EV_SEND_REQUEST and [TxBuff avail]: startTimerBackOff -> BACKOFF." << endl;
			updateMacState(BACKOFF_2);
			NB = 0;
			BE = macMinBE;
			startTimer(TIMER_BACKOFF);
		} else {
			// queue is full, message has to be deleted
			EV << "(12) FSM State IDLE_1, EV_SEND_REQUEST and [TxBuff not avail]: dropping packet -> IDLE." << endl;
			msg->setName("MAC ERROR");
			msg->setKind(NicControlType::PACKET_DROPPED);
			sendControlUp(msg);
			droppedPacket.setReason(DroppedPacket::QUEUE);
			bb->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
			updateMacState(IDLE_1);
		}
		break;
	case EV_DUPLICATE_RECEIVED:
		EV << "(15) FSM State IDLE_1, EV_DUPLICATE_RECEIVED: setting up radio tx -> WAITSIFS." << endl;
		//sendUp(decapsMsg(static_cast<MacSeqPkt *>(msg)));
		delete msg;
		radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
		updateMacState(WAITSIFS_6);
		startTimer(TIMER_SIFS);
		break;

	case EV_FRAME_RECEIVED:
		EV << "(15) FSM State IDLE_1, EV_FRAME_RECEIVED: setting up radio tx -> WAITSIFS." << endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
		updateMacState(WAITSIFS_6);
		startTimer(TIMER_SIFS);
		break;

	case EV_BROADCAST_RECEIVED:
		EV << "(23) FSM State IDLE_1, EV_BROADCAST_RECEIVED: Nothing to do." << endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusBackoff(t_mac_event event, cMessage *msg)
{
	switch(event) {
	case EV_TIMER_BACKOFF:
		EV << "(2) FSM State BACKOFF, EV_TIMER_BACKOFF:"
		   << " starting CCA timer." << endl;
		startTimer(TIMER_CCA);
		updateMacState(CCA_3);
		radio->execute(SingleChannelRadioAccNoise3::ENTER_RX);
		break;
	case EV_DUPLICATE_RECEIVED:
		// suspend current transmission attempt,
		// transmit ack,
		// and resume transmission when entering manageQueue()
		EV << "(28) FSM State BACKOFF, EV_DUPLICATE_RECEIVED:"
		  << "suspending current transmit tentative and transmitting ack";
		transmissionAttemptInterruptedByRx = true;
		cancelEvent(backoffTimer);
		//sendUp(decapsMsg(static_cast<MacSeqPkt *>(msg)));
		delete msg;
		radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
		updateMacState(WAITSIFS_6);
		startTimer(TIMER_SIFS);
		break;
	case EV_FRAME_RECEIVED:
		// suspend current transmission attempt,
		// transmit ack,
		// and resume transmission when entering manageQueue()
		EV << "(28) FSM State BACKOFF, EV_FRAME_RECEIVED:"
		  << "suspending current transmit tentative and transmitting ack";
		transmissionAttemptInterruptedByRx = true;
		cancelEvent(backoffTimer);
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
		updateMacState(WAITSIFS_6);
		startTimer(TIMER_SIFS);
		break;
	case EV_BROADCAST_RECEIVED:
		EV << "(29) FSM State BACKOFF, EV_BROADCAST_RECEIVED:"
		  << "sending frame up and resuming normal operation." <<endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusCCA(t_mac_event event, cMessage *msg)
{
  switch(event) {
    case EV_TIMER_CCA:
      EV << "(25) FSM State CCA_3, EV_TIMER_CCA" << endl;
      if(rssi<busyRSSI) {
	EV << "(3) FSM State CCA_3, EV_TIMER_CCA, [Channel Idle]: -> TRANSMITFRAME_4." << endl;
	updateMacState(TRANSMITFRAME_4);
	toLog.clear();
	toLog.str("");
	toLog << "h -t " << simTime(); // time
	toLog << " -s " << macaddress.getAddress();  // src
	if(macQueue.front()->getDestAddr()==MACAddress::BROADCAST_ADDRESS.getAddress())
	  toLog << " -d -1 "; // broadcast code for nam
	else
	  toLog << " -d " << macQueue.front()->getDestAddr();  // dest
	toLog << " -p DATA "; // packet type
	toLog << " -e " << macQueue.front()->getByteLength(); // size in bytes
	toLog << " -a 1 "; // packet type
	toLog << " -c 0 ";  // flow ID
	toLog << " -i 0 ";   // packet ID
	tracer->namLog(toLog.str());
	radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
	MacPkt * msg = check_and_cast<MacPkt *>(macQueue.front()->dup());
	double configuredber = (check_and_cast<RadioAccNoise3PhyControlInfo *>(macQueue.front()->getControlInfo())->getBitrate());
	RadioAccNoise3PhyControlInfo * pco = new RadioAccNoise3PhyControlInfo(configuredber);
	msg->setControlInfo(check_and_cast<RadioAccNoise3PhyControlInfo*>(pco));
	//sendDown(msg);
	// give time for the radio to be in Tx state before transmitting
	sendDelayed(msg, aTurnaroundTime, lowergateOut);
	nbTxFrames++;
      } else {
        // Channel was busy, increment 802.15.4 backoff timers as specified.
	EV << "(7) FSM State CCA_3, EV_TIMER_CCA, [Channel Busy]: "
	<< " increment counters." << endl;
	NB = NB+1;
	BE = min(BE+1, macMaxBE);

	// decide if we go for another backoff or if we drop the frame.
	if(NB > macMaxCSMABackoffs) {
		// drop the frame
		EV << "Tried " << NB << " backoffs, all reported a busy "
		<< "channel. Dropping the packet." << endl;
		cMessage * mac = macQueue.front();
		macQueue.pop_front();
		txAttempts = 0;
		nbDroppedFrames++;
		mac->setName("MAC ERROR");
		mac->setKind(NicControlType::PACKET_DROPPED);
		sendControlUp(mac);
		manageQueue();
	} else {
	// redo backoff
 	  updateMacState(BACKOFF_2);
	  startTimer(TIMER_BACKOFF);
        }
      }
      break;

    case EV_DUPLICATE_RECEIVED:
 	EV << "(26) FSM State CCA_3, EV_DUPLICATE_RECEIVED:"
 	<< " setting up radio tx -> WAITSIFS." << endl;
 	// suspend current transmission attempt,
 	// transmit ack,
 	// and resume transmission when entering manageQueue()
 	transmissionAttemptInterruptedByRx = true;
 	cancelEvent(ccaTimer);
 	sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
 	delete msg;
 	radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
 	updateMacState(WAITSIFS_6);
 	startTimer(TIMER_SIFS);
 	break;

    case EV_FRAME_RECEIVED:
	EV << "(26) FSM State CCA_3, EV_FRAME_RECEIVED:"
	<< " setting up radio tx -> WAITSIFS." << endl;
	// suspend current transmission attempt,
	// transmit ack,
	// and resume transmission when entering manageQueue()
	transmissionAttemptInterruptedByRx = true;
	cancelEvent(ccaTimer);
	sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
	delete msg;
	radio->execute(SingleChannelRadioAccNoise3::ENTER_TX);
	updateMacState(WAITSIFS_6);
	startTimer(TIMER_SIFS);
	break;
    case EV_BROADCAST_RECEIVED:
	EV << "(24) FSM State BACKOFF, EV_BROADCAST_RECEIVED:"
	<< " Nothing to do." << endl;
	sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
	delete msg;
	break;
    default:
	fsmError(event, msg);
  }
}

void csma::updateStatusTransmitFrame(t_mac_event event, cMessage *msg)
{
  if(event == EV_FRAME_TRANSMITTED) {
//    delete msg;
    MacPkt * packet = macQueue.front();
    radio->execute(SingleChannelRadioAccNoise3::ENTER_RX);

    if(packet->getDestAddr()!=MACAddress::BROADCAST_ADDRESS.getAddress()) {
      //unicast
      EV << "(4) FSM State TRANSMITFRAME_4, "
      << "EV_FRAME_TRANSMITTED [Unicast]: RadioSetupRx "
      << " -> WAITACK." << endl;
      updateMacState(WAITACK_5);
      startTimer(TIMER_RX_ACK);
    } else {
      //broadcast
      EV << "(27) FSM State TRANSMITFRAME_4, EV_FRAME_TRANSMITTED "
      << " [Broadcast]: RadioSetupRx, manageQueue..." << endl;
      macQueue.pop_front();
      delete packet;
      manageQueue();
    }
  } else {
    fsmError(event, msg);
  }
}

void csma::updateStatusWaitAck(t_mac_event event, cMessage *msg)
{
	cMessage * mac;
	switch(event) {
	case EV_ACK_RECEIVED:
		EV << "(5) FSM State WAITACK_5, EV_ACK_RECEIVED: "
		<< " ProcessAck, manageQueue..." << endl;
		if(rxAckTimer->isScheduled())
			cancelEvent(rxAckTimer);
		mac = static_cast<cMessage *>(macQueue.front());
		macQueue.pop_front();
		txAttempts = 0;
		mac->setName("MAC SUCCESS");
		mac->setKind(NicControlType::TRANSMISSION_OVER);
		sendControlUp(mac);
		delete msg;
		manageQueue();
		break;
	case EV_ACK_TIMEOUT:
		EV << "(12) FSM State WAITACK_5, EV_ACK_TIMEOUT:"
		<< " incrementCounter/dropPacket, manageQueue..." << endl;
		manageMissingAck(event, msg);
		break;
	case EV_BROADCAST_RECEIVED:
	case EV_FRAME_RECEIVED:
		sendUp(decapsMsg(static_cast<MacPkt*>(msg)));
	case EV_DUPLICATE_RECEIVED:
		EV << "Error ! Received a frame during SIFS !" << endl;
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}

}

void csma::manageMissingAck(t_mac_event event, cMessage *msg)
{
	toLog.clear();
	toLog.str("");
	toLog << "d -t " << simTime();
	toLog << " -s " << macaddress.getAddress();
	toLog << " -d " << macQueue.front()->getDestAddr();
	toLog << " -p DATA -a 2";
	tracer->namLog(toLog.str());
	if(txAttempts < macMaxFrameRetries+1) {
		// increment counter
		txAttempts++;
		EV << "I will retransmit this packet (I already tried "
		<< txAttempts << " times)." << endl;
	} else {
		// drop packet
		EV << "Packet was transmitted " << txAttempts
		<< " times and I never got an Ack. I drop the packet." << endl;
		cMessage * mac = macQueue.front();
		macQueue.pop_front();
		txAttempts = 0;
		mac->setName("MAC ERROR");
		mac->setKind(NicControlType::PACKET_DROPPED);
		sendControlUp(mac);
	}
	manageQueue();
}
void csma::updateStatusSIFS(t_mac_event event, cMessage *msg)
{
	switch(event) {
	case EV_TIMER_SIFS:
		EV << "(17) FSM State WAITSIFS_6, EV_TIMER_SIFS:"
		<< " sendAck -> TRANSMITACK." << endl;
		updateMacState(TRANSMITACK_7);
		toLog.clear();
		toLog.str("");
		toLog << "h -t " << simTime(); // time
		toLog << " -s " << macaddress.getAddress();  // src
		toLog << " -d " << ackMessage->getDestAddr();  // dst
		toLog << " -p ACK "; // packet type
		toLog << " -e " << ackMessage->getByteLength(); // size in bytes
		toLog << " -a 2 "; // packet type
		toLog << " -c 0 ";  // flow ID
		toLog << " -i 0 ";   // packet ID
		tracer->namLog(toLog.str());
		sendDown(ackMessage);
		nbTxAcks++;
//		sendDelayed(ackMessage, aTurnaroundTime, lowergateOut);
		ackMessage = NULL;
		break;
	case EV_TIMER_BACKOFF:
		// Backoff timer has expired while receiving a frame. Restart it
		// and stay here.
		EV << "(16) FSM State WAITSIFS_6, EV_TIMER_BACKOFF. "
		<< "Restart backoff timer and don't move." << endl;
		startTimer(TIMER_BACKOFF);
		break;
	case EV_BROADCAST_RECEIVED:
	case EV_FRAME_RECEIVED:
		EV << "Error ! Received a frame during SIFS !" << endl;
		sendUp(decapsMsg(static_cast<MacPkt*>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusTransmitAck(t_mac_event event, cMessage *msg)
{
	if(event==EV_FRAME_TRANSMITTED) {
		EV << "(19) FSM State TRANSMITACK_7, EV_FRAME_TRANSMITTED:"
		<< " ->manageQueue." << endl;
		radio->execute(SingleChannelRadioAccNoise3::ENTER_RX);
//		delete msg;
		manageQueue();
	} else {
		fsmError(event, msg);
	}
}

void csma::updateStatusNotIdle(cMessage *msg)
{
	EV << "(20) FSM State NOT IDLE, EV_SEND_REQUEST. Is a TxBuffer available ?" << endl;
	if (macQueue.size() <= queueLength) {
		macQueue.push_back(static_cast<MacPkt *>(msg));
		EV << "(21) FSM State NOT IDLE, EV_SEND_REQUEST"
		<<" and [TxBuff avail]: enqueue packet and don't move." << endl;
	} else {
		// queue is full, message has to be deleted
		EV << "(22) FSM State NOT IDLE, EV_SEND_REQUEST"
		<< " and [TxBuff not avail]: dropping packet and don't move."
		<< endl;
		msg->setName("MAC ERROR");
		msg->setKind(NicControlType::PACKET_DROPPED);
		sendControlUp(msg);
		droppedPacket.setReason(DroppedPacket::QUEUE);
		bb->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
	}

}
/**
 * Updates state machine.
 */
void csma::executeMac(t_mac_event event, cMessage *msg)
{
	EV << "In executeMac" << endl;
	if(macState != IDLE_1 && event == EV_SEND_REQUEST) {
		updateStatusNotIdle(msg);
	} else {
		switch(macState) {
		case IDLE_1:
			updateStatusIdle(event, msg);
			break;
		case BACKOFF_2:
			updateStatusBackoff(event, msg);
			break;
		case CCA_3:
			updateStatusCCA(event, msg);
			break;
		case TRANSMITFRAME_4:
			updateStatusTransmitFrame(event, msg);
			break;
		case WAITACK_5:
			updateStatusWaitAck(event, msg);
			break;
		case WAITSIFS_6:
			updateStatusSIFS(event, msg);
			break;
		case TRANSMITACK_7:
			updateStatusTransmitAck(event, msg);
			break;
		default:
			EV << "Error in CSMA FSM: an unknown state has been reached. macState=" << macState << endl;
		}
	}
}

void csma::manageQueue() {
	if(macQueue.size() != 0) {
		EV << "(manageQueue) there are " << macQueue.size() << " packets to send, entering backoff wait state." << endl;
		if(! backoffTimer->isScheduled())
			startTimer(TIMER_BACKOFF);
		updateMacState(BACKOFF_2);
		if( transmissionAttemptInterruptedByRx) {
		  // resume a transmission cycle which was interrupted by
		  // a frame reception during CCA check
		  transmissionAttemptInterruptedByRx = false;
		} else {
		  // initialize counters if we start a new transmission
		  // cycle from zero
 		  NB = 0;
		  BE = macMinBE;
		}
	} else {
		EV << "(manageQueue) no packets to send, entering IDLE state." << endl;
		updateMacState(IDLE_1);
	}
}

void csma::updateMacState(t_mac_states newMacState)
{
	namPrevColor = macStatesColors.at(macState);
	macState = newMacState;
	toLog.clear();
	toLog.str("");
	toLog << "n -t " << simTime();
	toLog << " -s " << macaddress.getAddress();
	toLog << " -S COLOR -o " << namPrevColor;
	toLog << " -c " << macStatesColors.at(macState);
	tracer->namLog(toLog.str());
}

/*
 * Called by the FSM machine when an unknown transition is requested.
 */
void csma::fsmError(t_mac_event event, cMessage *msg)
{
	EV << "FSM Error ! In state " << macState << ", received unknown event:" << event << "." << endl;
	if (msg != NULL)
		delete msg;
}

void csma::startTimer(t_mac_timer timer) {
	if(timer==TIMER_BACKOFF) {
		scheduleAt(scheduleBackoff(), backoffTimer);
	} else if (timer==TIMER_CCA) {
		simtime_t ccaTime = rxSetupTime+ccaDetectionTime;
		EV << "(startTimer) ccaTimer value=" <<  ccaTime
		<< "(rxSetupTime,ccaDetectionTime:" << rxSetupTime
		<<  "," << ccaDetectionTime <<")." << endl;
		scheduleAt(simTime()+rxSetupTime+ccaDetectionTime, ccaTimer);
	} else if (timer==TIMER_SIFS) {
		EV << "(startTimer) sifsTimer value=" << sifs << endl;
		scheduleAt(simTime()+sifs, sifsTimer);
	} else if (timer==TIMER_RX_ACK) {
		EV << "(startTimer) rxAckTimer value=" << macAckWaitDuration << endl;
		scheduleAt(simTime()+macAckWaitDuration, rxAckTimer);
	} else {
		EV << "Unknown timer requested to start:" << timer << endl;
	}
}

double csma::scheduleBackoff() {
	double d = std::pow((double)2,(int)BE);
	int v = (int) d-1;
	int r = intuniform(1, v, 0);
	double value = r*aUnitBackoffPeriod.dbl();
	nbBackoffs = nbBackoffs + 1;
	backoffValues = backoffValues + value;
	// this guarantees that we have the time to setup the radio into rx state
	//value = max(value-rxSetupTime,0);
	EV << "(startTimer) backoffTimer value=" << value
	<< " (BE=" << BE << ", 2^BE-1= " << v << "r="
	<< r << ")" << endl;
	value = value + simTime().dbl();
	return value;
}

/*
 * Binds timers to events and executes FSM.
 */
void csma::handleSelfMsg(cMessage *msg)
{
	EV << "timer routine." << endl;
	if(msg==backoffTimer)
		executeMac(EV_TIMER_BACKOFF, msg);
	else if(msg==ccaTimer)
		executeMac(EV_TIMER_CCA, msg);
	else if(msg==sifsTimer)
		executeMac(EV_TIMER_SIFS, msg);
	else if(msg==rxAckTimer) {
		nbMissedAcks++;
		executeMac(EV_ACK_TIMEOUT, msg);
	} else
		EV << "CSMA Error: unknown timer fired:" << msg << endl;
}


/**
 * Compares the address of this Host with the destination address in
 * frame. Generates the corresponding event.
 */
void csma::handleLowerMsg(cMessage *msg)
{
	MacPkt *macPkt = static_cast<MacPkt *>(msg);
	unsigned long src = macPkt->getSrcAddr();
	unsigned long dest = macPkt->getDestAddr();
	long SeqNr = macPkt->getSequenceId();
	long ExpectedNr = 0;

	EV << "Received frame name= " << macPkt->getName()
	<< ", myState=" << macState << " src=" << macPkt->getSrcAddr()
	<< " dst=" << macPkt->getDestAddr() << " myAddr="
	<< macaddress.getAddress() << endl;


	if(macPkt->getDestAddr() == macaddress.getAddress())
	{
		if(strcmp(macPkt->getName(), "CSMA-Ack")) {
			// This is a data message addressed to us
			// and we should send an ack.
			// we build the ack packet here because we need to
			// copy data from macPkt (src).
			EV << "Received a data packet addressed to me,"
			<< " preparing an ack..." << endl;
			nbRxFrames++;
			if(ackMessage != NULL)
				delete ackMessage;
			ackMessage = new MacPkt("CSMA-Ack");
			ackMessage->setSrcAddr(macaddress.getAddress());
			ackMessage->setDestAddr(macPkt->getSrcAddr());
			ackMessage->setBitLength(ackLength);
			RadioAccNoise3PhyControlInfo *pco = new RadioAccNoise3PhyControlInfo(bitrate);
			ackMessage->setControlInfo(pco);

			//Check for duplicates by checking expected seqNr of sender
			if(SeqNrChild.find(src) == SeqNrChild.end()){
				//no record of current child -> add expected next number to map
				SeqNrChild[src] = SeqNr + 1;
				EV << "Adding a new child to the map of Sequence numbers:" << src << endl;
				executeMac(EV_FRAME_RECEIVED, macPkt);
			}
			else{
				ExpectedNr = SeqNrChild[src];
				EV << "Expected Sequence number is " << ExpectedNr <<
				" and number of packet is " << SeqNr << endl;
				if(SeqNr < ExpectedNr){
					//Duplicate Packet, count and do not send to upper layer
					nbDuplicates++;
					executeMac(EV_DUPLICATE_RECEIVED, macPkt);
				}
				else{
					SeqNrChild[src] = SeqNr + 1;
					executeMac(EV_FRAME_RECEIVED, macPkt);
				}

			}

		} else if(macQueue.size() != 0) {
			// message is an ack, and it is for us.
			// Is it from the right node ?
			MacPkt * firstPacket = static_cast<MacPkt *>(macQueue.front());
			if(macPkt->getSrcAddr() == firstPacket->getDestAddr()) {
				nbRecvdAcks++;
				executeMac(EV_ACK_RECEIVED, macPkt);
			} else {
				EV << "Error! Received an ack from an unexpected source: src=" << macPkt->getSrcAddr() << ", I was expecting from node addr=" << firstPacket->getDestAddr() << endl;
				delete macPkt;
			}
		} else  {
			EV << "Error! Received an Ack while my send queue was empty. src=" << macPkt->getSrcAddr() << "." << endl;
			delete macPkt;
		}
	}
	else if (dest == MACAddress::BROADCAST_ADDRESS.getAddress()){
		executeMac(EV_BROADCAST_RECEIVED, macPkt);
	} else {
		EV << "packet not for me, deleting...\n";
		delete macPkt;
	}
}

void csma::handleLowerControl(cMessage *msg)
{
	if(msg->getKind() == NicControlType::TRANSMISSION_OVER) {
		executeMac(EV_FRAME_TRANSMITTED, msg);
	}
	else if(msg->getKind() == NicControlType::PACKET_DROPPED) {
		EV << "control message: PACKED DROPPED" << endl;
	} else {
		EV << "Invalid control message type (type=NOTHING) : name="
		<< msg->getName() << " modulesrc="
		<< msg->getSenderModule()->getFullPath()
		<< "." << endl;
	}
	delete msg;
}

/**
 * Update the internal copies of interesting BB variables
 *
 */
void csma::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{
	Enter_Method_Silent();
	BasicLayer::receiveBBItem(category, details, scopeModuleId);

	if(category == catRadioState) {
		radioState = static_cast<const RadioAccNoise3State *>(details)->getState();
		// radio just told us its state
	}
	else if(category == catRSSI) {
		rssi = static_cast<const RSSI *>(details)->getRSSI();
		if(radioState == RadioAccNoise3State::RX) {
			// we could do something here if we wanted to.
		}
	}
}

cMessage *csma::decapsMsg(MacPkt * macPkt)
{
    cMessage * msg = macPkt->decapsulate();
    double ber = static_cast<RadioAccNoise3PhyControlInfo*>( macPkt->getControlInfo() )
    				->getBER();
    msg->setControlInfo( new MacControlInfo( macPkt->getSrcAddr(), ber) );
    return msg;
}

