//***************************************************************************
//* file:        BaseStationHost.ned
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
//* description: BaseStationHost Compound Module
//***************************************************************************

package org.mf2.networks.baseStationNet;

import org.mf2.contrib.applLayer.PollApplLayer;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.contrib.nic.CoreTestNic;
import org.mf2.core.basicModules.BasicMobility;
import org.mf2.core.blackboard.Blackboard;


// The BaseStationHost is a compound module that implements a test base station. 
// The test base station contains the PollApplLayer which periodically polls all
// clients in the network.
//
// @author Daniel Willkomm
module BaseStationHost
{
    parameters:
        string logName; //TKEnv display name
        double numClients; // number of clients in the network
        @display("bgb=180,200,white,,;bgp=10,10");
    gates:
        input radioIn; // gate for sendDirect
    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=130,70;b=25,25,,black,,");
        }
        mobility: BasicMobility {
            parameters:
                @display("p=130,140;i=cogwheel2");
        }
        appl: PollApplLayer {
            parameters:
                numClients = numClients;
                @display("b=35,26;p=60,50;i=app");
        }
        arp: SimpleArp {
            parameters:
                @display("p=200,225;b=30,25");
        }
        net: SimpleNetwLayer {
            parameters:
                @display("b=32,30;p=60,108;i=prot1");
        }
        nic: CoreTestNic {
            parameters:
                @display("b=32,30;p=60,166;i=iface");
        }
    connections:
        nic.uppergateOut --> net.lowergateIn;
        nic.uppergateIn <-- net.lowergateOut;
        nic.upperControlOut --> net.lowerControlIn;

        net.uppergateOut --> appl.lowergateIn;
        net.uppergateIn <-- appl.lowergateOut;
        net.upperControlOut --> appl.lowerControlIn;

        radioIn --> nic.radioIn;

}

