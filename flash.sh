#!/bin/bash


pushd cmmcAdmin
./spiffs.sh
popd 
rsync -av cmmcAdmin/dist data
pio run -t uploadfs
#popd

