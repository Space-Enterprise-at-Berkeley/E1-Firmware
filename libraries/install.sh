#!/usr/bin/env bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
         machine=Linux
         rm -rf ~/Arduino/libraries/Barometer ~/Arduino/libraries/GPS ~/Arduino/libraries/IMU ~/Arduino/libraries/loadCell ~/Arduino/libraries/pressureTransducer ~/Arduino/libraries/tempController ~/Arduino/libraries/thermocouple
         cp -rf * ~/Arduino/libraries/
         ;;
    Darwin*)
      machine=Mac
      rm -rf ~/Documents/Arduino/libraries/Barometer ~/Documents/Arduino/libraries/GPS ~/Documents/Arduino/libraries/IMU ~/Documents/Arduino/libraries/loadCell ~/Documents/Arduino/libraries/pressureTransducer ~/Documents/Arduino/libraries/tempController ~/Documents/Arduino/libraries/thermocouple
      cp -rf * ~/Documents/Arduino/libraries/
      ;;
    CYGWIN*|MINGW*)
        machine=Windows
        # WHERE IS THE ARDUINO PATH ON WINDOWS??
        rm -rf "/c/Program Files (x86)/Arduino/libraries/Barometer" "/c/Program Files (x86)/Arduino/libraries/GPS" "/c/Program Files (x86)/Arduino/libraries/IMU" "/c/Program Files (x86)/Arduino/libraries/loadCell" "/c/Program Files (x86)/Arduino/libraries/pressureTransducer" "/c/Program Files (x86)/Arduino/libraries/tempController" "/c/Program Files (x86)/Arduino/libraries/thermocouple"
        cp -rf * "/c/Program Files (x86)/Arduino/libraries"
        ;;
    *)
        machine="UNKNOWN:${unameOut}"
esac
echo "Finished copying on" ${machine}
