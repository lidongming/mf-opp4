//
// BMAC for MF 2.02
//
//  Created by Anna Foerster on 10/10/08.
//  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
//
//

package org.mf2.networks.bmac;

import org.mf2.contrib.applLayer.BurstApplLayer;
import org.mf2.contrib.bmac.NicBMAC;
import org.mf2.contrib.mobility.ConstSpeedMobility;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.core.blackboard.Blackboard;


module BMACHost

{
    parameters:
        @display("bgb=250,250,white,,;bgp=10,10");
    gates:
        input radioIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=200,39");

        }
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=200,114");

        }
        appl: BurstApplLayer {
            parameters:
                @display("p=90,50");

        }
        arp: SimpleArp {
            parameters:
                @display("p=200,189");

        }
        net: SimpleNetwLayer {
            parameters:
                @display("p=90,114");

        }
        nic: NicBMAC {
            parameters:
                @display("p=90,179");

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
