# odroid-go-template
Bare project using makefiles for odroid-go Arduino programs

You will need to set up certain ENV variables for the Makefile to work.  The toolchain and libraries, specifically, are
likely to live in different locations on different peoples' machines.


```
export ARDUINO_IDE='/home/mschwartz/Downloads/arduino-1.8.5'
export ARDUINO_ROOT='/home/mschwartz/Arduino'
export ODROID_TOOLCHAIN='$(ARDUINO_ROOT)/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin'
export ESP32_ROOT='$(ARDUINO_ROOT)/hardware/espressif/esp32/tools'
typeset -U path
path+=$ODROID_TOOLCHAIN
path+=$ESP32_ROOT
```

To build your sketch, you need the one .ino file.  You will probably rename the one in this repo.

The command:
```
% make
```
will build your sketch.  It will not be uploaded into your device.

The command:
```
% make flash
```
will build your sketch and upload it into your device.  If you are doing a development cycle of edit/build/flash, then
this is the commannd you want.  You will also use this command to simply flash your device at any time.  

THe process of building the sketch is rather optimal.  Only files you change will be recompiled, etc.

The Makefile creates two subdirectories that are in .gitignore so they won't end up in the repo on GitHub.  The
directories are build/ and cache/.  The build/ directory is where the app is compiled and built.  The cache/ directory
is where libraries are built and preserved for next build.

By default, the Makefile is rather silent.  Minimal progress is reported.  If you want extremely verbose output, use:
```
make verbose=true
```

Other make targets:
```
make prefs
```
Dumps the toolchain preferences and exits.

```
make clean
```
Remove temporary/intermediate files and directories.  This will force a complete rebuild of your sketch.





