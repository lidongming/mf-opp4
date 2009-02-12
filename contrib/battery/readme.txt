BatteryModule - HowTo to install and run
Anna Foerster, Universita della Svizzera Italiana, Switzerland.
31.12.2008

New since last version:
The BatteryModule now incorporates also sleeping mode of the nodes. 
Additionally, the usage of the stats folder can be switched on and off and the nodes can start with lower battery capacity than he maximum possible.
The module also writes some statistics information to a vector file - time spent in each of the modes.

Introduction
The BatteryModule is a mobility framework component module, which requires you to have your simulation build with Omnet++ and MF. 
It uses three parameters - sleepPower, receivePower and sendPower, which denote the needed power (mA/sec) for sleeping/receiving/sending packets in a WSN.
It will update the initial power level of each node and will write the result into a file (one file per node in the network). The 
current level of the battery is also  published to the BlackBoard of MF and can be used for implementing different behaviors (like 
stop the simulation, when the battery is depleted).

Installation and Usage
1. Copy all files (BatteryModule.ned, BattertModule.cc, BatteryModule.h and BatteryState.h) into your simulation folder.
2. Update the ned file of your Host to use the BatteryModule:
	
	import
    	...
		"BatteryModule",

module MyHost
	parameters:
		...

	gates:
        ...
		
	submodules: 
        ...
		battery: BatteryModule;
			display: "b=32,30;p=60,108;i=prot1";
			
    connections: 
    ...
    
    ...
 endmodule
 
 3. Update your omnetpp.ini to include the parameters of BatteryModule:
 	mySim.statsFolder="/whereToGo"   			# it is important, that there is a "battery" subfolder in the statsFolder!!!!
	mySim.host[*].battery.fullStats = 0; 			# whether to write the debug information to the stats folder or not (problematic with very large simulations and other output files) 
 	mySim.host[*].battery.debug = 0; 			# whether to write our debug or not 
	mySim.host[*].battery.updateTimer = 1;		# how often to update the battery power level (it will be updated automatically when switching sending/receiving OR on updateTimer)
	mySim.host[*].battery.batteryCapacity = 20000;	# the starting capacity of this node: you can define, of course different levels for each host
	mySim.host[*].battery.batteryCapacity = 30000;	# the theoretical maximum of the capacity for this node: you can define, of course different levels for each host; this value is tru for two AA batteries
	mySim.host[*].battery.batteryTX = 8;		# this value is for MSB430 sensor nodes.
	mySim.host[*].battery.batteryRECV = 8;		# this value is for MSB430 sensor nodes.
	mySim.host[*].battery.batterySLEEP = 1;		# this value is for MSB430 sensor nodes.
	
4. rerun opp_makemake

5. if needed/wanted, register for the event BatteryState (defined in BatteryState.h):
	// subscribe to the batteryState.
		BatteryState bat;
		catBattery = bb->subscribe(this, &bat, -1);
		
	// see MF documentation for more info about the BlackBoard and how to use it.
	
6. re-compile your simulation

7. run it!

8. you should see a number of *.power files in your /whereToGo/battery/ folder with the output of the module.


ENJOY!!
 	
 



