#!/usr/bin/python3
# -*- coding: utf-8 -*- 

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# A429 Tools - Tests
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# import the A429 tools
import a429tools


if __name__ == "__main__" :

    # create a frame with label 123 (in octal) and SDI 1
    my_frame = a429tools.createFrame(0o123, 1)
    print('1 frame =', hex(my_frame))

    # extract the label
    my_label = a429tools.readLabel(my_frame)
    print('2 Label =', oct(my_label))


    ### BCD ###

    # set a value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BCD,    15,    15,     0,      7999,   1234,  a429tools.SSM_NO)
    print('3 frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,          LSB pos, size, eng.min, eng.max
    my_value, my_ssm = a429tools.readValue(my_frame, a429tools.FRAME_FORMAT_BCD,   15,    15,     0,      7999)
    print('4 value =', my_value, '/', a429tools.ssm2text(my_ssm))


    ### BNR ###

    # set a value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BNR,    11,    18,     0,     2**18,    1234,  a429tools.SSM_NO)
    print('5 frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,          LSB pos, size, eng.min, eng.max
    my_value = a429tools.readValue(my_frame, a429tools.FRAME_FORMAT_BNR,   11,    18,     0,     2**18)
    print('6 value =', my_value, '/', a429tools.ssm2text(my_ssm))


    ### Discrete ###

    # set a value
    #                              frame,       frame format,               LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_DISCRETE,   11,    19,     0,     2**19,    1234,  a429tools.SSM_NO)
    print('7 frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,              LSB pos, size, eng.min, eng.max
    my_value = a429tools.readValue(my_frame, a429tools.FRAME_FORMAT_DISCRETE,  11,    19,     0,     2**19)
    print('8 value =', my_value, '/', a429tools.ssm2text(my_ssm))

