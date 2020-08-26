#!/usr/bin/python3
# -*- coding: utf-8 -*- 

HOST = "localhost"
PORT = 4223

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_arinc429 import BrickletARINC429
import sys

# For this test connect two ARINC429 Bricklets: 
# TX from Bricklet 1 with RX1 and RX2 from Bricklet2
# TX from Bricklet 2 with RX1 and RX2 from Bricklet1 

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    a1 = BrickletARINC429("a1", ipcon) # Create device object
    a2 = BrickletARINC429("a2", ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected


    # 1 MASTER RESET
    error = a1.debug_write_register(0x04, [])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 1 !!! Error during Master Reset: {0}'.format(error))
    else:
        print('A1 1 Master Reset OK')
    error = a2.debug_write_register(0x04, [])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('1 !!! Error during Master Reset: {0}'.format(error))
    else:
        print('1 Master Reset OK')



    # 2+3 ACLK DIVISION REGISTER
    error = a1.debug_write_register(0x38, [0x00])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 2 !!! Error during ACLK write: {0}'.format(error))
    else:
        print('A1 2 ACLK write OK')
    error = a2.debug_write_register(0x38, [0x00])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('2 !!! Error during ACLK write: {0}'.format(error))
    else:
        print('2 ACLK write OK')

    ret = a1.debug_read_register(0xD4)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 3 !!! Error during ACLK read: {0}'.format(error))
    else:
        print('A1 3 ACLK read OK')

    if ret.value[0] != 0x00:
        print('A1 3 !!! ACLK write/read mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('A1 3 ACLK write/read match OK')


    ret = a2.debug_read_register(0xD4)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('3 !!! Error during ACLK read: {0}'.format(error))
    else:
        print('3 ACLK read OK')

    if ret.value[0] != 0x00:
        print('3 !!! ACLK write/read mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('3 ACLK write/read match OK')


    # 4+5 RECEIVE CONTROL REGISTER Ch. 1
    error = a1.debug_write_register(0x10, [0x88])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 4 !!! Error during Recv Control reg ch1 write: {0}'.format(error))
    else:
        print('A1 4 Recv Control reg ch1 write OK')
    error = a2.debug_write_register(0x10, [0x88])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('4 !!! Error during Recv Control reg ch1 write: {0}'.format(error))
    else:
        print('4 Recv Control reg ch1 write OK')

    ret = a1.debug_read_register(0x94)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 5 !!! Error during Recv Control reg ch1 read: {0}'.format(error))
    else:
        print('A1 5 Recv Control reg ch1 read OK')

    if ret.value[0] != 0x88:
        print('A1 5 !!! Recv Control reg ch1 write/read mismatch: {0} vs {1}'.format(0x88, ret.value[0]))
    else:
        print('A1 5 Recv Control reg ch1 write/read match OK')
    ret = a2.debug_read_register(0x94)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('5 !!! Error during Recv Control reg ch1 read: {0}'.format(error))
    else:
        print('5 Recv Control reg ch1 read OK')

    if ret.value[0] != 0x88:
        print('5 !!! Recv Control reg ch1 write/read mismatch: {0} vs {1}'.format(0x88, ret.value[0]))
    else:
        print('5 Recv Control reg ch1 write/read match OK')


    # 6+7 RECEIVE CONTROL REGISTER Ch. 2
    error = a1.debug_write_register(0x24, [0x88])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 6 !!! Error during Recv Control reg ch2 write: {0}'.format(error))
    else:
        print('A1 6 Recv Control reg ch2 write OK')
    error = a2.debug_write_register(0x24, [0x88])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('6 !!! Error during Recv Control reg ch2 write: {0}'.format(error))
    else:
        print('6 Recv Control reg ch2 write OK')

    ret = a1.debug_read_register(0xB4)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 7 !!! Error during Recv Control reg ch2 read: {0}'.format(error))
    else:
        print('A1 7 Recv Control reg ch2 read OK')

    if ret.value[0] != 0x88:
        print('A1 7 !!! Recv Control reg ch2 write/read mismatch: {0} vs {1}'.format(0x88, ret.value[0]))
    else:
        print('A1 7 Recv Control reg ch2 write/read match OK')
    ret = a2.debug_read_register(0xB4)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('7 !!! Error during Recv Control reg ch2 read: {0}'.format(error))
    else:
        print('7 Recv Control reg ch2 read OK')

    if ret.value[0] != 0x88:
        print('7 !!! Recv Control reg ch2 write/read mismatch: {0} vs {1}'.format(0x88, ret.value[0]))
    else:
        print('7 Recv Control reg ch2 write/read match OK')


    # 8+9 TRANSMIT CONTROL REGISTER
    error = a1.debug_write_register(0x08, [0x64])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 8 !!! Error during Transmit Control reg write: {0}'.format(error))
    else:
        print('A1 8 Transmit Control reg write OK')
    error = a2.debug_write_register(0x08, [0x64])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('8 !!! Error during Transmit Control reg write: {0}'.format(error))
    else:
        print('8 Transmit Control reg write OK')


    ret = a1.debug_read_register(0x84)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 9 !!! Error during Transmit Control reg read: {0}'.format(error))
    else:
        print('A1 9 Transmit Control reg read OK')

    if ret.value[0] != 0x64:
        print('A1 9 !!! Transmit Control reg write/read mismatch: {0} vs {1}'.format(0x64, ret.value[0]))
    else:
        print('A1 9 Transmit Control reg write/read match OK')
    ret = a2.debug_read_register(0x84)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('9 !!! Error during Transmit Control reg read: {0}'.format(error))
    else:
        print('9 Transmit Control reg read OK')

    if ret.value[0] != 0x64:
        print('9 !!! Transmit Control reg write/read mismatch: {0} vs {1}'.format(0x64, ret.value[0]))
    else:
        print('9 Transmit Control reg write/read match OK')



    # 10+11 FLAG / INTERRUPT ASSIGNMENT REGISTER
    error = a1.debug_write_register(0x34, [0x33])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 10 !!! Error during Flag / Int Assignment reg write: {0}'.format(error))
    else:
        print('A1 10 Flag / Int Assignment reg write OK')
    error = a2.debug_write_register(0x34, [0x33])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('10 !!! Error during Flag / Int Assignment reg write: {0}'.format(error))
    else:
        print('10 Flag / Int Assignment reg write OK')

    ret = a1.debug_read_register(0xD0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 11 !!! Error during Flag / Int Assignment reg read: {0}'.format(error))
    else:
        print('A1 11 Flag / Int Assignment reg read OK')

    if ret.value[0] != 0x33:
        print('A1 11 !!! Flag / Int Assignment reg write/read mismatch: {0} vs {1}'.format(0x33, ret.value[0]))
    else:
        print('A1 11 Flag / Int Assignment reg write/read match OK')
    ret = a2.debug_read_register(0xD0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('11 !!! Error during Flag / Int Assignment reg read: {0}'.format(error))
    else:
        print('11 Flag / Int Assignment reg read OK')

    if ret.value[0] != 0x33:
        print('11 !!! Flag / Int Assignment reg write/read mismatch: {0} vs {1}'.format(0x33, ret.value[0]))
    else:
        print('11 Flag / Int Assignment reg write/read match OK')


    # 12 RECEIVER 1 STATUS REGISTER
    ret = a1.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 12 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('A1 12 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x01:
        print('A1 12 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('A1 12 Receiver 1 Status reg value OK')
    ret = a2.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('12 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('12 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x01:
        print('12 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('12 Receiver 1 Status reg value OK')



    # 13 RECEIVER 2 STATUS REGISTER
    ret = a1.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('A1 13 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('A1 13 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x01:
        print('A1 13 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('A1 13 Receiver 2 Status reg value OK')
    ret = a2.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('13 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('13 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x01:
        print('13 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('13 Receiver 2 Status reg value OK')


    # 14 R1FLAG, R2FLAG
    ret = a1.debug_get_discretes()
    r1flag = ret.rx_discretes & (1 << 1)
    r2flag = ret.rx_discretes & (1 << 6)
    if r1flag != 0:
        print('A1 14 !!! Error during R1FLAG check')
    else:
        print('A1 14 R1FLAG check OK')

    if r2flag != 0:
        print('A1 14 !!! Error during R2FLAG check')
    else:
        print('A1 14 R2FLAG check OK')

    # 15 TEMPTY
    tempty = ret.tx_discretes & (1 << 0)
    if tempty == 0:
        print('A1 15 !!! Error during TEMPTY check')
    else:
        print('A1 15 TEMPTY check OK')

    ret = a2.debug_get_discretes()
    r1flag = ret.rx_discretes & (1 << 1)
    r2flag = ret.rx_discretes & (1 << 6)
    if r1flag != 0:
        print('14 !!! Error during R1FLAG check')
    else:
        print('14 R1FLAG check OK')

    if r2flag != 0:
        print('14 !!! Error during R2FLAG check')
    else:
        print('14 R2FLAG check OK')

    # 15 TEMPTY
    tempty = ret.tx_discretes & (1 << 0)
    if tempty == 0:
        print('15 !!! Error during TEMPTY check')
    else:
        print('15 TEMPTY check OK')


    # 16 WRITE TO TX FIFO
    error = a1.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('A1 16 !!! Error during TX FIFO write: {0}'.format(error))
    else:
        print('A1 16 TX FIFO write OK')
    error = a2.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('16 !!! Error during TX FIFO write: {0}'.format(error))
    else:
        print('16 TX FIFO write OK')


    # 17 R1INT, R2INT
    ret = a1.debug_get_discretes()
    r1int = ret.rx_discretes & (1 << 0)
    r2int = ret.rx_discretes & (1 << 5)
    if r1int != 0:
        print('A1 17 !!! Error during R1INT check')
    else:
        print('A1 17 R1INT check OK')

    if r2int != 0:
        print('A1 17 !!! Error during R2INT check')
    else:
        print('A1 17 R2INT check OK')
    # 18 R1FLAG, R2FLAG
    r1flag = ret.rx_discretes & (1 << 1)
    r2flag = ret.rx_discretes & (1 << 6)
    if r1flag == 0:
        print('A1 18 !!! Error during R1FLAG check')
    else:
        print('A1 18 R1FLAG check OK')

    if r2flag == 0:
        print('A1 18 !!! Error during R2FLAG check')
    else:
        print('A1 18 R2FLAG check OK')

    ret = a2.debug_get_discretes()
    r1int = ret.rx_discretes & (1 << 0)
    r2int = ret.rx_discretes & (1 << 5)
    if r1int != 0:
        print('17 !!! Error during R1INT check')
    else:
        print('17 R1INT check OK')

    if r2int != 0:
        print('17 !!! Error during R2INT check')
    else:
        print('17 R2INT check OK')
    # 18 R1FLAG, R2FLAG
    r1flag = ret.rx_discretes & (1 << 1)
    r2flag = ret.rx_discretes & (1 << 6)
    if r1flag == 0:
        print('18 !!! Error during R1FLAG check')
    else:
        print('18 R1FLAG check OK')

    if r2flag == 0:
        print('18 !!! Error during R2FLAG check')
    else:
        print('18 R2FLAG check OK')


    # 19 RECEIVER 1 STATUS REGISTER
    ret = a1.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('19 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('19 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x00:
        print('19 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('19 Receiver 1 Status reg value OK')
    ret = a2.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('19 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('19 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x00:
        print('19 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('19 Receiver 1 Status reg value OK')


    # 20 READ RECEIVER 1 FIFO
    ret = a1.debug_read_register(0xA0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('20 !!! Error during Receiver 1 FIFO read: {0}'.format(error))
    else:
        print('20 Receiver 1 FIFO read OK')

    if tuple(ret.value) != (0x01, 0x02, 0x03, 0x04):
        print('20 !!! Receiver 1 FIFO write/read mismatch: {0} vs {1}'.format((0x01, 0x02, 0x03, 0x04), tuple(ret.value)))
    else:
        print('20 Receiver 1 FIFO write/read OK')
    ret = a2.debug_read_register(0xA0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('20 !!! Error during Receiver 1 FIFO read: {0}'.format(error))
    else:
        print('20 Receiver 1 FIFO read OK')

    if tuple(ret.value) != (0x01, 0x02, 0x03, 0x04):
        print('20 !!! Receiver 1 FIFO write/read mismatch: {0} vs {1}'.format((0x01, 0x02, 0x03, 0x04), tuple(ret.value)))
    else:
        print('20 Receiver 1 FIFO write/read OK')


    # 21 RECEIVER 1 STATUS REGISTER
    ret = a1.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('21 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('21 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x01:
        print('21 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('21 Receiver 1 Status reg value OK')
    ret = a2.debug_read_register(0x90)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('21 !!! Error during Receiver 1 Status reg read: {0}'.format(error))
    else:
        print('21 Receiver 1 Status reg read OK')

    if ret.value[0] != 0x01:
        print('21 !!! Receiver 1 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('21 Receiver 1 Status reg value OK')

    # 22 R1FLAG
    ret = a1.debug_get_discretes()
    r1flag = ret.rx_discretes & (1 << 1)
    if r1flag != 0:
        print('22 !!! Error during R1FLAG check')
    else:
        print('22 R1FLAG check OK')
    ret = a2.debug_get_discretes()
    r1flag = ret.rx_discretes & (1 << 1)
    if r1flag != 0:
        print('22 !!! Error during R1FLAG check')
    else:
        print('22 R1FLAG check OK')

    # 23 READ RECEIVER 1 FIFO
    ret = a1.debug_read_register(0xA0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('23 !!! Error during Receiver 1 FIFO read: {0}'.format(error))
    else:
        print('23 Receiver 1 FIFO read OK')

    if tuple(ret.value) == (0x01, 0x02, 0x03, 0x04): # TODO: CHECK FOR ERROR HERE?
        print('23 !!! Receiver 1 FIFO write/read mismatch: {0} vs {1}'.format((0x01, 0x02, 0x03, 0x04), tuple(ret.value)))
    else:
        print('23 Receiver 1 FIFO write/read OK')
    ret = a2.debug_read_register(0xA0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('23 !!! Error during Receiver 1 FIFO read: {0}'.format(error))
    else:
        print('23 Receiver 1 FIFO read OK')

    if tuple(ret.value) == (0x01, 0x02, 0x03, 0x04): # TODO: CHECK FOR ERROR HERE?
        print('23 !!! Receiver 1 FIFO write/read mismatch: {0} vs {1}'.format((0x01, 0x02, 0x03, 0x04), tuple(ret.value)))
    else:
        print('23 Receiver 1 FIFO write/read OK')

    # 24 RECEIVER 2 STATUS REGISTER
    ret = a1.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('24 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('24 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x00:
        print('24 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('24 Receiver 2 Status reg value OK')
    ret = a2.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('24 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('24 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x00:
        print('24 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x00, ret.value[0]))
    else:
        print('24 Receiver 2 Status reg value OK')


    # 25 READ RECEIVER 2 FIFO
    ret = a1.debug_read_register(0xC0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('25 !!! Error during Receiver 2 FIFO read: {0}'.format(error))
    else:
        print('25 Receiver 2 FIFO read OK')

    if tuple(ret.value) != (0x01, 0x02, 0x03, 0x04): # We just check that it is _not_ the original message
        print('25 !!! Receiver 2 FIFO unexpected read'.format(tuple(ret.value)))
    else:
        print('25 Receiver 2 FIFO read OK')
    ret = a2.debug_read_register(0xC0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('25 !!! Error during Receiver 2 FIFO read: {0}'.format(error))
    else:
        print('25 Receiver 2 FIFO read OK')

    if tuple(ret.value) != (0x01, 0x02, 0x03, 0x04): # We just check that it is _not_ the original message
        print('25 !!! Receiver 2 FIFO unexpected read'.format(tuple(ret.value)))
    else:
        print('25 Receiver 2 FIFO read OK')


    # 26 RECEIVER 2 STATUS REGISTER
    ret = a1.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('26 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('26 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x01:
        print('26 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('26 Receiver 2 Status reg value OK')
    ret = a2.debug_read_register(0xB0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('26 !!! Error during Receiver 2 Status reg read: {0}'.format(error))
    else:
        print('26 Receiver 2 Status reg read OK')

    if ret.value[0] != 0x01:
        print('26 !!! Receiver 2 Status reg value mismatch: {0} vs {1}'.format(0x01, ret.value[0]))
    else:
        print('26 Receiver 2 Status reg value OK')


    # 27 R2FLAG
    ret = a1.debug_get_discretes()
    r2flag = ret.rx_discretes & (1 << 6)
    if r2flag != 0:
        print('27 !!! Error during R1FLAG check')
    else:
        print('27 R2FLAG check OK')
    ret = a2.debug_get_discretes()
    r2flag = ret.rx_discretes & (1 << 6)
    if r2flag != 0:
        print('27 !!! Error during R1FLAG check')
    else:
        print('27 R2FLAG check OK')


    # 28 READ RECEIVER 1 FIFO
    ret = a1.debug_read_register(0xC0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('28 !!! Error during Receiver 2 FIFO read: {0}'.format(error))
    else:
        print('28 Receiver 2 FIFO read OK')

    if tuple(ret.value) == (0x01, 0x02, 0x03, 0x04): # We just check that it is _not_ the original message
        print('28 !!! Receiver 2 FIFO unexpected read: {0}'.format(tuple(ret.value)))
    else:
        print('28 Receiver 2 FIFO read OK')
    ret = a2.debug_read_register(0xC0)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('28 !!! Error during Receiver 2 FIFO read: {0}'.format(error))
    else:
        print('28 Receiver 2 FIFO read OK')

    if tuple(ret.value) == (0x01, 0x02, 0x03, 0x04): # We just check that it is _not_ the original message
        print('28 !!! Receiver 2 FIFO unexpected read: {0}'.format(tuple(ret.value)))
    else:
        print('28 Receiver 2 FIFO read OK')


    # 29+30 TRANSMIT CONTROL REGISTER
    error = a1.debug_write_register(0x08, [0x44])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('29 !!! Error during Transmit Control reg write: {0}'.format(error))
    else:
        print('29 Transmit Control reg write OK')
    error = a2.debug_write_register(0x08, [0x44])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('29 !!! Error during Transmit Control reg write: {0}'.format(error))
    else:
        print('29 Transmit Control reg write OK')

    ret = a1.debug_read_register(0x84)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('30 !!! Error during Transmit Control reg read: {0}'.format(error))
    else:
        print('30 Transmit Control reg read OK')

    if ret.value[0] != 0x44:
        print('30 !!! Transmit Control reg write/read mismatch: {0} vs {1}'.format(0x44, ret.value[0]))
    else:
        print('30 Transmit Control reg write/read match OK')
    ret = a2.debug_read_register(0x84)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('30 !!! Error during Transmit Control reg read: {0}'.format(error))
    else:
        print('30 Transmit Control reg read OK')

    if ret.value[0] != 0x44:
        print('30 !!! Transmit Control reg write/read mismatch: {0} vs {1}'.format(0x44, ret.value[0]))
    else:
        print('30 Transmit Control reg write/read match OK')


    # 31 WRITE TO TX FIFO (32x)
    for i in range(1, 33):
        error = a1.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
        if error != BrickletARINC429.RW_ERROR_OK:
            print('31 !!! Error during TX FIFO write ({0}/32): {1}'.format(i, error))
        else:
            print('31 TX FIFO write ({0}/32) OK'.format(i))
        error = a2.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
        if error != BrickletARINC429.RW_ERROR_OK:
            print('31 !!! Error during TX FIFO write ({0}/32): {1}'.format(i, error))
        else:
            print('31 TX FIFO write ({0}/32) OK'.format(i))


    # 32 TFULL
    ret = a1.debug_get_discretes()
    tfull = ret.tx_discretes & (1 << 1)
    if tfull == 0:
        print('32 !!! Error during TFULL check')
    else:
        print('32 TFULL check OK')
    ret = a2.debug_get_discretes()
    tfull = ret.tx_discretes & (1 << 1)
    if tfull == 0:
        print('32 !!! Error during TFULL check')
    else:
        print('32 TFULL check OK')


    # 33 WRITE TO TX FIFO
    error = a1.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('33 !!! Error during TX FIFO write: {0}'.format(error))
    else:
        print('33 TX FIFO write OK')
    error = a2.debug_write_register(0x0C, [0x01, 0x02, 0x03, 0x04])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('33 !!! Error during TX FIFO write: {0}'.format(error))
    else:
        print('33 TX FIFO write OK')


    # 34 TRANSMIT TX BUFFER
    error = a1.debug_write_register(0x40, [])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('34 !!! Error during Transmit TX Buffer write: {0}'.format(error))
    else:
        print('34 Transmit TX Buffer write OK')
    error = a2.debug_write_register(0x40, [])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('34 !!! Error during Transmit TX Buffer write: {0}'.format(error))
    else:
        print('34 Transmit TX Buffer write OK')


    # 35 TFULL
    ret = a1.debug_get_discretes()
    tfull = ret.tx_discretes & (1 << 1)
    if tfull != 0:
        print('35 !!! Error during TFULL check')
    else:
        print('35 TFULL check OK')
    ret = a2.debug_get_discretes()
    tfull = ret.tx_discretes & (1 << 1)
    if tfull != 0:
        print('35 !!! Error during TFULL check')
    else:
        print('35 TFULL check OK')


    # 36+37 PRIO FILTER CHANNEL 1
    error = a1.debug_write_register(0x18, [0x42, 0x43, 0x44])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('36 !!! Error during Prio Filter Ch1 reg write: {0}'.format(error))
    else:
        print('36 Prio Filter Ch1 reg write OK')
    error = a2.debug_write_register(0x18, [0x42, 0x43, 0x44])
    if error != BrickletARINC429.RW_ERROR_OK:
        print('36 !!! Error during Prio Filter Ch1 reg write: {0}'.format(error))
    else:
        print('36 Prio Filter Ch1 reg write OK')

    ret = a1.debug_read_register(0x9C)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('37 !!! Error during Prio Filter Ch1 reg read: {0}'.format(error))
    else:
        print('37 Prio Filter Ch1 reg read OK')

    if tuple(ret.value) != (0x42, 0x43, 0x44):
        print('37 !!! Prio Filter Ch1 reg write/read mismatch: {0} vs {1}'.format((0x42, 0x43, 0x44), tuple(ret.value)))
    else:
        print('37 Prio Filter Ch1 reg write/read match OK')
    ret = a2.debug_read_register(0x9C)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('37 !!! Error during Prio Filter Ch1 reg read: {0}'.format(error))
    else:
        print('37 Prio Filter Ch1 reg read OK')

    if tuple(ret.value) != (0x42, 0x43, 0x44):
        print('37 !!! Prio Filter Ch1 reg write/read mismatch: {0} vs {1}'.format((0x42, 0x43, 0x44), tuple(ret.value)))
    else:
        print('37 Prio Filter Ch1 reg write/read match OK')


    # 38+39 FIFO FILTER CHANNEL 1
    error = a1.debug_write_register(0x14, tuple(range(32)))
    if error != BrickletARINC429.RW_ERROR_OK:
        print('38 !!! Error during FIFO Filter Ch1 reg write: {0}'.format(error))
    else:
        print('38 FIFO Filter Ch1 reg write OK')
    error = a2.debug_write_register(0x14, tuple(range(32)))
    if error != BrickletARINC429.RW_ERROR_OK:
        print('38 !!! Error during FIFO Filter Ch1 reg write: {0}'.format(error))
    else:
        print('38 FIFO Filter Ch1 reg write OK')

    ret = a1.debug_read_register(0x98)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('39 !!! Error during FIFO Filter Ch1 reg read: {0}'.format(error))
    else:
        print('39 FIFO Filter Ch1 reg read OK')

    if tuple(ret.value) != tuple(range(32)):
        print('39 !!! FIFO Filter Ch1 reg write/read mismatch: {0} vs {1}'.format(tuple(range(32)), tuple(ret.value)))
    else:
        print('39 FIFO Filter Ch1 reg write/read match OK')
    ret = a2.debug_read_register(0x98)
    if ret.rw_error != BrickletARINC429.RW_ERROR_OK:
        print('39 !!! Error during FIFO Filter Ch1 reg read: {0}'.format(error))
    else:
        print('39 FIFO Filter Ch1 reg read OK')

    if tuple(ret.value) != tuple(range(32)):
        print('39 !!! FIFO Filter Ch1 reg write/read mismatch: {0} vs {1}'.format(tuple(range(32)), tuple(ret.value)))
    else:
        print('39 FIFO Filter Ch1 reg write/read match OK')

    ipcon.disconnect()
