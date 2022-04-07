/*******************************************************************************
 *
 *  File:           main.cpp
 *
 *  Function:       t.b.d.
 *
 *  Copyright:      Copyright (c) 2022 Clockworks UG c/o Moritz Moxter <moritz@clockworks.io
 *
 *  License:        Proprietary Software.
 *
 *  Author:         Moritz Moxter
 *
 *  Description:    t.b.d.
 *
 ******************************************************************************/

#include "main.h"

/// The SSID of the wifi used
const char* ssid = WIFI_SSID;
/// The password of the wifi used
const char* password = WIFI_PASSWORD;

/// The port where the UDP server is listening
constexpr uint16_t serverPort = UDP_PORT;

/// Buffer for incoming UDP packets
char packetBuffer[UDP_BUFFER_SIZE];

/// The mode the ambient bar currently uses.
led_bar_mode mode = OFF;

/// Reference instance to interact with the LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LED, PIN, NEO_GRB + NEO_KHZ800);




/**
 * Setup the board for network communication and LED control.
 */
void setup() {
  // setup and connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // setup led control
  pixels.begin();
  // run in RGB mode
  mode = RGB;
}

/**
 */
void loop() {
  //pixels.setPixelColor(i, pixels.Color(0, 0, 255));
}
