//***************************************************************************
// * file:        Network.ned
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
// * description: a network to test the 802.11 modules
// *              
// ***************************************************************************
// * changelog:   $Revision: 103 $
// *              last modified:   $Date: 2004-09-23 09:56:00 +0200 (Do, 23 Sep 2004) $
// *              by:              $Author: omfw-willkomm $
// **************************************************************************/

package org.mf2.networks.aloha;

import org.mf2.core.channelControl.ChannelControl;


network Sim
{
    parameters:
        // parametes for the framework
        double playgroundSizeX = default(40);
        double playgroundSizeY = default(40);
        double numHosts = default(2);

        @display("bgb=$playgroundSizeX,$playgroundSizeY,white,,;bgp=0,0");
    submodules:
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=48,38");
        }
        host[numHosts]: Host {
            parameters:
                @display("p=102,130;");

        }
    connections allowunconnected:
    // all connections and gates are to be generated dynamically

}
