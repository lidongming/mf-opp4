//***************************************************************************
//* file:        BaseStationNetwork.ned
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
//* description: A centalized base station example network
//*
//***************************************************************************
//* changelog:   $Revision: 114 $
//*              last modified:   $Date: 2004-09-30 15:58:26 +0200 (Do, 30 Sep 2004) $
//*              by:              $Author: omfw-willkomm $
//***************************************************************************

package org.mf2.networks.baseStationNet;

import org.mf2.core.channelControl.ChannelControl;


// A WLAN like base station controlled network
//
// A network consisting of a base station and numClients clients. The base 
// station is fixed and the clients move around. The base station periodically
// polls all reachable clients in a round robin manner
//
// @author Daniel Willkomm
module BaseStationNetwork
{
    parameters:
        double playgroundSizeX; // x size of the area the nodes are in (in meters)
        double playgroundSizeY; // y size of the area the nodes are in (in meters)
        double numClients; // total number of clients in the network

        @display("bgb=$playgroundSizeX,$playgroundSizeY,white,,;bgp=10,10");
    submodules:
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=60,30;i=eye14");
        }
        baseStationHost: BaseStationHost {
            parameters:
                numClients = numClients;
                @display("50=50;i=pctower2");
        }
        clientHost[numClients]: ClientHost {
            parameters:
                @display("p=50,50;i=laptop");
        }
    connections allowunconnected:
                         // all connections and gates are to be generated dynamically

}


// example centralized base station network simulation for the Mobility Framework
network baseStationSim extends BaseStationNetwork
{
    parameters:
}

