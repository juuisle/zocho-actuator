| Supported Targets | ESP32 |
| ----------------- | ----- |

# Zocho-Ten Smart Actuator

The piece of software you are viewing is written in "freeRTOS style" with the ESP-IDF framework, provided by the manufactor of the ESP32. To learn more about ESP-IDF, please refer to the documentation at: 
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming


### Configure the project

```
idf.py menuconfig
```

### Configure in code

Change wifi setting in 'zocho_wifi.c' and mqtt setting in 'zocho_mqtt.c'

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)





