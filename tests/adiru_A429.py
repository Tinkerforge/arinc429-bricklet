# -*- coding: utf-8 -*-


# import A429 tools
import a429tools

# import math library
import math


##############################################################################
#                                                                            #
#  ADIRU Label Definitions (SA / LR)                                         #
#                                                                            #
##############################################################################

# shortcuts for the SDI values
sdi3 = a429tools.SDI_3
data = a429tools.SDI_DATA

# shortcuts for the frame format
bnr  = a429tools.FRAME_FORMAT_BNR
bcd  = a429tools.FRAME_FORMAT_BCD
disc = a429tools.FRAME_FORMAT_DISCRETE

# init values position
lat     = 53.53850
lon     =  9.85823

# init value altitude
alt     =  60

# init values direction
track   =  10
heading = 150

# init values speed
speed   = 200
speedNS = speed * math.cos(math.radians(heading))
speedEW = speed * math.sin(math.radians(heading))

# init values wind
wind_d  =  24
wind_s  =  30

# init values data
eq_id   = 168
disc_w  =  12


# create dictionary for the frames
irs_frames = dict()

# add all the label definitions to the dictionary
#                    alias                                    refresh   SDI   frame     default        LSB   size    min.       max.     num.
#                    name                                       rate   value  format   eng.value    position        engineering value  decimals
#                     [0]                                        [1]    [2]     [3]     [4]  [5]       [6]    [7]     [8]        [9]     [10]

irs_frames[0o076] = ['ADIRU_076_GPS_Altitude_3',                 1000,  data,   bnr,     alt, 'ft',      9,    20,      0,   +131072,      0]
irs_frames[0o103] = ['ADIRU_103_GPS_Track_Angle_True_3',         1000,  sdi3,   bnr,   track, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o110] = ['ADIRU_110_GPS_Pres_Pos_Latitude_3',        1000,  data,   bnr,     lat, '°',       9,    20,    -90,       +90,      5]
irs_frames[0o111] = ['ADIRU_111_GPS_Pres_Pos_Longitude_3',       1000,  data,   bnr,     lon, '°',       9,    20,   -180,      +180,      5]
irs_frames[0o112] = ['ADIRU_112_GPS_Ground_Speed_3',             1000,  sdi3,   bnr,   speed, 'kt',     14,    15,      0,     +4096,      0]
irs_frames[0o120] = ['ADIRU_120_GPS_Pres_Pos_Latitude_Fine_3',   1000,  sdi3,   bnr,       0, '°',      18,    11,      0, +0.000172,      6]
irs_frames[0o121] = ['ADIRU_121_GPS_Pres_Pos_Longitude_Fine_3',  1000,  sdi3,   bnr,       0, '°',      18,    11,      0, +0.000172,      6]
irs_frames[0o125] = ['ADIRU_125_GPS_UTC_TIME_3',                 1000,  sdi3,   bcd,       0, 'H:M:S',  11,    19,      0,     2**19,      0]
irs_frames[0o260] = ['ADIRU_260_GPS_UTC_DATE_3',                 1000,  sdi3,   bcd,       0, 'D:M:Y',  11,    19,      0,     2**19,      0]
irs_frames[0o264] = ['ADIRU_264_Hybrid_HOR_Fig_Of_Merit_3',      1000,  sdi3,   bnr,       0, 'NM',     11,    18,      0,       +16,      2]
irs_frames[0o377] = ['ADIRU_377_Equipment_Identification_IR_3',  1000,  sdi3,   bcd,   eq_id, '',       11,    12,      0,     2**12,      0]

irs_frames[0o150] = ['ADIRU_150_GPS_UTC_3',                       500,  sdi3,   bnr,       0, 'UTC',    11,    18,      0,     2**18,      0]
irs_frames[0o270] = ['ADIRU_270_Discrete_Word_1_3',               500,  sdi3,   disc, disc_w, '',       11,    19,      0,     2**19,      0]

irs_frames[0o012] = ['ADIRU_012_ground_speed_3',                  250,  sdi3,   bcd,   speed, 'kt',     15,    15,      0,     +4095,      0]
irs_frames[0o013] = ['ADIRU_013_track_angle_true_3',              250,  sdi3,   bcd,track*10, '1/10 °', 15,    14,      0,     +3599,      0]
irs_frames[0o015] = ['ADIRU_015_Wind_Speed_3',                    250,  sdi3,   bcd,  wind_s, 'kt',     19,    10,      0,      +255,      0]
irs_frames[0o016] = ['ADIRU_016_Wind_direction_true_3',           250,  sdi3,   bcd,  wind_d, '°',      19,    10,      0,      +359,      0]

irs_frames[0o310] = ['ADIRU_310_Present_Position_Latitude_3',     200,  data,   bnr,     lat, '°',       9,    20,    -90,       +90,      5]
irs_frames[0o311] = ['ADIRU_311_Present_Position_Longitudinal_3', 200,  data,   bnr,     lon, '°',       9,    20,   -180,      +180,      5]

