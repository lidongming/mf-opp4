//***************************************************************************
// * file:        Host.ned
// *
// * author:      Marc Loebbers
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
// * description: a host for the aloha test network
// **************************************************************************/


package org.mf2.networks.aloha;

import org.mf2.contrib.applLayer.BasicApplLayer;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.contrib.nic.AlohaSnrNic;
import org.mf2.core.basicModules.BasicMobility;
import org.mf2.core.blackboard.Blackboard;


module Host

{
    parameters:
        string applType; //type of the application layer

        @display("bgb=250,250;bgp=10,10;i=device/pocketpc");
    gates:
        input radioIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=200,36");

        }
        mobility: BasicMobility {
            parameters:
                @display("p=200,114");

        }
        arp: SimpleArp {
            parameters:
                @display("p=200,189");

        }
        appl: <applType> like BasicApplLayer {
            parameters:
                @display("p=90,36");

        }
        net: SimpleNetwLayer {
            parameters:
                @display("p=90,114");

        }
        nic: AlohaSnrNic {
            parameters:
                @display("p=90,189");

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
