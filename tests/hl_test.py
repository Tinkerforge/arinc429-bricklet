#!/usr/bin/python3
# -*- coding: utf-8 -*- 

HOST = "localhost"
PORT = 4223
UID = "a1"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_arinc429 import BrickletARINC429
import sys
import traceback

# First tests of high level functions

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    a429 = BrickletARINC429(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    a429.set_channel_configuration(a429.A429_CHANNEL_TX, a429.A429_PARITY_PARITY, a429.A429_SPEED_LS)
    a429.set_channel_configuration(a429.A429_CHANNEL_RX, a429.A429_PARITY_PARITY, a429.A429_SPEED_LS)

    print(a429.get_channel_configuration(a429.A429_CHANNEL_TX1))
    print(a429.get_channel_configuration(a429.A429_CHANNEL_RX1))
    print(a429.get_channel_configuration(a429.A429_CHANNEL_RX2))
    try:
        print(a429.get_channel_configuration(a429.A429_CHANNEL_RX)) # <-- exception expected!
    except:
        traceback.print_exc()


    ipcon.disconnect()
