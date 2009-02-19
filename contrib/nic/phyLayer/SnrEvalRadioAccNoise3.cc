// file:        SnrEvalRadioAccNoise3.cc
//
//  author:      Marc L�bbers, Amre El-Hoiydi, Jérôme Rousselot
// copyright:   (c) by Tralafitty
//              Telecommunication Networks Group
//              TU-Berlin
// email:       loebbers@tkn.tu-berlin.de
//              (C) 2007 CSEM
// part of: framework implementation developed by tkn description: - a
// snrEval extension for the use with the other 802.11 modules
// Change log:  Modified SnrEval80211 to match 802.15.4

#include "SnrEvalRadioAccNoise3.h"
#include "ConstsAccNoise3.h"
#include "AirFrameRadioAccNoise3_m.h"
#include "PhyControlInfo.h"


Define_Module(SnrEvalRadioAccNoise3);

//const double BasicSnrEval::speedOfLight = ChannelControl::speedOfLight;

void SnrEvalRadioAccNoise3::initialize(int stage)
{
  ChannelAccess::initialize(stage);
  if (stage == 0) {

    //EV << "initializing stage 0\n";
    uppergateIn = findGate("uppergateIn");
    uppergateOut = findGate("uppergateOut");
    upperControlOut = findGate("upperControlOut");
    headerLength = par("headerLength");
    hasPar("transmitterPower") ? transmitterPower =
      par("transmitterPower").doubleValue() : transmitterPower =
      static_cast < double >(cc->par("pMax"));
    hasPar("carrierFrequency") ? carrierFrequency =
      par("carrierFrequency").doubleValue() : carrierFrequency =
      static_cast < double >(cc->par("carrierFrequency"));
    hasPar("alpha") ? alpha = par("alpha").doubleValue() : alpha =
      static_cast < double >(cc->par("alpha"));

    catActiveChannel = bb->subscribe(this, &channel, getParentModule()->getId());

    /*
     * hasPar("thermalNoise") ? thermalNoise =
      FWMath::dBm2mW(par("thermalNoise").doubleValue()) : thermalNoise =
      FWMath::dBm2mW(DEFAULT_THERMAL_NOISE);
*/
	thermalNoise = FWMath::dBm2mW(-100); // DEFAULT_THERMAL_NOISE = -100

    sfdLength = par("sfdLength");
    stats = par("stats").boolValue();
    trace = par("trace").boolValue();

    frameDurations.setName("airframeDurations");

    nbCollisions = 0;

    useTorus = false;
    if (cc->hasPar("useTorus"))
      useTorus = cc->par("useTorus").boolValue();

    playground.x = cc->par("playgroundSizeX");
    playground.y = cc->par("playgroundSizeY");

    hasPar("publishRSSIAlways") ? publishRSSIAlways =
      par("publishRSSIAlways").boolValue() : publishRSSIAlways = false;

    radioState = RadioAccNoise3State::SLEEP;
    rssi.setRSSI(thermalNoise);

    //indication.setState(MediumIndication::IDLE);

    // subscribe for information about the radio

    RadioAccNoise3State cs;
    Bitrate br;

    catRadioState = bb->subscribe(this, &cs, getParentModule()->getId());
    catRSSI = bb->getCategory(&rssi);
    catBitrate = bb->subscribe(this, &br, getParentModule()->getId());
    //catIndication = bb->getCategory(&indication);
  } else if (stage == 1) {
    if (alpha < cc->par("alpha").doubleValue())
      error("SnrEval::initialize() alpha can't be smaller than in \
                   ChannelControl. Please adjust your omnetpp.ini file accordingly");

    if (transmitterPower > cc->par("pMax").doubleValue())
      error("SnrEval::initialize() tranmitterPower can't be bigger than \
                   pMax in ChannelControl! Please adjust your omnetpp.ini file accordingly");

    if (carrierFrequency < cc->par("carrierFrequency").doubleValue())
      error("SnrEval::initialize() carrierFrequency can't be smaller than in \
                   ChannelControl. Please adjust your omnetpp.ini file accordingly");

    txOverTimer = new cMessage("txOverTimer");


    waveLength = speedOfLight / carrierFrequency;
    pathLossAlphaHalf = alpha / 2.0;

    // initialize the pointer of the snrInfo with NULL to indicate
    // that currently no message is received

    snrInfo.ptr = NULL;
    snrInfo.rcvdPower = 0;
    snrInfo.sList = SnrList();

    recvTime = 0.0;
    nicModuleId = getParentModule()->getId();
    noiseLevel = thermalNoise;

    state = MONITOR;
    // we can publish since we are in stage 1
    bb->publishBBItem(catRSSI, &rssi, nicModuleId);

//    EV << "carrierFrequency: " << carrierFrequency
//      << " waveLength: " << waveLength
//      << " pathLossAlpha: " << alpha
//      << " pathLossAlphaHalf: " << pathLossAlphaHalf
//      << " publishRSSIAlways: " << publishRSSIAlways
//      << " noiseLevel: " << noiseLevel << endl;
  }

}

