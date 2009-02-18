package org.mf2.contrib.nic.phyLayer;

//***************************************************************************
// * file:        P2PPhyLayer.ned
// *
// * author:      Marc Loebbers, Daniel Willkomm
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *
// *              This program is free software; you can redistribute it 
// *              and/or modify it under the terms of the GNU General Public 
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later 
// *              version.
// *              For further information see file COPYING 
// *              in the top level directory
// ***************************************************************************
// * part of:     framework implementation developed by tkn
// * description: a very simple physical layer implementation
// ***************************************************************************/


// A very simple physical layer
//
// This is the simplest physical layer you can think of.
//
// It decides upon one simple parameter pBit (bit error rate)
// about the correctness of a frame. Collisions, bit errors,
// interference and anything else you can think of are combined in
// this single parameter.
//
// @author Marc Loebbers, Daniel Willkomm
simple P2PPhyLayer

{
    parameters:
        double coreDebug; // debug switch for the core framework
        double pBit; // bit error rate
        double bitrate; // bitrate (in bits/sec)
        double transmitterPower; // power used to transmit messages (in mW)
        double headerLength; // length  (in bits) of the AirFrame header

        @display("i=block/classifier");
    gates:
        input uppergateIn; // from MAC layer
        output upperControlOut; // output control information
        output uppergateOut; // to MAC layer
        input radioIn; // gate for sendDirect

}
