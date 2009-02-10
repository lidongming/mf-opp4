package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        SnrDecider.ned
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
// Decider module to be used with SnrEval
//
simple SnrDecider
{
    parameters:
        bool debug; // debug switch
        volatile double snrThresholdLevel;
    gates:
        output uppergateOut;
        input lowergateIn;
}
