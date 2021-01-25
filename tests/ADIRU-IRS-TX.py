#!/usr/bin/python3
# -*- coding: utf-8 -*- 

#######################################################################################
#                                                                                     #
#                          ADIRU - IRS Simulation - TX                                #
#                          ===========================                                #
#                                                                                     #
#   This code sets up an ADIRU - IRS simulation with a selected sub-set of labels.    #
#                                                                                     #
#                                                                                     #
#######################################################################################


# addresses
HOST = "localhost"   # host to which the A429 bricklet is connected
PORT = 4223          # port to which the brick daemon  is bound on the HOST
UID  = "a2"          # unique identifier of the A429 bricklet

# import the TinkerForge communication library and the A429 bricklet library
from tinkerforge.ip_connection     import IPConnection
from tinkerforge.bricklet_arinc429 import BrickletARINC429

# import standard libraries
import sys
import math
import time

# import A429 tools
import a429tools

# import the ADIRU definitions
import adiru_A429


#######################################################################################
# Callback Functions                                                                  #
#######################################################################################

# A429 bricklet heartbeat with information on statistics data
def heartbeat_cb(seq_number, timestamp, frames_processed, frames_lost) :

	print('Heartbeat #',        seq_number,                                   \
	      ' processed TX/RX =', frames_processed[0],'/', frames_processed[1], \
#	      ' en route =',        frames_processed[0]  -   frames_processed[1], \
	      ' lost TX/RX =',      frames_lost[0],     '/', frames_lost[1],      \
	      ' timestamp =',       timestamp                                     \
	      )


# A429 bricklet scheduler message with information on the transmit progress
def scheduler_cb(channel, seq_number, timestamp, token) :

	print('Scheduler #',  seq_number, \
	      ' channel =',   channel,    \
	      ' token =',     token,      \
	      ' timestamp =', timestamp   \
	      )


#######################################################################################
# Main Function                                                                       #
#######################################################################################

