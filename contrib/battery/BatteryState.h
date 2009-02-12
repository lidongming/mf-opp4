/*
 *  BatteryState.h
 *
 *  Created by Anna Foerster on 10/31/06.
 *  Copyright 2006-2009 Universita della Svizzera Italiana. All rights reserved.
 *
 */

#ifndef BATTERY_STATE_H
#define BATTERY_STATE_H

#include <omnetpp.h>
#include <Blackboard.h>
#include <sstream>

/**
 * @brief Class to hold the battery state of a host
 * @author Anna Foerster
 * @sa Blackboard
 */
class  BatteryState : public BBItem
{
    BBITEM_METAINFO(BBItem);

protected:
    /** @brief holds the state for this battery (0 .. 1.0) */
    double state;

public:

    /** @brief Constructor*/
    BatteryState(double r=0.f) : BBItem(), state(r) {
    };

    /** @brief get current battery */
    double getBatteryState() const  {
        return state;
    }

    /** @brief set battery */
    void setBatteryState(double r) {
        state = r;
    }

    /** @brief Enables inspection */
    std::string info() const {
        std::ostringstream ost;
        ost << " battery state level is "<<state<<" mA";
        return ost.str();
    }
};

#endif


