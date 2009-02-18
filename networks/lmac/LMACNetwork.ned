package org.mf2.networks.lmac;

import org.mf2.core.channelControl.ChannelControl;


network Sim
{
    parameters:
        // parametes for the framework
        double playgroundSizeX = default(40);
        double playgroundSizeY = default(40);
        double numHosts = default(2);
        @display("bgs=10;bgg=10;bgi=background/terrain");
    submodules:
//        pm: PropagationModel;  // XXX FIXME workd only with the probabilistic radio model patch
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=50,25");
        }
        host[numHosts]: LMACHost {
            parameters:
                @display("p=50,50;i=misc/node2_vs,#f49600,100;is=vs");

        }
}