void SnrEvalRadioAccNoise3::finish()
{
  ChannelAccess::finish();
  cancelAndDelete(txOverTimer);
  if (snrInfo.ptr != NULL) {
    AirFrameRadioAccNoise3 * toRemove = snrInfo.ptr;
    recvBuff.erase(snrInfo.ptr);
    delete toRemove;
  }
  snrInfo.sList.clear();
  cRecvBuff::iterator iter;
  if (!recvBuff.empty())
    for (iter = recvBuff.begin(); iter != recvBuff.end(); ) {
      cRecvBuff::iterator toDelete = iter;
      AirFrameRadioAccNoise3 * el = iter->first;
      iter++;
      recvBuff.erase(toDelete);
      delete el; // remove frames "owned" by this module
    }

}

/*This function executes the finite state machine.
 */
void
  SnrEvalRadioAccNoise3::execute(Events event, AirFrameRadioAccNoise3 * frame)
{
  //EV << "In SNREvalRadioAccNoise3 execute" << endl;
  switch (state) {
    case MONITOR:
      switch (event) {
	case NEW_FRAME:	// transition 1
//	  EV <<
//	    "MONITOR state, a frame started, updating noise level accordingly."
//	    << endl;
	  addToNoiseLevel(frame);
	  break;
	case END_FRAME:	// transition 2
//	  EV <<
//	    "MONITOR state, a frame finished, updating noise level accordingly."
//	    << endl;
	  removeFromNoiseLevel(frame);
         recvBuff.erase(frame);
	  delete frame;

	  break;
	case RADIO_ENTER_RX:	// transition 3
	  enterState(SYNC);
	  break;
	case RADIO_LEAVE_RX:
	  enterState(MONITOR);	// nothing to do
	  break;
	default:
	  fsmError(event);
      }
      break;
    case SYNC:
      switch (event) {
	case NEW_FRAME:
//	  EV << "Received a NEW_FRAME while in SYNC, trying to synchronize..."
//	    << endl;
	  if (syncOnSFD(frame)) {
	    //EV << "SYNC succeeded, I will enter into DECODING state" << endl;
	    rssi.setRSSI(rssi.getRSSI() + recvBuff[frame]);
	    // Put frame and related SnrList in receive buffer
	    snrInfo.ptr = frame;
	    snrInfo.rcvdPower = recvBuff[frame];
	    snrInfo.sList.clear();

	    // add initial snr value
	    addNewSnr();
	    rssi.setRSSI(rssi.getRSSI() + recvBuff[frame]);
	    indication.setState(MediumIndication::BUSY);
	    if(publishRSSIAlways || radioState == RadioAccNoise3State::RX)
	      bb->publishBBItem(catRSSI, &rssi, nicModuleId);
	    enterState(DECODING);	// transition 7
	  } else {
//	    EV << "SYNC failed ! Frame will be treated as additive noise." <<
//	      endl;
	    addToNoiseLevel(frame);	// transition 4
	  }
	  break;
	case END_FRAME:	// transition 5
	  //EV << "SYNC state, a noisy frame ended, updating snr." << endl;
	  removeFromNoiseLevel(frame);
	  addNewSnr();
         recvBuff.erase(frame);
	  delete frame;

	  break;
	case RADIO_LEAVE_RX:	// transition 6
	  enterState(MONITOR);
	  break;
	case RADIO_ENTER_RX:
	  enterState(SYNC);	// nothing to do
	  break;
	default:
	  fsmError(event);
      }
      break;
    case DECODING:
      switch (event) {
	case NEW_FRAME:	// transition 8
//	  EV <<
//	    "(8) NEW_FRAME while in DECODING mode, I am considering the frame as noise..."
//	    << endl;
	  addToNoiseLevel(frame);
	  addNewSnr();
	  //delete frame;
	  //EV << "finished switch section." << endl;
	  break;
	case END_FRAME:	// transitions 10 and 11
	  //EV << "DECODING state, END_FRAME happened..." << endl;
	  if (snrInfo.ptr == frame)	// we received the complete frame
	  {
//	    EV << "(10) End of sync'ed frame. Sending it up to Decider." <<
//	      endl;
	    recvBuff.erase(frame);
	    sendUp(frame, snrInfo.sList);
	    // delete the pointer to indicate that no message is currently
	    // being received and clear the list
	    snrInfo.ptr = NULL;
	    snrInfo.sList.clear();
	    rssi.setRSSI(noiseLevel);
    	    if(publishRSSIAlways || radioState == RadioAccNoise3State::RX)
	      bb->publishBBItem(catRSSI, &rssi, nicModuleId);
	    enterState(SYNC);	// restart search for preambles
           // No need to delete frame as it was passed to the decider

	  } else {
//	    EV <<
//	      "(9) DECODING state, END_FRAME was for a noisy frame, updating Snr."
//	      << endl;
	    // frame was noise
	    removeFromNoiseLevel(frame);	// transition 9
	    addNewSnr();
//	    EV << " noise frame ended during a reception "
//	      << " radiostate = " << radioState << endl;
	    if (publishRSSIAlways || (radioState == RadioAccNoise3State::RX))
	      bb->publishBBItem(catRSSI, &rssi, nicModuleId);
           recvBuff.erase(frame);
	    delete frame;
	  }
	  break;
	case RADIO_LEAVE_RX:	// transition 11
	  // remove references to the frame which was being received
//	  EV <<
//	    "(11) Radio left Rx mode while DECODING. Dropping frame being received."
//	    << endl;
	  recvBuff.erase(snrInfo.ptr);
	  addToNoiseLevel(snrInfo.ptr);
	  snrInfo.ptr = NULL;
	  snrInfo.sList.clear();
	  enterState(MONITOR);
	  break;
	case RADIO_ENTER_RX:	// nothing to do
	  enterState(DECODING);
	  break;
	default:
	  fsmError(event);
      }
      break;
    default:
      fsmError(event);
  }
}

