//***************************************************************************
//* file:        DVDNetwork.ned
//*
//* author:      Danilo Blasi
//*
//* copyright:   (C) 2008 ST Microelectronics (STM) at Lecce, Italy.
//*
//*              This program is free software; you can redistribute it 
//*              and/or modify it under the terms of the GNU General Public 
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later 
//*              version.
//*              For further information see file COPYING 
//*              in the top level directory.
//***************************************************************************
//* part of:     WASP Herd Control stack implementation
//* description: Test network of DVDHost modules
//***************************************************************************

package org.mf2.networks.dvdistanceTest;

import org.mf2.core.channelControl.ChannelControl;


// @brief Network of DVDHost modules, used for testing the DV Distance protocol.
// @author Danilo Blasi
module DVDNetwork

{
    parameters:

        double playgroundSizeX; // x-size of the network area [m]
        double playgroundSizeY; // y-size of the network area [m]
        double numHosts; // number of the network's hosts

        @display("bgb=$playgroundSizeX,$playgroundSizeY,white,,;bgp=10,10");

    submodules:

        channelcontrol: ChannelControl {
            parameters:

                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;

                @display("p=60,30;i=eye14");

        }
        host[numHosts]: DVDHost {
            parameters:
                @display("p=50,50;i=laptop");

        }
        simTracer: SimTracer;
    connections allowunconnected:

}

network dvdSim extends DVDNetwork
{
    parameters:
}
