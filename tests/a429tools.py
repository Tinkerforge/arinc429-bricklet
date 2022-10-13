# -*- coding: utf-8 -*-

##############################################################################
#                                                                            #
#  A429 Tools for creating Frames and writing / reading Data to/from Frames  #
#                                                                            #
#  V0.92  5.03.2021, Ralph Lembcke                                           #
#                                                                            #
##############################################################################


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# constants - !!! keep in line with the A429 Bricklet API !!!
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FRAME_FORMAT_BCD          =  1  # A429 encoding schema 'BCD'
FRAME_FORMAT_BNR          =  2  # A429 encoding schema 'BNR'
FRAME_FORMAT_DISCRETE     =  3  # A429 encoding schema 'discrete'

SSM_DATA                  =  0  # SSM is used for data
SSM_NO                    =  1  # normal operation
SSM_NCD                   =  2  # no computed data
SSM_FW                    =  3  # failure warning
SSM_FT                    =  4  # functional test
SSM_PLUS                  =  5  # normal operation, positive value
SSM_MINUS                 =  6  # normal operation, negative value

SDI_SDI0                  =  0  # SDI 0
SDI_SDI1                  =  1  # SDI 1
SDI_SDI2                  =  2  # SDI 2
SDI_SDI3                  =  3  # SDI 3
SDI_DATA                  =  4  # SDI bits are used to hold a part of the value data

CHANNEL_TX                =  0  # all TX channels
CHANNEL_TX1               =  1  # TX channel 1
CHANNEL_RX                = 32  # all RX channels
CHANNEL_RX1               = 33  # RX channel 1
CHANNEL_RX2               = 34  # RX channel 2

PARITY_DATA               =  0  # the parity bit is used to hold a part of the value data
PARITY_AUTO               =  1  # the parity bit is used for parity, parity is managed by the A429 bricklet automatically

SPEED_HS                  =  0  # A429 high bus speed
SPEED_LS                  =  1  # A429 low  bus speed

CHANNEL_MODE_PASSIVE      =  0  # channel is inactive (TX: high-Z)
CHANNEL_MODE_ACTIVE       =  1  # channel is active
CHANNEL_MODE_RUN          =  2  # TX only: TX scheduler is running

FRAME_STATUS_NEW          =  0  # initial reception or 1st reception after a timeout
FRAME_STATUS_UPDATE       =  1  # successive reception with a new value
FRAME_STATUS_TIMEOUT      =  2  # no reception any more

SCHEDULER_JOB_SKIP        =  0  # do nothing
SCHEDULER_JOB_CALLBACK    =  1  # send a scheduler callback
SCHEDULER_JOB_STOP        =  2  # stop the scheduler (channel mode changes to CHANNEL_MODE_ACTIVE)
SCHEDULER_JOB_JUMP        =  3  # continue the schedule at the given index
SCHEDULER_JOB_RETURN      =  4  # resume   the schedule at the index position of the last JUMP + 1
SCHEDULER_JOB_DWELL       =  5  # wait for the given time (in milliseconds)
SCHEDULER_JOB_SINGLE      =  6  # transmit the referenced frame a single time (can be         re-enabled via                TX_MODE_TRANSMIT)
SCHEDULER_JOB_CYCLIC      =  7  # transmit the referenced frame in each cycle (can be muted / re-enabled via TX_MODE_MUTE / TX_MODE_TRANSMIT)
SCHEDULER_JOB_RETRANS_RX1 =  8  # transmit the referenced frame as received on RX 1 in each cycle
SCHEDULER_JOB_RETRANS_RX2 =  9  # transmit the referenced frame as received on RX 2 in each cycle

