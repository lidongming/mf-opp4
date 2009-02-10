//***************************************************************************
// * file:        GENic.ned
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
// * description: an example nic using the Gilbert-Elliot modules and csma..
// *              
// ***************************************************************************
// * changelog:   $Revision: 272 $
// *              last modified:   $Date: 2006-05-04 16:37:37 +0200 (Do, 04 Mai 2006) $
// *              by:              $Author: koepke $
// **************************************************************************/


package org.mf2.networks.gilbert_elliot;

import org.mf2.contrib.nic.macLayer.CSMAMacLayer;
import org.mf2.contrib.nic.phyLayer.ErrAndCollDecider;
import org.mf2.contrib.nic.phyLayer.GilbertElliotSnr;
import org.mf2.contrib.nic.radios.SingleChannelRadio;


module GENic
{
    parameters:
        @display("bgb=101,180,white,,;bgp=10,10");
    gates:
        input radioIn; // to receive AirFrames
        input uppergateIn; // from upper layers
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection

    submodules:
        mac: CSMAMacLayer {
            parameters:
                @display("p=60,50;b=32,30;i=prot2");
        }
        decider: ErrAndCollDecider {
            parameters:
                @display("p=68,100;b=16,16,,white,,;i=box2_s");
        }
        snrEval: GilbertElliotSnr {
            parameters:
                @display("p=60,150;b=32,30;i=prot3");
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
