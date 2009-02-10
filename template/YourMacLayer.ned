//****************************************************************************
// * file:        YourMacLayer.ned
// *
// * author:      Your Name
// *
// * copyright:   (C) 2004 Your Institution
// *
// *              This program is free software; you can redistribute it 
// *              and/or modify it under the terms of the GNU General Public 
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later 
// *              version.
// *              For further information see file COPYING 
// *              in the top level directory
// ***************************************************************************
// * part of:     Your Simulation
// * description: - Your Description
// *
// ***************************************************************************
// * changelog:   $Revision: 388 $
// *              last modified:   $Date: 2006-08-10 11:02:52 +0200 (Do, 10 Aug 2006) $
// *              by:              $Author: willkomm $
// ***************************************************************************/


simple YourMacLayer
{
    parameters:
        bool debug; // debug switch
        // level at which we consider medium busy
        double busyRSSI;
        // length of MAC header
        double headerLength;
        // length of the MAC queue
        double defaultChannel;
        // bit rate 
        double bitrate;
    gates:
        input uppergateIn;
        output uppergateOut;
        input lowergateIn;
        output lowergateOut;
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from phy layer
}
