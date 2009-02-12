// 
// file:        BatteryModule.ned
//
// author:     Anna Egorova-Foerster
//
// copyright:   (C) 2006 University of Lugano, Switzerland.
//
//              This program is free software; you can redistribute it 
//              and/or modify it under the terms of the GNU General Public 
//              License as published by the Free Software Foundation; either
//              version 2 of the License, or (at your option) any later 
//              version.
//              For further information see file COPYING 
//              in the top level directory
// 
// part of:     Smart Feedback-Based Network Framework
//  
// Modifications:
// 2009-02-11 Rudolf Hornig: ported to OMNeT++ 4 and refactored 
// 

package org.mf2.contrib.battery;

//
// A linear battery
//
// Tracks the RadioState of the node(RECV and TX) and the simTime and subtracts the used value every time the
// state has changed or every N seconds ( the updateInterval parameter of the module).
// Publishes the battery state as 0..1 double on the blackboard.
// The battery stats are also written to an output vector. 
//
simple Battery
{
    parameters:
        double maxBatteryCapacity @unit(mAh) = default(3000mAh); // the max battery capacity of this type of nodes in mAh
        double batteryCapacity @unit(mAh) = default(this.maxBatteryCapacity);	// the initial capacity of the battery in mAh
        double batteryTX @unit(mA) = default(35mA);    // current drawn during SEND with full power (MICA 2 default value: RADIO + CPU)
        double batteryRECV @unit(mA) = default(18mA);  // current drawn during RECV (MICA 2 default value: RADIO + CPU)
        double batterySLEEP @unit(mA) = default(15uA); // current drawn during SLEEP (MICA 2 default value: both RADIO and CPU sleep)
        int updateInterval @unit(s) = default(1s); // how often to update the battery state
}
