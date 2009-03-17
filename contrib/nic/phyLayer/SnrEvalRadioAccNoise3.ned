package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        SnrEvalRadioAccNoise3.ned
// *
// * author:      Marc Loebbers, Amre El-Hoiydi, Jérôme Rousselot
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *              (C) 2007 CSEM
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


simple SnrEvalRadioAccNoise3
{
    parameters:
        bool debug = default(false); // debug switch
        bool trace; // vector log switch
        bool stats; // scalar statistics switch
        double headerLength @unit(bit); // Total length of PHY header (usually sync+sfd) 
        double transmitterPower @unit(mW);  // Radiated power at antenna.
        double carrierFrequency @unit(Hz);  // Carrier frequency of the signal (Used for pathloss evaluation).
        double thermalNoise @unit(dBm);      // Thermal noise in receiver.
        bool publishRSSIAlways;				// Always publish RSSI changes to the blackboard independently of  the radio state.
        double sfdLength @unit(bit); // Length of Start Frame Delimiter (used to compute probability of successful synchronization)

    gates:
        input uppergateIn;
        input radioIn;
        output uppergateOut;
        output upperControlOut;
}
