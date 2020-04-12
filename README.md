WiFi Logger component
====================
WiFi logger - Log messages over WiFi, using either TCP, UDP or websockets (future releases)

## Requirements

* `protocol_examples_common (esp-idf/examples)`

## Installation

```
cd <your_esp_idf_project>
mkdir components
cd components
git clone https://github.com/VedantParanjape/esp-wifi-logger.git wifi_logger
```

## Usage

* Usage pattern similar to how ESP_LOGX() is used.
* Use `wifi_log()` function to print logs over wifi
* Example: `wifi_log(TAG, "%s", "logger test");`
* Call `start_wifi_logger()` in `void app_main()` to start the logger. Logging function `wifi_log` can be called to log messages.

* Configure `menuconfig`
  * `Example Connection Configuration` *Set WiFi SSID and password*
  * `Component config`
  * `WiFi Logger configuration`
    * `Network Protocol (TCP/UDP)` - Set network protocol to be used 
    * `Server IP Address` - Set the IP Address of the server which will receive log messages sent by ESP32
    * `Port` - Set the Port of the server

*IP Address of the server can be found out by running `ifconfig` on a linux machine*

## Configuration

```
idf.py menuconfig
```
* `Example Connection Configuration`
  * `WiFi SSID` -  Set WiFi SSID to connect
  * `WiFi Password` - Set WiFi Password

* `Component config`
  * `WiFi Logger configuration`
    * `Network Protocol (TCP/UDP)` - Set network protocol to be used 
    * `Server IP Address` - Set the IP Address of the server which will receive log messages sent by ESP32
    * `Port` - Set the Port of the server
    * `Queue Size` - ***Advanced Config, change at your own risk***** Set the freeRTOS Queue size used to pass log messages to logger task.
    * `logger buffer size` - ***Advanced Config, change at your own risk*** Set the buffer size of char array used to generate log messages in ESP format
