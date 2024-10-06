#include "esphome.h"
#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#else 
#include "esphome/components/ethernet/ethernet_component.h"
#endif

#include <WiFiUdp.h>

WiFiUDP Udp;

#define NTP_PORT 123
#define NTP_PACKET_SIZE 48
// buffers for receiving and sending data
byte packetBuffer[NTP_PACKET_SIZE];

const unsigned long seventyYears = 2208988800UL; // to convert unix time to epoch

namespace esphome {
namespace ntp_server {


void startNTP() {
  Udp.begin(NTP_PORT);
}

void processNTP() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE);
    IPAddress Remote = Udp.remoteIP();
    int PortNum = Udp.remotePort();

    Serial.print("NTP request from ");
    Serial.println(Remote.toString());

#ifdef DEBUG
    Serial.println();
    Serial.print("Received UDP packet size ");
    Serial.println(packetSize);
    Serial.print("From ");

    for (int i = 0; i < 4; i++) {
      Serial.print(Remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.print(PortNum);

    byte LIVNMODE = packetBuffer[0];
    Serial.print("  LI, Vers, Mode :");
    Serial.print(LIVNMODE, HEX);

    byte STRATUM = packetBuffer[1];
    Serial.print("  Stratum :");
    Serial.print(STRATUM, HEX);

    byte POLLING = packetBuffer[2];
    Serial.print("  Polling :");
    Serial.print(POLLING, HEX);

    byte PRECISION = packetBuffer[3];
    Serial.print("  Precision :");
    Serial.println(PRECISION, HEX);

    for (int z = 0; z < NTP_PACKET_SIZE; z++) {
      Serial.print(packetBuffer[z], HEX);
      if (((z + 1) % 4) == 0) {
        Serial.println();
      }
    }
    Serial.println();
#endif

    uint32_t tempval;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t timestamp = tv.tv_sec + seventyYears; // unix to utc

    packetBuffer[0] = 0b00100100; // LI, Version, Mode

    if (tv.tv_sec < seventyYears / 2) {
      packetBuffer[1] = 16; // for now - force sync
      Serial.println("NTP Server likely has bad time (year is not recent) - setting stratum to 16 to block sync.");
    } else {
      packetBuffer[1] = 4; // recommended because accuracy is limited to nearest second
    }

    packetBuffer[2] = 6;    // polling minimum
    packetBuffer[3] = 0xFA; // precision

    packetBuffer[4] = 0; // root delay
    packetBuffer[5] = 0;
    packetBuffer[6] = 8;
    packetBuffer[7] = 0;

    packetBuffer[8] = 0; // root dispersion
    packetBuffer[9] = 0;
    packetBuffer[10] = 0xC;
    packetBuffer[11] = 0;

#ifdef DEBUG
    Serial.println(timestamp);
    // print_date(gps);
#endif

    tempval = timestamp;

    // Set refid to IP address if not locked
    #ifdef USE_WIFI
    IPAddress myIP = wifi::global_wifi_component->get_ip_addresses()[0];
    #else
    IPAddress myIP = ethernet::global_eth_component->get_ip_addresses()[0];
    #endif
    packetBuffer[12] = myIP[0];
    packetBuffer[13] = myIP[1];
    packetBuffer[14] = myIP[2];
    packetBuffer[15] = myIP[3];

    // reference timestamp
    packetBuffer[16] = (tempval >> 24) & 0XFF;
    tempval = timestamp;
    packetBuffer[17] = (tempval >> 16) & 0xFF;
    tempval = timestamp;
    packetBuffer[18] = (tempval >> 8) & 0xFF;
    tempval = timestamp;
    packetBuffer[19] = (tempval)&0xFF;

    packetBuffer[20] = 0;
    packetBuffer[21] = 0;
    packetBuffer[22] = 0;
    packetBuffer[23] = 0;

    // copy originate timestamp from incoming UDP transmit timestamp
    packetBuffer[24] = packetBuffer[40];
    packetBuffer[25] = packetBuffer[41];
    packetBuffer[26] = packetBuffer[42];
    packetBuffer[27] = packetBuffer[43];
    packetBuffer[28] = packetBuffer[44];
    packetBuffer[29] = packetBuffer[45];
    packetBuffer[30] = packetBuffer[46];
    packetBuffer[31] = packetBuffer[47];

    // receive timestamp
    packetBuffer[32] = (tempval >> 24) & 0XFF;
    tempval = timestamp;
    packetBuffer[33] = (tempval >> 16) & 0xFF;
    tempval = timestamp;
    packetBuffer[34] = (tempval >> 8) & 0xFF;
    tempval = timestamp;
    packetBuffer[35] = (tempval)&0xFF;

    packetBuffer[36] = 0;
    packetBuffer[37] = 0;
    packetBuffer[38] = 0;
    packetBuffer[39] = 0;

    // transmitt timestamp
    packetBuffer[40] = (tempval >> 24) & 0XFF;
    tempval = timestamp;
    packetBuffer[41] = (tempval >> 16) & 0xFF;
    tempval = timestamp;
    packetBuffer[42] = (tempval >> 8) & 0xFF;
    tempval = timestamp;
    packetBuffer[43] = (tempval)&0xFF;

    packetBuffer[44] = 0;
    packetBuffer[45] = 0;
    packetBuffer[46] = 0;
    packetBuffer[47] = 0;

    // Reply to the IP address and port that sent the NTP request

    Udp.beginPacket(Remote, PortNum);
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
  }
}

void NTP_Server::setup() {
}

bool first_loop_flag = true;

void NTP_Server::loop() {
  if (first_loop_flag) { // wifi must init first...
    first_loop_flag = false;
    startNTP();
  }

  processNTP();
}

} // namespace ntp_server
} // namespace esphome
