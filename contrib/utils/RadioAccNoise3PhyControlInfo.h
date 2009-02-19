/* -*- mode:c++ -*- *******************************************************
 * file:        RadioAccNoise3PhyControlInfo.h
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2007 CSEM SA
 *              This program is free software; you can redistribute it 
 *              and/or modify it under the terms of the GNU General Public 
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later 
 *              version.
 *              For further information see file COPYING 
 *              in the top level directory
 **************************************************************************
 * part of:     framework implementation developed by tkn
 * description: - control info to pass physical information 
 **************************************************************************/

#ifndef RADIOACCNOISE3_PHYCONTROLINFO_H
#define RADIOACCNOISE3_PHYCONTROLINFO_H

#include <omnetpp.h>
#include "PhyControlInfo.h"
/**
 * @brief Control info to pass information to and from the PHY layer
 *
 * The physical layer needs some additional information from the MAC,
 * like the bit rate to use, and passes up some information about the
 * signal strength. 
 * 
 *
 * @author Jerome Rousselot
 **/
class RadioAccNoise3PhyControlInfo:public PhyControlInfo
{
protected:
	/** @brief preamble length in bits */
  long preambleLength;
  double errorRate;
  double rssi;
  
public:
    /** @brief Default constructor*/
 RadioAccNoise3PhyControlInfo(const double br = 0, const double s = 0, const long pl = 16, const double ber=1E-6, double rss=-10):PhyControlInfo(br, s),
    preambleLength(pl), errorRate(ber), rssi(rss)
  {
  };

    /** @brief Destructor*/
  virtual ~ RadioAccNoise3PhyControlInfo() {
  };

    /** @brief get preamble length in bits */
  virtual const long getPreambleLength()
  {
    return preambleLength;
  };

  virtual const double getBER()
  {
    return errorRate;
  };
  
  virtual const double getRSSI()
  {
      return rssi;
  };
  
    /** @brief set preamble length in bits */
  virtual void getPreambleLength(const long pl)
  {
    preambleLength = pl;
  };

};

#endif
