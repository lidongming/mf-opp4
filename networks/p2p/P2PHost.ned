//***************************************************************************
//* file:        P2PHost.ned
//*
//* author:      Daniel Willkomm
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
//* description: P2PHost Compound Module
//***************************************************************************

package org.mf2.networks.p2p;

import org.mf2.contrib.applLayer.BasicApplLayer;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.contrib.nic.P2PNic;
import org.mf2.core.basicModules.BasicMobility;
import org.mf2.core.blackboard.Blackboard;


// The P2PHost is a compound module that implements a very
// simple peer to peer host. Instead of an "snrEval" and "decider" module
// it just contains a simple "phy" module that sends unicast messages
//
// @author Daniel Willkomm
module P2PHost
{
    parameters:
        string applLayer; // application layer type

        @display("bgb=180,200,white,,;bgp=10,10");
    gates:
        input radioIn @directIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=130,33");
        }
        mobility: BasicMobility {
            parameters:
                @display("p=130,166;i=cogwheel2");
        }
        arp: SimpleArp {
            parameters:
                @display("p=130,100");
        }
        appl: <applLayer> like BasicApplLayer {
            parameters:
                @display("b=35,26;p=60,50;i=app");
        }
        net: SimpleNetwLayer {
            parameters:
                @display("b=32,30;p=60,108;i=prot1");
        }
        nic: P2PNic {
            parameters:
                @display("b=32,30;p=60,166;i=iface");

        }
    connections:
        nic.uppergateOut --> net.lowergateIn;
        nic.uppergateIn <-- net.lowergateOut;
        nic.upperControlOut --> net.lowerControlIn;

        net.uppergateOut --> appl.lowergateIn;
        net.upperControlOut --> appl.lowerControlIn;
        net.uppergateIn <-- appl.lowergateOut;

        radioIn --> nic.radioIn;

}
