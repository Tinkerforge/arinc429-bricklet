# -*- coding: utf-8 -*-

##############################################################################
#                                                                            #
#  A429 Tools for creating Frames and writing / reading Data to/from Frames  #
#                                                                            #
##############################################################################


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# constants
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FRAME_FORMAT_RAW      = 0
FRAME_FORMAT_BCD      = 1
FRAME_FORMAT_BNR      = 2
FRAME_FORMAT_DISCRETE = 3

SSM_DATA  = 0
SSM_NO    = 1
SSM_NCD   = 2
SSM_FW    = 3
SSM_FT    = 4
SSM_PLUS  = 5  # internal only
SSM_MINUS = 6  # internal only


# keep the following assignments in line with the A429 bricklet library

SDI_0    = 0
SDI_1    = 1
SDI_2    = 2
SDI_3    = 3
SDI_DATA = 4

PARITY_DATA = 0
PARITY_AUTO = 1

SPEED_HS = 0
SPEED_LS = 1

FRAME_STATUS_NEW     = 0
FRAME_STATUS_UPDATE  = 1
FRAME_STATUS_TIMEOUT = 2

CHANNEL_TX   = 0
CHANNEL_TX1  = 1
CHANNEL_TX2  = 2
CHANNEL_TX3  = 3
CHANNEL_TX4  = 4
CHANNEL_TX5  = 5
CHANNEL_TX6  = 6
CHANNEL_TX7  = 7
CHANNEL_TX8  = 8
CHANNEL_TX9  = 9
CHANNEL_TX10 = 10
CHANNEL_TX11 = 11
CHANNEL_TX12 = 12

CHANNEL_RX   = 32
CHANNEL_RX1  = 33
CHANNEL_RX2  = 34
CHANNEL_RX3  = 35
CHANNEL_RX4  = 36
CHANNEL_RX5  = 37
CHANNEL_RX6  = 38
CHANNEL_RX7  = 39
CHANNEL_RX8  = 40
CHANNEL_RX9  = 41
CHANNEL_RX10 = 42
CHANNEL_RX11 = 43
CHANNEL_RX12 = 44


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# low-level functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def binary2bcd(binary) :

    bcd    = 0
    shift  = 0

    while (binary) :
        bcd    +=  int(binary % 10) << (4*shift)
        shift  +=  1
        binary /= 10

    return bcd


def bcd2binary(bcd) :

    binary = 0
    factor = 1

    while (bcd) :
        binary += (bcd % 16) * factor
        factor *= 10
        bcd     = bcd >> 4

    return binary


def scalefactor(a429_frame_format, size, eng_min, eng_max) :

    # check the size argument
    if (size < 1) : return 1

    # make sure size is an integer
    size = int(size)

    if (a429_frame_format == FRAME_FORMAT_BNR) :

        # the scaling assumes that the biggest engineering value is encoded
        # by the number 2^size, although in practice the biggest value that
        # can be encoded is 2^size - 1
        return max(abs(eng_min), abs(eng_max)) / (2**size)

    # don't scale the data in any of the other frame formats
    return 1


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# helper functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# convert parity encoding to text
def parity2text(parity) :

	if(parity == PARITY_DATA) : return 'data'
	if(parity == PARITY_AUTO) : return 'auto'

	return 'error - undefined parity encoding'


# convert speed encoding to text
def speed2text(speed) :

	if(speed == SPEED_HS) : return 'high speed'
	if(speed == SPEED_LS) : return 'low speed'

	return 'error - undefined spped encoding'

# convert SSM encoding to text
def ssm2text(ssm) :

    if   (ssm == SSM_DATA) : return 'SSM used for data'
    elif (ssm == SSM_NO  ) : return  'NO (normal operation)'
    elif (ssm == SSM_NCD ) : return 'NCD (no computed data)'
    elif (ssm == SSM_FW  ) : return  'FW (failure warning)'
    elif (ssm == SSM_FT  ) : return  'FT (functional test)'
    else                   : return  'error - undefined SSM encoding'


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# high level functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def createFrame(label, sdi) :

    # check the arguments
    if ((label > 255) or (label < 0) or (sdi > 4) or (sdi < 0)) :
        return -1

    # make sure label and SDI are integers
    label = int(label)
    sdi   = int(sdi)

    # assemble the raw frame
    if (sdi == SDI_DATA) :
        return (label)
    else :
        return (label) | (sdi << 8)


def readLabel(frame) :

    # extract the lowest 8 bits
    return frame & 0x000000FF


def readExtendedLabel(frame) :

    # extract the lowest 10 bits
    return frame & 0x000003FF


def readSDI(frame) :

    # extract bits 8..9
    return (frame & 0x00000300) >> 8


def readRawSSM(frame) :

    # extract bits 29..30
    return (frame & 0x60000000) >> 29


