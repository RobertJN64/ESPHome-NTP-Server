# ESPHome NTP Server

Adds NTP capabilities to the ESPHome platform.

** Fixed duplicate sources in "NTP Server info"

** Fixed Ethernet/Wifi compatibility

## Basic Usage

Host an NTP server from an ESP device. Works with any time source in the ESPHome ecosystem.

Example using GPS time source:

```yaml
uart:
  baud_rate: 9600
  rx_pin: 16
  tx_pin: 17
gps:

ntp_server:

time:
  - platform: gps
    id: gps_time

external_components:
  source:
    type: git
    url: https://github.com/RobertJN64/ESPHome-NTP-Server
  refresh: 30s
  components: [ ntp_server ]
```

## Advanced Usage

Provides more info about the inner workings of the NTP server. Publishes info including the last time provider to the NTP server, and the difference between different time sources.

Example using GPS and SNTP time sources:

```yaml
uart:
  baud_rate: 9600
  rx_pin: 16
  tx_pin: 17
gps:

time:
  - platform: gps
    id: gps_time
  - platform: sntp
    id: ext_ntp_time

ntp_server:

text_sensor:
  - platform: ntp_server_info
    time:
      name: NTP Server Info

external_components:
  source:
    type: git
    url: https://github.com/RobertJN64/ESPHome-NTP-Server
  refresh: 30s
  components: [ ntp_server, ntp_server_info, time ]
```

## Limitations

- SNTP will not show as a time source because it does not call the synchronize_epoch() function in ESPHome. It will still set the time correctly for the NTP server
