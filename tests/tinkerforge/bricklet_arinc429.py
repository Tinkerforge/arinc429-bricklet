# -*- coding: utf-8 -*-
#############################################################
# This file was automatically generated on 2020-10-18.      #
#                                                           #
# Python Bindings Version 2.1.26                            #
#                                                           #
# If you have a bugfix for this file and want to commit it, #
# please fix the bug in the generator. You can find a link  #
# to the generators git repository on tinkerforge.com       #
#############################################################

#### __DEVICE_IS_NOT_RELEASED__ ####

from collections import namedtuple

try:
    from .ip_connection import Device, IPConnection, Error, create_char, create_char_list, create_string, create_chunk_data
except ValueError:
    from ip_connection import Device, IPConnection, Error, create_char, create_char_list, create_string, create_chunk_data

DebugGetDiscretes = namedtuple('DebugGetDiscretes', ['rx_discretes', 'tx_discretes'])
DebugReadRegisterLowLevel = namedtuple('DebugReadRegisterLowLevel', ['value_length', 'value_data', 'rw_error'])
GetCapabilities = namedtuple('Capabilities', ['tx_total_scheduler_tasks', 'tx_used_scheduler_tasks', 'rx_total_frame_filters', 'rx_used_frame_filters'])
GetHeartbeatCallbackConfiguration = namedtuple('HeartbeatCallbackConfiguration', ['period', 'value_has_to_change'])
GetChannelConfiguration = namedtuple('ChannelConfiguration', ['parity', 'speed'])
ReadFrame = namedtuple('ReadFrame', ['status', 'frame', 'age'])
GetRXCallbackConfiguration = namedtuple('RXCallbackConfiguration', ['enabled', 'value_has_to_change', 'timeout'])
GetScheduleEntry = namedtuple('ScheduleEntry', ['job', 'frame_index', 'frame', 'dwell_time'])
GetSPITFPErrorCount = namedtuple('SPITFPErrorCount', ['error_count_ack_checksum', 'error_count_message_checksum', 'error_count_frame', 'error_count_overflow'])
GetIdentity = namedtuple('Identity', ['uid', 'connected_uid', 'position', 'hardware_version', 'firmware_version', 'device_identifier'])
DebugReadRegister = namedtuple('DebugReadRegister', ['value', 'rw_error'])

