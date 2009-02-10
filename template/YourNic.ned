//***************************************************************************
//* file:        YourNic.ned
//*
//* author:      Marc Loebbers
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
//* description: template file for a nic definition
//*
//***************************************************************************
//* changelog:   $Revision: 32 $
//*              last modified:   $Date: 2004-06-04 16:13:09 +0200 (Fr, 04 Jun 2004) $
//*              by:              $Author: omfw-loebbers $
//***************************************************************************

import YourMacLayer;
import YourDecider;
import YourSnrEval;


// The nic (network interface card) is a compound module that contains the 
// MAC and PHY layerof a host within the Mobility Framework
//
// The BasicNic is the most simple nic you can create for the MF
//
// IMPORTANT:<br> In order to work with the ChannelControl module the snrEval
// module has to be called "snrEval" in the ned file.
//
//
// @author YOU!
module YourNic
{
    parameters:
        @display("bgb=101,180,white,,;bgp=10,10");
    gates:
        input uppergateIn; // to upper layers
        output uppergateOut; // from upper layers
    submodules:
        mac: YourMacLayer {
            parameters:
                @display("p=60,50;b=32,30;i=prot2");
        }
        decider: YourDecider {
            parameters:
                @display("p=68,100;b=16,16,,white,,;i=box2_s");
        }
        snrEval: YourSnrEval {
            parameters:
                @display("p=60,150;b=32,30;i=prot3");
        }
    connections:
        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;

        mac.uppergateOut --> uppergateOut;
        mac.uppergateIn <-- uppergateIn;

}

