package org.mf2.contrib.nic.phyLayer;

//***************************************************************************
// * file:        Decider80211.ned
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


//
// Decider module of the 802.11 modules
//
simple Decider80211
{
    parameters:
        bool debug; // debug switch
        double snirThreshold;
    gates:
        output uppergateOut;
        output upperControlOut;
        input lowergateIn;
        input lowerControlIn;
}
