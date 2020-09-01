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

    a429.set_channel_configuration(a429.CHANNEL_TX, a429.PARITY_PARITY, a429.SPEED_LS)
    a429.set_channel_configuration(a429.CHANNEL_RX, a429.PARITY_PARITY, a429.SPEED_LS)

    print(a429.get_channel_configuration(a429.CHANNEL_TX1))
    print(a429.get_channel_configuration(a429.CHANNEL_RX1))
    print(a429.get_channel_configuration(a429.CHANNEL_RX2))
    try:
        print(a429.get_channel_configuration(a429.CHANNEL_RX)) # <-- exception expected!
    except:
        print("exception OK")
#        traceback.print_exc()

    a429.set_channel_mode(a429.CHANNEL_TX1, a429.CHANNEL_MODE_ACTIVE)
    print(a429.get_channel_mode(a429.CHANNEL_TX1))
    a429.set_channel_mode(a429.CHANNEL_TX1, a429.CHANNEL_MODE_PASSIVE)
    print(a429.get_channel_mode(a429.CHANNEL_TX1))
    a429.set_channel_mode(a429.CHANNEL_TX1, a429.CHANNEL_MODE_PASSIVE)
    print(a429.get_channel_mode(a429.CHANNEL_TX1))

    print(a429.get_prio_labels(a429.CHANNEL_RX1))
    a429.set_prio_labels(a429.CHANNEL_RX, [1, 2, 3])
    print(a429.get_prio_labels(a429.CHANNEL_RX1))
    a429.clear_prio_labels(a429.CHANNEL_RX)
    print(a429.get_prio_labels(a429.CHANNEL_RX1))

    ipcon.disconnect()
