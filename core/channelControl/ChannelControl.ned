package org.mf2.core.channelControl;

//***************************************************************************
// * file:        ChannelControl.ned
// *
// * author:      Steffen Sroka, Daniel Willkomm
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


// Module to control the channel and handle all connection
// related stuff
//
// The central module that coordinates the connections between all
// nodes, and handles dynamic gate creation.  ChannelControl therefore
// periodically communicates with the mobility module and
// ChannelAccess.
//
// The four parameters pMax, sat, alpha, and carrierFrequency are used
// to calculate the interference distance between nodes. The values used
// here in ChannelControl are used to calculate the upper bound,
// i.e. they can be redifined in the snrEval modules, but never such
// that the maximal interference distance is exeeded.
//       
// @author Steffen Sroka, Daniel Willkomm
// @see BasicMobility
//
simple ChannelControl
{
    parameters:
        // debug switch for core framework
        bool coreDebug; // debug switch for core framework
        bool sendDirect;          // define which ChannelControl module to use
        bool useTorus;          // wrap connections
        double playgroundSizeX; // x size of the playground (in meters)
        double playgroundSizeY; // y size of the playground (in meters)
        // maximum sending power used for this network (in mW)
        double pMax; // maximum sending power used for this network (in mW)
        // minimum signal attenuation threshold (in dBm)
        double sat; // minimum signal attenuation threshold (in dBm)
        // minimum path loss coefficient
        double alpha; // minimum path loss coefficient
        // minimum carrier frequency of the channel (in Hz)
        double carrierFrequency; // minimum carrier frequency of the channel (in Hz)
}