if __name__ == "__main__" :

	# ----- set-up --------------------------------------------------------------------

	# create the required objects
	ipcon = IPConnection()               # create ipcon communication  object
	a429  = BrickletARINC429(UID, ipcon) # create A429 bricklet device object

	# establish communication with the A429 bricklet
	ipcon.connect(HOST, PORT)            # connect to brick daemon (don't use any device object before ipcon is connected)
	ipcon.set_auto_reconnect(True)       # if connection is lost, automatically try to re-connect

	# register the callback functions
	a429.register_callback(a429.CALLBACK_HEARTBEAT,         heartbeat_cb)
	a429.register_callback(a429.CALLBACK_SCHEDULER_MESSAGE, scheduler_cb)


	# ----- base initialization -------------------------------------------------------

	print('ADIRU A429 Transmitter'  )
	print('----------------------\n')

	# reset the whole ARINC429 bricklet (revert it to pristine power-up state)
	a429.restart()

	# configure TX channel to automatic parity and high speed
	a429.set_channel_configuration(channel = a429.CHANNEL_TX1, \
	                               parity  = a429.PARITY_AUTO, \
	                               speed   = a429.SPEED_HS     \
	                              )

	# read-back the channel configuration
	ret = a429.get_channel_configuration(a429.CHANNEL_TX1)

	print('TX1 Parity:', a429tools.parity2text(ret.parity), \
	      ' Speed:',     a429tools.speed2text(ret.speed)    \
	     )


	# ----- set-up the TX scheduler ---------------------------------------------------

	# load the TX frame table
	#
	# The TX frame table holds the frames (= composite of label number, SDI and data) the
	# TX scheduler will transmit. The TX scheduler will access the frames in the frame table
	# by reference to the frame_index under which the frames are stored.
	#
	# As the label numbers are unique here (no label number is used in combination with more
	# than one SDI value), we can keep things simple and store the frames at index numbers
	# that match their label numbers.


	# we will set all frame values to status 'normal operation'
	normal_op = a429tools.SSM_NO

	# just for counting the number of uploaded entries
	num_labels = 0

	# step through all frame definitions
	for label in adiru_A429.irs_frames :

		# increment the counter
		num_labels += 1

		# get the frame definition
		definition = adiru_A429.irs_frames[label]

		# create the frame           (label, SDI value    )
		frame = a429tools.createFrame(label, definition[2])

		# check the result
		if (frame < 0) :
			print('Label', oct(label),': illegal label number or SDI')
			continue

		# set the default value   (frame, frame format,  lsb position,  size,          eng.min,       eng.max,       eng.value,     SSM value)
		frame = a429tools.setValue(frame, definition[3], definition[6], definition[7], definition[8], definition[9], definition[4], normal_op)

		# check the result
		if (frame < 0) :
			print('Label', oct(label),': error in value definition')
			continue

		# load the frame
		a429.write_frame_scheduled(channel     = a429.CHANNEL_TX1, \
		                           frame_index = label,            \
		                           frame       = frame             \
		                          )


	print('TX frame table written,', num_labels, 'entries made')


	# load the TX scheduler table
	#
	# The TX scheduler table holds the sequence of frame transmissions for one complete
	# transmission cycle. If a frames (label number) shall be sent more than once during
	# one cycle, it needs to be placed as often into the schedule as it is supposed to be
	# sent. The scheduler can be set to dwell for a given time of milliseconds after each
	# frame, or can transmit two or more frames back-to-back with the dwell time set to
	# zero. Beware that the transmission FIFO buffer can hold 32 frames at maximum, so after
	# latest 32 frames sent back-to-back a dwell time needs to be set that is sufficiently
	# long for the FIFO to drain.
	#
	# The transmission of one frame takes 0.36 ms in high speed mode,
	#                                     2.88 ms in low  speed mode.
	#
	# There are various possibilities how to distribute the dwell times to avoid FIFO overflow
	# as well as to match with the overall cycle time and the individual cycle times of the
	# labels. Here, we place all labels having the same refresh rate back-to-back and then give
	# this whole block a single dwell time sufficient to drain the FIFO and to delay the next
	# transmission such that it hits its intended transmission time within the cycle. This works
	# here because none of the refresh rate groups comprise more than 32 labels.
	# Furthermore, we use combinations of JUMP and RETURN to only need to store the labels of
	# each transmission block once.


	def load_label_group(frame_definitions, refresh_rate, job_index_start) :

		# set 1st index position to write to
		job_index = job_index_start

		# step through all keys in order of ascending keys (=labels)
		for label in sorted(frame_definitions) :

			# get the definition data
			definition = frame_definitions[label]

			# skip frames that do not match the requested refresh rate
			if(definition[1] != refresh_rate) : continue

			# load the label
			a429.set_schedule_entry(channel     = a429.CHANNEL_TX1,          \
			                        job_index   = job_index ,                \
			                        job         = a429.SCHEDULER_JOB_CYCLIC, \
			                        frame_index = label,                     \
			                        dwell_time  = 0                          \
			                       )

			# increment the job index
			job_index  += 1

		# load a return command
		a429.set_schedule_entry(channel     = a429.CHANNEL_TX1,          \
		                        job_index   = job_index,                 \
		                        job         = a429.SCHEDULER_JOB_RETURN, \
		                        frame_index = 0,                         \
		                        dwell_time  = 0                          \
		                       )

		# done, return number of labels loaded
		return job_index - job_index_start


	def load_label_group_call(group_index_start, group_time) :

		global job_index

		# load the call
		a429.set_schedule_entry(channel     = a429.CHANNEL_TX1,          \
		                        job_index   = job_index,                 \
		                        job         = a429.SCHEDULER_JOB_JUMP,   \
		                        frame_index = group_index_start,         \
		                        dwell_time  = group_time                 \
		                       )

		# increment the job index
		job_index += 1

		# done
		return


	# load the   20 ms group
	labels_0020_index = 800
	c = load_label_group(adiru_A429.irs_frames,   20, labels_0020_index)
	print('loaded   20 ms group,', c, 'labels')

	# load the   40 ms group
	labels_0040_index = 825
	c = load_label_group(adiru_A429.irs_frames,   40, labels_0040_index)
	print('loaded   40 ms group,', c, 'labels')

	# load the  100 ms group
	labels_0100_index = 850
	c = load_label_group(adiru_A429.irs_frames,  100, labels_0100_index)
	print('loaded  100 ms group,', c, 'labels')

	# load the  200 ms group
	labels_0200_index = 875
	c = load_label_group(adiru_A429.irs_frames,  200, labels_0200_index)
	print('loaded  200 ms group,', c, 'labels')

	# load the  250 ms group
	labels_0250_index = 900
	c = load_label_group(adiru_A429.irs_frames,  250, labels_0250_index)
	print('loaded  250 ms group,', c, 'labels')

	# load the  500 ms group
	labels_0500_index = 925
	c = load_label_group(adiru_A429.irs_frames,  500, labels_0500_index)
	print('loaded  500 ms group,', c, 'labels')

	# load the 1000 ms group
	labels_1000_index = 950
	c = load_label_group(adiru_A429.irs_frames, 1000, labels_1000_index)
	print('loaded 1000 ms group,', c, 'labels')
	

	# the scheduler starts at job index position zero
	job_index = 0

	# load the calls of the label groups
	#
	# t =   0 ms -> all groups
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  4)	#   40 ms group, needs to be >= 4 ms
	load_label_group_call(labels_0100_index,  3)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0200_index,  1)	#  200 ms group, needs to be >= 1 ms
	load_label_group_call(labels_0250_index,  2)	#  250 ms group, needs to be >= 2 ms
	load_label_group_call(labels_0500_index,  1)	#  500 ms group, needs to be >= 1 ms
	load_label_group_call(labels_1000_index,  4)	# 1000 ms group, needs to be >= 4 ms

	# t =  20 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t =  40 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t =  60 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t =  80 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 100 ms -> groups: 20 ms, 100 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0100_index, 10)	#  100 ms group, needs to be >= 3 ms

	# t = 120 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 140 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 160 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 180 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 200 ms -> groups: 20 ms, 40 ms, 100 ms, 200 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  5)	#   40 ms group, needs to be >= 4 ms
	load_label_group_call(labels_0100_index,  5)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0200_index,  5)	#  200 ms group, needs to be >= 1 ms

	# t = 220 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 240 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  5)	#   40 ms group, needs to be >= 4 ms

	# t = 250 ms -> groups: 250 ms
	load_label_group_call(labels_0250_index, 10)	#  250 ms group, needs to be >= 2 ms

	# t = 260 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 280 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 300 ms -> groups: 20 ms, 100 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0100_index, 10)	#  100 ms group, needs to be >= 3 ms

	# t = 320 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 340 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 360 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 380 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 400 ms -> groups: 20 ms, 40 ms, 100 ms, 200 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  5)	#   40 ms group, needs to be >= 4 ms
	load_label_group_call(labels_0100_index,  5)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0200_index,  5)	#  200 ms group, needs to be >= 1 ms

	# t = 420 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 440 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 460 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 480 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 500 ms -> groups: 20 ms, 100 ms, 250 ms, 500 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0100_index,  5)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0250_index,  5)	#  250 ms group, needs to be >= 2 ms
	load_label_group_call(labels_0500_index,  5)	#  500 ms group, needs to be >= 1 ms

	# t = 520 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 540 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 560 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 580 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 600 ms -> groups: 20 ms, 40 ms, 100 ms, 200 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  5)	#   40 ms group, needs to be >= 4 ms
	load_label_group_call(labels_0100_index,  5)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0200_index,  5)	#  200 ms group, needs to be >= 1 ms

	# t = 620 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 640 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 660 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 680 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 700 ms -> groups: 20 ms, 100 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0100_index, 10)	#  100 ms group, needs to be >= 3 ms

	# t = 720 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 740 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms

	# t = 750 ms -> groups: 250 ms
	load_label_group_call(labels_0250_index, 10)	#  250 ms group, needs to be >= 2 ms

	# t = 760 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 780 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 800 ms -> groups: 20 ms, 40 ms, 100 ms, 200 ms
	load_label_group_call(labels_0020_index,  5)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index,  5)	#   40 ms group, needs to be >= 4 ms
	load_label_group_call(labels_0100_index,  5)	#  100 ms group, needs to be >= 3 ms
	load_label_group_call(labels_0200_index,  5)	#  200 ms group, needs to be >= 1 ms

	# t = 820 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 840 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 860 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 880 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 900 ms -> groups: 20 ms, 100 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0100_index, 10)	#  100 ms group, needs to be >= 3 ms

	# t = 920 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 940 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms

	# t = 960 ms -> groups: 20 ms, 40 ms
	load_label_group_call(labels_0020_index, 10)	#   20 ms group, needs to be >= 5 ms
	load_label_group_call(labels_0040_index, 10)	#   40 ms group, needs to be >= 4 ms

	# t = 980 ms -> groups: 20 ms
	load_label_group_call(labels_0020_index, 20)	#   20 ms group, needs to be >= 5 ms


	# place a scheduler callback event to notify on each completed transmit cycle
	a429.set_schedule_entry(channel     = a429.CHANNEL_TX1,            \
	                        job_index   = job_index,                   \
	                        job         = a429.SCHEDULER_JOB_CALLBACK, \
	                        frame_index = 0,                           \
	                        dwell_time  = 0                            \
	                       )

	# increment the job index
	job_index  += 1

	# end the cycle by jumping back to index 0 (else the scheduler would run into the first label group)
	a429.set_schedule_entry(channel     = a429.CHANNEL_TX1,          \
	                        job_index   = job_index,                 \
	                        job         = a429.SCHEDULER_JOB_JUMP,   \
	                        frame_index = 0,                         \
	                        dwell_time  = 0                          \
	                       )


	# ----- get resource usage --------------------------------------------------------

	# read capabilities (resource usage)
	ret = a429.get_capabilities()
	print('TX scheduler entries used/total:',  ret.tx_used_scheduler_jobs, '/', ret.tx_total_scheduler_jobs)


	# ----- set-up the callbacks ------------------------------------------------------

	# configure the heartbeat (statistics) callback:
	# trigger every second, but only if the statistics data have changed
	a429.set_heartbeat_callback_configuration(period = 1, value_has_to_change = True)

	# read-back the heartbeat (statistics) callback configuration
	ret = a429.get_heartbeat_callback_configuration()

	if (ret.period > 0) :
		print('Statistics callback activated')
	else :
		print('Statistics callback activation failed')


	print("\n*** press <enter>  to start the scheduler ***"  )
	input(  "*** press <CTRL-C> to stop  the scheduler ***\n") # use raw_input() in Python 2


	# ----- start TX ------------------------------------------------------------------

	# start the TX scheduler
	a429.set_channel_mode(channel = a429.CHANNEL_TX1, mode = a429.CHANNEL_MODE_RUN)
	print('TX scheduler started')

	# read-back the TX scheduler mode
	ret = a429.get_channel_mode(channel = a429.CHANNEL_TX1)

	if(ret == a429.CHANNEL_MODE_RUN) :
		print('TX scheduler is running')
	else :
		print('TX scheduler start failed')


	# ----- running... ----------------------------------------------------------------

	# define a few shortcuts
	sdi3             = a429tools.SDI_3
	bnr              = a429tools.FRAME_FORMAT_BNR
	bcd              = a429tools.FRAME_FORMAT_BCD
	normal_operation = a429tools.SSM_NO

	# create the frames for the UTC labels
	frame_260_UTC_date = a429tools.createFrame(0o260, sdi3)
	frame_150_UTC_time = a429tools.createFrame(0o150, sdi3)
	frame_125_UTC_time = a429tools.createFrame(0o125, sdi3)

	try:
		while(True) :

			# get the current time & date
			tm = time.gmtime(time.time())

			# debug output
			print('Updating Time/Date:',tm.tm_hour,':',tm.tm_min,':',tm.tm_sec,' ',tm.tm_mday,'-', tm.tm_mon,'-',tm.tm_year%100)

			# assemble the data values
			data_260_UTC_date  = tm.tm_mday << 13 | tm.tm_mon << 8 | (tm.tm_year % 100) << 0
			data_150_UTC_time  = tm.tm_hour << 13 | tm.tm_min << 7 | tm.tm_sec  << 1 | 1
			data_125_UTC_time  = tm.tm_hour << 12 | tm.tm_min * 10 + math.floor(tm.tm_sec/6)

			# update the frames with the new values(frame, format, lsb_pos, size, min, max, data, SSM)
			frame_260_UTC_date = a429tools.setValue(frame_260_UTC_date, bcd, 11, 19, 0, 2**19, data_260_UTC_date, normal_operation)
			frame_150_UTC_time = a429tools.setValue(frame_150_UTC_time, bnr, 11, 18, 0, 2**18, data_150_UTC_time, normal_operation)
			frame_125_UTC_time = a429tools.setValue(frame_125_UTC_time, bcd, 11, 19, 0, 2**19, data_125_UTC_time, normal_operation)

			# update the TX frame table on the A429 bricklet
			a429.write_frame_scheduled(channel = a429.CHANNEL_TX1, frame_index = 0o260, frame = frame_260_UTC_date)
			a429.write_frame_scheduled(channel = a429.CHANNEL_TX1, frame_index = 0o150, frame = frame_150_UTC_time)
			a429.write_frame_scheduled(channel = a429.CHANNEL_TX1, frame_index = 0o125, frame = frame_125_UTC_time)

			# wait a second
			time.sleep(1)

	except KeyboardInterrupt :

		print('\nshutting down...')


	# ----- clean up ------------------------------------------------------------------

	# stop the TX scheduler
	a429.set_channel_mode(channel = a429.CHANNEL_TX, mode = a429.CHANNEL_MODE_ACTIVE)
	print('scheduler stopped')

	# stop the statistics callback
	a429.set_heartbeat_callback_configuration(period = 0, value_has_to_change = False)
	print('callback stopped')

	# set TX channel to passive mode
	a429.set_channel_mode(channel = a429.CHANNEL_TX1, mode = a429.CHANNEL_MODE_PASSIVE)
	print('TX1 channel switched to passive mode')

	# disconnect from brick daemon
	ipcon.disconnect()

	# all done
	print('done.\n')

# =====================================================================================

