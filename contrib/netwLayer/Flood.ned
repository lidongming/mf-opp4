package org.mf2.contrib.netwLayer;
//***************************************************************************
// * file:        Flood.ned
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
// * description: a simple flooding protocol
// *              the user can decide whether to use plain flooding or not
// ***************************************************************************
// * changelog:   $Revision: 283 $
// *              last modified:   $Date: 2006-05-26 07:59:19 +0200 (Fr, 26 Mai 2006) $
// *              by:              $Author: koepke $
// **************************************************************************/


//
// A simple flooding protocol
//
// This implementation uses plain flooding, i.e. it "remembers"
// (stores) already broadcasted messages in a list and does not
// rebroadcast them again, if it gets another copy of that message.
//
// The maximum number of entires for that list can be defined in the
// .ini file (@ref bcMaxEntries) as well as the time after which an entry
// is deleted (@ref bcDelTime).
//
// If you prefere a memory-less version you can comment out the 
// <verbatim> #define PLAINFLOODING </verbatim>
//
// @author Daniel Willkomm
//
simple Flood
{
    parameters:
        bool debug; // debug switch
        double headerLength; // length of the network header
        bool plainFlooding; // flag whether to use plain flooding
        // Max number of entries in the list of already broadcasted messages
        double bcMaxEntries;
        // Time after which an entry for an already broadcasted msg can be deleted
        double bcDelTime;
        double defaultTtl; // Default time-to-live (ttl) used for this module
    gates:
        input uppergateIn; // from application layer
        output uppergateOut; // to application layer
        output upperControlOut; // for control messages to upper layer
        input lowergateIn; // from MAC layer
        input lowerControlIn;   // for control messages from mac layer
        output lowergateOut; // to MAC layer
}
