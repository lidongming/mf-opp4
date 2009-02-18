package org.mf2.contrib.nic.macLayer;
//***************************************************************************
//* file:        CSMAMacLayer.ned
//*
//* author:      Marc L�bbers
//*
//* copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
//*              Technische Universitaet Berlin, Germany.
//*
//*              This program is free software; you can redistribute it
//*              and/or modify it under the terms of the GNU General Public
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later
//*              version.
//*              For further information see file COPYING
//*              in the top level directory
//***************************************************************************
//* part of:     framework implementation developed by tkn
//* description: MAC layer class for the CSMAMac
//**************************************************************************/


//
// CSMA MAC protocol
//
simple CSMAMacLayer
{
    parameters:
        bool debug; // debug switch
        // level at which we consider medium busy
        double busyRSSI;
        // how long is a slot?
        double slotDuration;
        // maximum time between a packet and its ack
        double difs;
        // maximum number of transmission attempts
        double maxTxAttempts;
        // length of MAC header
        double headerLength;
        // length of the MAC queue
        double queueLength;
        // default channel 
        double defaultChannel;
        // bit rate 
        double bitrate;
        // contention window
        double contentionWindow;

        @display("i=block/ifcard");
    gates:
        input uppergateIn;
        output uppergateOut;
        input lowergateIn;
        output lowergateOut;
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from phy layer
}