bool SnrEvalRadioAccNoise3::syncOnSFD(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(syncOnSFD)" << endl;
  double BER;
  double sfdErrorProbability;

  BER = evalBER(frame);
  sfdErrorProbability = 1.0 - pow((1.0 - BER), sfdLength);
//  EV << "syncOnSFD: BER=" << BER << ", errorProb=" << sfdErrorProbability <<
//    endl;
  return sfdErrorProbability < uniform(0, 1, 0);
}

double SnrEvalRadioAccNoise3::evalBER(AirFrameRadioAccNoise3 * frame)
{
//  EV << "(evalBER) frame rxPower=" << recvBuff[frame] << ", ber=" << max(0.5
//									 *
//									 exp
//									 (-recvBuff
//									  [frame]
//									  /
//									  (2
//									   *
//									   noiseLevel)),
//									 DEFAULT_BER_LOWER_BOUND)
//    << "." << endl;
  return max(0.5 * exp(-recvBuff[frame] / (2 * noiseLevel)),
	     DEFAULT_BER_LOWER_BOUND);
}

void SnrEvalRadioAccNoise3::addToNoiseLevel(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(addToNoiseLevel)" << endl;
  noiseLevel += recvBuff[frame];
  rssi.setRSSI(rssi.getRSSI() + recvBuff[frame]);
  if(publishRSSIAlways || radioState == RadioAccNoise3State::RX)
    bb->publishBBItem(catRSSI, &rssi, nicModuleId);
}

void
  SnrEvalRadioAccNoise3::removeFromNoiseLevel(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(removeFromNoiseLevel)" << endl;
  noiseLevel -= recvBuff[frame];
  rssi.setRSSI(rssi.getRSSI() - recvBuff[frame]);
  if(publishRSSIAlways || radioState == RadioAccNoise3State::RX)
    bb->publishBBItem(catRSSI, &rssi, nicModuleId);
}

void SnrEvalRadioAccNoise3::enterState(States newState)
{
//  EV << "Leaving state " << state << " and entering state " << newState <<
//    " (radio is in state " << radioState << ")." << endl;
  state = newState;
}

void SnrEvalRadioAccNoise3::fsmError(Events event)
{
//  EV << "Error in state machine ! Current state is " << state <<
//    " and event is " << event << "." << endl;
}

