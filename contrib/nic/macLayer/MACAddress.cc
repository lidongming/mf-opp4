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
 * changelog:   imported from omnetpp INET Framework 20061020 into
 * 				mobility framework 2.0p3.
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 **************************************************************************/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <ctype.h>
#include "MACAddress.h"


unsigned int MACAddress::autoAddressCtr;

//
// Converts hex string into a byte array 'destbuf'. Destbuf is 'size'
// chars long -- if hext string is shorter, destbuf is filled with zeros;
// if destbuf is longer, it is truncated. Non-hex characters are
// discarded before conversion. Returns number of bytes converted from hex.
//
static int hextobin(const char *hexstr, unsigned char *destbuf, int size)
{
    int k=0;
    const char *s = hexstr;
    for (int pos=0; pos<size; pos++)
    {
        if (!s || !*s)
        {
            destbuf[pos] = 0;
        }
        else
        {
            while (*s && !isxdigit(*s)) s++;
            if (!*s) {destbuf[pos]=0; continue;}
            unsigned char d = isdigit(*s) ? (*s-'0') : islower(*s) ? (*s-'a'+10) : (*s-'A'+10);
            d = d<<4;
            s++;

            while (*s && !isxdigit(*s)) s++;
            if (!*s) {destbuf[pos]=0; continue;}
            d += isdigit(*s) ? (*s-'0') : islower(*s) ? (*s-'a'+10) : (*s-'A'+10);
            s++;

            destbuf[pos] = d;
            k++;
        }
    }
    return k;
}

const MACAddress MACAddress::UNSPECIFIED_ADDRESS;
const MACAddress MACAddress::BROADCAST_ADDRESS("ff:ff:ff:ff:ff:ff");

MACAddress::MACAddress()
{
    address[0]=address[1]=address[2]=address[3]=address[4]=address[5]=0;
}
MACAddress::MACAddress(unsigned long longaddr)
{
	for (int i = 0; i < 6; i++) {
		if(longaddr > 0) {
			address[5-i] = longaddr % 256;
			longaddr = ( longaddr - address[5-i] ) / 256;
		} else
			address[5-i] = 0;
	}
}
MACAddress::MACAddress(const char *hexstr)
{
    setAddress(hexstr);
}

MACAddress& MACAddress::operator=(const MACAddress& other)
{
    memcpy(address, other.address, MAC_ADDRESS_BYTES);
    return *this;
}

unsigned int MACAddress::getAddressArraySize() const
{
    return 6;
}

unsigned char MACAddress::getAddress(unsigned int k) const
{
    if (k>=6) throw cRuntimeError("Array of size 6 indexed with %d", k);
    return address[k];
}

unsigned long MACAddress::getAddress() const
{
	unsigned long longaddr = 0;
	
	for(int i=0; i < 6; i++) 
	{
		longaddr = longaddr*256+address[i];
	}
	return longaddr;
}
void MACAddress::setAddress(unsigned int k, unsigned char addrbyte)
{
    if (k>=6) throw cRuntimeError("Array of size 6 indexed with %d", k);
    address[k] = addrbyte;
}

void MACAddress::setAddress(const char *hexstr)
{
    if (!hexstr)
        throw cRuntimeError("MACAddress::setAddress(const char *): got null pointer");
    if (hextobin(hexstr, address, MAC_ADDRESS_BYTES)!=MAC_ADDRESS_BYTES)
        throw cRuntimeError("MACAddress::setAddress(const char *): hex string \"%s\" too short, should be 12 hex digits", hexstr);
}

void MACAddress::setAddressBytes(unsigned char *addrbytes)
{
    memcpy(address, addrbytes, MAC_ADDRESS_BYTES);
}

void MACAddress::setBroadcast()
{
    address[0]=address[1]=address[2]=address[3]=address[4]=address[5]=0xff;
}

bool MACAddress::isBroadcast() const
{
    return (address[0]&address[1]&address[2]&address[3]&address[4]&address[5])==0xff;
}

bool MACAddress::isUnspecified() const
{
    return !(address[0] || address[1] || address[2] || address[3] || address[4] || address[5]);
}

std::string MACAddress::str() const
{
    char buf[20];
    char *s = buf;
    for (int i=0; i<MAC_ADDRESS_BYTES; i++, s+=3)
        sprintf(s,"%2.2X-",address[i]);
    *(s-1)='\0';
    return std::string(buf);
}

bool MACAddress::equals(const MACAddress& other) const
{
    return memcmp(address, other.address, MAC_ADDRESS_BYTES)==0;
}

int MACAddress::compareTo(const MACAddress& other) const
{
    return memcmp(address, other.address, MAC_ADDRESS_BYTES);
}

//InterfaceToken MACAddress::formInterfaceIdentifier() const
//{
//    const unsigned char *b = address;
//    uint32 high = (b[0]<<24) | (b[1]<<16) | (b[2]<<8) | 0xff;
//    uint32 low =  (0xfe<<24) | (b[3]<<16) | (b[4]<<8) | b[5];
//    return InterfaceToken(low, high, 64);
//}

MACAddress MACAddress::generateAutoAddress()
{
    ++autoAddressCtr;

    unsigned char addrbytes[6];
    addrbytes[0] = 0x0A;
    addrbytes[1] = 0xAA;
    addrbytes[2] = (autoAddressCtr>>24)&0xff;
    addrbytes[3] = (autoAddressCtr>>16)&0xff;
    addrbytes[4] = (autoAddressCtr>>8)&0xff;
    addrbytes[5] = (autoAddressCtr)&0xff;

    MACAddress addr;
    addr.setAddressBytes(addrbytes);
    return addr;
}

