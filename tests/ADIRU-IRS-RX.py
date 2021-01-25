#!/usr/bin/python3
# -*- coding: utf-8 -*- 

#######################################################################################
#                                                                                     #
#                          ADIRU - IRS Simulation - RX                                #
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
# Callback Function                                                                   #
#######################################################################################

# A429 bricklet frame message with information on changes in the received labels
def frame_cb(channel, seq_number, timestamp, status, frame, age) :

	# we are only interested in frames received on channel RX1
	if(channel != a429tools.CHANNEL_RX1) : return

	# extract label and SDI from received frame
	label = a429tools.readLabel(frame)
	sdi   = a429tools.readSDI(frame)

	# get the frame definition data
	definition = adiru_A429.irs_frames[label]

	# decode the frame
	value,ssm = a429tools.readValue(frame, definition[3], definition[6], definition[7], definition[8], definition[9],definition[10])

	# check if the SDI field is used for data
	if(definition[2] == a429tools.SDI_DATA) : sdi = '(data)'

	# determine status
	if  (status == a429tools.FRAME_STATUS_NEW    ) :
		status_msg = '*new* '
	elif(status == a429tools.FRAME_STATUS_UPDATE ) :
		status_msg = 'update'
	elif(status == a429tools.FRAME_STATUS_TIMEOUT) :
		status_msg = '*lost*'
	else :
		status_msg = '??????'


	print( 'cb#',      seq_number, status_msg,         \
	      ' label =',  oct(label), ':', definition[0], \
	      ' SDI =',    sdi,                            \
	      ' value =',  value, definition[5],           \
	      ' SSM =',    ssm,                            \
	      ' age =',    age, 'ms @', timestamp          \
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

	# register the callback function
	a429.register_callback(a429.CALLBACK_FRAME_MESSAGE,     frame_cb    )


	# ----- base initialization -------------------------------------------------------

	print('ADIRU A429 Receiver'  )
	print('-------------------\n')

	# configure RX1 channel to automatic parity and high speed
	a429.set_channel_configuration(channel = a429.CHANNEL_RX1, parity = a429.PARITY_AUTO, speed = a429.SPEED_HS)

	# read-back the channel configuration
	ret = a429.get_channel_configuration(a429.CHANNEL_RX1)
	print('RX1 Parity:', a429tools.parity2text(ret.parity))
	print('RX1 Speed :', a429tools.speed2text(ret.speed))


	# ----- set-up the RX filters -----------------------------------------------------

	# load the RX filter table
	#
	# Loading a combination of label number and SDI value into the RX filter table will
	# allow an arriving frame to pass the RX filter and therefore get processed.
	#
	# We use the RX1 receive channel for surveillance of the TX1 transmit channel
	for label in adiru_A429.irs_frames :

		# get the definition data
		definition = adiru_A429.irs_frames[label]

		# set the filter
		a429.set_rx_filter(channel = a429.CHANNEL_RX1, \
		                   label   = label,            \
		                   sdi     = definition[2]     \
		                  )

	print('RX1 filters loaded')


	# ----- get resource usage --------------------------------------------------------

	# read capabilities (resource usage)
	ret = a429.get_capabilities()
	print('RX1 frame filters used/total: ', ret.rx_used_frame_filters[0], '/', ret.rx_total_frame_filters )


	# ----- set-up the callbacks ------------------------------------------------------

	# configure the RX frame reception callback:
	# trigger when a label's data have changed and when a label is overdue for 1005 ms (5 ms later than normal schedule period)
	a429.set_rx_callback_configuration(channel             = a429.CHANNEL_RX1, \
	                                   enabled             = True,             \
	                                   value_has_to_change = True,             \
	                                   timeout             = 1005              \
	                                   )

	# read-back the RX frame reception callback configuration
	ret = a429.get_rx_callback_configuration(channel = a429.CHANNEL_RX1)

	if (ret.enabled == True) :
		print('RX callback activated')
	else :
		print('RX callback activation failed')


	print("\n*** press <enter>  to start the receiver ***"  )
	input(  "*** press <CTRL-C> to stop  the receiver ***\n") # use raw_input() in Python 2


	# ----- start RX ------------------------------------------------------------------

	# start the RX filter
	a429.set_channel_mode(channel = a429.CHANNEL_RX1, mode = a429.CHANNEL_MODE_ACTIVE)

	# read-back the RX filter mode
	ret = a429.get_channel_mode(channel = a429.CHANNEL_RX1)

	if(ret == a429.CHANNEL_MODE_ACTIVE) :
		print('RX1 filters active')
	else :
		print('RX1 filters activation failed')


	# ----- running... ----------------------------------------------------------------

	try:
		while(True) :

			# nothing actively to do here, just let the callbacks pop up...
			time.sleep(0.1)

	except KeyboardInterrupt :

		print('\nshutting down...')


	# ----- clean up ------------------------------------------------------------------

	# stop the callback
	a429.set_rx_callback_configuration(channel = a429.CHANNEL_RX, enabled = False, value_has_to_change = True, timeout = 1000)
	print('RX callback deactivated')

	# set RX1 channel to passive mode
	a429.set_channel_mode(channel = a429.CHANNEL_RX1, mode = a429.CHANNEL_MODE_PASSIVE)
	print('RX1 switched to passive mode')

	# disconnect from brick daemon
	ipcon.disconnect()

	# all done
	print('done.\n')

# =====================================================================================

