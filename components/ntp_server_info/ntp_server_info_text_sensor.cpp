#include "ntp_server_info_text_sensor.h"
#include "esphome/core/log.h"
#include <Arduino.h>

namespace esphome {
namespace ntp_server_info {

void NTPServoInfo::update() {
  String state = "Last system time source: ";
  state += esphome::time::lastTimeSource;
  this->publish_state(state.c_str()); // TODO - publish source time deltas
}

} // namespace ntp_server_info
} // namespace esphome