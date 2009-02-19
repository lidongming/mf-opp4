/*
 *  author: Jerome Rousselot
 *  copyright: CSEM 2007
 *
 * All Mac protocols subclassing this class will be resolvable by the UBiquitousARP resolver.
 */

# ifndef IPINTERFACE_H
# define IPINTERFACE_H

#include <iostream>
#include "MACAddress.h"
#include "IPAddress.h"
#include "BasicLayer.h"

using namespace std;

/**
 * @brief A subclass of BasicLayer which provides an IP and a MAC Address.
 *
 * For more info, see the NED file.
 *
 * @ingroup macLayer
 * @author Jérôme Rousselot
 */

class BasicIPMacLayer: public BasicLayer
{
public:

	  virtual void initialize(int stage);
	  virtual void finish();
	  
	  MACAddress getMACAddress();
	  IPAddress getIPAddress();
	  void setAddresses(const char * addressString, const unsigned int ipvalue);
	  
	  
protected:
    /*
     * @brief MAC Address
     */
    MACAddress macaddress;
	
	/*
	 * @brief IP Address
	 */
	IPAddress ipaddress;
};
#endif

