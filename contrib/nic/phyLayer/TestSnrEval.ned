package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        TestSnrEval.ned
// *
// * author:      Daniel Willkomm
// *
// * copyright:   (C) 2006 Telecommunication Networks Group (TKN) at
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
// * description: class for the snrEval module, keeps track of the 
// *              snir levels during a packet reception
// ***************************************************************************


//
// Test SnrEval module to test core functionality
//
simple TestSnrEval
{
    parameters:
        bool coreDebug; // debug switch
        double headerLength;
        @display("i=block/mac");
    gates:
        input uppergateIn;
        input radioIn;
        output uppergateOut;
        output upperControlOut;
}