/**
 * This function is called right after a packet arrived, i.e. right
 * before it is buffered for 'transmission time'.
 *
 *
 * First must to decide whether the message is "really" received or
 * whether it's receive power is so low that it is just treated as noise.
 * If the energy of the message is high enough to really receive it
 * an snr list (@ref SnrList) should be created to be able to store
 * sn(i)r information for that message. Every time a new message
 * a new snr value with a timestamp is added to that list.
 * If the receive power of the frame is below the receiver sensibility, the
 * frame is added to the accumulated noise.
 *
 **/
void
  SnrEvalRadioAccNoise3::handleLowerMsgStart(AirFrameRadioAccNoise3 * frame)
{
//  EV << "(handleLowerMsgStart)" << endl;
//  EV << "frame type is " << frame->getKind() << ", frame name=" << frame->
//    getName() << ", frame length=" << frame->getBitLength() << endl;
//      AirFrameRadioAccNoise3 * af = check_and_cast<AirFrameRadioAccNoise3*>(frame);
//      // store the receive power in the recvBuff
//      recvBuff[af] = calcRcvdPower(af);
//      execute(NEW_FRAME, af);
  recvBuff[frame] = calcRcvdPower(frame);
  execute(NEW_FRAME, frame);

}


/**
 * This function is called right after the transmission is over,
 * i.e. right after unbuffering.
 *
 * First check the current radio state. The radio must not be switched from RECV
 * state before the end of message is received. Otherwise drop the message.
 * Additionally the snr information of the currently being received message (if any)
 * has to be updated with the receivetime as timestamp and a new snr value.
 * The new SnrList and the AirFrameRadioAccNoise3are sent to the decider.
 *
 **/
void SnrEvalRadioAccNoise3::handleLowerMsgEnd(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(handleLowerMsgEnd)" << endl;
  if (frame->getKind() == AIRFRAME) {
//    EV << "frame type is " << frame->getKind() << ", frame name=" << frame->
//      getName() << ", frame length=" << frame->getBitLength() << endl;
    AirFrameRadioAccNoise3 *frame3 =
      check_and_cast < AirFrameRadioAccNoise3 * >(frame);
    //EV << "frame casted" << endl;
    execute(END_FRAME, frame3);
  } else {
//    EV <<
//      "frame type is not an AIRFRAME, what should I do ?" << endl;
  }
}


void SnrEvalRadioAccNoise3::handleMessage(cMessage * msg)
{
  //EV << "(handleMessage)" << endl;
  if (msg->getArrivalGateId() == uppergateIn) {
    AirFrameRadioAccNoise3 *frame = encapsMsg(msg);

    handleUpperMsg(frame);
  } else if (msg == txOverTimer) {
    //EV << "transmission over" << endl;
    sendControlUp(new
		  cMessage("TRANSMISSION_OVER",
			   NicControlType::TRANSMISSION_OVER));
  } else if (msg->isSelfMessage()) {
    if (msg->getKind() == RECEPTION_COMPLETE) {
      //EV << "frame is completely received now\n";
      // unbuffer the message
      AirFrameRadioAccNoise3 *frame = unbufferMsg(msg);
	  // was  there a collision ?
	  //if()
      handleLowerMsgEnd(frame);
    } else {
      handleSelfMsg(msg);
    }
  } else {
    // msg must come from channel
    AirFrameRadioAccNoise3 *frame =
      static_cast < AirFrameRadioAccNoise3 * >(msg);
    handleLowerMsgStart(frame);
    bufferMsg(frame);
  }
}

void SnrEvalRadioAccNoise3::handleSelfMsg(cMessage * msg)
{
  //EV << "Received an unexpected self message ! " << endl;
}

/**
 * The SnrEval has subscreibed the NewRadioState and will be informed each time the
 * radio state changes. The time radio switched to RECV must be noted.
 **/

void
  SnrEvalRadioAccNoise3::receiveBBItem(int category, const BBItem * details,
				       int scopeModuleId)
{
  //EV << "(receiveBBItem)" << endl;

  ChannelAccess::receiveBBItem(category, details, scopeModuleId);
  if (category == catActiveChannel) {
    channel = *(static_cast < const ActiveChannel * >(details));
  } else if (category == catRadioState) {
    RadioAccNoise3State::States oldRadioState = radioState;
    radioState =
      static_cast < const RadioAccNoise3State *>(details)->getState();
    if (radioState == RadioAccNoise3State::RX) {
      recvTime = simTime();
      //EV << "Radio switched to RECV at T= " << recvTime << endl;
      if(publishRSSIAlways || radioState == RadioAccNoise3State::RX)
        bb->publishBBItem(catRSSI, &rssi, nicModuleId);
      execute(RADIO_ENTER_RX, NULL);
    } else if (oldRadioState == RadioAccNoise3State::RX) {
      //EV << "Radio left RX mode at T=" << simTime() << endl;
      execute(RADIO_LEAVE_RX, NULL);
    }
  } else if (category == catBitrate) {
    bitrate = static_cast < const Bitrate *>(details)->getBitrate();
  }
}

