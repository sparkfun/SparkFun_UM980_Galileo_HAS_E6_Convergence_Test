/*
  SparkFun UM980 Galileo HAS/E6 Time-To-Convergence Test
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 14th, 2024
  License: MIT. Please see LICENSE.md for more information.

  This example shows how to setup the UM980 and to monitor the solution type for
  a Galileo/HAS PPP based convergence solution. Only time, deviations, SIV, and
  solution type are output. If you need NMEA sentences or positional information,
  this is not the example you are looking for.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Triband GNSS RTK Breakout - UM980 (GPS-23286) https://www.sparkfun.com/products/23286
  SparkFun RTK Torch (GPS-25662) https://www.sparkfun.com/products/25662

  This sketch is specifically designed to run on an RTK Torch but is compatible with the UM980 breakout board.

  Goals of this sketch:
    Output the system time, deviation LLH, and fix status
    Reset once PPP converged
    Avoid most prints
*/

int pin_UART1_TX = 27; //TX and RX pins for the RTK Torch
int pin_UART1_RX = 26;
int pin_GNSS_DR_Reset = 22; // Push low to reset GNSS/DR

#include <SparkFun_Unicore_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_Unicore_GNSS

UM980 myGNSS;

HardwareSerial SerialGNSS(1); //Use UART1 on the ESP32

unsigned long lastCheck = 0;

unsigned long startTime = 0;
unsigned long convergingStartTime = 0;
unsigned long timeToConvergence = 0;

void setup()
{
  Serial.begin(115200);
  delay(250);
  Serial.println();
  Serial.println("SparkFun UM980 Galileo HAS/E6 Time-To-Convergence Test");

  pinMode(pin_GNSS_DR_Reset, OUTPUT);

  //The CONFIG response can be ~500 bytes overruning the ESP32 RX buffer of 256 bytes
  //Increase the size of the RX buffer
  SerialGNSS.setRxBufferSize(1024);

  //We must start the serial port before using it in the library
  SerialGNSS.begin(115200, SERIAL_8N1, pin_UART1_RX, pin_UART1_TX);

  //myGNSS.enableDebugging(); // Print all debug to Serial

  startSystem(); //Setup UM980 and start timer
}

void loop()
{
  if (Serial.available())
  {
    byte incoming = Serial.read();
    if (incoming == 'r')
      ESP.restart();
  }

  myGNSS.update(); //Regularly call to parse any new data

  if (millis() - lastCheck > 1000)
  {
    lastCheck = millis();
    printUpdate();
  }
}

void printUpdate()
{
  Serial.printf("%0.0f,", millis() / 1000.0);
  Serial.printf("%0.3f,", myGNSS.getLatitudeDeviation());
  Serial.printf("%0.3f,", myGNSS.getLongitudeDeviation());
  Serial.printf("%0.3f,", myGNSS.getAltitudeDeviation());
  Serial.printf("%d,", myGNSS.getSIV());

  int positionType = myGNSS.getPositionType();
  Serial.printf("%d,", positionType);

  if (positionType == 0) Serial.print("No solution,");
  else if (positionType == 8) Serial.print("Velocity computed using instantaneous Doppler,");
  else if (positionType == 16) Serial.print("Single point positioning,");
  else if (positionType == 17) Serial.print("Pseudorange differential solution,");
  else if (positionType == 18) Serial.print("SBAS positioning,");
  else if (positionType == 32) Serial.print("L1 float solution,");
  else if (positionType == 33) Serial.print("Ionosphere-free float solution,");
  else if (positionType == 34) Serial.print("Narrow-lane float solution,");
  else if (positionType == 48) Serial.print("L1 fixed solution,");
  else if (positionType == 49) Serial.print("Wide-lane fixed solution,");
  else if (positionType == 50) Serial.print("Narrow-lane fixed solution,");
  else if (positionType == 68)
  {
    Serial.print("PPP solution converging,");

    if (convergingStartTime == 0) convergingStartTime = millis();
    if (convergingStartTime > 0)
    {
      Serial.print("Seconds in converging phase: ");
      Serial.print((millis() - (convergingStartTime - startTime)) / 1000.0, 0);
      Serial.print("s");
    }
  }
  else if (positionType == 69)
  {
    Serial.print("Precise Point Positioning,");

    if (timeToConvergence == 0) timeToConvergence = millis();

    if (timeToConvergence > 0)
    {
      Serial.print("Seconds in converging phase: ");
      Serial.print((millis() - (convergingStartTime - startTime)) / 1000.0, 0);
      Serial.print("s");

      Serial.print(" - Total time to convergence: ");
      Serial.print((timeToConvergence - startTime) / 1000.0, 0);
      Serial.print("s");
    }

    //Reset UM980
    ESP.restart();
  }
  else Serial.print("Unknown,");
  Serial.println();
}

void startSystem()
{
  digitalWrite(pin_GNSS_DR_Reset, LOW); // Reset UM980

  delay(500);

  digitalWrite(pin_GNSS_DR_Reset, HIGH); // Tell UM980 and DR to boot

  delay(500);

  if (myGNSS.begin(SerialGNSS) == false) //Give the serial port over to the library
  {
    Serial.println("UM980 failed to respond. Check ports and baud rates. Freezing...");
    while (true);
  }
  //Serial.println("UM980 detected!");

  //E6 reception requires version 11833 or greater
  int um980Version = 0;

  for (int x = 0 ; x < 4 ; x++)
  {
    um980Version = String(myGNSS.getVersion()).toInt(); //Convert the string response to a value
    if (um980Version != 0) break;
  }

  if (um980Version < 11833)
  {
    Serial.println("E6 requires 11833 or newer. Please update the firmware on your UM980. Freezing...");
    while (true);
  }

  if (myGNSS.isConfigurationPresent("CONFIG SIGNALGROUP 2") == false)
  {
    Serial.println("Device not ready for E6. Freezing...");
    while (1);
  }

  if (myGNSS.isConfigurationPresent("CONFIG PPP ENABLE E6-HAS") == false)
  {
    if (myGNSS.sendCommand("CONFIG PPP ENABLE E6-HAS") == false)
      Serial.println("Galileo E6 service config error");
  }

  Serial.println("seconds, lat deviation, lon, alt, siv, fix type,");

  startTime = millis();
}
