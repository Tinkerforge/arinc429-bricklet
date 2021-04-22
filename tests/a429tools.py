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

FRAME_FORMAT_BCD      = 1
FRAME_FORMAT_BNR      = 2
FRAME_FORMAT_DISCRETE = 3

SSM_DATA  = 0
SSM_NO    = 1
SSM_NCD   = 2
SSM_FW    = 3
SSM_FT    = 4
SSM_PLUS  = 5
SSM_MINUS = 6

SDI_SDI0 = 0
SDI_SDI1 = 1
SDI_SDI2 = 2
SDI_SDI3 = 3
SDI_DATA = 4

CHANNEL_TX  = 0
CHANNEL_TX1 = 1
CHANNEL_RX  = 32
CHANNEL_RX1 = 33
CHANNEL_RX2 = 34

PARITY_DATA = 0
PARITY_AUTO = 1

SPEED_HS = 0
SPEED_LS = 1

CHANNEL_MODE_PASSIVE = 0
CHANNEL_MODE_ACTIVE = 1
CHANNEL_MODE_RUN = 2

FRAME_STATUS_NEW = 0
FRAME_STATUS_UPDATE = 1
FRAME_STATUS_TIMEOUT = 2

SCHEDULER_JOB_SKIP = 0
SCHEDULER_JOB_CALLBACK = 1
SCHEDULER_JOB_STOP = 2
SCHEDULER_JOB_JUMP = 3
SCHEDULER_JOB_RETURN = 4
SCHEDULER_JOB_DWELL = 5
SCHEDULER_JOB_SINGLE = 6
SCHEDULER_JOB_CYCLIC = 7
SCHEDULER_JOB_RETRANS_RX1 = 8
SCHEDULER_JOB_RETRANS_RX2 = 9

TX_MODE_TRANSMIT = 0
TX_MODE_MUTE     = 1


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
    # init the internal vars
    bcd    = 0
    shift  = 0

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
    # init the internal vars
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
    # don't scale if the size argument has an illegal value
    if (size < 1) : return 1

    # make sure the size is an integer
    size = int(size)

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
    # init the result var
    result = 0

    # abort if the size argument has an illegal value
    if (size < 1) : return 1

    # make sure the size is an integer
    size = int(size)

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
    # abort if the size argument has an illegal value
    if (size < 1) : return 1

    # make sure the size is an integer
    size = int(size)

    return (2**size) - int(value)


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# enum to text functions
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def parity2text(parity) :
    """
    parity2text(parity)

    converts a parity enum value into a human-readable string
    """
    if (parity == PARITY_DATA) : return 'data'
    if (parity == PARITY_AUTO) : return 'auto'

    return 'error - undefined parity encoding'


def speed2text(speed) :
    """
    speed2text(parity)

    converts a speed enum value into a human-readable string
    """
    if (speed == SPEED_HS) : return 'high speed'
    if (speed == SPEED_LS) : return 'low speed'

    return 'error - undefined speed encoding'


def ssm2long_text(ssm) :
    """
    ssm2text(parity)

    converts a SSM enum value into a human-readable string
    """
    if   (ssm == SSM_DATA) : return 'SSM used for data'
    elif (ssm == SSM_NO  ) : return  'NO (normal operation)'
    elif (ssm == SSM_NCD ) : return 'NCD (no computed data)'
    elif (ssm == SSM_FW  ) : return  'FW (failure warning)'
    elif (ssm == SSM_FT  ) : return  'FT (functional test)'
    else                   : return  'error - undefined SSM encoding'


def ssm2text(ssm) :
    """

    ssm2text(parity)

    converts a SSM enum value into a human-readable string
    """
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
    # check the arguments
    if ((label > 255) or (label < 0) or (sdi > 4) or (sdi < 0)) : return -1

    # make sure the label and the SDI are integers
    label = int(label)
    sdi   = int(sdi)

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
            sign = -1
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
            sign = -1
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

        # abort if the engineering value is negative
        if (engineering_value < 0) : return -1

        # make sure the engineering value is an integer
        engineering_value = int(engineering_value)

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
            ssm_value = SSM_NO
            engineering_value *= -1


    elif(a429_frame_format == FRAME_FORMAT_BNR) :

        # in case of a negative value, convert the data value to 2's complement
        if (frame & (1 << 28)) :
            sign  = -1
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
