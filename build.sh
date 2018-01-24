#!/bin/sh

cd atmega328
platformio run
cd ..

cd esp8266
node node_modules/gulp/bin/gulp.js
platformio run
cd ..
