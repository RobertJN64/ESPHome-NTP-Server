#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace ntp_server_info {

class NTPServoInfo : public PollingComponent, public text_sensor::TextSensor {
public:
  void update() override;
};

} // namespace ntp_server_info
} // namespace esphome