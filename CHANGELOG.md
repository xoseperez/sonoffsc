# SonoffSC change log

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [1.1.1] 2017-10-06
### Changed
- Reduced comm speed to 9600 bauds to get more reliable results

### Fixed
- Fixed color translation from RGB to long

## [1.1.0] 2017-10-01
Lots of changes from ESPurna

## [1.0.0] 2017-06-27
### Added
- Support for microwave presence sensor
- Support for small fan to move air and get better dust measurements (testing)
- Reset via MQTT
- Debug messages via UDP
- Debug messages in program memory (still migrating)
- Force user to change default password
- New favicon

### Changed
- Support for RGB LED ring based on the WS2812FX library
- Embedded web code inside firmware (fast access, more space for code)
- Do not scan networks if just one defined
- Using "restart" instead of "reset" to reset board (cleaner sessions)
- Flash memory layout (will probably require erasing and reflash ESP8266)
- Using long values for communication between microcontrollers

### Fixed
- Wifi persistance bug in ESP8266

## [0.3.0] 2017-02-13
### Added
- Support for RGB LED ring (thanks to Blair Thompson)

## [0.2.0] 2017-01-12
### Added
- Enable/disable clap mode with voice commands using Alexa

### Fixed
- Ported MQTT connectivity fixes from ESPurna firmware

### Removed
- NTP support removed

## [0.1.0] 2017-01-10
- Initial stable version
