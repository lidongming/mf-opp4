// empty host to test radio

package org.mf2.testSuite.SingleChannelRadio;

import org.mf2.contrib.nic.radios.SingleChannelRadio;
import org.mf2.core.blackboard.Blackboard;


module BasicHost
{
    parameters:
        // parameters for the framework
        double numHosts;
        string appendDisplay;
        string color;

        @display("bgb=250,250,$color,,;bgp=10,10");
    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=50,30;b=30,25");
        }
        statechanger: RadioStateChanger {
            parameters:
                @display("p=50,90;b=30,25");
        }
        radio: SingleChannelRadio {
            parameters:
                @display("p=200,30;b=30,25");

        }
    connections:

}
