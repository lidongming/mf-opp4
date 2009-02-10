package org.mf2.contrib.nic;
//***************************************************************************
//* file:        CoreTestNic.ned
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
//* description:  basic network interace card Compound Module
//*
//***************************************************************************

import org.mf2.contrib.nic.macLayer.AlohaMacLayer;
import org.mf2.contrib.nic.phyLayer.TestDecider;
import org.mf2.contrib.nic.phyLayer.TestSnrEval;
import org.mf2.contrib.nic.radios.SingleChannelRadio;


// The nic (network interface card) is a compound module that contains the 
// MAC and PHY of a host within the Mobility Framework
//
// The coreTesdNic is just for core testing purposes
//
// IMPORTANT:<br> In order to work with the ChannelControl module the snrEval
// module has to be called "snrEval" in the ned file.
//
// @author Daniel Willkomm
module CoreTestNic
{
    parameters:
        @display("bgb=101,180,white,,;bgp=10,10");
    gates:
        input uppergateIn; // to upper layers
        output uppergateOut; // from upper layers
        output upperControlOut; // control information 
        input radioIn; // radioIn gate for sendDirect
    submodules:
        mac: AlohaMacLayer {
            parameters:
                @display("p=60,50;b=32,30;i=prot2");
        }
        decider: TestDecider {
            parameters:
                @display("p=68,100;b=16,16,,white,,;i=box2_s");
        }
        snrEval: TestSnrEval {
            parameters:
                @display("p=60,150;b=32,30;i=prot3");
        }
        radio: SingleChannelRadio {
            parameters:
                @display("p=90,50;b=32,30,,red,,");

        }
    connections:
        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;
        snrEval.upperControlOut --> mac.lowerControlIn;
        mac.uppergateOut --> uppergateOut;
        mac.upperControlOut --> upperControlOut;
        mac.uppergateIn <-- uppergateIn;

        radioIn --> snrEval.radioIn;

}

