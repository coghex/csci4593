#!/bin/bash

echo "omnetpp.default:"
zcat traces/omnetpp.Z | ./cachesim configs/defaultconfig
