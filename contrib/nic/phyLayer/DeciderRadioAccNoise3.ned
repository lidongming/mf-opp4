package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        Decider802154.ned
// *
// * author:      Marc Loebbers, Amre El-Hoiydi, Jérôme Rousselot
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *              (C) 2007-2008 CSEM
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
// Decider module of the 802.15.4 modules
//
simple DeciderRadioAccNoise3
{
    parameters:
        bool debug; // debug switch
        double berLowerBound; // minimum possible bit error rate (BER floor).
    gates:
        output uppergateOut;
        output upperControlOut;
        input lowergateIn;
        input lowerControlIn;
}
