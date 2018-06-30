# CHANGE THIS!
INO=skel.ino

ifndef ARDUINO_IDE
$(error ARDUINO_IDE is not set)
endif
ifndef ESP32_ROOT
$(error ESP32_ROOT is not set)
endif

IDE=${ARDUINO_IDE}
BUILDER=${IDE}/arduino-builder

BUILD_PATH=${PWD}/build
CACHE_PATH=${PWD}/cache

ifndef verbose
VERBOSE=-quiet
else
VERBOSE=-verbose
endif

#VERBOSE=-verbose

bin:
	@mkdir -p ${BUILD_PATH} ${CACHE_PATH}

	@echo "======> compiling sketch"
	@${BUILDER} -compile \
		-logger=machine \
	       	-hardware /home/mschwartz/Downloads/arduino-1.8.5/hardware \
	       	-hardware /home/mschwartz/Arduino/hardware \
	       	-tools /home/mschwartz/Downloads/arduino-1.8.5/tools-builder \
	       	-tools /home/mschwartz/Downloads/arduino-1.8.5/hardware/tools/avr \
	       	-built-in-libraries /home/mschwartz/Downloads/arduino-1.8.5/libraries \
	       	-libraries /home/mschwartz/Arduino/libraries \
	       	-fqbn=espressif:esp32:odroid_esp32:FlashMode=qio,FlashFreq=80,PartitionScheme=default,UploadSpeed=921600,DebugLevel=none \
	       	-ide-version=10805 \
	       	-build-path ${BUILD_PATH} \
	       	-warnings=none \
	       	-build-cache ${CACHE_PATH} \
	       	-prefs=build.warn_data_percentage=75 \
	       	$(VERBOSE) $(INO)

	@echo "======> generating partitions file"
	@python "$(ESP32_ROOT)/gen_esp32part.py" \
		-q "$(ESP32_ROOT)/partitions/default.csv" \
		"$(BUILD_PATH)/$(INO).partitions.bin"

	@echo "======> generating bin from elf file"
	@python "$(ESP32_ROOT)/esptool.py" \
		--chip esp32 \
		elf2image \
		--flash_mode "dio" \
		--flash_freq "80m" \
		--flash_size "16MB" \
		-o "$(BUILD_PATH)/$(INO).bin" \
		"$(BUILD_PATH)/$(INO).elf"
 
flash: bin
	@echo "======> FLASHING DEVICE"
	@python "${ESP32_ROOT}/esptool.py" \
		--chip esp32 \
		--port COM1 \
		--baud 921600 \
		--before default_reset \
		--after hard_reset write_flash \
		-z \
		--flash_mode dio \
		--flash_freq 80m \
		--flash_size detect \
		0xe000  $(ESP32_ROOT)/partitions/boot_app0.bin \
		0x1000  $(ESP32_ROOT)/sdk/bin/bootloader_qio_80m.bin \
		0x10000 $(BUILD_PATH)/$(INO).bin \
		0x8000  $(BUILD_PATH)/$(INO).partitions.bin

prefs:
	${BUILDER} --dump-prefs \
		-dump-prefs \
		       -logger=machine \
		       -hardware ${ARDUINO_IDE}/hardware \
		       -hardware /home/mschwartz/Arduino/hardware \
		       -tools /home/mschwartz/Downloads/arduino-1.8.5/tools-builder \
		       -tools /home/mschwartz/Downloads/arduino-1.8.5/hardware/tools/avr \
		       -built-in-libraries /home/mschwartz/Downloads/arduino-1.8.5/libraries \
		       -libraries /home/mschwartz/Arduino/libraries \
		       -fqbn=espressif:esp32:odroid_esp32:FlashMode=qio,FlashFreq=80,PartitionScheme=default,UploadSpeed=921600,DebugLevel=none \
		       -ide-version=10805 \
		       -build-path ${BUILD_PATH} \
		       -warnings=none \
		       -build-cache ${CACHE_PATH} \
		       -prefs=build.warn_data_percentage=75 \
		       $(VERBOSE) $(INO)

clean:
	@rm -rf ${BUILD_PATH} ${CACHE_PATH)}

