package org.mf2.contrib.nic;
//***************************************************************************
// * file:        Nic80211.ned
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
// **************************************************************************/

import org.mf2.contrib.nic.macLayer.Mac80211;
import org.mf2.contrib.nic.phyLayer.Decider80211;
import org.mf2.contrib.nic.phyLayer.SnrEval80211;
import org.mf2.contrib.nic.radios.SingleChannelRadio;


//
// This NIC implements an 802.11 network interface card.
//
// @see Mac80211, Decider80211, SnrEval80211
// @author Marc Loebbers
//
module Nic80211
{
    @display("i=block/ifcard");
    gates:
        input uppergateIn; // to upper layers
        output uppergateOut; // from upper layers
        output upperControlOut; // control information 
        input radioIn; // radioIn gate for sendDirect

    submodules:
        mac: Mac80211 {
            parameters:
                @display("p=96,48;i=block/layer");
        }
        decider: Decider80211 {
            parameters:
                @display("p=137,126;is=s");
        }
        snrEval: SnrEval80211 {
            parameters:
                @display("p=96,192;i=block/wrxtx");
        }
        radio: SingleChannelRadio {
            @display("p=32,48");
        }
    connections:
        radioIn --> snrEval.radioIn;
        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;
        snrEval.upperControlOut --> decider.lowerControlIn;
        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        decider.upperControlOut --> mac.lowerControlIn;

        mac.uppergateOut --> uppergateOut;
        mac.upperControlOut --> upperControlOut;
        mac.uppergateIn <-- uppergateIn;
}

