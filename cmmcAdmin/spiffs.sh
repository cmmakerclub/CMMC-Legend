#!/bin/bash

npm run build

cp -v dist/index.html dist/index.htm

gzip dist/static/css/*
gzip dist/static/fonts/*
gzip dist/static/img/*
gzip dist/static/js/*

rm -Rvf dist/static/img/*
rm -Rvf dist/static/fonts/*



#rm -v dist/static/fonts/*.ttf
#rm -v dist/static/fonts/*.woff
#rm -v dist/static/fonts/*.woff2
#rm -v dist/static/fonts/*.eot
#rm -v dist/static/img/*.svg
#rm -v dist/static/js/*.js
#rm -v dist/static/js/*.js
#rm -v dist/static/css/*.css