irs_frames[0o315] = ['ADIRU_315_Wind_Speed_3',                    100,  sdi3,   bnr,  wind_s, 'kt',     14,    15,      0,      +256,      0]
irs_frames[0o316] = ['ADIRU_316_Wind_Direction_True_3',           100,  sdi3,   bnr,  wind_d, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o366] = ['ADIRU_366_N_S_Velocity_3',                  100,  sdi3,   bnr, speedNS, 'kt',     14,    15,  -4096,     +4096,      0]
irs_frames[0o367] = ['ADIRU_367_E_W_Velocity_3',                  100,  sdi3,   bnr, speedEW, 'kt',     14,    15,  -4096,     +4096,      0]
irs_frames[0o254] = ['ADIRU_254_Hybrid_Pres_Pos_Latitude_3',      100,  data,   bnr,     lat, '°',       9,    20,    -90,       +90,      5]
irs_frames[0o255] = ['ADIRU_255_Hybrid_Pres_Pos_Longitude_3',     100,  data,   bnr,     lon, '°',       9,    20,   -180,      +180,      5]

irs_frames[0o132] = ['ADIRU_132_Hybrid_True_Heading_3',            40,  sdi3,   bnr, heading, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o175] = ['ADIRU_175_Hybrid_Ground_Speed_3',            40,  sdi3,   bnr,   speed, 'kt',     14,    15,      0,     +4096,      0]
irs_frames[0o261] = ['ADIRU_261_Hybrid_Altitude_3',                40,  data,   bnr,     alt, 'ft',      9,    20,      0,   +131072,      0]
irs_frames[0o312] = ['ADIRU_312_Ground_Speed_3',                   40,  sdi3,   bnr,   speed, 'kt',     14,    15,      0,     +4096,      0]
irs_frames[0o313] = ['ADIRU_313_Track_Angle_True_3',               40,  sdi3,   bnr,   track, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o314] = ['ADIRU_314_True_Heading_3',                   40,  sdi3,   bnr, heading, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o320] = ['ADIRU_320_Magnetic_Heading_3',               40,  sdi3,   bnr, heading, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o321] = ['ADIRU_321_Drift_Angle_3',                    40,  sdi3,   bnr,       0, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o322] = ['ADIRU_322_Flight_Path_Angle_3',              40,  sdi3,   bnr,       0, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o361] = ['ADIRU_361_Inertial_Altitude_3',              40,  data,   bnr,     alt, 'ft',      9,    20,      0,   +131072,      0]
irs_frames[0o365] = ['ADIRU_365_Inertial_Vertical_Speed_3',        40,  sdi3,   bnr,       0, 'ft/min', 14,    15, -32768,    +32768,      0]

irs_frames[0o324] = ['ADIRU_324_Pitch_Angle_3',                    20,  sdi3,   bnr,       0, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o325] = ['ADIRU_325_Roll_Angle_3',                     20,  sdi3,   bnr,       0, '°',      14,    15,   -180,      +180,      0]
irs_frames[0o326] = ['ADIRU_326_Body_Axis_Pitch_Rate_3',           20,  sdi3,   bnr,       0, '°/sec',  14,    15,   -128,      +128,      0]
irs_frames[0o327] = ['ADIRU_327_Body_Axis_Roll_Rate_3',            20,  sdi3,   bnr,       0, '°/sec',  14,    15,   -128,      +128,      0]
irs_frames[0o330] = ['ADIRU_330_Body_Axis_Yaw_Rate_3',             20,  sdi3,   bnr,       0, '°/sec',  14,    15,   -128,      +128,      0]
irs_frames[0o052] = ['ADIRU_052_Pitch_Angular_Acceleration_3',     20,  sdi3,   bnr,       0, '°/sec²', 14,    15,    -64,       +64,      0]
irs_frames[0o053] = ['ADIRU_053_Roll_Angular_Acceleration_3',      20,  sdi3,   bnr,       0, '°/sec²', 14,    15,    -64,       +64,      0]
irs_frames[0o054] = ['ADIRU_054_Yaw_Angular_Acceleration_3',       20,  sdi3,   bnr,       0, '°/sec²', 14,    15,    -64,       +64,      0]
irs_frames[0o331] = ['ADIRU_331_Body_Longitude_Acceleration_3',    20,  sdi3,   bnr,       0, 'g',      14,    15,     -4,        +4,      2]
irs_frames[0o332] = ['ADIRU_332_Body_Latitude_Acceleration_3',     20,  sdi3,   bnr,       0, 'g',      14,    15,     -4,        +4,      2]
irs_frames[0o333] = ['ADIRU_333_Body_Normal_Acceleration_3',       20,  sdi3,   bnr,       0, 'g',      14,    15,     -4,        +4,      2]
irs_frames[0o364] = ['ADIRU_364_Vertical_Accelaration_3',          20,  sdi3,   bnr,       0, 'g',      14,    15,     -4,        +4,      2]

# =====================================================================================
