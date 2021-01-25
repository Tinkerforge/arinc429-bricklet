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
# For this test connect TX with RX1 and RX2 (i.e. all 'A' together, all 'B' together) #
#######################################################################################


# callback function for the heartbeat
def heartbeat_cb(seq_number, timestamp, frames_processed, frames_lost) :

    print('Heartbeat #', seq_number, ' processed =', frames_processed, ' lost =', frames_lost, ' timestamp =', timestamp)


#callback function for the frame message
def frame_cb(channel, seq_number, timestamp, status, frame, age) :

    print('new Frame #', seq_number, ' channel =', channel, ' status =', status, ' frame =', frame, ' age =', age, 'ms', ' timestamp =', timestamp)


#callback function for the scheduler message
def scheduler_cb(channel, seq_number, timestamp, token) :

    print('Scheduler #', seq_number, ' channel =', channel, ' token =', token, ' timestamp =', timestamp)  


# main function
if __name__ == "__main__":

    ipcon = IPConnection()               # create IP connection
    a429  = BrickletARINC429(UID, ipcon) # create device object

    ipcon.connect(HOST, PORT)            # connect to brickd (don't use device before ipcon is connected)


    # for whatever happened before: reset the ARINC429 bricklet
    a429.restart()


    #################################################################
    # Test  1 - capabilities
    #################################################################

    print('\nCapabilities')
    print('------------')

    ret = a429.get_capabilities()

    print('TX Scheduler Jobs total/used:', ret.tx_total_scheduler_jobs, '/', ret.tx_used_scheduler_jobs)
    print('RX Frame Filters  total/used:', ret.rx_total_frame_filters,  '/', ret.rx_used_frame_filters )


    #################################################################    
    # Test  2 - general - channel config
    #################################################################

    print('\nChannel Configuration')
    print('---------------------')

    a429.set_channel_configuration(channel = a429.CHANNEL_TX, parity = a429.PARITY_AUTO, speed = a429.SPEED_LS)
    a429.set_channel_configuration(channel = a429.CHANNEL_RX, parity = a429.PARITY_AUTO, speed = a429.SPEED_LS)

    ret_t1 = a429.get_channel_configuration(a429.CHANNEL_TX1)
    ret_r1 = a429.get_channel_configuration(a429.CHANNEL_RX1)
    ret_r2 = a429.get_channel_configuration(a429.CHANNEL_RX2)
    
    print('TX  Parity:', ret_t1.parity, ' Speed:', ret_t1.speed)
    print('RX1 Parity:', ret_r1.parity, ' Speed:', ret_r1.speed)
    print('RX2 Parity:', ret_r2.parity, ' Speed:', ret_r2.speed)


    #################################################################
    # Test  3 - RX - filters
    #################################################################
    
    print('\nRX Filters')
    print('----------')

    # set filters in parallel for RX1 and RX2
    a429.set_rx_filter(channel = a429.CHANNEL_RX, label = 1, sdi = a429.SDI_DATA)
    a429.set_rx_filter(channel = a429.CHANNEL_RX, label = 2, sdi = a429.SDI_DATA)
    a429.set_rx_filter(channel = a429.CHANNEL_RX, label = 3, sdi = a429.SDI_DATA)

    # clear filters selective for RX1 and RX2
    a429.clear_rx_filter(channel = a429.CHANNEL_RX1, label = 3, sdi = a429.SDI_DATA)
    a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 1, sdi = a429.SDI_DATA)
    a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 2, sdi = a429.SDI_DATA)

    # replace filter for label 3 on RX2 by an SDI-selective filter
    a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_DATA)
    a429.set_rx_filter(  channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI1)
    a429.set_rx_filter(  channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI2)

    # set and clear another filter
    a429.set_rx_filter(  channel = a429.CHANNEL_RX2, label = 4, sdi = a429.SDI_SDI1)
    a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 4, sdi = a429.SDI_SDI1)

    print('wrote filters')

    ret11  = a429.get_rx_filter(channel = a429.CHANNEL_RX1, label = 1, sdi = a429.SDI_DATA)
    ret12  = a429.get_rx_filter(channel = a429.CHANNEL_RX1, label = 2, sdi = a429.SDI_DATA)
    ret13  = a429.get_rx_filter(channel = a429.CHANNEL_RX1, label = 3, sdi = a429.SDI_DATA)

    ret21  = a429.get_rx_filter(channel = a429.CHANNEL_RX2, label = 1, sdi = a429.SDI_DATA)
    ret22  = a429.get_rx_filter(channel = a429.CHANNEL_RX2, label = 2, sdi = a429.SDI_DATA)
    ret23  = a429.get_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_DATA)
    ret231 = a429.get_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI1)
    ret232 = a429.get_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI2)

    print('filters read-back:')
    print('Channel 1, Label 1       : filter set =', ret11 )  # shall deliver true 
    print('Channel 1, Label 2       : filter set =', ret12 )  # shall deliver true
    print('Channel 1, Label 3       : filter set =', ret13 )  # shall deliver false

    print('Channel 2, Label 1       : filter set =', ret21 )  # shall deliver false
    print('Channel 2, Label 2       : filter set =', ret22 )  # shall deliver false
    print('Channel 2, Label 2, DATA : filter set =', ret23 )  # shall deliver false
    print('Channel 2, Label 3, SDI1 : filter set =', ret231)  # shall deliver true
    print('Channel 2, Label 3, SDI2 : filter set =', ret232)  # shall deliver true


    # try to set a filters that already exists
    print('\ntrying to set filters that alreay exist, shall fail (return False):')
    print('set again RX1, label 1, SDI_DATA: result =', a429.set_rx_filter(channel = a429.CHANNEL_RX1, label = 1, sdi = a429.SDI_DATA))
    print('set again RX2, label 3, SDI1    : result =', a429.set_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI1))

    # try to overwrite existing filters
    print('\ntrying to overwrite already existing filters, shall fail (return False):')
    print('overwrite label+DATA with SDI1    : result =', a429.set_rx_filter(channel = a429.CHANNEL_RX1, label = 1, sdi = a429.SDI_SDI1))
    print('overwrite label+SDI  with SDI_DATA: result =', a429.set_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_DATA))

    # try to clear filters that have alreayd been cleared
    print('\ntrying to clear filters that have already been cleared, shall fail (return False):')
    print('clear again RX1, label 3, SDI_DATA: result =', a429.clear_rx_filter(channel = a429.CHANNEL_RX1, label = 3, sdi = a429.SDI_DATA))
    print('clear again RX2, label 4, SDI1    : result =', a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 4, sdi = a429.SDI_SDI1))

    # try to clear filters with an inappropriate SDI selection
    print('\ntrying to clear filters with inappropriate SDI selection, shall fail (return False):')
    print('clearing label+DATA with SDI1    : result =', a429.clear_rx_filter(channel = a429.CHANNEL_RX1, label = 1, sdi = a429.SDI_SDI1))
    print('clearing label+SDI  with SDI_DATA: result =', a429.clear_rx_filter(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_DATA))


    # activate channels
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_ACTIVE)

    print('\nset channel mode to active')


    # read-back
    ret1 = a429.get_channel_mode(channel = a429.CHANNEL_RX1)
    ret2 = a429.get_channel_mode(channel = a429.CHANNEL_RX2)
    
    print('mode read-back:')
    print('RX1 channel mode: ', ret1)
    print('RX2 channel mode: ', ret2)


    #################################################################
    # Test  4 - TX - channel mode
    #################################################################

    print('\nTX Channel Mode')
    print('---------------')

    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE)

    ret = a429.get_channel_mode(channel = a429.CHANNEL_TX1)

    print('TX channel mode: ', ret)


    #################################################################
    # Test  5 - TX - direct transmit
    #################################################################

    print('\nTX direct Transmit')
    print('------------------')

    # define frame (label 1, no sdi, data = 0)
    frame = 1

    a429.write_frame_direct(channel = a429.CHANNEL_TX, frame = frame)

    print('transmitted frame:', frame)


    #################################################################
    # Test  6 - TX - scheduled transmit
    #################################################################

    print('\nTX scheduled Transmit')
    print('---------------------')

    # define frames
    #         label   SDI         Data                   dwell time
    frame1  =   1   +  0  * 256 +  1  * 1024 ; dwell1  =     5
    frame2  =   2   +  0  * 256 +  2  * 1024 ; dwell2  =    95
    frame31 =   3   +  1  * 256 +  3  * 1024 ; dwell31 =    50
    frame32 =   3   +  2  * 256 +  3  * 1024 ; dwell32 =   100

    # write frames
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = frame1 )
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 1, frame = frame2 )
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 2, frame = frame31)
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 3, frame = frame32)

    print('wrote frames')

    # write schedule
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 0,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 0,                         \
                            dwell_time  = dwell1)
   
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 1,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 1,                         \
                            dwell_time  = dwell2)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 2,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 2,                         \
                            dwell_time  = dwell31)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 3,                         \
                            job         = a429.SCHEDULER_JOB_CYCLIC, \
                            frame_index = 3,                         \
                            dwell_time  = dwell32)   

    # comment next line in to send only label 1
    # a429.clear_schedule_entries(channel = a429.CHANNEL_TX, job_index_first = 1, job_index_last = 3)

    print('wrote schedule')

    # read-back schedule
    ret1 = a429.get_schedule_entry(a429.CHANNEL_TX1, job_index  = 0)
    ret2 = a429.get_schedule_entry(a429.CHANNEL_TX1, job_index  = 1)
    ret3 = a429.get_schedule_entry(a429.CHANNEL_TX1, job_index  = 2)
    ret4 = a429.get_schedule_entry(a429.CHANNEL_TX1, job_index  = 3)    

    print('schedule read-back:')
    print('Job 1: job =', ret1.job, ' Frame =', ret1.frame, ' dwell-time =', ret1.dwell_time, ' ms')
    print('Job 2: job =', ret2.job, ' Frame =', ret2.frame, ' dwell-time =', ret2.dwell_time, ' ms')
    print('Job 3: job =', ret3.job, ' Frame =', ret3.frame, ' dwell-time =', ret3.dwell_time, ' ms')
    print('Job 4: job =', ret4.job, ' Frame =', ret4.frame, ' dwell-time =', ret4.dwell_time, ' ms')

    # read capabilities (resource usage)
    ret = a429.get_capabilities()

    print('\nResource Usage now:')
    print('TX Scheduler Jobs total/used:', ret.tx_total_scheduler_jobs, '/', ret.tx_used_scheduler_jobs)
    print('RX Frame Filters  total/used:', ret.rx_total_frame_filters,  '/', ret.rx_used_frame_filters )


    input("\npress <enter> key to start the TX scheduler...\n") # use raw_input() in Python 2
    # start scheduler
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_RUN)

    # check scheduler is running
    ret = a429.get_channel_mode(channel = a429.CHANNEL_TX1)

    if ret == a429.CHANNEL_MODE_RUN : print('TX Channel Mode: run')
    else                            : print('ERROR: channel not in mode RUN')


    #################################################################
    # Test  7 - RX - polled read
    #################################################################
    
    print('\nRX polled read')
    print('--------------')

    time.sleep(1)

    ret11  = a429.read_frame(channel = a429.CHANNEL_RX1, label = 1, sdi = a429.SDI_DATA)
    ret12  = a429.read_frame(channel = a429.CHANNEL_RX1, label = 2, sdi = a429.SDI_DATA)
    ret13  = a429.read_frame(channel = a429.CHANNEL_RX1, label = 3, sdi = a429.SDI_DATA)

    ret21  = a429.read_frame(channel = a429.CHANNEL_RX2, label = 1, sdi = a429.SDI_DATA)
    ret22  = a429.read_frame(channel = a429.CHANNEL_RX2, label = 2, sdi = a429.SDI_DATA)
    ret231 = a429.read_frame(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI1)
    ret232 = a429.read_frame(channel = a429.CHANNEL_RX2, label = 3, sdi = a429.SDI_SDI2)

    print('RX1, label 1, DATA : status =', ret11.status,  ' frame =', ret11.frame,  ' age =', ret11.age,  'ms')
    print('RX1, label 2, DATA : status =', ret12.status,  ' frame =', ret12.frame,  ' age =', ret12.age,  'ms')
    print('RX1, label 3, DATA : status =', ret13.status,  ' frame =', ret13.frame,  ' age =', ret13.age,  'ms')

    print('RX2, label 1, DATA : status =', ret21.status,  ' frame =', ret21.frame,  ' age =', ret21.age,  'ms')
    print('RX2, label 2, DATA : status =', ret22.status,  ' frame =', ret22.frame,  ' age =', ret22.age,  'ms')
    print('RX2, label 3, SDI1 : status =', ret231.status, ' frame =', ret231.frame, ' age =', ret231.age, 'ms')
    print('RX2, label 3, SDI2 : status =', ret231.status, ' frame =', ret232.frame, ' age =', ret231.age, 'ms')

    input("\npress <enter> key to continue...\n") # use raw_input() in Python 2


    #################################################################
    # Test  8 - RX - callback read
    #################################################################

    print('\nRX callback')
    print('-----------')

    # register callback function
    a429.register_callback(a429.CALLBACK_FRAME_MESSAGE, frame_cb)

    # configure callback
    a429.set_rx_callback_configuration(channel = a429.CHANNEL_RX, enabled = True, value_has_to_change = False, timeout = 1000)

    print('RX  callback configured')

    # configuration read-back
    ret1 = a429.get_rx_callback_configuration(channel = a429.CHANNEL_RX1)
    ret2 = a429.get_rx_callback_configuration(channel = a429.CHANNEL_RX2)

    print('RX1 read-back: enabled =', ret1.enabled, ' value-has-to-change =', ret1.value_has_to_change, ' timeout =', ret1.timeout, 'ms')
    print('RX2 read-back: enabled =', ret2.enabled, ' value-has-to-change =', ret2.value_has_to_change, ' timeout =', ret2.timeout, 'ms')


    #################################################################
    # Test  9 - general - heartbeat
    #################################################################

    print('\nHeartbeat')
    print('---------')

    # register callback function
    a429.register_callback(a429.CALLBACK_HEARTBEAT, heartbeat_cb)

    # configure callback
    a429.set_heartbeat_callback_configuration(period = 1, value_has_to_change = False)

    print('heartbeat configured')

    # configuration read-back
    ret = a429.get_heartbeat_callback_configuration()

    print('heartbeat read-back: period =', ret.period, 'value-has-to-change =', ret.value_has_to_change)

    input("\npress <enter> key to continue...\n\n") # use raw_input() in Python 2


    #################################################################
    # Test 10 - RX - callback on change only
    #################################################################

    print('\nRX callback on-change-only')
    print('--------------------------')

    # reconfigure callback on RX1 to on-change-only
    a429.set_rx_callback_configuration(channel = a429.CHANNEL_RX1, enabled = True, value_has_to_change = True, timeout = 1000)

    print('Callback for RX1 changed to on-change-only')


    #################################################################
    # Test 11- RX - timeout
    #################################################################

    input("\npress <enter> key to stop TX activity...\n\n") # use raw_input() in Python 2

    print('\nRX Timeout')
    print('----------')

    # stop scheduler
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE)

    print('scheduler is stopped')


    #################################################################
    # Test 12 - general - clean-ups
    #################################################################

    input("\npress <enter> key to terminate...\n\n") # use raw_input() in Python 2

    print('\nClean-up')
    print('--------')

    # clear TX schedule
    a429.clear_schedule_entries(channel = a429.CHANNEL_TX, job_index_first = 0, job_index_last = 511)
    print('cleared complete TX schedule')

    # read capabilities (resource usage)
    ret = a429.get_capabilities()

    print('\nResource Usage now:')
    print('TX Scheduler Jobs total/used:', ret.tx_total_scheduler_jobs, '/', ret.tx_used_scheduler_jobs)
    print('RX Frame Filters  total/used:', ret.rx_total_frame_filters,  '/', ret.rx_used_frame_filters )


    #################################################################
    # Test 13 - standard filters
    #################################################################

    print('\nStandard-Filters')
    print('----------------')

    a429.set_rx_standard_filters(channel = a429.CHANNEL_RX)

    # read capabilities (resource usage)
    ret = a429.get_capabilities()

    print('standard RX filters set:')
    print('RX Frame Filters total/used:', ret.rx_total_frame_filters,'/', ret.rx_used_frame_filters  )

    # callbacks on RX1 only
    a429.set_rx_callback_configuration(channel = a429.CHANNEL_RX1, enabled = True,  value_has_to_change = False, timeout = 10000)
    a429.set_rx_callback_configuration(channel = a429.CHANNEL_RX2, enabled = False, value_has_to_change = True,  timeout = 10000)
    print('\nCallsbacks on RX1 only, 10 sec timeout')

    # reset statistics counters
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE )
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_ACTIVE )
    print('cleared statistics counters')

    # send frames with labels 0..255
    print('\nsending frames with labels 0..255:') 

    for label in range(0, 256) :
        a429.write_frame_direct(channel = a429.CHANNEL_TX, frame = label)
        time.sleep(0.003)  # a frame takes 2.88 ms to transmit 

    time.sleep(2)
    print('...done')


    #################################################################
    # Test 14 - schedule with single transmit & callback
    #################################################################

    print('\nscheduled single-transmits')
    print('--------------------------')

    # register callback function
    a429.register_callback(a429.CALLBACK_SCHEDULER_MESSAGE, scheduler_cb)

    # create the schedule
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,            \
                            job_index   = 0,                          \
                            job         = a429.SCHEDULER_JOB_SINGLE,  \
                            frame_index = 0,                          \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,            \
                            job_index   = 1,                          \
                            job         = a429.SCHEDULER_JOB_CALLBACK,\
                            frame_index = 42,                         \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,            \
                            job_index   = 2,                          \
                            job         = a429.SCHEDULER_JOB_DWELL,   \
                            frame_index = 0,                          \
                            dwell_time  = 100)

    # clear the TX buffer (frames with value 0 will not be transmitted in JOB_SINGLE mode)
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = 0)

    # start scheduler & clear statistics counters
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_RUN    )
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_ACTIVE )
    print('scheduler is set up and started, statistics counters are cleared')

    print('waiting 5 seconds...')
    time.sleep(5)

    # write 1st frame
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = 1)
    print('\nFrame with label 1 is programmed')

    print('waiting 5 seconds...')
    time.sleep(5)

    # write 2nd frame
    a429.write_frame_scheduled(channel = a429.CHANNEL_TX, frame_index = 0, frame = 2)
    print('\nFrame with label 2 is programmed')

    print('waiting 5 seconds...')
    time.sleep(5)

    # stop scheduler
    # a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE)

    # make scheduler stop itself
    print('putting a stop command into the schedule')
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,          \
                            job_index   = 0,                        \
                            job         = a429.SCHEDULER_JOB_STOP,  \
                            frame_index = 0,                        \
                            dwell_time  = 0)

    time.sleep(5)

    print('scheduler stopped')


    #################################################################
    # Test 15 - schedule with retransmits from RX1
    #################################################################

    print('\nscheduled retransmit')
    print('--------------------')

    # change schedule to retransmit
    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,                \
                            job_index   = 0,                              \
                            job         = a429.SCHEDULER_JOB_RETRANS_RX1, \
                            frame_index = 10,                             \
                            dwell_time  = 0)

    a429.set_schedule_entry(channel     = a429.CHANNEL_TX,           \
                            job_index   = 1,                         \
                            job         = a429.SCHEDULER_JOB_DWELL,  \
                            frame_index = 0,                         \
                            dwell_time  = 200)

    print('scheduler is set up for a retransmit of label 10 received on RX1')

    # start scheduler & clear statistics counters
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_ACTIVE )
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_RUN    )
    print('scheduler started, statistics counters are cleared')

    print('waiting 5 seconds...')
    time.sleep(5)

    # send some other label
    a429.write_frame_direct(channel = a429.CHANNEL_TX, frame = 20)
    print('\nsent a frame with label 20, waiting 5 seconds...')
    time.sleep(5)

    # send triggering label
    a429.write_frame_direct(channel = a429.CHANNEL_TX, frame = 10)
    print('\nsent a frame with label 10, shall trigger the retransmit:')
    time.sleep(5)


    #################################################################
    # finish
    #################################################################

    print('\nclean-up')
    print('--------')

    # clear all RX filters
    a429.clear_all_rx_filters(channel = a429.CHANNEL_RX)
    print('cleared all RX filters')

    # clear TX schedule
    a429.clear_schedule_entries(channel = a429.CHANNEL_TX, job_index_first = 0, job_index_last = 511)
    print('cleared complete TX schedule')

    # set mode to passive
    a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_PASSIVE)
    a429.set_channel_mode(channel = a429.CHANNEL_RX, mode = a429.CHANNEL_MODE_PASSIVE)
    print('switched all channels to passive mode')

    # disconnect
    ipcon.disconnect()

    # all done
    print('\nall tests done.\n')

