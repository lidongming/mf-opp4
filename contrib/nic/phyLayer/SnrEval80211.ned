package org.mf2.contrib.nic.phyLayer;

//***************************************************************************
// * file:        SnrEval80211.ned
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


simple SnrEval80211
{
    parameters:
        bool debug = default(false); // debug switch
        double headerLength; // ignored
        double transmitterPower;
        double carrierFrequency;
        double thermalNoise;
        double pathLossAlpha;
        bool publishRSSIAlways;
        double sensitivity;

        @display("i=block/mac");
    gates:
        input uppergateIn;
        input radioIn;
        output uppergateOut;
        output upperControlOut;
}
