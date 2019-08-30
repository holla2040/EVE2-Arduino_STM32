# references
# git://repo.or.cz/openocd.git
#   ./bootstrap
#   ./configure
#   make
#   sudo make install
# https://github.com/texane/stlink.git
#   make


ino = $(wildcard *.ino)
arduinopath = ~/arduino
remoteip = 10.9.0.18
#extra = -verbose 
extra = -quiet
githead = $(shell git rev-parse --short HEAD)

bin:
	@ mkdir -p /tmp/arduino_build /tmp/arduino_cache
	# @ /home/holla/arduino/arduino-builder -logger=machine -hardware /home/holla/arduino/hardware -hardware /home/holla/.arduino15/packages -hardware /home/holla/Arduino/hardware -tools /home/holla/arduino/tools-builder -tools /home/holla/arduino/hardware/tools/avr -tools /home/holla/.arduino15/packages -built-in-libraries /home/holla/arduino/libraries -libraries /home/holla/Arduino/libraries -fqbn=Arduino_STM32:STM32F1:genericSTM32F103C:device_variant=STM32F103C8,upload_method=STLinkMethod,cpu_speed=speed_72mhz,opt=osstd -ide-version=10808 -build-path /tmp/arduino_build -warnings=none -build-cache /tmp/arduino_cache -prefs=build.warn_data_percentage=75 $(extra) $(ino)
	@ /home/holla/arduino/arduino-builder -compile -logger=machine -hardware /home/holla/arduino/hardware -hardware /home/holla/.arduino15/packages -hardware /home/holla/Arduino/hardware -tools /home/holla/arduino/tools-builder -tools /home/holla/arduino/hardware/tools/avr -tools /home/holla/.arduino15/packages -built-in-libraries /home/holla/arduino/libraries -libraries /home/holla/Arduino/libraries -fqbn=Arduino_STM32:STM32F1:genericSTM32F103C:device_variant=STM32F103C8,upload_method=STLinkMethod,cpu_speed=speed_72mhz,opt=osstd -ide-version=10808 -build-path /tmp/arduino_build -warnings=none -build-cache /tmp/arduino_cache -prefs=build.warn_data_percentage=75 $(extra) -jobs 4 -prefs compiler.cpp.extra_flags=-DGITHEAD=\"$(githead)\" $(ino)
	@ls -sh /tmp/arduino_build/$(ino).bin


usb: 
	/home/holla/Arduino/hardware/Arduino_STM32/tools/linux/maple_upload ttyACM0 2 1EAF:0003 /tmp/arduino_build/$(ino).bin 

flashlocal:
	~/bin/st-flash write /tmp/arduino_build/$(ino).bin  0x8000000

flashremote:
	@ echo "scp to "$(remoteip)", flashing"
	@ scp /tmp/arduino_build/$(ino).bin $(remoteip):/tmp
	@ ssh $(remoteip) st-flash write /tmp/$(ino).bin  0x8000000

flashremoteboth: 
	@ echo "flashremote"
	@ echo "scp /tmp/arduino_build/"$(ino)".bin to "$(remoteip)"/tmp"
	@ scp /tmp/arduino_build/$(ino).bin $(remoteip):/tmp
	@ echo 'st-flash --serial $(stlink8)  write /tmp/$(ino).bin  0x8000000'
	@ echo 'st-flash --serial $(stlink12) write /tmp/$(ino).bin  0x8000000'
	@ ssh $(remoteip) 'st-flash --serial $(stlink8) write /tmp/$(ino).bin  0x8000000;st-flash --serial $(stlink12) write /tmp/$(ino).bin  0x8000000'

flash8: 
	@ echo "flashremote"
	@ echo "scp /tmp/arduino_build/"$(ino)".bin to "$(remoteip)"/tmp"
	@ scp /tmp/arduino_build/$(ino).bin $(remoteip):/tmp
	@ echo 'st-flash --serial $(stlink8)  write /tmp/$(ino).bin  0x8000000'
	@ ssh $(remoteip) 'st-flash --serial $(stlink8) write /tmp/$(ino).bin  0x8000000'

flash12: 
	@ echo "flashremote"
	@ echo "scp /tmp/arduino_build/"$(ino)".bin to "$(remoteip)"/tmp"
	@ scp /tmp/arduino_build/$(ino).bin $(remoteip):/tmp
	@ echo 'st-flash --serial $(stlink12)  write /tmp/$(ino).bin  0x8000000'
	@ ssh $(remoteip) 'st-flash --serial $(stlink12) write /tmp/$(ino).bin  0x8000000'

flashspare: 
	@ echo "flashremote"
	@ echo "scp /tmp/arduino_build/"$(ino)".bin to "$(remoteip)"/tmp"
	@ scp /tmp/arduino_build/$(ino).bin $(remoteip):/tmp
	@ echo 'st-flash --serial $(stlinkspare)  write /tmp/$(ino).bin  0x8000000'
	@ ssh $(remoteip) 'st-flash --serial $(stlinkspare) write /tmp/$(ino).bin  0x8000000'

resetlocal:
	@ echo "local reset"
	@ st-flash reset

resetremote:
	@ echo "$(remoteip) reset"
	@ ssh $(remoteip) st-flash reset

reset8:
	@ echo "$(remoteip) reset"
	@ ssh $(remoteip) st-flash --serial $(stlink8) reset

reset12:
	@ echo "$(remoteip) reset"
	@ ssh $(remoteip) st-flash --serial $(stlink12) reset

resetspare:
	@ echo "$(remoteip) reset"
	@ ssh $(remoteip) st-flash --serial $(stlinkspare) reset

probelocal:
	@ echo "local probe"
	@st-info --probe

proberemote:
	@ echo "$(remoteip) probe"
	@ ssh $(remoteip) st-info --probe
	@ echo
	@ ssh $(remoteip) st-info --probe | grep serial | awk '{print $2}'

watch:
	@ while [ 1 ]; do clear;make; sleep 1; inotifywait -e modify *.h *.cpp *.ino; done

clean:
	rm -rf /tmp/arduino_build/*
	rm -rf /tmp/arduino_cache/*

unlock:
	- openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init;reset halt;stm32f1x unlock 0;reset halt;exit"

# don't remove this
unlockold:
	@ echo "hold reset"
	@ sleep 5
	- openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init;reset halt; flash banks;stm32f1x mass_erase 0;stm32f1x unlock 0"
	@ echo "\n\n\nrelease reset"
	@ sleep 5
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init;reset halt; flash banks;stm32f1x mass_erase 0;stm32f1x unlock 0"


flash: flashlocal
reset: resetlocal
probe: proberemote
