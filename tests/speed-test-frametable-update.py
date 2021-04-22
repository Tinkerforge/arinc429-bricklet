#!/usr/bin/python3
# -*- coding: utf-8 -*- 

HOST = "localhost"
PORT = 4223
UID  = "a2"

from tinkerforge.ip_connection     import IPConnection
from tinkerforge.bricklet_arinc429 import BrickletARINC429

import sys
import time


#######################################################################################
#                                                                                     #
#######################################################################################


# callback function for the heartbeat
def heartbeat_cb(channel, status, seq_number, timestamp, frames_processed, frames_lost) :

    print('Heartbeat #', seq_number, ' processed =', frames_processed, ' lost =', frames_lost, ' timestamp =', timestamp)



# main function
if __name__ == "__main__":

    ipcon = IPConnection()               # create IP connection
    a429  = BrickletARINC429(UID, ipcon) # create device object

    ipcon.connect(HOST, PORT)            # connect to brickd (don't use device before ipcon is connected)


    # for whatever happened before: reset the ARINC429 bricklet
    a429.restart()


    #################################################################    
    # channel config
    #################################################################

    a429.set_channel_configuration(channel = a429.CHANNEL_TX, parity = a429.PARITY_AUTO, speed = a429.SPEED_HS)
    a429.set_channel_configuration(channel = a429.CHANNEL_RX, parity = a429.PARITY_AUTO, speed = a429.SPEED_HS)

    ret_t1 = a429.get_channel_configuration(a429.CHANNEL_TX1)
    ret_r1 = a429.get_channel_configuration(a429.CHANNEL_RX1)
    ret_r2 = a429.get_channel_configuration(a429.CHANNEL_RX2)
    
    print('TX  Parity:', ret_t1.parity, ' Speed:', ret_t1.speed)
    print('RX1 Parity:', ret_r1.parity, ' Speed:', ret_r1.speed)
    print('RX2 Parity:', ret_r2.parity, ' Speed:', ret_r2.speed)


    #################################################################
    # TX schedule
    #################################################################

    # define frames
    #         label   SDI         Data
    frame1  =   1   +  0  * 256 +  1  * 1024
    frame2  =   1   +  0  * 256 +  2  * 1024


    # write frames
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = frame1)
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 1, frame = frame2)

    # in high speed mode a single frame including inter-frame-gap takes 0.36 ms
    # schedule: 6x frame1, 5x frame2 -> total time of one schedule cycle is 11 * 0.36 ms = 3.96 ms
    # with a dwell time of 4 ms per cycle, this schedule produces 27.500 frames within 10 seconds 

    dwell = 4

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 0,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)
   
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 1,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 2,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 3,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 4,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 5,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = 0)


    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 6,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 7,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 8,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 9,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 10,                        \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = dwell)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 11,                        \
                            job         = a429.SCHEDULER_JOB_JUMP,   \
                            frame_index = 0,                         \
                            dwell_time  = 0)


    print('wrote TX frames and schedule')


    #################################################################
    # heartbeat
    #################################################################
    
    # register heartbeat callback function
    a429.register_callback(a429.CALLBACK_HEARTBEAT_MESSAGE, heartbeat_cb)
    
    # configure callback
    a429.set_heartbeat_callback_configuration(channel = a429.CHANNEL_RX, period = 1000, enabled = True, value_has_to_change = False)

    print('heartbeat configured')


    #################################################################
    # start TX
    #################################################################

    # make sure all above set-up commands have arrived at the bricklet before the metered test run starts
    time.sleep(0.5)

    # start scheduler
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_RUN)

    print('Test started')

    # choose one stopping option:
    #input("\npress <enter> key to stop the test...\n\n") # use raw_input() in Python 2
    #time.sleep(10)

    counter = 2000

    while(counter) :

        counter -= 1
        a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = frame1)
        #time.sleep(0.001)


    #################################################################
    # clean-up
    #################################################################

    # stop transmission
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE)
    print('\nTest stopped\n')

    # stop heartbeat callbacks
    a429.set_heartbeat_callback_configuration(channel = a429.CHANNEL_RX, period = 0, enabled = False, value_has_to_change = False)
    print('\nstopped callbacks')

    # clear TX schedule
    a429.clear_schedule_entries(channel = a429.CHANNEL_TX, job_index_first = 0, job_index_last = 999)
    print('cleared complete TX schedule')

    # set mode to passive
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_PASSIVE)
    print('switched all channels to passive mode')

    # disconnect
    ipcon.disconnect()

    # all done
    print('done.\n')

