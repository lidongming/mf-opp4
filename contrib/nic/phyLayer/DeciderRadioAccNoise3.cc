/***************************************************************************
 * file:        DeciderRadioAccNoise3.cc
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
 ***************************************************************************/


#include "MacPkt_m.h"
#include "DeciderRadioAccNoise3.h"
#include "ConstsAccNoise3.h"

Define_Module(DeciderRadioAccNoise3);

/**
 * First we have to initialize the module from which we derived ours,
 * in this case BasicDecider.
 *
 */
void DeciderRadioAccNoise3::initialize(int stage)
{
  BasicLayer::initialize(stage);

  if (stage == 0) {
    BER_LOWER_BOUND = par("berLowerBound").doubleValue();
  }
}

/**
 * Handle message from lower layer. The minimal snir is read in and
 * it is computed wether the packet has collided or has bit errors or
 * was received correctly. The corresponding kind is set and it is
 * handed on to the upper layer.
 *
 */
void DeciderRadioAccNoise3::handleLowerMsg(cMessage * m)
{
  double rxDuration;
  bool noErrors = true;
  simtime_t snrDuration;
  double snirMin, snirAvg;
  double ber, avgBER, bestBER;
  double errorProbability;

  //EV << "Decider got a message from SnrEval:" << endl;
  AirFrameRadioAccNoise3 *af = static_cast < AirFrameRadioAccNoise3 * >(m);
  SnrControlInfo *cInfo =
    static_cast < SnrControlInfo * >(af->removeControlInfo());

  const SnrList & rList = cInfo->getSnrList();

  SnrList::const_iterator iter = rList.begin();
  MacPkt *mac;

  snirMin = iter->snr;
  // Evaluate bit errors for each snr value
  // and stops as soon as we have an error.
  // TODO: add error correction code.
  //EV << "rList size=" << rList.size() << endl;

  unsigned int counter = 0;
  avgBER = 0;
  bestBER = 0.5;
  snirAvg = 0;

  for (; iter != rList.end();) {
    counter++;
    if (noErrors) {
      if (counter < rList.size()) {
	SnrList::const_iterator nextsnr = iter;
	nextsnr++;
	snrDuration = nextsnr->time - iter->time;
	//EV << "Current segment length=" << nextsnr->time << " - " << iter->
	 // time;
      } else {
	//EV << "Current segment length=" << simTime() << " - " << iter->time;
	snrDuration = simTime() - iter->time;
      }

      int nbBits = int (snrDuration.dbl() * af->getBitrate());

      // non-coherent detection of m-ary orthogonal signals in an AWGN
      // Channel
      // Digital Communications, John G. Proakis, section 4.3.2
      // p. 212, (4.3.32)
      //  Pm = sum(n=1,n=M-1){(-1)^(n+1)choose(M-1,n) 1/(n+1) exp(-nkgamma/(n+1))}
      // Pb = 2^(k-1)/(2^k - 1) Pm

      // non-coherent detection of binary signals in an AWGN Channel
      // Digital Communications, John G. Proakis, section 4.3.1
      // p.207, (4.3.19)
      ber = std::max(0.5 * exp(-0.5 * iter->snr), BER_LOWER_BOUND);
      avgBER = ber*nbBits;
      snirAvg = snirAvg + iter->snr*snrDuration.dbl();

      if(ber < bestBER) {
          bestBER = ber;
      }
      errorProbability = 1.0 - pow((1.0 - ber), nbBits);
      noErrors = errorProbability < uniform(0, 1);
//      EV << "Found no errors up to now ; current segment: nbBits=" << nbBits
//	<< "(snrDuration=" << snrDuration << ", bitrate=" << af->
//	getBitrate() << ")," << ", ber=" << ber << ", errorProbability=" <<
//	errorProbability << ", noErrors=" << noErrors << "." << endl;
    }
    if (iter->snr < snirMin)
      snirMin = iter->snr;
    iter++;
  }

  avgBER = avgBER / af->getBitLength();
  snirAvg = snirAvg / af->getBitLength();
  double rssi = (int) -log10(snirAvg);
  if (noErrors) {
//    EV <<
//      "packet was received correctly, it is now delivered to upper layer...\n";
    mac = static_cast < MacPkt * >(af->decapsulate());
    mac->
      setControlInfo(new
		     RadioAccNoise3PhyControlInfo(af->getBitrate(), snirMin,
						 af->getPreambleLength(), avgBER, rssi));
    sendUp(mac);
    af->removeControlInfo();
    delete af;

  } else {
    //EV << "Packet has BIT ERRORS! It is lost!\n";
    af->setName("BIT_ERRORS");
    af->setKind(NicControlType::PACKET_DROPPED);
    sendControlUp(af);
  }
  delete cInfo;
}

cMessage *DeciderRadioAccNoise3::decapsMsg(AirFrameRadioAccNoise3 * frame)
{
  cMessage *m = frame->decapsulate();

  // delete the AirFrane
  delete frame;

  // retrun the MacPkt
  return m;
}

void DeciderRadioAccNoise3::finish()
{
  BasicLayer::finish();
}
