/* -*- mode:c++ -*- *******************************************************
 * file:        MacControlInfo.h
 *
 * author:      Daniel Willkomm
 *
 * copyright:   (C) 2005 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it 
 *              and/or modify it under the terms of the GNU General Public 
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later 
 *              version.
 *              For further information see file COPYING 
 *              in the top level directory
 **************************************************************************
 * part of:     framework implementation developed by tkn
 * description: - control info to pass next hop to the MAC layer
 **************************************************************************/

#ifndef MACCONTROLINFO_H
#define MACCONTROLINFO_H

#include <omnetpp.h>

/**
 * @brief Control info to pass next hop L2 addr from netw to MAC layer
 * 
 * Control Info to pass interface information from the network to the
 * MAC layer Currently the only information necessary is the MAC
 * address of the next hop, which has to be determined by ARP or some
 * similar mechanism
 *
 *
 * @ingroup basicUtils
 * @ingroup utils
 * @author Daniel Willkomm
 **/
class MacControlInfo : public cPolymorphic
{
  protected:
    /** @brief netw address of the sending or receiving node*/
    unsigned long nextHopMac;
    double errorRate;
    double rssi;
    
  public:
    /** @brief Default constructor*/
    MacControlInfo(const unsigned long addr,double ber=0.9, double rss=-10) : nextHopMac(addr),errorRate(ber), rssi(rss) {};
    /** @brief Destructor*/
    virtual ~MacControlInfo(){};

    /** @brief Getter method*/
    virtual const unsigned long getNextHopMac() {
	return nextHopMac;
    };

    /** @brief Setter method*/
    virtual void setNextHopMac(const unsigned long addr){
	nextHopMac = addr;
    };

    virtual const double getBER() {
	return errorRate;
    };
    
    virtual const double getRSSI() {
	return rssi;
    }
};


#endif
