#!/bin/bash

pushd cmmcAdmin 
./spiffs.sh
rm -v dist/static/img/*.svg
rm -v dist/static/js/app*.js
rm -v dist/static/js/vendor*.js
rm -v dist/static/css/*.css
rm -v dist/static/fonts/*.ttf
rm -v dist/static/fonts/*.eot
rm -v dist/static/fonts/*.woff
rm -v dist/static/fonts/*.woff2 

popd 
rsync --delete -av cmmcAdmin/dist/ data --progress
pio run -t uploadfs 
