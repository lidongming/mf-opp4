/*
 *
 *  author: Jerome Rousselot
 *  copyright: CSEM 2007
 *
 * All Mac protocols subclassing this class will be resolvable by the UBiquitousARP resolver.
 *
 */

#include "BasicIPMacLayer.h"

MACAddress BasicIPMacLayer::getMACAddress()
{
	Enter_Method_Silent();
	return macaddress;
}

IPAddress BasicIPMacLayer::getIPAddress()
{
	Enter_Method_Silent();
	return ipaddress;
}

void BasicIPMacLayer::setAddresses(const char * addressString, unsigned int ipvalue)
{
	Enter_Method_Silent();
	if (!strcmp(addressString, "auto")) {
       	// assign automatic address
       	macaddress = MACAddress::generateAutoAddress();
    } else {
		macaddress.setAddress(addressString);
	}
	ipaddress.set(ipvalue);
	recordScalar("Address", ipvalue);
	return;
}

void BasicIPMacLayer::initialize(int stage)
{
	BasicLayer::initialize(stage);
}
void BasicIPMacLayer::finish()
{
	BasicLayer::finish();
}
