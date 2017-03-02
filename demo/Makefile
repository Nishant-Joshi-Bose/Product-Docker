PROG = capsense_web_server
#MODULE_CFLAGS = -DMG_ENABLE_FILESYSTEM=0
MODULE_CFLAGS=-DMG_ENABLE_THREADS -DMG_ENABLE_FILESYSTEM=1 -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -lwiringPi tinyplay.c -Itinyalsa/include -Ltinyalsa/src -ltinyalsa -Wl,-rpath=/home/pi/Eddie-Demo/capsense/tinyalsa/src
include ./examples.mk
