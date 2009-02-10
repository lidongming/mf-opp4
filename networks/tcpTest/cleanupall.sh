#!/bin/bash
sed 's/sca.*"//g' omnetpp.sca > scalars.tmp
grep -v "^run" scalars.tmp > scalars.clean
rm scalars.tmp

