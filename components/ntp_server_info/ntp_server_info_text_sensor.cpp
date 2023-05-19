#include "ntp_server_info_text_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ntp_server_info {

void NTPServoInfo::update() {
  this->publish_state(esphome::time::lastTimeSource);
}

} // namespace ntp_server_info
} // namespace esphome