TX_MODE_TRANSMIT          =  0  # re-enable transmission (trigger another single transmit)
TX_MODE_MUTE              =  1  # stop the  transmission


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# low-level functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def binary2bcd(binary) :
    """
    binary2bcd(binary)

    Takes a binary encoded integer number and delivers an integer that actually holds
    the number in BCD coding. The BCD encoded number will have as many bits / BCD digits
    as are needed to represent the binary number.
    """

    # initialize the internal vars
    bcd   = 0
    shift = 0

    # convert from binary to BCD, working through the 10th, 100th, ...
    while (binary) :
        bcd    +=  int(binary % 10) << (4*shift)
        shift  +=  1
        binary /= 10

    return bcd


def bcd2binary(bcd) :
    """
    bcd2binary(bcd)

    Takes a BCD encoded integer number and delivers an integer that holds the number
    in plain binary encoding. The binary encoded number will have as many bits as are
    needed to represent the VCD number.
    """

    # initialize the internal vars
    binary = 0
    factor = 1

    # convert from BCD to binary, working through the BCD digits
    while (bcd) :
        binary += (bcd % 16) * factor
        factor *= 10
        bcd     = bcd >> 4

    return binary


def scalefactor(size, eng_min, eng_max) :
    """
    scalefactor(size, eng_min, eng_max)

    Delivers the scaling factor needed for encoding / decoding values in the
    A429 BNR (binary) format:

    size    : number of bits the encoded value will occupy in the A429 frame
    eng_min : smallest engineering value allowed (may be negative)
    eng_max : biggest  engineering value allowed

    Usage:

    encoded_value     = round(engineering_value / scalefactor())
    engineering_value = encoded_value * scalefactor()
    """

    # don't scale if the size argument is not an integer or otherwise illegal
    if not isinstance(size, int) or (size < 1) : return 1

    # the scaling assumes that the biggest engineering value is encoded
    # by the number 2^size, although in practice the biggest value that
    # can be encoded is 2^size - 1
    return max(abs(eng_min), abs(eng_max)) / (2**size)


def mirror(num, size) :
    """
    mirror(num, size)

    Exchanges the value in the MSB with that in the LSB, in the MSB-1 with the
    LSB+1 and so on, whereby the number has 'size' bits.

    Example:

    mirror(0b11000101, 8) yields 0b10100011
    """

    # initialize the result var
    result = 0

    # make sure the size is an integer
    size = int(size)

    # abort if the size argument has an illegal value
    if (size < 1) : return 1

    # shift the bits of num out to the right and into result to the left
    for i in range(size):
        result <<= 1
        result  |= num & 1
        num    >>= 1

    return result


def twos_complement(value, size) :
    """
    twos_complement(value, size)

    Delivers the 2's complement of the given value,
    assuming the number is sized 'size' bits.
    """

    # make sure the size is an integer
    size = int(size)

    # abort if the size argument has an illegal value
    if (size < 1) : return 1

    return (2**size) - int(value)


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# enum to text functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def parity2text(parity) :
    """
    parity2text(parity)

    converts a parity enum value into a human-readable string
    """

    switcher = {PARITY_DATA : 'data',
                PARITY_AUTO : 'auto',
               }

    return switcher.get(parity, 'error - undefined parity encoding')


def speed2text(speed) :
    """
    speed2text(parity)

    converts a speed enum value into a human-readable string
    """

    switcher = {SPEED_HS : 'high speed',
                SPEED_LS :  'low speed',
               }

    return switcher.get(speed, 'error - undefined speed encoding')


def ssm2long_text(ssm) :
    """
    ssm2text(parity)

    converts a SSM enum value into a human-readable string
    """

    switcher = {SSM_DATA : 'SSM used for data',
                SSM_NO   :  'NO (normal operation)',
                SSM_NCD  : 'NCD (no computed data)',
                SSM_FW   :  'FW (failure warning)',
                SSM_FT   :  'FT (functional test)',
               }

    return switcher.get(speed, 'error - undefined SSM encoding')