def setValue(frame, a429_frame_format, lsb_position, size, \
             engineering_min, engineering_max, engineering_value, ssm_value) :

    # check the arguments
    if (     (size                <  1)
         or  (lsb_position        <  9)
         or ((lsb_position + size > 30) and (ssm_value != SSM_DATA))
         or ((lsb_position + size > 32) and (ssm_value == SSM_DATA))
       ):
        return -1

    # clip the engineering value to the engineering min/max values
    if   (engineering_value < engineering_min) : engineering_value = engineering_min
    elif (engineering_value > engineering_max) : engineering_value = engineering_max

    # get the sign
    if (engineering_value >= 0) : sign = +1
    else                        : sign = -1

    # strip the sign and scale the engineering value to get the data value
    data_value = round(abs(engineering_value) / scalefactor(a429_frame_format, size, engineering_min, engineering_max))

    # conditionally convert the data value from binary to BCD
    if(a429_frame_format == FRAME_FORMAT_BCD) :
        data_value = binary2bcd(data_value)

    # in BNR format, clip the data value to the max. possible value
    if(a429_frame_format == FRAME_FORMAT_BNR) :
        if (data_value >= 2**size) :
            data_value  = 2**size - 1

    # make sure data value is an integer
    data_value = int(data_value)

    # clear existing SSM
    frame &= ~(3 << 29)

    # clear existing data value
    frame &= ~((2**size - 1) << (lsb_position - 1))
    
    # place the new data value into the frame
    frame |=  ((data_value ) << (lsb_position - 1))

    # done in case of no SSM
    if ((a429_frame_format == FRAME_FORMAT_RAW) or (ssm_value == SSM_DATA)) :
        return frame

    # default SSM value to NCD
    ssm = 1

    # BCD frame format
    if   (a429_frame_format == FRAME_FORMAT_BCD) :

        # encode the SSM value
        if   (ssm_value == SSM_NO ) :
            if (sign > 0) : ssm = 0
            else          : ssm = 3
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2

    # BNR frame format
    elif (a429_frame_format == FRAME_FORMAT_BNR) :

        # place the sign valuer into the frame
        if (sign < 0) : frame |= 1 << 28

        # encode the SSM value
        if   (ssm_value == SSM_FW ) : ssm = 0
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2
        elif (ssm_value == SSM_NO ) : ssm = 3

    # discrete frame format
    elif (a429_frame_format == FRAME_FORMAT_DISCRETE) :

        # encode the SSM value
        if   (ssm_value == SSM_NO ) : ssm = 0
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2
        elif (ssm_value == SSM_FW ) : ssm = 3

    # make sure the SSM is an integer
    ssm = int(ssm)

    # place the SSM value into the frame
    frame |= ssm << 29

    # done
    return frame


def readValue(frame, a429_frame_format, lsb_position, size, \
              engineering_min, engineering_max, decimals) :

    # check the arguments
    if (    (size                <  1)
         or (lsb_position        <  9)
         or (lsb_position + size > 32)
       ):
        return -1

    # make sure frame is an integer
    frame = int(frame)

    # extract the data value and the raw SSM
    value = (frame >> (lsb_position - 1)) & (2**size - 1)
    ssm   = (frame >> 29                ) & (3          )

    # conditionally convert the data value from BCD to binary
    if(a429_frame_format == FRAME_FORMAT_BCD) :
        value = bcd2binary(value)

    # un-scale the data value to get the engineering value
    engineering_value = value * scalefactor(a429_frame_format, size, engineering_min, engineering_max)

    # conditionally round the engineering value
    if(a429_frame_format == FRAME_FORMAT_BNR) :
        engineering_value = round(engineering_value, decimals)

    # extract the SSM - BCD frame format
    if   (a429_frame_format == FRAME_FORMAT_BCD) :

        # decode the SSM
        if   (ssm == 0) : ssm_value = SSM_NO
        elif (ssm == 1) : ssm_value = SSM_NCD
        elif (ssm == 2) : ssm_value = SSM_FT
        elif (ssm == 3) :
            ssm_value          = SSM_NO
            engineering_value *= -1

    # extract the SSM - BNR frame format
    elif (a429_frame_format == FRAME_FORMAT_BNR) :

        # get & apply the sign
        if (frame & (1 << 28)) : engineering_value *= -1

        # decode the SSM
        if   (ssm == 0) : ssm_value = SSM_FW
        elif (ssm == 1) : ssm_value = SSM_NCD
        elif (ssm == 2) : ssm_value = SSM_FT
        elif (ssm == 3) : ssm_value = SSM_NO

    # extract the SSM - discrete frame format
    elif (a429_frame_format == FRAME_FORMAT_DISCRETE) :

        # decode the SSM
        if   (ssm == 0) : ssm_value = SSM_NO
        elif (ssm == 1) : ssm_value = SSM_NCD
        elif (ssm == 2) : ssm_value = SSM_FT
        elif (ssm == 3) : ssm_value = SSM_FW

    else :

        # any other or undefined frame format
        ssm_value = SSM_DATA

    # done, return tuple of value and SSM
    return engineering_value, ssm_value

# ============================================================================
