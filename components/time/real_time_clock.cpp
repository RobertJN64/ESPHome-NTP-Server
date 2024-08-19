// ROBERTJN64TIME
// Modified to allow ntp debug info

#include "real_time_clock.h"
#include "esphome/core/log.h"
#ifdef USE_HOST
#include <sys/time.h>
#else
#include "lwip/opt.h"
#endif
#ifdef USE_ESP8266
#include "sys/time.h"
#endif
#ifdef USE_RP2040
#include <sys/time.h>
#endif
#include <cerrno>

#include <cinttypes>

namespace esphome {
namespace time {

const char *lastTimeSource = "Local RTC";
int timeSourceC = 0; // how many time sources
TimeDelta timeDelta[MAX_TIME_SOURCE_N] = {};

static const char *const TAG = "time";

RealTimeClock::RealTimeClock() = default;
void RealTimeClock::call_setup() {
  this->apply_timezone_();
  PollingComponent::call_setup();
}
void RealTimeClock::synchronize_epoch_(uint32_t epoch) {

  const char *comp_source = this->get_component_source();
  ESP_LOGD(TAG, "Sync epoch from");
  ESP_LOGD(TAG, "%s", comp_source);
  lastTimeSource = comp_source;
  time_t old_epoch = now().timestamp;

  bool new_source = true;
  for (int i = 0; i < timeSourceC; i++) {
    TimeDelta tD = timeDelta[i];//fix dup sources
    if (tD.source == comp_source) {
      new_source = false;
      tD.delta = 0;
    } else {
      tD.delta += (old_epoch - epoch); // if new timestamp is newer, delta should be negative
    }
  }
  if (new_source) {
    TimeDelta nTimeDelta;
    nTimeDelta.source = comp_source;
    nTimeDelta.delta = 0;
    timeDelta[timeSourceC] = nTimeDelta;
    timeSourceC += 1;
  }



  // Update UTC epoch time.
  struct timeval timev {
    .tv_sec = static_cast<time_t>(epoch), .tv_usec = 0,
  };
  ESP_LOGVV(TAG, "Got epoch %" PRIu32, epoch);
  struct timezone tz = {0, 0};
  int ret = settimeofday(&timev, &tz);
  if (ret == EINVAL) {
    // Some ESP8266 frameworks abort when timezone parameter is not NULL
    // while ESP32 expects it not to be NULL
    ret = settimeofday(&timev, nullptr);
  }

  // Move timezone back to local timezone.
  this->apply_timezone_();

  if (ret != 0) {
    ESP_LOGW(TAG, "setimeofday() failed with code %d", ret);
  }

  auto time = this->now();
  ESP_LOGD(TAG, "Synchronized time: %04d-%02d-%02d %02d:%02d:%02d", time.year, time.month, time.day_of_month, time.hour,
           time.minute, time.second);

  this->time_sync_callback_.call();
}

void RealTimeClock::apply_timezone_() {
  setenv("TZ", this->timezone_.c_str(), 1);
  tzset();
}

}  // namespace time
}  // namespace esphome
