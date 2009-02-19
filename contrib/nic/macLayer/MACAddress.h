/***************************************************************************
 * file:        MACAddress.cc
 *
 * author:      Jérôme Rousselot
 *
 * copyright:   (C) 2003 CTIE, Monash University
 *				(C) 2007 CSEM, Jérôme Rousselot
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 * changelog:   
 * 				June 2007:
 * 				imported from omnetpp INET Framework 20061020 into
 * 				mobility framework 2.0p3.
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 **************************************************************************/

#ifndef MACADDRESS_H_
#define MACADDRESS_H_

#include <string>
#include <omnetpp.h>


#define MAC_ADDRESS_BYTES 6


/**
 * @brief Stores an IEEE 802 MAC address (6 octets = 48 bits).
 * 
 * @ingroup macLayer
 * @author Jérôme Rousselot
 */
class MACAddress
{
  private:
    unsigned char address[6];   // 6*8=48 bit address
    static unsigned int autoAddressCtr; // global counter for generateAutoAddress()

  public:
    /** @brief Returns the unspecified (null) MAC address */
    static const MACAddress UNSPECIFIED_ADDRESS;

    /** @brief Returns the broadcast (ff:ff:ff:ff:ff:ff) MAC address */
    static const MACAddress BROADCAST_ADDRESS;

    /**
     * @brief Default constructor initializes address bytes to zero.
     */
    MACAddress();

    /**
     * @brief Constructor which accepts hex string or the string "auto".
     */
    MACAddress(const char *hexstr);
    
    /*
     * @brief Constructor which accepts a mac address as an unsigned long int. 
     */
    MACAddress(unsigned long longaddr);

    /**
     * @brief Copy constructor.
     */
    MACAddress(const MACAddress& other) {operator=(other);}

    /**
     * @brief Assignment.
     */
    MACAddress& operator=(const MACAddress& other);

    /**
     * @brief Returns 6.
     */
    unsigned int getAddressArraySize() const;

    /**
     * @brief Returns the kth byte of the address.
     */
    unsigned char getAddress(unsigned int k) const;
    
    /**
     * @brief Returns address as a long.
     */
    unsigned long getAddress() const;

    /**
     * @brief Sets the kth byte of the address.
     */
    void setAddress(unsigned int k, unsigned char addrbyte);

    /**
     * @brief Converts address value from hex string. The string "auto" is also
     * accepted, it'll generate a unique address starting with "A0 00".
     */
    void setAddress(const char *hexstr);

    /**
     * @brief Returns pointer to internal binary representation of address (array of 6 unsigned chars).
     */
    unsigned char *getAddressBytes() {return address;}

    /**
     * @brief Sets address bytes. The argument should point to an array of 6 unsigned chars.
     */
    void setAddressBytes(unsigned char *addrbytes);

    /**
     * @brief Sets the address to the broadcast address (hex ff:ff:ff:ff:ff:ff).
     */
    void setBroadcast();

    /**
     * @brief Returns true this is the broadcast address (hex ff:ff:ff:ff:ff:ff).
     */
    bool isBroadcast() const;

    /**
     * @brief Returns true this is a multicast logical address (starts with bit 1).
     */
    bool isMulticast() const  {return address[0]&0x80;};

    /**
     * @brief Returns true if all address bytes are zero.
     */
    bool isUnspecified() const;

    /**
     * @brief Converts address to a hex string.
     */
    std::string str() const;

    /**
     * @brief Returns true if the two addresses are equal.
     */
    bool equals(const MACAddress& other) const;

    /**
     * @brief Returns true if the two addresses are equal.
     */
    bool operator==(const MACAddress& other) const {return (*this).equals(other);}

    /**
     * @brief Returns true if the two addresses are not equal.
     */
    bool operator!=(const MACAddress& other) const {return !(*this).equals(other);}

    /**
     * @brief Returns -1, 0 or 1 as result of comparison of 2 addresses.
     */
    int compareTo(const MACAddress& other) const;

    /**
     * Create interface identifier (IEEE EUI-64) which can be used by IPv6
     * stateless address autoconfiguration.
     */
    //InterfaceToken formInterfaceIdentifier() const;

    /**
     * @brief Generates a unique address which begins with 0a:aa and ends in a unique
     * suffix.
     */
    static MACAddress generateAutoAddress();

};

inline std::ostream& operator<<(std::ostream& os, const MACAddress& mac)
{
    return os << mac.str();
}

#endif