/**
 * The Snr information of the buffered message is updated....
 **/
void SnrEvalRadioAccNoise3::addNewSnr()
{
  //EV << "(addNewSnr)" << endl;
  snrInfo.sList.push_back(SnrListEntry());
  snrInfo.sList.back().time = simTime();
  snrInfo.sList.back().snr = snrInfo.rcvdPower / noiseLevel;
//  EV << "New Snr added: " << snrInfo.sList.back().snr
//    << " at time:" << snrInfo.sList.back().time << endl;
}

/**
 * The Snr information of the buffered message is updated....
 *
 * If the frame arrived before the radio was ready to received, the snir
 * information must be new calculated.
 * The new snir information is a SnrList that lists all different snr
 * levels together with the point of time starting from which the radio
 * is switched in the received mode.
 *
 **/
void SnrEvalRadioAccNoise3::modifySnrList(SnrList & list)
{
  //EV << "(modifySnrList)" << endl;
  SnrList::iterator iter;
  for (iter = list.begin(); iter != list.end(); iter++) {
    //EV << " MessageSnr_time: " << iter->time << endl;
    //EV << " recvtime: " << recvTime << endl;
    if (iter->time >= recvTime) {
      list.erase(list.begin(), iter);
      iter->time = recvTime;
      break;
    }
  }
  if (iter == list.end()) {
    list.erase(list.begin(), --iter);
    list.begin()->time = recvTime;
  }
}

/**
 * This function simply calculates with how much power the signal
 * arrives "here". If a different way of computing the path loss is
 * required this function can be redefined.
 * Returns: Lp = Lamda² / (16 Pi² d^alpha)
 * e.g. PRx = PTx/Lp
 **/
double SnrEvalRadioAccNoise3::calcPathloss(AirFrameRadioAccNoise3 * frame)
{
  simtime_t time;
  double sqrdistance;
  double Lp;

  //EV << "(calcPathloss)" << endl;
  Coord myPos(hostMove.startPos);
  HostMove rHm(frame->getHostMove());
  Coord framePos(rHm.startPos);

  // Calculate the receive power of the message
  // get my position
  time = simTime() - hostMove.startTime;
  if (hostMove.speed != 0) {
    myPos.x += time.dbl() * hostMove.speed * hostMove.direction.x;
    myPos.y += time.dbl() * hostMove.speed * hostMove.direction.y;
  }
  //get Position of the sending node
  time = simTime() - rHm.startTime;

  if (rHm.speed != 0) {
    framePos.x += time.dbl() * rHm.speed * rHm.direction.x;
    framePos.y += time.dbl() * rHm.speed * rHm.direction.y;
  }
  //calculate distance and receive power
  if (useTorus) {
    sqrdistance = myPos.sqrTorusDist(framePos, playground);
  } else {
    sqrdistance = myPos.sqrdist(framePos);
  }

//  EV << "receiving frame start pos x: " << framePos.x
//    << " y: " << framePos.y
//    << " myPos x: " << myPos.x << " y: " << myPos.y
//    << " distance: " << sqrt(sqrdistance) << " Torus: " << useTorus << endl;

  Lp =
    (waveLength * waveLength) / (16.0 * M_PI * M_PI *
				 pow(sqrdistance, pathLossAlphaHalf));
  Lp = 1 / Lp;
  return Lp;
}

const double SnrEvalRadioAccNoise3::speedOfLight = 300000000.0;




/**
 * The packet is put in a buffer for the time the transmission would
 * last in reality. A timer indicates when the transmission is
 * complete. So, look at unbufferMsg to see what happens when the
 * transmission is complete..
 */
void SnrEvalRadioAccNoise3::bufferMsg(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(bufferMsg)" << endl;
  // set timer to indicate transmission is complete
  cMessage *timer = new cMessage("rxOverTimer", RECEPTION_COMPLETE);
  timer->setContextPointer(frame);
  scheduleAt(simTime() + frame->getDuration(), timer);
}

/**
 * Get the context pointer to the now completely received AirFrame and
 * delete the self message
 */
