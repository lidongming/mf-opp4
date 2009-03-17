//
//  LMACLayer.cc
// LMAC for MF 2.02, omnetpp 3.4
//
//  Created by Anna Foerster on 10/10/08.
//  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
//
//

package org.mf2.networks.lmac;

import org.mf2.contrib.applLayer.BurstApplLayer;
import org.mf2.contrib.lmac.NicLMAC;
import org.mf2.contrib.mobility.ConstSpeedMobility;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.core.blackboard.Blackboard;


module LMACHost

{
    parameters:
        @display("bgb=250,250,white,,");
    gates:
        input radioIn @directIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=200,50");

        }
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=200,130");

        }
        appl: BurstApplLayer {
            parameters:
                @display("p=90,50");

        }
        arp: SimpleArp {
            parameters:
                @display("p=200,208");

        }
        net: SimpleNetwLayer {
            parameters:
                @display("p=90,130");

        }
        nic: NicLMAC {
            parameters:
                @display("p=90,208");

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
