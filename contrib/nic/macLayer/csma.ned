package org.mf2.contrib.nic.macLayer;
//***************************************************************************
//* file:        CSMA.ned
//*
//* author:      Jerome Rousselot, Marc Loebbers
//* 
//* copyright:   (C) 2007 CSEM SA
//*		 (C) 2004 Telecommunication Networks Group (TKN) at
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
// csma protocol
//
simple csma
{
    parameters:

        // debug switch
        bool debug;
        bool stats;
        bool trace;
        // level at which we consider medium busy (in dBm)
        double busyRSSI;

        // simple interframe space. Time to wait between receiving a frame and acknowledging it.
        // Should be bigger than the maximum time for switching between Tx and Rx at the receiver.
        double sifs;
        // maximum number of transmission attempts
        double maxTxAttempts;
        // length of MAC header
        double headerLength;
        // size of the MAC queue (maximum number of packets in Tx buffer)
        double queueLength;
        // default channel 
        double defaultChannel;
        // bit rate 
        double bitrate;
        // complete MAC ack message length (in bits) 
        // (! headerLength is not added to this)
        double ackLength;
        // minimum backoff exponent
        double macMinBE;
        // maximum backoff exponent
        double macMaxBE;
        // maximum backoffs
        double macMaxCSMABackoffs;
        // maximum number of frame retransmission
        double macMaxFrameRetries;
        // base unit for all backoff calculations
        double aUnitBackoffPeriod;
        // time to wait for an acknowledgement after transmitting a unicast frame.
        double macAckWaitDuration;
        // Clear Channel Assessment detection time
        double ccaDetectionTime;
        // Time to setup radio to reception state
        double rxSetupTime;
        // Time to switch radio from Rx to Tx state
        double aTurnaroundTime;
        // MAC address as hex string (12 hex digits), or
        // "auto". "auto" values will be replaced by
        // a generated MAC address in init stage 0.
        string macaddress;
        // IPv4 address *as a decimal integer !*    
        double ipaddress;

    gates:
        input uppergateIn;
        output uppergateOut;
        input lowergateIn;
        output lowergateOut;
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from phy layer
}

