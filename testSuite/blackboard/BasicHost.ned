// empty host to test bb

package org.mf2.testSuite.blackboard;

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
        statechanger: StateChanger {
            parameters:
                @display("p=50,90;b=30,25");
        }
        statevisualize: StateVisualize {
            parameters:
                @display("p=200,30;b=30,25");
        }
        stateview: StateView {
            parameters:
                @display("p=200,110;b=30,25");
        }
        testparam: TestStateSub {
            parameters:
                @display("p=200,160;b=30,25");

        }
    connections:

}
