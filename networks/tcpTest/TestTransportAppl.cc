#include "TestTransportAppl.h"
#include "TCPSocket.h"

Define_Module(TestTransportAppl);

void TestTransportAppl::initialize(int stage) {

  EV << "prova!!!!"<< endl;

  BasicApplLayer::initialize(stage);

  if(stage == 0) {
    EV << "in initialize() stage 0...";
    int value = par("headerLength");
    EV << "headerLength is " << value << endl;
  }
  else if(stage==1) {
    EV << "in initialize() stage 1...";
    // Application address configuration: equals to host IP address
    cModule * cmod = getParentModule()->getSubmodule("nic")->getSubmodule("mac");
    BasicIPMacLayer * ipif = (BasicIPMacLayer *) cmod;
    myAppAddr = ipif->getIPAddress().getInt();
    //    sentPackets = 0;

    receivingSocket.setCallbackObject(this,NULL);
    receivingSocket.setOutputGate(gate("lowergateOut"));

    if(myAppAddr == 0) 	// the sink does not generate packets to itself. 
      //scheduleNextPacket();
      scheduleAt(simTime()+5,new cMessage("send",1));
    if(myAppAddr == 10)
	scheduleAt(simTime()+3,new cMessage("listen",3));
  }
}

void TestTransportAppl::handleSelfMsg(cMessage *msg) {

  if (msg->getKind()==1) {
    EV << "Messaggio correttamente schedulato"<<endl;
    socket.setOutputGate(gate("lowergateOut"));
    socket.connect(IPAddress(10), 2000);
    msg = new cMessage("data1");
    msg->setByteLength(16*1024);
    socket.send(msg);
    scheduleAt(simTime()+5000,new cMessage("close",2));
  } else if (msg->getKind()==2) {
    socket.close();
  } else if (msg->getKind()==3) {
    receivingSocket.bind(2000);
    receivingSocket.listenOnce();
  } else
    EV << "ERROR: Unknown message";
}

void TestTransportAppl::handleLowerMsg(cMessage *msg) {
  if (receivingSocket.belongsToSocket(msg))
    receivingSocket.processMessage(msg); // dispatch to socketXXXX() methods
}

void TestTransportAppl::socketDataArrived(int connId, void *yourPtr, cMessage *msg, bool urgent) {
  	cout << simTime() << endl;
	cout << "****************************************************"<<endl;
	cout << "***********  Some data has arrived!!  **************"<<endl;
  	cout << "****************************************************"<<endl;
}

void TestTransportAppl::socketEstablished(int connId, void *yourPtr) {
  	cout << simTime() << endl;
  	cout << "****************************************************"<<endl;
	cout << "Socket Connected";
  	cout << "****************************************************"<<endl;
}
