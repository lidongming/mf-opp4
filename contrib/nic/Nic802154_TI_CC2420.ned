package org.mf2.contrib.nic;
//***************************************************************************
// * file:        Nic802154_TI_CC2420.ned
// *
// * author:      Jérôme Rousselot, Marc Loebbers
// *
// * copyright:   (C) 2008 CSEM SA, Neuchatel, Switzerland.
// *		  (C) 2004 Telecommunication Networks Group (TKN) at
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

import org.mf2.contrib.nic.radios.SingleChannelRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.DeciderRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.SnrEvalRadioAccNoise3;
import org.mf2.contrib.nic.macLayer.csma;


//
// This NIC implements a Texas Instruments CC 2420 802.15.4 network interface card.
// @author Jerome Rousselot
//

module Nic802154_TI_CC2420

{
    parameters:
        bool tiDebug;
        double MacQueueLength;
        double MacMaxTxAttempts;
        double MacMinBE;
        double MacMaxBE;
        double MacMaxCSMABackoffs;
        double MacMaxFrameRetries;
        //       MacDoStats : const;	
        double emissionPower;


    gates:
        input uppergateIn; // from upper layers
        input radioIn; // to receive AirFrames
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection

    submodules:
        decider: DeciderRadioAccNoise3 {
            parameters:
                debug = tiDebug;
                berLowerBound = 1E-6; // lower bound on ber performance, between 0 and 1.
                @display("p=106,157;i=block/process_s");

        }
        snrEval: SnrEvalRadioAccNoise3 {
            parameters:
                debug = tiDebug;
                publishRSSIAlways = false;
                headerLength = 48; // ieee 802.15.4
                transmitterPower = emissionPower;
                carrierFrequency = 2.4E+9;
                thermalNoise = -110;
                sfdLength = 8;
                @display("p=96,236;i=block/wrxtx");

        }
        radio: SingleChannelRadioAccNoise3 {
            parameters:
                debug = false;
                bitrate = 250000 bps;
                delaySetupRx = 0.001792 s;
                delaySetupTx = 0.001792 s;
                delayRxTx = 0.000192 s;
                delayTxRx = 0.000192 s;
                sleepPower = 0.0000000693 W; // 0.021 microA * 3.3 V
                rxPower = 0.06204 W;   // 18.8 mA * 3.3 V
                txPower = 0.05742 W;   // 17.4 mA * 3.3 V
                rxTxPower = 0.06204 W; // Upper bound : 18.8 mA * 3.3 V
                txRxPower = 0.06204 W; // idem
                setupTxPower = 0.002109 W; // (ESetupPower+ESetupXtal+ETxCalibrate)/TSetupTx
                setupRxPower = 0.002259 W; // (ESetupPower+ESetupXtal+ERxCalibrate)/TSetupRx
                @display("p=200,30;b=30,25");

        }
        mac: csma {
            parameters:
                debug = false;
                headerLength = 72;
                queueLength = MacQueueLength;
                maxTxAttempts = MacMaxTxAttempts;
                bitrate = 250000;
                defaultChannel = 0;
                busyRSSI = -97;
                ccaDetectionTime = 0.000128; // 8 symbols
                rxSetupTime = 0.001792;
                aTurnaroundTime = 0.000192;    // 12 symbols
                sifs = 0.000192;              // 12 symbols
                macMinBE = MacMinBE;
                macMaxBE = MacMaxBE;
                macMaxCSMABackoffs = MacMaxCSMABackoffs;
                aUnitBackoffPeriod = 0.00032;
                macMaxFrameRetries = MacMaxFrameRetries;
                macAckWaitDuration = 0.00056;  // 1+12+10+12 symbols
                stats = true;
                ackLength = 40;

                @display("p=96,87;i=block/layer");

        }
    connections:

        radioIn --> snrEval.radioIn;

        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;
        snrEval.upperControlOut --> decider.lowerControlIn;

        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        decider.upperControlOut --> mac.lowerControlIn;

        mac.uppergateOut --> uppergateOut;
        mac.upperControlOut --> upperControlOut;
        mac.uppergateIn <-- uppergateIn;

}

