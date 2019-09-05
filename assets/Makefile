fnt_cvt:
	cd "/home/holla/.wine/drive_c/users/holla/My Documents/EVE Asset Builder"; \
	wine "./EVE Asset Builder.exe"  fnt_cvt -f legacy -i Z:/home/holla/r/d/display/eve2/src/EVE2-Arduino_STM32/assets/ibm-plex-mono.semibold.ttf -s 80 -d 10000 -c setfont -l 32 -t Z:/home/holla/r/d/display/eve2/src/EVE2-Arduino_STM32/assets/numbers.txt -o  Z:/home/holla/r/d/display/eve2/src/EVE2-Arduino_STM32/assets/output
	clear
	find output
	cp output/ibm-plex-mono_80/L1/ibm-plex-mono_80_L1.rawh /home/holla/r/d/display/eve2/src/EVE2-Arduino_STM32/fonts

clean:
	rm -rf output/*
