package org.mf2.contrib.nic;
//***************************************************************************
//* file:        AlohaSnrNic.ned
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
//* description:  snr network interace card Compound Module
//*
//***************************************************************************
//* changelog:   $Revision: 279 $
//*              last modified:   $Date: 2006-05-08 14:18:06 +0200 (Mo, 08 Mai 2006) $
//*              by:              $Author: koepke $
//***************************************************************************

import org.mf2.contrib.nic.macLayer.AlohaMacLayer;
import org.mf2.contrib.nic.phyLayer.SnrDecider;
import org.mf2.contrib.nic.radios.SingleChannelRadio;
import org.mf2.contrib.nic.phyLayer.SnrEval;


// This is the easiest nic to implement "real" network behaviour. It uses
// the CsmaMacLayer and the SnrDecider and SnrEval modules.
//
// The csma mac provides basic medium access and is used here to prevent a
// host from trying to send a message while it is still in receive mode.
//
// The phy layer modules allow to account for collision and take fading into
// account so that messages can actually be lost.
//
// IMPORTANT:<br> In order to work with the ChannelControl module the snrEval
// module has to be called "snrEval" in the ned file.
//
// @see BasicMacLayer, SnrDecider, SnrEval
//
// @author Daniel Willkomm
module AlohaSnrNic
{
    @display("i=block/ifcard");
    gates:
        input uppergateIn; // from upper layers
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection
        input radioIn; // to receive AirFrames

    submodules:
        mac: AlohaMacLayer {
            parameters:
                @display("p=60,50;b=32,30;i=prot2");
        }
        decider: SnrDecider {
            parameters:
                @display("p=83,99;b=16,16,,white;i=box2_s");
        }
        snrEval: SnrEval {
            parameters:
                @display("p=60,150;b=32,30;i=prot3");
        }
        radio: SingleChannelRadio {
            parameters:
                @display("p=121,45");

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