def ssm2text(ssm) :
    """

    ssm2text(parity)

    converts a SSM enum value into a human-readable string
    """

    switcher = {SSM_DATA : 'data',
                SSM_NO   : 'NO',
                SSM_NCD  : 'NCD',
                SSM_FW   : 'FW',
                SSM_FT   : 'FT',
               }

    return switcher.get(speed, 'error - undefined SSM encoding')


    if   (ssm == SSM_DATA) : return 'data'
    elif (ssm == SSM_NO  ) : return 'NO'
    elif (ssm == SSM_NCD ) : return 'NCD'
    elif (ssm == SSM_FW  ) : return 'FW'
    elif (ssm == SSM_FT  ) : return 'FT'
    else                   : return 'error'



# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# high level functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def createFrame(label, sdi) :
    """
    createFrame(label, sdi)

    Delivers a raw A429 frame filled with the given label and SDI value.
    The label needs to be supplied in binary format, i.e. use the 0oLLL
    notation to make use of python's built-in octal to binary conversion.
    Within the delivered frame, the label will have it's LSB aligned with
    the LSB of the frame, as expected by the TinkerForge Arinc429 Bricklet:

    label = frame & 0x000000FF

    The sdi argument may either be a value between 0 and 3 (SDI_0 ... SDI_3,
    or 4 (SDI_DATA) to omit the imprinting of the SDI value.

    If the label or sdi argument are illegal, a value of -1 will be delivered.
    """

    # make sure the label and the SDI are integers
    label = int(label)
    sdi   = int(sdi)

    # check the arguments
    if ((label > 255) or (label < 0) or (sdi > 4) or (sdi < 0)) : return -1

    # assemble the raw frame
    if (sdi == SDI_DATA) : return (label)
    else                 : return (label) | (sdi << 8)


def readLabel(frame) :
    """
    readLabel(frame)

    Delivers the value of the label in binary format. Within the frame,
    the label needs to have it's LSB aligned with the LSB of the frame,
    as delivered by the TinkerForge Arinc429 Bricklet.
    """

    # extract the lowest 8 bits
    return frame & 0x000000FF


def readExtendedLabel(frame) :
    """
    readExtendedLabel(frame)

    Delivers the combination of the value of the label and the SDI in
    binary format. Actually, it returns the lowest 10 bits of the provided
    frame.
    """

    # extract the lowest 10 bits
    return frame & 0x000003FF


def readSDI(frame) :
    """
    readSDI(frame)

    Delivers the value of the SDI in binary format.
    """

    # extract bits 8..9
    return (frame & 0x00000300) >> 8


def readLabelSDI(frame) :
    """
    readLabelSDI(frame)

    Delivers the values of the label and the SDI as a tupel in binary format.
    """

    # extract the lowest 8 bits and bits 8..9 separately
    return frame & 0x000000FF, (frame & 0x00000300) >> 8


def readRawSSM(frame) :
    """
    readRawSSM(frame)

    Delivers the value of the SSM in raw binary format.
    """

    # extract bits 29..30
    return (frame & 0x60000000) >> 29


def combineSSM(ssm_1, ssm_2) :
    """
    returns the highest priority SSM value from two given SSM values
    """

    # work through given SSMs by priority
    if ((ssm_1 == SSM_FW  ) or (ssm_2 == SSM_FW  )) : return SSM_FW
    if ((ssm_1 == SSM_NCD ) or (ssm_2 == SSM_NCD )) : return SSM_NCD
    if ((ssm_1 == SSM_FT  ) or (ssm_2 == SSM_FT  )) : return SSM_NCD
    if ((ssm_1 == SSM_NO  ) or (ssm_2 == SSM_NO  )) : return SSM_NO

    # default - no SSM
    return SSM_DATA


