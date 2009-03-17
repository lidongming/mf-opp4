package org.mf2.contrib.netwLayer;
//***************************************************************************
// * file:        SimpleNetwLayer.ned
// *
// * author:      Daniel Willkomm
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
// * description: network layer: general class for the network layer
// *              subclass to create your own network layer
// ***************************************************************************
// * changelog:   $Revision: 260 $
// *              last modified:   $Date: 2006-04-13 17:32:54 +0200 (Do, 13 Apr 2006) $
// *              by:              $Author: koepke $
// **************************************************************************/


// Basic class for the network layer
// 
// This is the generic class for all network layer modules. If you
// want to implement your own network layer you have to subclass your
// module from this class.
//
// @author Daniel Willkomm
simple SimpleNetwLayer
{
    parameters:
        bool debug = default(false); // debug switch for the core framework
        double headerLength; // length of the network packet header (in bits)
        @display("i=block/layer");
    gates:
        input uppergateIn; // from application layer
        output uppergateOut; // to application layer
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from mac layer
        input lowergateIn; // from MAC layer
        output lowergateOut; // to MAC layer
}
