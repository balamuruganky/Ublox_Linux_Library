# Ublox_Linux_Library
The aim of this repository to port the Sparkfun_Ublox_Arduino_Library to linux platform.

## How to pull the code
* git clone https://github.com/balamuruganky/Ublox_Linux_Library
* cd Ublox_Linux_Library
* git submodule update --init

## How to compile
* mkdir build
* cd build
* cmake ..
* make

## How to execute test
* cd ../bin (Assuming the current directorty is build directory, created above)
* ./ublox_f9p_test (Ublox library calls)
* ./ublox_f9p_i2c_test /dev/ublox_i2c 0x42 (or without command line arguments)

## License
This repository consists files from different other repositories such as Arduino and Sparkfun_Ublox_Arduino_Library. Please consider the licenses according to the files where it is from.

## Linux Examples
Feel free to port the examples from Sparkfun_Ublox_Arduino_Library to linux.

## Caveats

