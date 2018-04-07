#!/bin/bash

pushd dist
#tee ap.json <<EOF
#{"ssid":"ssid","password":"password"}
#EOF
#tee	sta.json <<EOF
#{"ssid":"ssid","password":"password"}
#EOF
popd
pushd ~/projects/NatRTCUserMemory/

pio run -t uploadfs
popd

