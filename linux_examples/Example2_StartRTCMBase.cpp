/*
  This is modified from
   ../SparkFun_u-blox_GNSS_Arduino_Library/examples/NEO-M8P-2/Example2_StartRTCMBase/Example2_StartRTCMBase.ino
   to compile under Linux.
*/

/*
  Note: compiles OK with v2.0 but is currently untested

  Send UBX binary commands to enable RTCM sentences on u-blox NEO-M8P-2 module
  By: Nathan Seidle
  SparkFun Electronics
  Date: September 7th, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example does all steps to configure and enable a NEO-M8P-2 as a base station:
    Begin Survey-In
    Once we've achieved 2m accuracy and 300s have passed, survey is complete
    Enable four RTCM messages
    Begin outputting RTCM bytes

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GNSS and a BlackBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>

#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGPS;


int main(int argc, char** argv) {
    printf("Starting uBlox ZED-F9P\n");
    printf("u-blox NEO-M8P-2 base station example\n");

    std::string  port = "/dev/ublox_i2c";
    int address = 0x42;

    if(argc == 1) {
      printf("\nStarting with default port and address.\n");
      return 0;
    } else if (argc == 3) {
      port = argv[1];
      address = std::stoi(argv[2], 0, 16);
    } else if(argc >= 4) {
      printf ("\nShould give the port and address as input arguments. Currently giving more arguments...\n");
      return 0;
    }


    printf("Looking for GPS on port %s at address 0x%02X\n", port.c_str(), address);

    TwoWire wireCom(address);
    wireCom.init(port.c_str());

    if (!(wireCom.available())) {
      printf("Failed to init i2c,  Please connect ublox GNSS module and try again... \n");
      return 0;
    }

    myGPS.begin(wireCom);


    myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    myGPS.saveConfiguration(); //Save the current settings to flash and BBR


    boolean response;

    //Check if Survey is in Progress before initiating one
    // From v2.0, the data from getSurveyStatus (UBX-NAV-SVIN) is returned in UBX_NAV_SVIN_t packetUBXNAVSVIN
    // Please see u-blox_structs.h for the full definition of UBX_NAV_SVIN_t
    // You can either read the data from packetUBXNAVSVIN directly
    // or can use the helper functions: getSurveyInActive; getSurveyInValid; getSurveyInObservationTime; and getSurveyInMeanAccuracy
    response = myGPS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (request can take a long time)

    if (response == false) {// Check if fresh data was received
      printf("Failed to get Survey In status. Exiting...\n");
      return 1;
    }

    if (myGPS.getSurveyInActive() == true) {  // Use the helper function
      printf("Survey already in progress.\n");
      return 1;
    } else {
      //Start survey
      response = myGPS.enableSurveyMode(300, 2.000); //Enable Survey in, 300 seconds, 2.0m
      if (response == false) {
        printf("Survey start failed.  Exiting...\n");
        return 1;
      }
      printf("Survey started. This will run until 300s has passed and less than 2m accuracy is achieved.\n");
    }


    //Begin waiting for survey to complete
    while (myGPS.getSurveyInValid() == false) { // Call the helper function

      // From v2.0, the data from getSurveyStatus (UBX-NAV-SVIN) is returned in UBX_NAV_SVIN_t packetUBXNAVSVIN
      // Please see u-blox_structs.h for the full definition of UBX_NAV_SVIN_t
      // You can either read the data from packetUBXNAVSVIN directly
      // or can use the helper functions: getSurveyInActive; getSurveyInValid; getSurveyInObservationTime; and getSurveyInMeanAccuracy
      response = myGPS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (req can take a long time)
      if (response == true) { // Check if fresh data was received
        printf("Time elapsed: %d\n", (int)myGPS.getSurveyInObservationTime());
        printf(" Accuracy: %f\n", myGPS.getSurveyInMeanAccuracy());
      } else {
        printf("SVIN request failed\n");
      }
      delay(1000);
    }
    printf("Survey valid!\n");

    response = true;
    response &= myGPS.enableRTCMmessage(UBX_RTCM_1005, COM_PORT_I2C, 1); //Enable message 1005 to output through I2C port, message every second
    response &= myGPS.enableRTCMmessage(UBX_RTCM_1077, COM_PORT_I2C, 1);
    response &= myGPS.enableRTCMmessage(UBX_RTCM_1087, COM_PORT_I2C, 1);
    response &= myGPS.enableRTCMmessage(UBX_RTCM_1230, COM_PORT_I2C, 10); //Enable message every 10 seconds

    if (response == true) {
      printf("RTCM messages enabled!\n");
    } else {
      printf("RTCM failed to enable. Are you sure you have an NEO-M8P?");
      return 1;
    }
    printf("Base survey complete! RTCM now broadcasting.\n");

    while (1) {
      myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.
      delay(250); //Don't pound too hard on the I2C bus
    }
    return 0;  // prevent warning, won't reach here.
}



//This function gets called from the SparkFun u-blox Arduino Library.
//As each RTCM byte comes in you can specify what to do with it
//Useful for passing the RTCM correction data to a radio, Ntrip broadcaster, etc.
void SFE_UBLOX_GNSS::processRTCM(uint8_t incoming)
{
  //Let's just pretty-print the HEX values for now
  if (myGPS.rtcmFrameCounter % 16 == 0) Serial.println();
  Serial.print(F(" "));
  if (incoming < 0x10) Serial.print(F("0"));
  Serial.print(incoming, HEX);
}
