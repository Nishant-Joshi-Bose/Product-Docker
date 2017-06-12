#!/usr/bin/env python
import pigpio
import time
pigpio.exceptions = False
pi = pigpio.pi()
address = 0x08
intpin = 4

# Bus scan
for x in range(0x04, 0x79):
    h = pi.i2c_open(1, x)
    if h >= 0:
        s = pi.i2c_read_byte(h)
        if s >= 0:
           print("Device {} found".format(x))
#        else:
#           print("Device {} not found".format(x))
        pi.i2c_close(h)

h = pi.i2c_open(1, 0x08)
reading = False

def handle_interrupt(gpio, level, tick):
   reading = True
   print(gpio, level, tick);
   (count, data) = pi.i2c_read_device(h, 128)
   print("Read {} bytes: {}".format(count,''.join('{:02x}'.format(x) for x in data)))
   reading = False

#set_mode(intpin, INPUT)
pi.callback(intpin, pigpio.RISING_EDGE, handle_interrupt)

while True:
#    for led in range(8):
#        print("Led: {}".format(led))
#        pi.i2c_write_device(h, [0x01, led, 0x17])
        pi.i2c_write_device(h, "{}") # Start telemetry
        while(reading):
            time.sleep(1)
        time.sleep(1)

pi.i2c_close(h);
