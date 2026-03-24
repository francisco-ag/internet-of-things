from machine import Pin
from time import sleep

led = Pin(12, Pin.OUT)

while True:
    led.on()
    sleep(10)
    led.off()
    sleep(10)