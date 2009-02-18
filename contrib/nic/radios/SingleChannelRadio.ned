package org.mf2.contrib.nic.radios;
//***************************************************************************
// * file:        SingleChannelRadio.ned
// *
// * author:      Andreas Koepke
// *
// * copyright:   (C) 2005 Telecommunication Networks Group (TKN) at
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

simple SingleChannelRadio
{
    parameters:
        bool debug;
        // time parameters, how long does it take to 
        // switch to sleep mode
        double swSleep;
        // switch to send mode
        double swSend;
        // switch to recv mode
        double swRecv;
        @display("i=device/antennatower");
}