class BrickletARINC429(Device):
    """
    TBD
    """

    DEVICE_IDENTIFIER = 2160
    DEVICE_DISPLAY_NAME = 'ARINC429 Bricklet'
    DEVICE_URL_PART = 'arinc429' # internal

    CALLBACK_HEARTBEAT = 7
    CALLBACK_FRAME_MESSAGE = 20


    FUNCTION_DEBUG_GET_DISCRETES = 1
    FUNCTION_DEBUG_READ_REGISTER_LOW_LEVEL = 2
    FUNCTION_DEBUG_WRITE_REGISTER_LOW_LEVEL = 3
    FUNCTION_GET_CAPABILITIES = 4
    FUNCTION_SET_HEARTBEAT_CALLBACK_CONFIGURATION = 5
    FUNCTION_GET_HEARTBEAT_CALLBACK_CONFIGURATION = 6
    FUNCTION_SET_CHANNEL_CONFIGURATION = 8
    FUNCTION_GET_CHANNEL_CONFIGURATION = 9
    FUNCTION_SET_CHANNEL_MODE = 10
    FUNCTION_GET_CHANNEL_MODE = 11
    FUNCTION_CLEAR_ALL_RX_FILTERS = 12
    FUNCTION_CLEAR_RX_FILTER = 13
    FUNCTION_SET_RX_STANDARD_FILTERS = 14
    FUNCTION_SET_RX_FILTER = 15
    FUNCTION_GET_RX_FILTER = 16
    FUNCTION_READ_FRAME = 17
    FUNCTION_SET_RX_CALLBACK_CONFIGURATION = 18
    FUNCTION_GET_RX_CALLBACK_CONFIGURATION = 19
    FUNCTION_WRITE_FRAME_DIRECT = 21
    FUNCTION_WRITE_FRAME_SCHEDULED = 22
    FUNCTION_CLEAR_SCHEDULE_ENTRIES = 23
    FUNCTION_SET_SCHEDULE_ENTRY = 24
    FUNCTION_GET_SCHEDULE_ENTRY = 25
    FUNCTION_RESET_A429 = 26
    FUNCTION_GET_SPITFP_ERROR_COUNT = 234
    FUNCTION_SET_BOOTLOADER_MODE = 235
    FUNCTION_GET_BOOTLOADER_MODE = 236
    FUNCTION_SET_WRITE_FIRMWARE_POINTER = 237
    FUNCTION_WRITE_FIRMWARE = 238
    FUNCTION_SET_STATUS_LED_CONFIG = 239
    FUNCTION_GET_STATUS_LED_CONFIG = 240
    FUNCTION_GET_CHIP_TEMPERATURE = 242
    FUNCTION_RESET = 243
    FUNCTION_WRITE_UID = 248
    FUNCTION_READ_UID = 249
    FUNCTION_GET_IDENTITY = 255

    RW_ERROR_OK = 0
    RW_ERROR_NO_WRITE = 1
    RW_ERROR_NO_READ = 2
    RW_ERROR_INVALID_OP_CODE = 3
    RW_ERROR_INVALID_LENGTH = 4
    RW_ERROR_SPI = 5
    CHANNEL_TX = 0
    CHANNEL_TX1 = 1
    CHANNEL_TX2 = 2
    CHANNEL_TX3 = 3
    CHANNEL_TX4 = 4
    CHANNEL_TX5 = 5
    CHANNEL_TX6 = 6
    CHANNEL_TX7 = 7
    CHANNEL_TX8 = 8
    CHANNEL_TX9 = 9
    CHANNEL_TX10 = 10
    CHANNEL_TX11 = 11
    CHANNEL_TX12 = 12
    CHANNEL_RX = 32
    CHANNEL_RX1 = 33
    CHANNEL_RX2 = 34
    CHANNEL_RX3 = 35
    CHANNEL_RX4 = 36
    CHANNEL_RX5 = 37
    CHANNEL_RX6 = 38
    CHANNEL_RX7 = 39
    CHANNEL_RX8 = 40
    CHANNEL_RX9 = 41
    CHANNEL_RX10 = 42
    CHANNEL_RX11 = 43
    CHANNEL_RX12 = 44
    SDI_SDI0 = 0
    SDI_SDI1 = 1
    SDI_SDI2 = 2
    SDI_SDI3 = 3
    SDI_DATA = 4
    PARITY_DATA = 0
    PARITY_AUTO = 1
    SPEED_HS = 0
    SPEED_LS = 1
    CHANNEL_MODE_PASSIVE = 0
    CHANNEL_MODE_ACTIVE = 1
    CHANNEL_MODE_RUN = 2
    FRAME_STATUS_UPDATE = 0
    FRAME_STATUS_TIMEOUT = 1
    SCHEDULER_JOB_EMPTY = 0
    SCHEDULER_JOB_MUTE = 1
    SCHEDULER_JOB_SINGLE = 2
    SCHEDULER_JOB_CYCLIC = 3
    A429_MODE_NORMAL = 0
    A429_MODE_DEBUG = 1
    BOOTLOADER_MODE_BOOTLOADER = 0
    BOOTLOADER_MODE_FIRMWARE = 1
    BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT = 2
    BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT = 3
    BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT = 4
    BOOTLOADER_STATUS_OK = 0
    BOOTLOADER_STATUS_INVALID_MODE = 1
    BOOTLOADER_STATUS_NO_CHANGE = 2
    BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT = 3
    BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT = 4
    BOOTLOADER_STATUS_CRC_MISMATCH = 5
    STATUS_LED_CONFIG_OFF = 0
    STATUS_LED_CONFIG_ON = 1
    STATUS_LED_CONFIG_SHOW_HEARTBEAT = 2
    STATUS_LED_CONFIG_SHOW_STATUS = 3

    def __init__(self, uid, ipcon):
        """
        Creates an object with the unique device ID *uid* and adds it to
        the IP Connection *ipcon*.
        """
        Device.__init__(self, uid, ipcon, BrickletARINC429.DEVICE_IDENTIFIER, BrickletARINC429.DEVICE_DISPLAY_NAME)

        self.api_version = (2, 0, 0)

        self.response_expected[BrickletARINC429.FUNCTION_DEBUG_GET_DISCRETES] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_DEBUG_READ_REGISTER_LOW_LEVEL] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_DEBUG_WRITE_REGISTER_LOW_LEVEL] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_CAPABILITIES] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_HEARTBEAT_CALLBACK_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_HEARTBEAT_CALLBACK_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_CHANNEL_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_GET_CHANNEL_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_CHANNEL_MODE] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_GET_CHANNEL_MODE] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_CLEAR_ALL_RX_FILTERS] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_CLEAR_RX_FILTER] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_RX_STANDARD_FILTERS] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_SET_RX_FILTER] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_RX_FILTER] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_READ_FRAME] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_RX_CALLBACK_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_RX_CALLBACK_CONFIGURATION] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_WRITE_FRAME_DIRECT] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_WRITE_FRAME_SCHEDULED] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_CLEAR_SCHEDULE_ENTRIES] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_SET_SCHEDULE_ENTRY] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_GET_SCHEDULE_ENTRY] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_RESET_A429] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_GET_SPITFP_ERROR_COUNT] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_BOOTLOADER_MODE] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_BOOTLOADER_MODE] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_WRITE_FIRMWARE_POINTER] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_WRITE_FIRMWARE] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_SET_STATUS_LED_CONFIG] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_GET_STATUS_LED_CONFIG] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_CHIP_TEMPERATURE] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_RESET] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_WRITE_UID] = BrickletARINC429.RESPONSE_EXPECTED_FALSE
        self.response_expected[BrickletARINC429.FUNCTION_READ_UID] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE
        self.response_expected[BrickletARINC429.FUNCTION_GET_IDENTITY] = BrickletARINC429.RESPONSE_EXPECTED_ALWAYS_TRUE

        self.callback_formats[BrickletARINC429.CALLBACK_HEARTBEAT] = (23, 'B H 3H 3H')
        self.callback_formats[BrickletARINC429.CALLBACK_FRAME_MESSAGE] = (19, 'B B H B I H')

        ipcon.add_device(self)

    def debug_get_discretes(self):
        """
        Debug function to read the discrete signals from the A429 chip.
        RX Discretes Bit   9: MB2-1   - pending frame in RX2, PRIO 1
                           8: MB2-2   -                            2
                           7: MB2-3   -                            3
                           6: R2FLAG  -                       FIFO
                           5: R2INT   -                       FIFO
                           4: MB1-1   - pending frame in RX1, PRIO 1
                           3: MB1-2   -                            2
                           2: MB1-3   -                            3
                           1: R1FLAG  -                       FIFO
                           0: R1INT   -                       FIFO
        TX Discretes Bit 2-7: unused
                           1: TFULL   - TX buffer full
                           0: TEMPTY  - TX buffer empty
        """
        self.check_validity()

        return DebugGetDiscretes(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_DEBUG_GET_DISCRETES, (), '', 12, 'H H'))

    def debug_read_register_low_level(self, op_code):
        """
        Debug function to execute a direct SPI read access on the A429 chip.
        """
        self.check_validity()

        op_code = int(op_code)

        return DebugReadRegisterLowLevel(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_DEBUG_READ_REGISTER_LOW_LEVEL, (op_code,), 'B', 42, 'B 32B B'))

    def debug_write_register_low_level(self, op_code, value_length, value_data):
        """
        Debug function to execute a direct SPI write access on the A429 chip.
        """
        self.check_validity()

        op_code = int(op_code)
        value_length = int(value_length)
        value_data = list(map(int, value_data))

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_DEBUG_WRITE_REGISTER_LOW_LEVEL, (op_code, value_length, value_data), 'B B 32B', 9, 'B')

    def get_capabilities(self):
        """

        """
        self.check_validity()

        return GetCapabilities(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_CAPABILITIES, (), '', 18, 'H H H 2H'))

    def set_heartbeat_callback_configuration(self, period, value_has_to_change):
        """
        Set the bricklet heartbeat which reports the statistics counter for
        processed frames and lost frames.
        The period is the period with which the :cb:`Heartbeat` callback
        is triggered periodically. A value of 0 turns the callback off.
        When 'Value Has To Change' is enabled, the heartbeat will only be
        sent if there is a change in the statistics numbers.
        """
        self.check_validity()

        period = int(period)
        value_has_to_change = bool(value_has_to_change)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_HEARTBEAT_CALLBACK_CONFIGURATION, (period, value_has_to_change), 'B !', 0, '')

    def get_heartbeat_callback_configuration(self):
        """
        Get the configuration of the bricklet heartbeat reporting the satistics counters.
        """
        self.check_validity()

        return GetHeartbeatCallbackConfiguration(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_HEARTBEAT_CALLBACK_CONFIGURATION, (), '', 10, 'B !'))

    def set_channel_configuration(self, channel, parity, speed):
        """
        Set the physical properties of the selected channel:
         * Channel:   channel to configure
         * Parity:    'parity' for automatic parity adjustment, 'transparent' for transparent mode
         * Speed:     'hs' for high speed (100 kbit/s), 'ls' for low speed (12.5 kbit/s)
        """
        self.check_validity()

        channel = int(channel)
        parity = int(parity)
        speed = int(speed)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_CHANNEL_CONFIGURATION, (channel, parity, speed), 'B B B', 0, '')

    def get_channel_configuration(self, channel):
        """
        Get the physical properties of the selected channel.
        """
        self.check_validity()

        channel = int(channel)

        return GetChannelConfiguration(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_CHANNEL_CONFIGURATION, (channel,), 'B', 10, 'B B'))

    def set_channel_mode(self, channel, mode):
        """
        Set the operating mode of the selected channel:
         * passive: the TX channel stops transmitting and becomes high-Z, the RX channel will not receive frames
         * active:  the TX channel is ready to send frames and the RX channel will receive frames
         * filtering: RX channels only - the bricklet will only forward frames that match with the set filters
         * running:   TX channels only - the scheduler will run and transmit labels according to the set schedule
        """
        self.check_validity()

        channel = int(channel)
        mode = int(mode)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_CHANNEL_MODE, (channel, mode), 'B B', 0, '')

    def get_channel_mode(self, channel):
        """
        Get the operating mode of the selected channel.
        """
        self.check_validity()

        channel = int(channel)

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_CHANNEL_MODE, (channel,), 'B', 9, 'B')

    def clear_all_rx_filters(self, channel):
        """
        Clear all RX filters in the given channel(s).
        """
        self.check_validity()

        channel = int(channel)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_CLEAR_ALL_RX_FILTERS, (channel,), 'B', 0, '')

    def clear_rx_filter(self, channel, label, sdi):
        """
        Clear one RX filter in the given channel(s).
        """
        self.check_validity()

        channel = int(channel)
        label = int(label)
        sdi = int(sdi)

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_CLEAR_RX_FILTER, (channel, label, sdi), 'B B B', 9, '!')

    def set_rx_standard_filters(self, channel):
        """
        Set a SDI_DATA filter for every label (0-255). Existing filters will be overwritten.
        """
        self.check_validity()

        channel = int(channel)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_RX_STANDARD_FILTERS, (channel,), 'B', 0, '')

    def set_rx_filter(self, channel, label, sdi):
        """
        Set a RX filter configuration for the selected channel(s):
         * Label: Arinc429 label
         * SDI:   when set to 'Address', 4 filters will be created, one for each possible SDI value
         * Timeout: time span with no new frame received after which a timeout message will be generated.
                    A timeout value of zero disables the timeout.
        """
        self.check_validity()

        channel = int(channel)
        label = int(label)
        sdi = int(sdi)

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_RX_FILTER, (channel, label, sdi), 'B B B', 9, '!')

    def get_rx_filter(self, channel, label, sdi):
        """
        Get the configuration of a RX filter.
        """
        self.check_validity()

        channel = int(channel)
        label = int(label)
        sdi = int(sdi)

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_RX_FILTER, (channel, label, sdi), 'B B B', 9, '!')

    def read_frame(self, channel, label, sdi):
        """
        Do a direct read from one of the RX channels and receive buffers. If a new frame
        was received 'Status' will return as 'true', else it will have a vlaue of 'false'
        and the last frame received will be repeated.
        """
        self.check_validity()

        channel = int(channel)
        label = int(label)
        sdi = int(sdi)

        return ReadFrame(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_READ_FRAME, (channel, label, sdi), 'B B B', 15, '! I H'))

    def set_rx_callback_configuration(self, channel, enabled, value_has_to_change, timeout):
        """
        Enable or disable the generation of callbacks on receiving A429 frames.
        If the `value has to change` parameter is set to TRUE, the callback is only
        triggered when the frame data have changed, else it is triggered on every
        reception of a new frame.
        """
        self.check_validity()

        channel = int(channel)
        enabled = bool(enabled)
        value_has_to_change = bool(value_has_to_change)
        timeout = int(timeout)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_RX_CALLBACK_CONFIGURATION, (channel, enabled, value_has_to_change, timeout), 'B ! ! H', 0, '')

    def get_rx_callback_configuration(self, channel):
        """
        Get the configuration of the RX frame callback.
        """
        self.check_validity()

        channel = int(channel)

        return GetRXCallbackConfiguration(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_RX_CALLBACK_CONFIGURATION, (channel,), 'B', 12, '! ! H'))

    def write_frame_direct(self, channel, frame):
        """
        Do an immediate transmit of an A429 frame on the selected transmit channel.
        """
        self.check_validity()

        channel = int(channel)
        frame = int(frame)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_WRITE_FRAME_DIRECT, (channel, frame), 'B I', 0, '')

    def write_frame_scheduled(self, channel, frame_index, frame):
        """
        Set or update a frame that is transmitted by the scheduler.
         * Frame Index: index number of the frame (the scheduler picks the frames by this index number)
         * Frame:       the A429 frame itself
        """
        self.check_validity()

        channel = int(channel)
        frame_index = int(frame_index)
        frame = int(frame)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_WRITE_FRAME_SCHEDULED, (channel, frame_index, frame), 'B H I', 0, '')

    def clear_schedule_entries(self, channel, task_index_first, task_index_last):
        """
        Clear a range of TX scheduler entries.
         * Channel:   selected TX channel
         * First:     first schedule entry to be cleared
         * Last:      last  schedule entry to be cleared
        """
        self.check_validity()

        channel = int(channel)
        task_index_first = int(task_index_first)
        task_index_last = int(task_index_last)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_CLEAR_SCHEDULE_ENTRIES, (channel, task_index_first, task_index_last), 'B H H', 0, '')

    def set_schedule_entry(self, channel, task_index, job, frame_index, dwell_time):
        """
        Set a TX scheduler entry:
         * Channel:     selected TX channel
         * Task Index:   schedule entry index
         * Job:         activity assigned to this entry
         * Frame Index: frame    assigned to this slot by frame index
         * Dwell_Time:  time to wait before executing the next job
        """
        self.check_validity()

        channel = int(channel)
        task_index = int(task_index)
        job = int(job)
        frame_index = int(frame_index)
        dwell_time = int(dwell_time)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_SCHEDULE_ENTRY, (channel, task_index, job, frame_index, dwell_time), 'B H B H B', 0, '')

    def get_schedule_entry(self, channel, task_index):
        """
        Get a TX scheduler entry.
        """
        self.check_validity()

        channel = int(channel)
        task_index = int(task_index)

        return GetScheduleEntry(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_SCHEDULE_ENTRY, (channel, task_index), 'B H', 16, 'B H I B'))

    def reset_a429(self, mode):
        """
        Resets all A429 operations.
        """
        self.check_validity()

        mode = int(mode)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_RESET_A429, (mode,), 'B', 0, '')

    def get_spitfp_error_count(self):
        """
        Returns the error count for the communication between Brick and Bricklet.

        The errors are divided into

        * ACK checksum errors,
        * message checksum errors,
        * framing errors and
        * overflow errors.

        The errors counts are for errors that occur on the Bricklet side. All
        Bricks have a similar function that returns the errors on the Brick side.
        """
        self.check_validity()

        return GetSPITFPErrorCount(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_SPITFP_ERROR_COUNT, (), '', 24, 'I I I I'))

    def set_bootloader_mode(self, mode):
        """
        Sets the bootloader mode and returns the status after the requested
        mode change was instigated.

        You can change from bootloader mode to firmware mode and vice versa. A change
        from bootloader mode to firmware mode will only take place if the entry function,
        device identifier and CRC are present and correct.

        This function is used by Brick Viewer during flashing. It should not be
        necessary to call it in a normal user program.
        """
        self.check_validity()

        mode = int(mode)

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_BOOTLOADER_MODE, (mode,), 'B', 9, 'B')

    def get_bootloader_mode(self):
        """
        Returns the current bootloader mode, see :func:`Set Bootloader Mode`.
        """
        self.check_validity()

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_BOOTLOADER_MODE, (), '', 9, 'B')

    def set_write_firmware_pointer(self, pointer):
        """
        Sets the firmware pointer for :func:`Write Firmware`. The pointer has
        to be increased by chunks of size 64. The data is written to flash
        every 4 chunks (which equals to one page of size 256).

        This function is used by Brick Viewer during flashing. It should not be
        necessary to call it in a normal user program.
        """
        self.check_validity()

        pointer = int(pointer)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_WRITE_FIRMWARE_POINTER, (pointer,), 'I', 0, '')

    def write_firmware(self, data):
        """
        Writes 64 Bytes of firmware at the position as written by
        :func:`Set Write Firmware Pointer` before. The firmware is written
        to flash every 4 chunks.

        You can only write firmware in bootloader mode.

        This function is used by Brick Viewer during flashing. It should not be
        necessary to call it in a normal user program.
        """
        self.check_validity()

        data = list(map(int, data))

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_WRITE_FIRMWARE, (data,), '64B', 9, 'B')

    def set_status_led_config(self, config):
        """
        Sets the status LED configuration. By default the LED shows
        communication traffic between Brick and Bricklet, it flickers once
        for every 10 received data packets.

        You can also turn the LED permanently on/off or show a heartbeat.

        If the Bricklet is in bootloader mode, the LED is will show heartbeat by default.
        """
        self.check_validity()

        config = int(config)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_SET_STATUS_LED_CONFIG, (config,), 'B', 0, '')

    def get_status_led_config(self):
        """
        Returns the configuration as set by :func:`Set Status LED Config`
        """
        self.check_validity()

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_STATUS_LED_CONFIG, (), '', 9, 'B')

    def get_chip_temperature(self):
        """
        Returns the temperature as measured inside the microcontroller. The
        value returned is not the ambient temperature!

        The temperature is only proportional to the real temperature and it has bad
        accuracy. Practically it is only useful as an indicator for
        temperature changes.
        """
        self.check_validity()

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_CHIP_TEMPERATURE, (), '', 10, 'h')

    def reset(self):
        """
        Calling this function will reset the Bricklet. All configurations
        will be lost.

        After a reset you have to create new device objects,
        calling functions on the existing ones will result in
        undefined behavior!
        """
        self.check_validity()

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_RESET, (), '', 0, '')

    def write_uid(self, uid):
        """
        Writes a new UID into flash. If you want to set a new UID
        you have to decode the Base58 encoded UID string into an
        integer first.

        We recommend that you use Brick Viewer to change the UID.
        """
        self.check_validity()

        uid = int(uid)

        self.ipcon.send_request(self, BrickletARINC429.FUNCTION_WRITE_UID, (uid,), 'I', 0, '')

    def read_uid(self):
        """
        Returns the current UID as an integer. Encode as
        Base58 to get the usual string version.
        """
        self.check_validity()

        return self.ipcon.send_request(self, BrickletARINC429.FUNCTION_READ_UID, (), '', 12, 'I')

    def get_identity(self):
        """
        Returns the UID, the UID where the Bricklet is connected to,
        the position, the hardware and firmware version as well as the
        device identifier.

        The position can be 'a', 'b', 'c', 'd', 'e', 'f', 'g' or 'h' (Bricklet Port).
        A Bricklet connected to an :ref:`Isolator Bricklet <isolator_bricklet>` is always as
        position 'z'.

        The device identifier numbers can be found :ref:`here <device_identifier>`.
        |device_identifier_constant|
        """
        return GetIdentity(*self.ipcon.send_request(self, BrickletARINC429.FUNCTION_GET_IDENTITY, (), '', 33, '8s 8s c 3B 3B H'))

    def debug_read_register(self, op_code):
        """
        Debug function to execute a direct SPI read access on the A429 chip.
        """
        op_code = int(op_code)

        ret = self.debug_read_register_low_level(op_code)

        return DebugReadRegister(ret.value_data[:ret.value_length], ret.rw_error)

    def debug_write_register(self, op_code, value):
        """
        Debug function to execute a direct SPI write access on the A429 chip.
        """
        op_code = int(op_code)
        value = list(map(int, value))

        value_length = len(value)
        value_data = list(value) # make a copy so we can potentially extend it

        if value_length > 32:
            raise Error(Error.INVALID_PARAMETER, 'Value can be at most 32 items long')

        if value_length < 32:
            value_data += [0] * (32 - value_length)

        return self.debug_write_register_low_level(op_code, value_length, value_data)

    def register_callback(self, callback_id, function):
        """
        Registers the given *function* with the given *callback_id*.
        """
        if function is None:
            self.registered_callbacks.pop(callback_id, None)
        else:
            self.registered_callbacks[callback_id] = function

ARINC429 = BrickletARINC429 # for backward compatibility
