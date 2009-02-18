//***************************************************************************
//* file:        ClientHost.ned
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
//* description: ClientHost Compound Module
//***************************************************************************

package org.mf2.networks.baseStationNet;

import org.mf2.contrib.applLayer.BasicApplLayer;
import org.mf2.contrib.mobility.ConstSpeedMobility;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.contrib.nic.CoreTestNic;
import org.mf2.core.blackboard.Blackboard;


// The ClientHost is a compound module that implements a very simple mobile
// client. It only consists of basic modules and the ClientApplLayer.
//
// @author Daniel Willkomm
module ClientHost
{
    parameters:
        string logName; //TKEnv display name
        string applLayer; //application layer type

        @display("bgb=180,200,white,,;bgp=10,10");
    gates:
        input radioIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=130,70");
        }
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=130,140;i=cogwheel2");
        }
        appl: <applLayer> like BasicApplLayer {
            parameters:
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

