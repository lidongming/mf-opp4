//****************************************************************************
// * file:        TestTransportAppl.ned
// *
// * author:      Matteo Migliavacca
// *
// * copyright:   (C) 2007 Polimi
// *
// ***************************************************************************


package org.mf2.networks.tcpTest;

// Application layer to test lower layer implementations. Generate periodic traffic addressed to a sink.
//
// @sa TestTransportAppl
// @author Matteo Migliavacca
simple TestTransportAppl
{
    parameters:
        bool debug;
        double headerLength; // length of the application message header (in bits)
    gates:
        input lowergateIn; // from network layer
        input lowerControlIn;
        output lowergateOut; // to network layer
}
