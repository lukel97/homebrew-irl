.PHONY: compile
.PHONY: upload

all: compile upload

compile: *.ino
	arduino-cli compile --fqbn Heltec-esp32:esp32:wifi_lora_32_V2
upload:
	arduino-cli upload -p /dev/cu.SLAB_USBtoUART --fqbn Heltec-esp32:esp32:wifi_lora_32_V2
