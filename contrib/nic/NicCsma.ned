package org.mf2.contrib.nic;
//***************************************************************************
// * file:        NicCsma.ned
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

import org.mf2.contrib.nic.radios.SingleChannelRadio;
import org.mf2.contrib.nic.macLayer.CSMAMacLayer;
import org.mf2.contrib.nic.phyLayer.SnrDecider;
import org.mf2.contrib.nic.phyLayer.SnrEval;

//
// The CsmaNic contains a csma-like MAC layer.
// The BasicDecider is used all messages will be delivered to
// the MAC and no packets get lost
//
// This nic is intended to be used in order to test the implementation
// of the csma protocol.
//
// IMPORTANT:<br> In order to work with the ChannelControl module the snrEval
// module has to be called "snrEval" in the ned file.
//
// @see BasicLayer, BasicDecider, BasicSnrEval
// @author Marc Loebbers
//

module NicCsma
{
    gates:
        input uppergateIn; // from upper layers
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection

        input radioIn; // to receive AirFrames
    submodules:
        mac: CSMAMacLayer {
            parameters:
                @display("p=96,87;i=block/layer");

        }
        decider: SnrDecider {
            parameters:
                @display("p=106,157;i=block/process_s");

        }
        snrEval: SnrEval {
            parameters:
                @display("p=96,236;i=block/wrxtx");

        }
        radio: SingleChannelRadio {
            parameters:
                @display("p=200,30;b=30,25");

        }
    connections:
        radioIn --> snrEval.radioIn;
        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;

        snrEval.upperControlOut --> mac.lowerControlIn;
        mac.uppergateOut --> uppergateOut;

        mac.upperControlOut --> upperControlOut;
        mac.uppergateIn <-- uppergateIn;
}
