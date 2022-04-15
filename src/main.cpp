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
/// Configure listening Multicast domain
const unsigned char multicast_ip_address_bytes[4] = { UDP_MULTICAST_ADDRESS };
IPAddress multicastIP(
  multicast_ip_address_bytes[0],
  multicast_ip_address_bytes[1],
  multicast_ip_address_bytes[2],
  multicast_ip_address_bytes[3]);

/// last time a check was made if wifi is still connected
unsigned long last_wifi_check = 0;
/// interval when to restart wifi after connection lost
const unsigned long wifi_check_delay = 20000;

/// The port where the UDP server is listening
constexpr uint16_t serverPort = UDP_PORT;
/// UDP server
WiFiUDP Udp;

/// Buffer for incoming UDP packets
uint8_t packetBuffer[UDP_BUFFER_SIZE];

/// The mode the ambient bar currently uses.
led_bar_mode mode = OFF;

/// Reference instance to interact with the LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LED, PIN, NEO_GRB + NEO_KHZ800);


/**
 * Setup the board for network communication and LED control.
 */
void setup() {
  Serial.begin(MONITOR_SPEED);
  // on board LED control Pin
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, LOW);
  // setup and connect to wifi
  Serial.println("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("\nConnected. Esp8266 IP: " + WiFi.localIP().toString());
  Serial.printf("UDP Server Port: %d\n", UDP_PORT);
  //Udp.begin(UDP_PORT);
  Serial.printf("Multicast IP: %d.%d.%d.%d\n", multicast_ip_address_bytes[0], multicast_ip_address_bytes[1], multicast_ip_address_bytes[2], multicast_ip_address_bytes[3]);
  if (!Udp.beginMulticast(WiFi.localIP(), multicastIP, UDP_PORT)) {
    Serial.println("Error on Multicast Init");
  }

  // setup led control
  pixels.begin();
  // run in RGB mode
  mode = RGB;
  // indicate setup was completed successfully
  digitalWrite(BOARD_LED, HIGH);
}

/**
 */
void loop() {
  /* First check if a calif wifi connection was established and retry if neccesary */
  unsigned long current_time = millis();
  if ((WiFi.status() != WL_CONNECTED) && (current_time - last_wifi_check >= wifi_check_delay)) {
    WiFi.disconnect();
    WiFi.reconnect();
    last_wifi_check = current_time;
  }
  /* Try to parse and handle incoming UDP messages */
  //
  uint16_t packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(packetBuffer, sizeof(packetBuffer));
    packetBuffer[packetSize] = 0;                     // add 0 delimiter for easier buffer handling
  }
  // protocol version 1
  if (packetBuffer[0] == 0x01) {
    // TODO: check address HERE
    // opcode RGBWI8_SET
    if (packetBuffer[1] == 0x01) {
      // uint8_t red = packetBuffer[4];
      // uint8_t green = packetBuffer[5];
      // uint8_t blue = packetBuffer[6];
      // uint8_t white = packetBuffer[7];
      uint8_t intensity = packetBuffer[8];
      uint32_t color = (unsigned int) atol((char*)packetBuffer+4);
      pixels.fill(color, 0, NUM_LED);
      pixels.setBrightness(intensity);
      pixels.show();
    }
  } else {
    digitalWrite(BOARD_LED, LOW);
    // this is an error, blink first led red
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(0, pixels.Color(10, 0, 0));
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(DELAYVAL);
    digitalWrite(BOARD_LED, HIGH);
  }
}
