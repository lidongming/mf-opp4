//
// C++ Interface: TestTransportAppl
//
// Description: 
//
//
// Author: Matteo Migliavacca <migliava@lap-migliavacca>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef TEST_TRANSPORT_APPL_H
#define TEST_TRANSPORT_APPL_H

#include <vector>
#include "BasicApplLayer.h"
#include "NetwControlInfo.h"
#include "cstat.h"
#include "BasicIPMacLayer.h"
#include "TCPSocket.h"

using namespace std;


class TestTransportAppl : public BasicApplLayer, public TCPSocket::CallbackInterface
{
 public:


  /** @brief Initialization of the module and some variables*/
  virtual void initialize(int);
  //  virtual void finish();

  //  ~TestTransportAppl();

  void handleSelfMsg(cMessage *msg);

  void handleLowerMsg(cMessage *msg);

  void socketDataArrived(int connId, void *yourPtr, cMessage *msg, bool urgent);

  void socketEstablished(int connId, void *yourPtr);

 protected:
  int myAppAddr;
  TCPSocket socket;
  TCPSocket receivingSocket;
};

#endif
