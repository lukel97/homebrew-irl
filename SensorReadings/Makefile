.PHONY: upload
.PHONY: debug
.PHONY: serial

all: SensorReadings.Heltec-esp32.esp32.wifi_lora_32_V2.bin

SensorReadings.Heltec-esp32.esp32.wifi_lora_32_V2.bin: SensorReadings.ino Secrets.h
	arduino-cli compile --fqbn Heltec-esp32:esp32:wifi_lora_32_V2 \
		--warnings default \
	   	--libraries google-cloud-iot-arduino 

# can be useful for debugging
# --build-properties build.code_debug=5

upload: SensorReadings.Heltec-esp32.esp32.wifi_lora_32_V2.bin
	arduino-cli upload -p /dev/cu.SLAB_USBtoUART --fqbn Heltec-esp32:esp32:wifi_lora_32_V2

serial:
	screen /dev/cu.SLAB_USBtoUART 115200
debug: upload serial
