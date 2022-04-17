/*******************************************************************************
 *
 *  File:           main.h
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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

#include "../config/wifi_credentials.h"

/// Onboard LED
#define BOARD_LED 2
/// Pin used to control the LEDs
#define PIN 0
/// Number of connected LEDS
#define NUM_LED 97
/// Delay used to update the LEDS - @deprecated for production use
#define DELAYVAL 200

/// Modes available for the LED Bar
enum led_bar_mode {
  OFF,              // Off mode - the bar is turned off
  RGB,              // RBG mode - procudes red-green-blue mixed color
  W,                // W(hite) mode - adjust color temperature and intensity
  ANIMATION         // Animation mode - select an animation an its speed
};

///
#define UDP_MULTICAST_ADDRESS 230, 120, 10, 1
/// The port where the UDP server is listening
#define UDP_PORT 8266
/// Maximum size of UDP packets expected + 1 byte
#define UDP_BUFFER_SIZE 10

/// Adress of the device. MSB, LSB
#define DEVICE_ADDRESS 0x01, 0x07
