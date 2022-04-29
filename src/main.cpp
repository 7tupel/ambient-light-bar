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
IPAddress multicast_ip(
  multicast_ip_address_bytes[0],
  multicast_ip_address_bytes[1],
  multicast_ip_address_bytes[2],
  multicast_ip_address_bytes[3]);

/// last time a check was made if wifi is still connected
unsigned long last_wifi_check = 0;
/// interval when to restart wifi after connection lost
const unsigned long wifi_check_delay = 20000;

/// UDP server
WiFiUDP Udp;

/// Buffer for incoming UDP packets
uint8_t packet_buffer[UDP_BUFFER_SIZE];

uint8_t device_address[2] = { DEVICE_ADDRESS };

/// The mode the ambient bar currently uses.
led_bar_mode mode = OFF;

/// Reference instance to interact with the LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LED, PIN, NEO_GRB + NEO_KHZ800);


void flash_board_led(uint8_t times, uint8_t speed) {
  for(uint8_t i; i < 0; i++) {
    digitalWrite(BOARD_LED, LOW);
    delay(speed * 100);
    digitalWrite(BOARD_LED, HIGH);
    delay(speed * 100);
  }
}

/**
 * Setup the board for network communication and LED control.
 */
void setup() {
  // setup serial debugging if enabled
  #ifdef SERIAL_DEBUGGING
    Serial.begin(MONITOR_SPEED);
  #endif

  // setup onboard LED for feedback if enabled
  #ifdef USE_LED
    // on board LED control Pin
    pinMode(BOARD_LED, OUTPUT);
    digitalWrite(BOARD_LED, LOW);
  #endif

  // setup and connect to wifi
  #ifdef SERIAL_DEBUGGING
    Serial.println("Connecting to Wifi");
  #endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  #ifdef SERIAL_DEBUGGING
    Serial.println("\nConnected. Esp8266 IP: " + WiFi.localIP().toString());
    Serial.printf("UDP Server Port: %d\n", UDP_PORT);
  #endif

  // setup UDP server
  #ifdef USE_MULTICAST
    if (!Udp.beginMulticast(WiFi.localIP(), multicast_ip, UDP_PORT)) {
      #ifdef SERIAL_DEBUGGING
        Serial.println("Error on Multicast Init");
      #endif
    } else {
      Serial.printf("Listening og Multicast IP: %d.%d.%d.%d\n",
        multicast_ip_address_bytes[0],
        multicast_ip_address_bytes[1],
        multicast_ip_address_bytes[2],
        multicast_ip_address_bytes[3]);
    }
  #else
    Udp.begin(UDP_PORT);
  #endif

  #ifdef USE_UNICAST_FALLBACK
    Udp.begin(UDP_PORT);
  #endif

  // setup led control
  pixels.begin();
  #ifdef USE_LED
    // indicate setup was completed successfully
    digitalWrite(BOARD_LED, HIGH);
  #endif
}

/**
 * Print the protocol version of the paket to serial.
 */
void print_protocol_version(uint8_t version) {
  Serial.printf("Protocol Version: %d (%X)", version, version);
}

/**
 * Print the opcode of the paket to serial.
 */
void print_op_code(uint8_t opcode) {
  Serial.printf("Op Code: ");
  switch(opcode) {
    case 0x01:
      Serial.printf("RGBI8_SET (%X)\n", opcode);
      break;
    case 0x02:
      Serial.printf("RGBWI8_SET (%X)\n", opcode);
      break;
    default:
      Serial.printf("UNKNOWN! - %X\n", opcode);
  }
}

/**
 * Print the parameter of a RGBI8 frame to serial.
 */
void print_rgbi8_data(uint8_t* data) {
  Serial.printf("Color (RGB): %X, %X, %X\n", data[2], data[1], data[0]);
  Serial.printf("Intensity: %X\n", data[3]);
}

/**
 * Print the parameter of a RGBWI8 frame to serial.
 */
void print_rgbwi8_data(uint8_t* data) {
  Serial.printf("Color (RGBW): %X, %X, %X, %X\n", data[3], data[2], data[1], data[0]);
  Serial.printf("Intensity: %X\n", data[4]);
}

/**
 */
void loop() {
  /* First check if a valid wifi connection was established and retry if neccesary */
  unsigned long current_time = millis();
  if ((WiFi.status() != WL_CONNECTED) && (current_time - last_wifi_check >= wifi_check_delay)) {
    #ifdef SERIAL_DEBUGGING
      Serial.println("Warning: Wifi not Connected!");
    #endif
    WiFi.disconnect();
    WiFi.reconnect();
    last_wifi_check = current_time;
  }

  /* Try to parse and handle incoming UDP messages */

  uint16_t packet_size = Udp.parsePacket();
  if (packet_size) {
    #ifdef SERIAL_DEBUGGING
      Serial.printf("Received %d bytes\n", packet_size);
    #endif
    Udp.read(packet_buffer, sizeof(packet_buffer));
    packet_buffer[packet_size] = 0;                     // add 0 delimiter for easier buffer handling

    #ifdef SERIAL_DEBUGGING
      Serial.println("Bytes:");
      for(int k=0; k<packet_size; k++) {
        Serial.print(packet_buffer[k], HEX);
        Serial.print("\t");
      }
      Serial.printf("\n\n");
    #endif

    /* Parse the protocol */

    #ifdef SERIAL_DEBUGGING
      print_protocol_version(packet_buffer[0]);
    #endif

    // protocol version 1
    if (packet_buffer[0] == 0x01) {
      #ifdef SERIAL_DEBUGGING
        print_op_code(packet_buffer[1]);
      #endif

      if (packet_buffer[1] == 0x01) {
        // opcode RGBI8_SET
        #ifdef SERIAL_DEBUGGING
          print_rgbi8_data(packet_buffer+2);
        #endif
        uint8_t intensity = packet_buffer[5];
        uint32_t color = 0x00 << 24 | packet_buffer[2] << 16 | packet_buffer[3] << 8 | packet_buffer[4];
        pixels.fill(color, 0, NUM_LED);
        pixels.setBrightness(intensity);
        pixels.show();
      } else if (packet_buffer[1] == 0x02) {
        // opcode RGBWI8_SET
        #ifdef SERIAL_DEBUGGING
          print_rgbwi8_data(packet_buffer+2);
        #endif
        uint8_t intensity = packet_buffer[6];
        uint32_t color = packet_buffer[5] << 24 | packet_buffer[4] << 16 | packet_buffer[3] << 8 | packet_buffer[2];
        pixels.fill(color, 0, NUM_LED);
        pixels.setBrightness(intensity);
        pixels.show();
      }
    } else {
      #ifdef USE_LED
        flash_board_led(3, 5);
      #endif
    }
  }
}