def setValue(frame, a429_frame_format, lsb_position, size, \
             engineering_min, engineering_max, engineering_value, ssm_value) :
    """
    setValue(frame, a429_frame_format, lsb_position, size, \
             engineering_min, engineering_max, engineering_value, ssm_value)

    Imprints a value into a given A429 frame. The value will be converted from
    engineering format to encoded format according to the given A429 frame
    format.

    frame            : raw frame with label and SDI, or set to 0 if the label and SDI will
                       be added later.

    a429_frame_format: either FRAME_FORMAT_BCD, FRAME_FORMAT_BNR, or FRAME_FORMAT_DISCRETE

    lsb_position     : bit position from where the encoded value will start to be put into
                       the frame. The 1st position left to the SDI bits will be position 11.

    size             : the number of bits the encoded value will take in the frame. If using
                       the SSM bits for data, lsb_position + size will be > 29 and the imprinting
                       of the SSM value will be automatically suppressed.

    engineering_min,
    engineering_max  : configures the scaling of BNR encoded values and serves as an min/max
                       limiter for the engineering value in all frame formats.

    engineering_value: the value to be encoded into the A429 frame.

    ssm_value        : the SSM value to be put into the A429 frame, it will encoded according
                       to the rules of the selected frame format.

    In case any of the supplied arguments has an illegal value, -1 will be returned.
    """

    # initialize internal vars
    sign = +1   # engineering value is >= 0
    ssm  =  1   # NCD as of A429 encoding

    # check the arguments
    if (    (size                <  1)
         or (lsb_position        <  9)
         or (lsb_position + size > 32)
       ) :
        return -1

    # make sure the size is an integer
    size = int(size)

    # clip the engineering value to the engineering min/max values
    if   (engineering_value < engineering_min) : engineering_value = engineering_min
    elif (engineering_value > engineering_max) : engineering_value = engineering_max


    # switch on frame format
    if (a429_frame_format == FRAME_FORMAT_BCD) :

        # make sure the engineering value is an integer
        engineering_value = int(engineering_value)

        # process negative values
        if (engineering_value < 0) :
            sign               = -1
            engineering_value *= -1

        # convert the engineering value into BCD encoding
        data_value = binary2bcd(engineering_value)

        # encode the SSM value
        if   (ssm_value == SSM_NO ) :
            if (sign > 0) :           ssm = 0
            else          :           ssm = 3
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2


    elif (a429_frame_format == FRAME_FORMAT_BNR) :

        # process negative values
        if (engineering_value < 0) :
            sign               = -1
            engineering_value *= -1

        # scale the engineering value
        data_value = round(engineering_value / scalefactor(size, engineering_min, engineering_max))

        # clip the encoded value to the max. possible value
        if (data_value >= 2**size) : data_value = 2**size - 1

        # negative value?
        if (sign < 0) :

            # yes, put the data value into it's 2's complement
            data_value = twos_complement(data_value, size)

            # place the negative marker into the frame
            frame |= 1 << 28

        # encode the SSM value
        if   (ssm_value == SSM_FW ) : ssm = 0
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2
        elif (ssm_value == SSM_NO ) : ssm = 3


    elif (a429_frame_format == FRAME_FORMAT_DISCRETE) :

        # make sure the engineering value is an integer
        engineering_value = int(engineering_value)

        # abort if the engineering value is negative
        if (engineering_value < 0) : return -1

        # in DISCRETE format, the engineering value goes straight into the A429 frame
        data_value = engineering_value

        # encode the SSM value
        if   (ssm_value == SSM_NO ) : ssm = 0
        elif (ssm_value == SSM_NCD) : ssm = 1
        elif (ssm_value == SSM_FT ) : ssm = 2
        elif (ssm_value == SSM_FW ) : ssm = 3

    else :

        # undefined frame format, abort
        return -1


    # clear the existing data value and insert the new data value
    frame &= ~((2**size - 1) << (lsb_position - 1))
    frame |=  ((data_value ) << (lsb_position - 1))

    # done in case the SSM bits are used for data
    if ((ssm_value == SSM_DATA) or (lsb_position + size > 30)) : return frame

    # clear the existing SSM value and insert the new SSM value
    frame &= ~( 3  << 29)
    frame |=  (ssm << 29)

    # done
    return frame