AirFrameRadioAccNoise3 *SnrEvalRadioAccNoise3::unbufferMsg(cMessage * msg)
{
  //EV << "(unbufferMsg)" << endl;
  AirFrameRadioAccNoise3 *frame =
    static_cast < AirFrameRadioAccNoise3 * >(msg->getContextPointer());
  delete msg;

  return frame;
}

/**
 * This function encapsulates messages from the upper layer into an
 * AirFrameRadioAccNoise3, copies the type and channel fields, adds the
 * headerLength, sets the pSend (transmitterPower) and returns the
 * AirFrameRadioAccNoise3.
 */
AirFrameRadioAccNoise3 *SnrEvalRadioAccNoise3::encapsMsg(cMessage * msg)
{
  //EV << "(encapsMsg)" << endl;
  AirFrameRadioAccNoise3 *frame;

  frame = new AirFrameRadioAccNoise3("AIRFRAME", AIRFRAME);
  PhyControlInfo *pco =
    static_cast < PhyControlInfo * >(msg->removeControlInfo());
  frame->setBitrate(pco->getBitrate());
  frame->setPSend(transmitterPower);
  frame->setBitLength(headerLength);
  frame->setHostMove(hostMove);
  frame->setChannelId(channel.getActiveChannel());
  assert(static_cast<cPacket*>(msg));
  cPacket* pkt = static_cast<cPacket*>(msg);
  frame->encapsulate(pkt);
  frame->setDuration(frame->getBitLength() / frame->getBitrate());
  frame->setHostMove(hostMove);
  delete pco;

//  EV << "SnrEvalRadioAccNoise3::encapsMsg duration: " << frame->
//    getDuration() << "\n";
  return frame;
}


double SnrEvalRadioAccNoise3::calcRcvdPower(AirFrameRadioAccNoise3 * frame)
{

  double pTx, pRx, Lp;

  //EV << "(calcRcvdPower)" << endl;
  pTx = frame->getPSend();
  Lp = calcPathloss(frame);
  pRx = pTx / Lp;
//  EV << "calcRcvdPower: pTx=" << pTx << ", Lp=" << Lp << ", pRx=" << pRx <<
//    "." << endl;
  return pRx;
}

// imported from BasicSnrEval

/**
 * Attach control info to the message and send message to the upper
 * layer.
 *
 * @param msg AirFrame to pass to the decider
 * @param list Snr list to attach as control info
 *
 * to be called within @ref handleLowerMsgEnd.
 */
void
  SnrEvalRadioAccNoise3::sendUp(AirFrameRadioAccNoise3 * msg,
				const SnrList & list)
{
  // create ControlInfo
  SnrControlInfo *cInfo = new SnrControlInfo;

  // attach the list to cInfo
  cInfo->setSnrList(list);
  //if(list.size == 1 && noiseLevel == thermalNoise) {
  //} else {
  //}

  // attach the cInfo to the AirFrame
  msg->setControlInfo(cInfo);
  send(static_cast < cMessage * >(msg), uppergateOut);
}

/**
 * send a control message to the upper layer
 */
void SnrEvalRadioAccNoise3::sendControlUp(cMessage * msg)
{
  //EV << "(sendControlUp)" << endl;
  send(msg, upperControlOut);
}

/**
 * Convenience function which calls sendToChannel with delay set
 * to 0.0.
 *
 * It also schedules the txOverTimer which indicates the end of
 * transmission to upper layers.
 *
 * @sa sendToChannel
 */
void SnrEvalRadioAccNoise3::sendDown(AirFrameRadioAccNoise3 * msg)
{
  //EV << "(sendDown)" << endl;
  if (!msg)
    EV << "msg == null !!!" << endl;
  else {
    //EV << "casting message" << endl;
    cMessage *msg2 = static_cast < cMessage * >(msg);

    //EV << "transmitting message" << endl;
    sendToChannel(msg2, 0.0);
    //EV << "finished transmitting" << endl;
  }
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to the channel.
 *
 * The MAC frame is already encapsulated in an AirFrame and all standard
 * header fields are set.
 */
void SnrEvalRadioAccNoise3::handleUpperMsg(AirFrameRadioAccNoise3 * frame)
{
  //EV << "(handleUpperMsg)" << endl;
  scheduleAt(simTime() + frame->getDuration(), txOverTimer);
  sendDown(frame);
  if(trace)
  	frameDurations.record(static_cast<double>(frame->getDuration()));
}
