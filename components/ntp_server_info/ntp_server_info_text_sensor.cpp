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

  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t timestamp = tv.tv_sec;
  esphome::time::ESPTime timeinfo = esphome::time::ESPTime::from_epoch_local(timestamp);
  state << "\nSystem has time: " << timeinfo.strftime("%Y-%m-%d %H:%M:%S");

  for (int i = 0; i < esphome::time::timeSourceC; i++) {
    esphome::time::TimeDelta td = esphome::time::timeDelta[i];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t timestamp = tv.tv_sec + td.delta;

    esphome::time::ESPTime timeinfo = esphome::time::ESPTime::from_epoch_local(timestamp);
    state << '\n ' << td.source << " has time: " << timeinfo.strftime("%Y-%m-%d %H:%M:%S");
  }

  this->publish_state(state.str());
}

} // namespace ntp_server_info
} // namespace esphome