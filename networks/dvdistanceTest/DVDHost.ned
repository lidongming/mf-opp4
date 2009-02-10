//***************************************************************************
//* file:        DVDHost.ned
//*
//* author:      Danilo Blasi
//*
//* copyright:   (C) 2008 St Microelectronics (STM) at Lecce, Italy.
//*
//*              This program is free software; you can redistribute it
//*              and/or modify it under the terms of the GNU General Public
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later
//*              version.
//*              For further information see file COPYING
//*              in the top level directory.
//***************************************************************************
//* part of:     WASP Herd Control stack implementation
//* description: Node module used for testing the DV Distance protocol
//***************************************************************************

package org.mf2.networks.dvdistanceTest;

import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.core.basicModules.BasicMobility;
import org.mf2.core.blackboard.Blackboard;


// @brief Node module used for testing the DV Distance protocol. It includes
//        the DVDistance simple module that works at network layer and on top
//        of a network interface card (nic) providing RSSI values for incoming
//        signals (current choice for nic is Nic802154_TI_CC2420 simple module).
// @author Danilo Blasi
module DVDHost

{
    parameters:

        @display("bgb=180,200,white,,;bgp=10,10");

    gates:

        input radioIn; // input gate for direct sending messages to the host

    submodules:

        blackboard: Blackboard {
            parameters:
                @display("p=130,50;b=25,25,,black,,");

        }
        mobility: BasicMobility {
            parameters:
                @display("p=130,150;i=cogwheel2");

        }
        arp: SimpleArp {
            parameters:
                @display("p=130,100;b=25,25");

        }
        net: DVDistance {
            parameters:
                @display("b=32,30;p=60,108;i=prot1");

        }
        nic: Nic802154_TI_CC2420 {
            parameters:
                @display("b=32,30;p=60,166;i=iface");

        }
    connections allowunconnected:

        nic.uppergateOut --> net.lowergateIn;
        nic.uppergateIn <-- net.lowergateOut;
        nic.upperControlOut --> net.lowerControlIn;
        radioIn --> nic.radioIn;

}
