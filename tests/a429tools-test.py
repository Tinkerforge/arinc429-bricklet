#!/usr/bin/python3
# -*- coding: utf-8 -*- 

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# A429 Tools - Tests
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# import the A429 tools
import a429tools


if __name__ == "__main__" :

    print('\n### raw Frame ###')

    # create a frame with label 123 (in octal) and SDI 1
    my_frame = a429tools.createFrame(0o123, 1)
    print('frame =', hex(my_frame))

    # extract the label
    my_label = a429tools.readLabel(my_frame)
    print('Label =', oct(my_label))


    print('\n### BCD ###')

    # set a positive value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BCD,    15,    14,   -7999,   +7999,   +1234,  a429tools.SSM_NO)
    print('BCD positive: frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,                  LSB pos, size, eng.min, eng.max, decimals
    my_value, my_ssm = a429tools.getValue(my_frame, a429tools.FRAME_FORMAT_BCD,   15,    14,   -7999,   +7999,     2)
    print('BCD positive: value =', my_value, '/', a429tools.ssm2text(my_ssm))


    # set a negative value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BCD,    15,    14,   -7999,   +7999,   -1234,  a429tools.SSM_NO)
    print('BCD negative: frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,                  LSB pos, size, eng.min, eng.max, decimals
    my_value, my_ssm = a429tools.getValue(my_frame, a429tools.FRAME_FORMAT_BCD,   15,    14,   -7999,   +7999,     2)
    print('BCD negative: value =', my_value, '/', a429tools.ssm2text(my_ssm))


    print('\n### BNR ###')

    # set a positive value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BNR,    11,    18,  -2000,   +2000, +1234.5678,  a429tools.SSM_NO)
    print('BNR positive: frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,                  LSB pos, size, eng.min, eng.max, decimals
    my_value, my_ssm = a429tools.getValue(my_frame, a429tools.FRAME_FORMAT_BNR,   11,    18,  -2000,   +2000,     2)
    print('BNR positive: value =', my_value, '/', a429tools.ssm2text(my_ssm))


    # set a negative value
    #                              frame,       frame format,           LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_BNR,    11,    18,  -2000,   +2000,  -1234.5678,  a429tools.SSM_NO)
    print('BNR negative: frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,                  LSB pos, size, eng.min, eng.max, decimals
    my_value, my_ssm = a429tools.getValue(my_frame, a429tools.FRAME_FORMAT_BNR,   11,    18,  -2000,   +2000,     2)
    print('BNR negative: value =', my_value, '/', a429tools.ssm2text(my_ssm))


    print('\n### Discrete ###')

    # set a value
    #                              frame,       frame format,               LSB pos, size, eng.min, eng.max, eng.value,  SSM value
    my_frame = a429tools.setValue(my_frame, a429tools.FRAME_FORMAT_DISCRETE,   11,    18,     0,     2**19,    1234,  a429tools.SSM_NO)
    print('Discrete: frame =', hex(my_frame))

    # read-back the value
    #                               frame,       frame format,                      LSB pos, size, eng.min, eng.max, decimals
    my_value, my_ssm = a429tools.getValue(my_frame, a429tools.FRAME_FORMAT_DISCRETE,  11,    18,     0,     2**19,     2) 
    print('Discrete: value =', my_value, '/', a429tools.ssm2text(my_ssm))

# ----------------------------------------------------------------------------

