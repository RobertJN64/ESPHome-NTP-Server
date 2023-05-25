#include "ntp_server_info_text_sensor.h"
#include "esphome/core/log.h"
#include "esphome.h"
#include <sstream>
#include <Arduino.h>

namespace esphome {
namespace ntp_server_info {

void NTPServoInfo::update() {
  std::stringstream state;
  state << "Last system time source: " << esphome::time::lastTimeSource;

  for (int i = 0; i < esphome::time::timeSourceC; i++) {
    esphome::time::TimeDelta td = esphome::time::timeDelta[i];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t timestamp = tv.tv_sec + td.delta;

    esphome::time::ESPTime timeinfo = esphome::time::ESPTime::from_epoch_local(timestamp);

    state << "\n Source: " << td.source << " has time: ";

    state << "\n Source: " << td.source << " has time: ";
    state << (uint_16)timeinfo.year << "-" << (uint_16)timeinfo.month << "-" << (uint_16)timeinfo.day_of_month << " ";
    state << (uint_16)timeinfo.hour << ":" << (uint_16)timeinfo.minute << ":" << (uint_16)timeinfo.second;
  }

  this->publish_state(state.str());
}

} // namespace ntp_server_info
} // namespace esphome