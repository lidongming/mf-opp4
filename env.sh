#!/bin/bash
#
# Small script to set the environment variable as required by the build system.
# Import them into your shell by typing "source env.sh"
#
# Branch this bzr branch and set the OMNET_PATH variable to point to your local copy of omnet++ 4:
OMNET_PATH=

#Nothing to change below this line

#omnet environment variables 
MOBILITY_FRAMEWORK_PATH=`pwd`
export PATH=$OMNET_PATH/bin:$PATH
export LD_LIBRARY_PATH=$MOBILITY_FRAMEWORK_PATH/core/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$MOBILITY_FRAMEWORK_PATH/contrib/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$OMNET_PATH/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$OMNET_INET_PATH/bin:$LD_LIBRARY_PATH
export TCL_LIBRARY=/usr/lib/tcl8.4

