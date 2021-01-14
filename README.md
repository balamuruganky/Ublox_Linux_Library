# Ublox_Linux_Library
The aim of this repository to port the Sparkfun_Ublox_Arduino_Library to linux platform.

## How to compile
* mkdir build
* cd build
* cmake ..
* make

## How to execute test
* cd ../bin (Assuming the current directorty is build directory, created above)
* ./ublox_f9p_test (Ublox library calls)

## License
This repository consists files from different other repositories such as Arduino and Sparkfun_Ublox_Arduino_Library. Please consider the licenses according to the files where it is from.

## Linux Examples
Feel free to port the examples from Sparkfun_Ublox_Arduino_Library to linux.

## Caveats
I2C is not tested. Please raise PR, if you find any issues.
