//
//  LMAC for MF 2.02, omnetpp 3.4
//
//  Created by Anna Foerster on 10/10/08.
//  Copyright 2008 Universita della Svizzera Italiana. All rights reserved.
//
//  Coverted to OMNeT++ 4 by Rudolf Hornig
//

package org.mf2.contrib.lmac;

import org.mf2.contrib.nic.phyLayer.SnrDecider;
import org.mf2.contrib.nic.phyLayer.SnrEval;
import org.mf2.contrib.nic.radios.SingleChannelRadio;


module NicLMACRPM
{
    gates:
        input uppergateIn; // from upper layers
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection

        input radioIn; // to receive AirFrames
    submodules:
        mac: LMACLayer {
            parameters:
                @display("p=96,87;i=block/layer");

        }
        decider: SnrDecider {
            parameters:
                @display("p=106,157;i=block/process_s");

        }
        snrEval: SnrEval { //  XXX FIXME was SnrEvalwithPropagation.
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
