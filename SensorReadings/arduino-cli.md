# Arduino-CLI setup

```
arduino-cli core install --additional-urls https://docs.heltec.cn/download/package_heltec_esp32_index.json Heltec-esp32:esp32
arduino-cli compile --fqbn Heltec-esp32:esp32:wifi_lora_32_V2
# list ports to get <port>
arduino-cli board list
arduino-cli upload -p <port> --fqbn Heltec-esp32:esp32:wifi_lora_32_V2
```