def getValue(frame, a429_frame_format, lsb_position, size, \
              engineering_min, engineering_max, decimals) :
    """
    engineering_value, ssm_value = getValue(frame, a429_frame_format, lsb_position, size, \
                                            engineering_min, engineering_max, decimals)

    Reads a value from a given A429 frame. The value will be converted from encoded
    format to engineering format according to the given A429 frame format.

    frame            : A429 frame holding the value and the SSM bits (if not used for data).

    a429_frame_format: either FRAME_FORMAT_BCD, FRAME_FORMAT_BNR, or FRAME_FORMAT_DISCRETE

    lsb_position     : bit position from where the encoded value will start. The 1st position
                       left to the SDI bits will be position 11.

    size             : the number of bits the encoded value takes in the frame. If using the
                       SSM bits for data, lsb_position + size will be > 29 and the returned
                       SSM will be forced to SSM_NO (normal operation).

    engineering_min,
    engineering_max  : used with BNR encoded frames only, configures the scaling between
                       encoded and engineering value.

    decimals         : used with BNR encoded frames only, number of decimal digits the
                       engineering value shall be rounded to.

    engineering_value: the decoded value.

    ssm_value        : the decoded SSM value.

    In case any of the supplied arguments has an illegal value, -1 will be returned.
    """

    # initialize internal vars
    sign = +1   # engineering value is >= 0

    # set default return values
    engineering_value = 0
    ssm_value         = SSM_NO

    # check the arguments
    if (    (size                <  1)
         or (lsb_position        <  9)
         or (lsb_position + size > 32)
       ):
        return -1

    # make sure that frame, size and decimals are integers
    frame    = int(frame)
    size     = int(size)
    decimals = int(decimals)

    # extract the data value and the raw SSM
    data_value = (frame >> (lsb_position - 1)) & (2**size - 1)
    data_ssm   = (frame >> 29                ) & (3          )


    # switch on frame format
    if(a429_frame_format == FRAME_FORMAT_BCD) :

        # convert the data value from BCD to binary
        engineering_value = bcd2binary(data_value)

        # decode the SSM
        if   (data_ssm == 0) : ssm_value = SSM_NO
        elif (data_ssm == 1) : ssm_value = SSM_NCD
        elif (data_ssm == 2) : ssm_value = SSM_FT
        elif (data_ssm == 3) :
            ssm_value          = SSM_NO
            engineering_value *= -1


    elif(a429_frame_format == FRAME_FORMAT_BNR) :

        # in case of a negative value, convert the data value to 2's complement
        if (frame & (1 << 28)) :
            sign       = -1
            data_value = twos_complement(data_value, size)

        # un-scale the data value to get the engineering value
        engineering_value = data_value * scalefactor(size, engineering_min, engineering_max)

        # apply the sign
        engineering_value *= sign

        # round the engineering value
        engineering_value = round(engineering_value, decimals)

        # decode the SSM
        if   (data_ssm == 0) : ssm_value = SSM_FW
        elif (data_ssm == 1) : ssm_value = SSM_NCD
        elif (data_ssm == 2) : ssm_value = SSM_FT
        elif (data_ssm == 3) : ssm_value = SSM_NO


    elif (a429_frame_format == FRAME_FORMAT_DISCRETE) :

        # in DISCRETE format, the engineering value got straight into the A429 frame
        engineering_value = data_value

        # conditionally decode the SSM
        if (lsb_position + size <= 30) :
            if   (data_ssm == 0) : ssm_value = SSM_NO
            elif (data_ssm == 1) : ssm_value = SSM_NCD
            elif (data_ssm == 2) : ssm_value = SSM_FT
            elif (data_ssm == 3) : ssm_value = SSM_FW
        else                     : ssm_value = SSM_DATA

    else :

        # undefined frame format, abort
        return -1

    # done, return a tuple of engineering value and SSM
    return engineering_value, ssm_value

# ============================================================================
