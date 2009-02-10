//***************************************************************************
// * file:        Network.ned
// *
// * author:      Marc Loebbers, der Schoenste!
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
// * description: a host in the 802.11 test network
// *              
// ***************************************************************************
// * changelog:   $Revision: 277 $
// *              last modified:   $Date: 2006-05-08 11:18:03 +0200 (Mo, 08 Mai 2006) $
// *              by:              $Author: koepke $
// **************************************************************************/


package org.mf2.networks.ieee802_11;

import org.mf2.contrib.applLayer.BurstApplLayer;
import org.mf2.contrib.mobility.ConstSpeedMobility;
import org.mf2.contrib.netwLayer.SimpleArp;
import org.mf2.contrib.netwLayer.SimpleNetwLayer;
import org.mf2.contrib.nic.Nic80211;
import org.mf2.core.blackboard.Blackboard;


module Host

{
    parameters:
        @display("bgb=250,250,white,,;bgp=10,10");
    gates:
        input radioIn; // gate for sendDirect

    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=200,75;b=30,25");

        }
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=200,150;b=30,25");

        }
        appl: BurstApplLayer {
            parameters:
                @display("p=90,50;b=100,20,rect");

        }
        arp: SimpleArp {
            parameters:
                @display("p=200,225;b=30,25");

        }
        net: SimpleNetwLayer {
            parameters:
                @display("p=90,100;b=100,20,rect");

        }
        nic: Nic80211 {
            parameters:
                @display("p=90,150;b=100,20,rect");

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
