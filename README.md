# WordClock-15x15

## Upload data/ to SPIFFS:

```
pio run --target buildfs # only for the 1st time or when files changes
pio run --target uploadfs
```

Please remember that the full path for a file must be < 32 bits (it can't be `/js/a-very-very-very-very-long-filename.min.js`).

## Software

* [VS Code](https://code.visualstudio.com/download)
* [PlatformIO](https://platformio.org/)

## Libraries

* https://github.com/PaulStoffregen/Time.git
* TFT_eSPI@1.4.21    
* Button2@1.0.0
* ArduinoJson
* ESP Async WebServer
* WebSockets
* https://github.com/dx168b/async-mqtt-client.git
* https://github.com/alanswx/ESPAsyncWiFiManager.git
* https://github.com/samguyer/FastLED.git
* https://github.com/Jorgen-VikingGod/LEDMatrix.git

All other required packages are provided by PlatformIO
