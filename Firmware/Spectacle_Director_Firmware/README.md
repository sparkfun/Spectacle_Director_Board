Spectacle Director Board Firmware
================================= 

### Compiling the Firmware

1. Install the [Espressif IoT Development Framework (ESP-IDF)](https://github.com/espressif/esp-idf).
2. Make sure to set `IDF_PATH` to the path of your "esp-idf" directory. E.g.:

	export IDF_PATH=/c/espressif/esp-idf

3. Type `make menuconfig` to configure the pre-processor and serial flashing defines.
	* Select "Serial Flasher Config", and change the **Default serial port** to your board's port
	* Set **Flash size** to **4MB**
	* (Optional) Set the default baud rate to 921600
	
### Flashing the Firmware

1. Type `make flash` to flash the firmware.
2. Or use an esptool.py command like:

	python esptool.py --chip esp32 --port COMPORT --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0x1000 build/bootloader/bootloader.bin 0x10000 build/spectacle_director.bin 0x8000 build/partitions_singleapp.bin

---

The structure of this directory, and the Makefile operation is based on the [ESP-IDF template](https://github.com/espressif/esp-idf-template).