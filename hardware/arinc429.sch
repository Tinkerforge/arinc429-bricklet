EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Arinc 429 Bricklet"
Date "2020-06-25"
Rev "1.0"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2020, T.Schneidermann <tim@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 550  7700 0    40   ~ 0
Copyright Tinkerforge GmbH 2020.\nThis documentation describes Open Hardware and is licensed under the\nCERN OHL v. 1.1.\nYou may redistribute and modify this documentation under the terms of the\nCERN OHL v.1.1. (http://ohwr.org/cernohl). This documentation is distributed\nWITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF\nMERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A\nPARTICULAR PURPOSE. Please see the CERN OHL v.1.1 for applicable\nconditions\n
$Comp
L tinkerforge:DRILL U2
U 1 1 4C6050A5
P 10650 6150
F 0 "U2" H 10700 6200 60  0001 C CNN
F 1 "DRILL" H 10650 6150 60  0000 C CNN
F 2 "kicad-libraries:DRILL_NP" H 10650 6150 60  0001 C CNN
F 3 "" H 10650 6150 60  0001 C CNN
	1    10650 6150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U3
U 1 1 4C6050A2
P 10650 6350
F 0 "U3" H 10700 6400 60  0001 C CNN
F 1 "DRILL" H 10650 6350 60  0000 C CNN
F 2 "kicad-libraries:DRILL_NP" H 10650 6350 60  0001 C CNN
F 3 "" H 10650 6350 60  0001 C CNN
	1    10650 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U5
U 1 1 4C60509F
P 11000 6350
F 0 "U5" H 11050 6400 60  0001 C CNN
F 1 "DRILL" H 11000 6350 60  0000 C CNN
F 2 "kicad-libraries:DRILL_NP" H 11000 6350 60  0001 C CNN
F 3 "" H 11000 6350 60  0001 C CNN
	1    11000 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U4
U 1 1 4C605099
P 11000 6150
F 0 "U4" H 11050 6200 60  0001 C CNN
F 1 "DRILL" H 11000 6150 60  0000 C CNN
F 2 "kicad-libraries:DRILL_NP" H 11000 6150 60  0001 C CNN
F 3 "" H 11000 6150 60  0001 C CNN
	1    11000 6150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR01
U 1 1 4C5FCFB4
P 2500 750
F 0 "#PWR01" H 2500 850 30  0001 C CNN
F 1 "VCC" H 2500 850 30  0000 C CNN
F 2 "" H 2500 750 60  0001 C CNN
F 3 "" H 2500 750 60  0001 C CNN
	1    2500 750 
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR02
U 1 1 4C5FCF4F
P 950 2150
F 0 "#PWR02" H 950 2150 30  0001 C CNN
F 1 "GND" H 950 2080 30  0001 C CNN
F 2 "" H 950 2150 60  0001 C CNN
F 3 "" H 950 2150 60  0001 C CNN
	1    950  2150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CON-SENSOR2 P1
U 1 1 4C5FCF27
P 950 1600
F 0 "P1" H 1100 2000 60  0000 C CNN
F 1 "CON-SENSOR2" V 1100 1600 60  0000 C CNN
F 2 "kicad-libraries:CON-SENSOR2" H 950 1600 60  0001 C CNN
F 3 "" H 950 1600 60  0001 C CNN
	1    950  1600
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:C C1
U 1 1 54F76B96
P 1550 1000
F 0 "C1" V 1650 1150 50  0000 L CNN
F 1 "10uF" V 1400 1000 50  0000 L CNN
F 2 "kicad-libraries:C0805" H 1550 1000 60  0001 C CNN
F 3 "" H 1550 1000 60  0001 C CNN
	1    1550 1000
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:C C2
U 1 1 54F77AA5
P 2300 1000
F 0 "C2" V 2400 1150 50  0000 L CNN
F 1 "1uF" V 2150 1000 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 2300 1000 60  0001 C CNN
F 3 "" H 2300 1000 60  0001 C CNN
	1    2300 1000
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:GND #PWR03
U 1 1 54F77AEA
P 2300 1450
F 0 "#PWR03" H 2300 1450 30  0001 C CNN
F 1 "GND" H 2300 1380 30  0001 C CNN
F 2 "" H 2300 1450 60  0001 C CNN
F 3 "" H 2300 1450 60  0001 C CNN
	1    2300 1450
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 2 1 5820E01A
P 3900 5950
F 0 "U1" H 3750 6900 60  0000 C CNN
F 1 "XMC1404" H 3900 4950 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4050 6700 60  0001 C CNN
F 3 "" H 4050 6700 60  0000 C CNN
	2    3900 5950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 1 1 5820E0F1
P 6600 5950
F 0 "U1" H 6450 6900 60  0000 C CNN
F 1 "XMC1404" H 6600 4950 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 6750 6700 60  0001 C CNN
F 3 "" H 6750 6700 60  0000 C CNN
	1    6600 5950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 4 1 5820E19E
P 4200 2600
F 0 "U1" H 4050 3250 60  0000 C CNN
F 1 "XMC1404" H 4200 1600 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4350 3350 60  0001 C CNN
F 3 "" H 4350 3350 60  0000 C CNN
	4    4200 2600
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 3 1 5820E1ED
P 4150 1200
F 0 "U1" H 4050 1700 60  0000 C CNN
F 1 "XMC1404" H 4150 700 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4300 1950 60  0001 C CNN
F 3 "" H 4300 1950 60  0000 C CNN
	3    4150 1200
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C3
U 1 1 5820F9DC
P 5800 5550
F 0 "C3" V 5850 5300 50  0000 L CNN
F 1 "100nF" V 5650 5450 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 5800 5550 60  0001 C CNN
F 3 "" H 5800 5550 60  0001 C CNN
	1    5800 5550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C5
U 1 1 5820FDE6
P 6150 5550
F 0 "C5" V 6200 5300 50  0000 L CNN
F 1 "100nF" V 6000 5450 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 6150 5550 60  0001 C CNN
F 3 "" H 6150 5550 60  0001 C CNN
	1    6150 5550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C6
U 1 1 5821039E
P 6150 6500
F 0 "C6" V 6200 6250 50  0000 L CNN
F 1 "220nF" V 6000 6400 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 6150 6500 60  0001 C CNN
F 3 "" H 6150 6500 60  0001 C CNN
	1    6150 6500
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C4
U 1 1 582104B4
P 5800 6500
F 0 "C4" V 5850 6250 50  0000 L CNN
F 1 "100nF" V 5650 6400 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 5800 6500 60  0001 C CNN
F 3 "" H 5800 6500 60  0001 C CNN
	1    5800 6500
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR04
U 1 1 5821096B
P 5800 5150
F 0 "#PWR04" H 5800 5250 30  0001 C CNN
F 1 "VCC" H 5800 5250 30  0000 C CNN
F 2 "" H 5800 5150 60  0001 C CNN
F 3 "" H 5800 5150 60  0001 C CNN
	1    5800 5150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR05
U 1 1 58210A4E
P 5800 6100
F 0 "#PWR05" H 5800 6200 30  0001 C CNN
F 1 "VCC" H 5800 6200 30  0000 C CNN
F 2 "" H 5800 6100 60  0001 C CNN
F 3 "" H 5800 6100 60  0001 C CNN
	1    5800 6100
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR06
U 1 1 58210B67
P 5800 5850
F 0 "#PWR06" H 5800 5850 30  0001 C CNN
F 1 "GND" H 5800 5780 30  0001 C CNN
F 2 "" H 5800 5850 60  0001 C CNN
F 3 "" H 5800 5850 60  0001 C CNN
	1    5800 5850
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR07
U 1 1 58210C80
P 5800 6800
F 0 "#PWR07" H 5800 6800 30  0001 C CNN
F 1 "GND" H 5800 6730 30  0001 C CNN
F 2 "" H 5800 6800 60  0001 C CNN
F 3 "" H 5800 6800 60  0001 C CNN
	1    5800 6800
	1    0    0    -1  
$EndComp
Text GLabel 2250 1600 2    60   Output ~ 0
S-CS
Text GLabel 2250 1700 2    60   Output ~ 0
S-CLK
Text GLabel 2250 1800 2    60   Output ~ 0
S-MOSI
Text GLabel 2250 1900 2    60   Input ~ 0
S-MISO
Text GLabel 3450 1000 0    47   Input ~ 0
S-CS
Text GLabel 3450 1200 0    47   Input ~ 0
S-CLK
Text GLabel 3450 1100 0    47   Input ~ 0
S-MOSI
Text GLabel 3450 1500 0    47   Output ~ 0
S-MISO
$Comp
L tinkerforge:LED D3
U 1 1 5823347E
P 900 4150
F 0 "D3" H 900 4250 50  0000 C CNN
F 1 "blue" H 900 4050 50  0000 C CNN
F 2 "kicad-libraries:D0603E" H 900 4150 50  0001 C CNN
F 3 "" H 900 4150 50  0000 C CNN
	1    900  4150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CONN_01X02 P2
U 1 1 58233528
P 2750 4600
F 0 "P2" H 2750 4750 50  0000 C CNN
F 1 "BOOT" V 2850 4600 50  0000 C CNN
F 2 "kicad-libraries:SolderJumper" H 2750 4500 50  0001 C CNN
F 3 "" H 2750 4500 50  0000 C CNN
	1    2750 4600
	-1   0    0    1   
$EndComp
Text Notes 4500 1350 0    39   ~ 0
SPI Slave\nP1.1 : USIC0_CH1-DX2E : SEL\nP1.2 : USIC0_CH1-DX0B : MOSI\nP1.3 : USIC0_CH1-DX1A : CLK\nP1.6 : USIC0_CH1-DOUT0 : MISO
$Comp
L tinkerforge:C C8
U 1 1 58274693
P 2100 5950
F 0 "C8" V 2050 5800 50  0000 L CNN
F 1 "10pF" V 2250 5900 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 2100 5950 60  0001 C CNN
F 3 "" H 2100 5950 60  0001 C CNN
	1    2100 5950
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:C C9
U 1 1 582748D4
P 2100 6550
F 0 "C9" V 1950 6500 50  0000 L CNN
F 1 "10pF" V 2250 6500 50  0000 L CNN
F 2 "kicad-libraries:C0603F" H 2100 6550 60  0001 C CNN
F 3 "" H 2100 6550 60  0001 C CNN
	1    2100 6550
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:GND #PWR09
U 1 1 58275257
P 1900 6700
F 0 "#PWR09" H 1900 6700 30  0001 C CNN
F 1 "GND" H 1900 6630 30  0001 C CNN
F 2 "" H 1900 6700 60  0001 C CNN
F 3 "" H 1900 6700 60  0001 C CNN
	1    1900 6700
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CRYSTAL_3225 X1
U 1 1 58276FBA
P 2650 6250
F 0 "X1" V 2400 6050 60  0000 C CNN
F 1 "16MHz" V 2650 6500 60  0000 C CNN
F 2 "kicad-libraries:CRYSTAL_3225" H 2650 6250 60  0001 C CNN
F 3 "" H 2650 6250 60  0000 C CNN
	1    2650 6250
	0    -1   -1   0   
$EndComp
$Comp
L tinkerforge:GND #PWR010
U 1 1 5827815F
P 1900 6100
F 0 "#PWR010" H 1900 6100 30  0001 C CNN
F 1 "GND" H 1900 6030 30  0001 C CNN
F 2 "" H 1900 6100 60  0001 C CNN
F 3 "" H 1900 6100 60  0001 C CNN
	1    1900 6100
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR011
U 1 1 58278535
P 2950 6500
F 0 "#PWR011" H 2950 6500 30  0001 C CNN
F 1 "GND" H 2950 6430 30  0001 C CNN
F 2 "" H 2950 6500 60  0001 C CNN
F 3 "" H 2950 6500 60  0001 C CNN
	1    2950 6500
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR012
U 1 1 5828358D
P 3050 4750
F 0 "#PWR012" H 3050 4750 30  0001 C CNN
F 1 "GND" H 3050 4680 30  0001 C CNN
F 2 "" H 3050 4750 60  0001 C CNN
F 3 "" H 3050 4750 60  0001 C CNN
	1    3050 4750
	1    0    0    -1  
$EndComp
Text Notes 2350 7050 0    39   ~ 0
TSX-3225 16.0000MF18X-AC0\nno C - 1,00017\n3pF - 1,00008\n4.7pF - 1,00005\n9pF - 1,00001\n10pF - 1,00000MHz
NoConn ~ 1300 1300
$Comp
L tinkerforge:XMC1XXX48 U1
U 5 1 5820E256
P 4200 4250
F 0 "U1" H 4050 4700 60  0000 C CNN
F 1 "XMC1404" H 4200 3650 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4350 5000 60  0001 C CNN
F 3 "" H 4350 5000 60  0000 C CNN
	5    4200 4250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R_PACK4 RP1
U 1 1 5A0AE535
P 1800 1950
F 0 "RP1" H 1800 2400 50  0000 C CNN
F 1 "82" H 1800 1900 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 1800 1950 50  0001 C CNN
F 3 "" H 1800 1950 50  0000 C CNN
	1    1800 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  2150 950  2050
Wire Wire Line
	2300 1400 1550 1400
Wire Wire Line
	1400 750  1550 750 
Wire Wire Line
	1400 1500 1400 750 
Wire Wire Line
	2300 1200 2300 1400
Wire Wire Line
	1550 750  1550 800 
Wire Wire Line
	2300 800  2300 750 
Connection ~ 2300 750 
Connection ~ 1550 750 
Wire Wire Line
	1550 1400 1550 1200
Connection ~ 1550 1400
Wire Wire Line
	6250 5800 6150 5800
Wire Wire Line
	5800 5750 5800 5800
Wire Wire Line
	6250 5200 5800 5200
Wire Wire Line
	5800 5150 5800 5200
Wire Wire Line
	6150 5750 6150 5800
Connection ~ 6150 5800
Wire Wire Line
	5800 5300 6150 5300
Wire Wire Line
	6150 5300 6150 5350
Connection ~ 5800 5300
Connection ~ 6150 5300
Wire Wire Line
	6150 5900 6250 5900
Wire Wire Line
	5800 6750 6150 6750
Wire Wire Line
	6150 6750 6150 6700
Wire Wire Line
	5800 6250 6150 6250
Wire Wire Line
	6150 6250 6150 6300
Wire Wire Line
	5800 6700 5800 6750
Connection ~ 6150 6750
Wire Wire Line
	6250 6150 5800 6150
Wire Wire Line
	5800 6100 5800 6150
Connection ~ 5800 6250
Connection ~ 6150 6250
Connection ~ 5800 6750
Connection ~ 5800 5800
Connection ~ 5800 6150
Connection ~ 5800 5200
Connection ~ 2300 1400
Wire Wire Line
	1300 1600 1600 1600
Wire Wire Line
	1300 1700 1600 1700
Wire Wire Line
	1300 1800 1600 1800
Wire Wire Line
	1300 1900 1400 1900
Wire Wire Line
	2850 6550 2850 6300
Wire Wire Line
	2850 6200 3550 6200
Wire Wire Line
	2850 5950 2850 6200
Wire Wire Line
	2850 6300 3550 6300
Wire Wire Line
	2300 5950 2650 5950
Wire Wire Line
	2300 6550 2650 6550
Connection ~ 2650 6550
Connection ~ 2650 5950
Wire Wire Line
	1900 6700 1900 6550
Wire Wire Line
	1900 6100 1900 5950
Wire Wire Line
	2950 6500 2950 6400
Wire Wire Line
	3450 1000 3800 1000
Wire Wire Line
	3450 1100 3800 1100
Wire Wire Line
	3450 1200 3800 1200
Wire Wire Line
	3450 1500 3800 1500
Wire Wire Line
	2950 4650 3050 4650
Wire Wire Line
	1300 1500 1400 1500
Wire Wire Line
	2000 1600 2250 1600
Wire Wire Line
	2250 1700 2000 1700
Wire Wire Line
	2000 1800 2250 1800
Wire Wire Line
	2250 1900 2000 1900
$Comp
L tinkerforge:C C7
U 1 1 5A0AE84A
P 1400 2150
F 0 "C7" H 1450 2250 50  0000 L CNN
F 1 "220pF" H 1450 2050 50  0000 L CNN
F 2 "kicad-libraries:C0402F" H 1400 2150 60  0001 C CNN
F 3 "" H 1400 2150 60  0000 C CNN
	1    1400 2150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR013
U 1 1 5A0AE8CA
P 1400 2400
F 0 "#PWR013" H 1400 2400 30  0001 C CNN
F 1 "GND" H 1400 2330 30  0001 C CNN
F 2 "" H 1400 2400 60  0001 C CNN
F 3 "" H 1400 2400 60  0001 C CNN
	1    1400 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1950 1400 1900
Connection ~ 1400 1900
Wire Wire Line
	1400 2350 1400 2400
NoConn ~ 3550 6500
NoConn ~ 3550 6400
NoConn ~ 3550 6100
NoConn ~ 3850 3950
NoConn ~ 3850 3400
NoConn ~ 3850 3300
NoConn ~ 3800 900 
Wire Wire Line
	3050 4650 3050 4750
$Comp
L tinkerforge:CONN_01X01 P3
U 1 1 5A65BF9D
P 2750 4350
F 0 "P3" H 2750 4250 50  0000 C CNN
F 1 "DEBUG" V 2850 4350 50  0000 C CNN
F 2 "kicad-libraries:DEBUG_PAD" H 2750 4350 50  0001 C CNN
F 3 "" H 2750 4350 50  0000 C CNN
	1    2750 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	2300 750  2500 750 
Wire Wire Line
	1550 750  2300 750 
Wire Wire Line
	1550 1400 1300 1400
Wire Wire Line
	6150 5800 5800 5800
Wire Wire Line
	6150 5800 6150 5900
Wire Wire Line
	5800 5300 5800 5350
Wire Wire Line
	6150 5300 6250 5300
Wire Wire Line
	6150 6750 6250 6750
Wire Wire Line
	5800 6250 5800 6300
Wire Wire Line
	6150 6250 6250 6250
Wire Wire Line
	5800 6750 5800 6800
Wire Wire Line
	5800 5800 5800 5850
Wire Wire Line
	5800 6150 5800 6250
Wire Wire Line
	5800 5200 5800 5300
Wire Wire Line
	2300 1400 2300 1450
Wire Wire Line
	2650 6550 2850 6550
Wire Wire Line
	2650 5950 2850 5950
Wire Wire Line
	1400 1900 1600 1900
$Comp
L tinkerforge:HI-3593 U6
U 1 1 5EF76882
P 8050 3100
F 0 "U6" H 7300 4300 50  0000 C CNN
F 1 "HI-3593PQIF" H 8050 3050 50  0000 C CNN
F 2 "kicad-libraries:PQFP-44_10x10mm" H 7950 3100 50  0001 C CNN
F 3 "" H 7950 3100 50  0001 C CNN
	1    8050 3100
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Rs R4
U 1 1 5EF844EC
P 10400 2350
F 0 "R4" V 10350 2250 31  0000 C CNN
F 1 "40,2k" V 10350 2450 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 2350 60  0001 C CNN
F 3 "" H 10400 2350 60  0000 C CNN
	1    10400 2350
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:CONN_01X08 P4
U 1 1 5EF85D09
P 11050 3150
F 0 "P4" H 11050 2700 50  0000 C CNN
F 1 "IND 8Pol" V 11150 3150 50  0000 C CNN
F 2 "kicad-libraries:OQ_8P" H 11050 3150 50  0001 C CNN
F 3 "" H 11050 3150 50  0000 C CNN
	1    11050 3150
	1    0    0    1   
$EndComp
NoConn ~ 8900 2250
NoConn ~ 8900 2550
NoConn ~ 8900 3000
NoConn ~ 8900 3300
$Comp
L tinkerforge:TVS D1
U 1 1 5EF975A2
P 9000 4500
F 0 "D1" V 8850 4400 40  0000 L CNN
F 1 "SMBJ-6.5CA" H 8800 4600 40  0000 L CNN
F 2 "kicad-libraries:SMB" H 9000 4500 60  0001 C CNN
F 3 "" H 9000 4500 60  0000 C CNN
	1    9000 4500
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D4
U 1 1 5EFC4E5F
P 9500 4500
F 0 "D4" V 9350 4400 40  0000 L CNN
F 1 "SMAJ-6.5CA" H 9300 4600 40  0000 L CNN
F 2 "kicad-libraries:DO-214AC" H 9500 4500 60  0001 C CNN
F 3 "" H 9500 4500 60  0000 C CNN
	1    9500 4500
	0    1    1    0   
$EndComp
Wire Wire Line
	10500 2350 10850 2350
Wire Wire Line
	10850 2350 10850 2800
Wire Wire Line
	10500 2450 10700 2450
Wire Wire Line
	10700 2450 10700 2900
Wire Wire Line
	10700 2900 10850 2900
Wire Wire Line
	10850 3100 10500 3100
Wire Wire Line
	10500 3200 10850 3200
NoConn ~ 8900 3850
NoConn ~ 8900 4150
Wire Wire Line
	10500 3950 10700 3950
Wire Wire Line
	10700 3950 10700 3400
Wire Wire Line
	10700 3400 10850 3400
Wire Wire Line
	10850 3500 10850 4050
Wire Wire Line
	10850 4050 10500 4050
Wire Wire Line
	8900 2350 10250 2350
Wire Wire Line
	8900 2450 10000 2450
Wire Wire Line
	8900 3100 9750 3100
Wire Wire Line
	8900 3200 9500 3200
Wire Wire Line
	8900 3950 9250 3950
Wire Wire Line
	8900 4050 9000 4050
Wire Wire Line
	9000 4250 9000 4050
Connection ~ 9000 4050
Wire Wire Line
	9000 4050 10300 4050
Wire Wire Line
	9250 4250 9250 3950
Connection ~ 9250 3950
Wire Wire Line
	9250 3950 10300 3950
Wire Wire Line
	9500 4250 9500 3200
Connection ~ 9500 3200
Wire Wire Line
	9500 3200 10300 3200
Wire Wire Line
	9750 4250 9750 3100
Connection ~ 9750 3100
Wire Wire Line
	9750 3100 10300 3100
Wire Wire Line
	10000 4250 10000 2450
Connection ~ 10000 2450
Wire Wire Line
	10000 2450 10300 2450
Wire Wire Line
	10250 4250 10250 2350
Connection ~ 10250 2350
Wire Wire Line
	10250 2350 10300 2350
Wire Wire Line
	10850 3000 10550 3000
Wire Wire Line
	10550 3000 10550 3300
Wire Wire Line
	10850 3300 10550 3300
Connection ~ 10550 3300
Wire Wire Line
	9000 4750 9000 5000
Wire Wire Line
	9000 5000 9250 5000
Wire Wire Line
	10550 3300 10550 5000
Wire Wire Line
	10250 4750 10250 5000
Connection ~ 10250 5000
Wire Wire Line
	10250 5000 10550 5000
Wire Wire Line
	10000 4750 10000 5000
Connection ~ 10000 5000
Wire Wire Line
	10000 5000 10250 5000
Wire Wire Line
	9750 4750 9750 5000
Connection ~ 9750 5000
Wire Wire Line
	9750 5000 10000 5000
Wire Wire Line
	9500 4750 9500 5000
Connection ~ 9500 5000
Wire Wire Line
	9500 5000 9750 5000
Wire Wire Line
	9250 4750 9250 5000
Connection ~ 9250 5000
Wire Wire Line
	9250 5000 9500 5000
$Comp
L tinkerforge:GND #PWR018
U 1 1 5F06AE63
P 9750 5100
F 0 "#PWR018" H 9750 5100 30  0001 C CNN
F 1 "GND" H 9750 5030 30  0001 C CNN
F 2 "" H 9750 5100 60  0001 C CNN
F 3 "" H 9750 5100 60  0001 C CNN
	1    9750 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 5100 9750 5000
$Comp
L tinkerforge:Cs C12
U 1 1 5F0771F0
P 7150 1350
F 0 "C12" H 7150 1300 31  0000 L CNN
F 1 "100nF" H 6950 1450 31  0000 L CNN
F 2 "kicad-libraries:C0603F" H 7150 1350 60  0001 C CNN
F 3 "" H 7150 1350 60  0001 C CNN
	1    7150 1350
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:Cs C11
U 1 1 5F07D935
P 6800 1350
F 0 "C11" H 6800 1300 31  0000 L CNN
F 1 "10nF" H 6650 1450 31  0000 L CNN
F 2 "kicad-libraries:C0603F" H 6800 1350 60  0001 C CNN
F 3 "" H 6800 1350 60  0001 C CNN
	1    6800 1350
	-1   0    0    1   
$EndComp
Wire Wire Line
	6800 1450 6800 1600
Wire Wire Line
	7150 1600 7150 1450
Wire Wire Line
	6400 1000 6400 1100
Wire Wire Line
	6400 1100 6450 1100
Wire Wire Line
	6800 1100 6800 1250
Wire Wire Line
	6800 1100 7150 1100
Wire Wire Line
	7150 1100 7150 1250
Connection ~ 6800 1100
Wire Wire Line
	7150 1100 7600 1100
Wire Wire Line
	7600 1100 7600 1750
Wire Wire Line
	7600 1750 7550 1750
Wire Wire Line
	7550 1750 7550 1850
Connection ~ 7150 1100
Wire Wire Line
	7600 1750 7650 1750
Wire Wire Line
	7650 1750 7650 1850
Connection ~ 7600 1750
$Comp
L tinkerforge:GND #PWR015
U 1 1 5F09D36E
P 6800 1600
F 0 "#PWR015" H 6800 1600 30  0001 C CNN
F 1 "GND" H 6800 1530 30  0001 C CNN
F 2 "" H 6800 1600 60  0001 C CNN
F 3 "" H 6800 1600 60  0001 C CNN
	1    6800 1600
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR014
U 1 1 5F0A0897
P 6400 1000
F 0 "#PWR014" H 6400 1100 30  0001 C CNN
F 1 "VCC" H 6400 1100 30  0000 C CNN
F 2 "" H 6400 1000 60  0001 C CNN
F 3 "" H 6400 1000 60  0001 C CNN
	1    6400 1000
	1    0    0    -1  
$EndComp
Text GLabel 5850 3900 0    50   Input ~ 0
M-CS
Text GLabel 5850 4000 0    50   Input ~ 0
M-MOSI
Text GLabel 5850 4100 0    50   Input ~ 0
M-CLK
Text GLabel 3250 5300 0    50   Input ~ 0
M-MISO
$Comp
L tinkerforge:R_PACK4 RP5
U 1 1 5F0B51D4
P 6300 4250
F 0 "RP5" H 6050 4650 50  0000 C CNN
F 1 "100" H 6500 4250 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 6300 4250 50  0001 C CNN
F 3 "" H 6300 4250 50  0000 C CNN
	1    6300 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 3900 6100 3900
Wire Wire Line
	6100 4000 5850 4000
Wire Wire Line
	5850 4100 6100 4100
Wire Wire Line
	6100 4200 5850 4200
Wire Wire Line
	6500 3900 7150 3900
Wire Wire Line
	7150 4000 6500 4000
Wire Wire Line
	6500 4100 7150 4100
Wire Wire Line
	7150 4200 6500 4200
Text GLabel 5800 2300 0    50   Input ~ 0
CLOCK
Text GLabel 5800 2150 0    50   Input ~ 0
RESET
Text GLabel 5800 2450 0    50   Output ~ 0
MB1-1
Text GLabel 5800 2550 0    50   Output ~ 0
MB1-2
Text GLabel 5800 2650 0    50   Output ~ 0
MB1-3
Text GLabel 5800 2750 0    50   Output ~ 0
MB2-1
Text GLabel 5800 2900 0    50   Output ~ 0
MB2-2
Text GLabel 5800 3000 0    50   Output ~ 0
MB2-3
Text GLabel 5800 3100 0    50   Output ~ 0
R2INT
Text GLabel 5800 3200 0    50   Output ~ 0
R2FLAG
Text GLabel 5800 3400 0    50   Output ~ 0
R1INT
Text GLabel 5800 3500 0    50   Output ~ 0
R1FLAG
Text GLabel 5800 3600 0    50   Output ~ 0
TEMPY
Text GLabel 5800 3700 0    50   Output ~ 0
TFULL
Wire Wire Line
	7750 4400 7750 4600
Wire Wire Line
	7750 4600 7850 4600
Wire Wire Line
	7850 4600 7850 4400
Wire Wire Line
	7850 4600 7950 4600
Wire Wire Line
	7950 4600 7950 4400
Connection ~ 7850 4600
$Comp
L tinkerforge:GND #PWR016
U 1 1 5F150720
P 7850 4700
F 0 "#PWR016" H 7850 4700 30  0001 C CNN
F 1 "GND" H 7850 4630 30  0001 C CNN
F 2 "" H 7850 4700 60  0001 C CNN
F 3 "" H 7850 4700 60  0001 C CNN
	1    7850 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 4700 7850 4600
NoConn ~ 7850 1850
NoConn ~ 7950 1850
NoConn ~ 8050 1850
$Comp
L tinkerforge:Cs C13
U 1 1 5F180F16
P 8150 4600
F 0 "C13" H 8050 4650 31  0000 L CNN
F 1 "47uF/10V" V 8250 4500 31  0000 L CNN
F 2 "kicad-libraries:C1206E" H 8150 4600 60  0001 C CNN
F 3 "" H 8150 4600 60  0000 C CNN
F 4 "885012108012" H 8150 4600 50  0001 C CNN "Nr."
	1    8150 4600
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Cs C15
U 1 1 5F182368
P 8450 4600
F 0 "C15" H 8350 4650 31  0000 L CNN
F 1 "47uF/10V" V 8550 4500 31  0000 L CNN
F 2 "kicad-libraries:C1206E" H 8450 4600 60  0001 C CNN
F 3 "" H 8450 4600 60  0000 C CNN
F 4 "885012108012" H 8450 4600 50  0001 C CNN "Nr."
	1    8450 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 4400 8200 4450
Wire Wire Line
	8200 4450 8150 4450
Wire Wire Line
	8150 4450 8150 4500
Wire Wire Line
	8400 4400 8400 4450
Wire Wire Line
	8400 4450 8450 4450
Wire Wire Line
	8450 4450 8450 4500
Wire Wire Line
	8150 4700 8150 4850
Wire Wire Line
	8150 4850 8300 4850
Wire Wire Line
	8450 4850 8450 4700
$Comp
L tinkerforge:GND #PWR017
U 1 1 5F1B8B82
P 8300 4900
F 0 "#PWR017" H 8300 4900 30  0001 C CNN
F 1 "GND" H 8300 4830 30  0001 C CNN
F 2 "" H 8300 4900 60  0001 C CNN
F 3 "" H 8300 4900 60  0001 C CNN
	1    8300 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 4900 8300 4850
Connection ~ 8300 4850
Wire Wire Line
	8300 4850 8450 4850
$Comp
L tinkerforge:R_PACK4 RP4
U 1 1 5F26A743
P 6300 3750
F 0 "RP4" H 6050 4150 50  0000 C CNN
F 1 "100" H 6500 3750 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 6300 3750 50  0001 C CNN
F 3 "" H 6300 3750 50  0000 C CNN
	1    6300 3750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R_PACK4 RP3
U 1 1 5F26B10F
P 6300 3250
F 0 "RP3" H 6050 3650 50  0000 C CNN
F 1 "100" H 6500 3250 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 6300 3250 50  0001 C CNN
F 3 "" H 6300 3250 50  0000 C CNN
	1    6300 3250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R_PACK4 RP2
U 1 1 5F26CC78
P 6300 2800
F 0 "RP2" H 6050 3200 50  0000 C CNN
F 1 "100" H 6500 2800 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 6300 2800 50  0001 C CNN
F 3 "" H 6300 2800 50  0000 C CNN
	1    6300 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 2450 7150 2450
Wire Wire Line
	7150 2550 6500 2550
Wire Wire Line
	7150 2650 6500 2650
Wire Wire Line
	6500 2750 7150 2750
Wire Wire Line
	7150 2850 6850 2850
Wire Wire Line
	6850 2850 6850 2900
Wire Wire Line
	6850 2900 6500 2900
Wire Wire Line
	6500 3000 6900 3000
Wire Wire Line
	6900 3000 6900 2950
Wire Wire Line
	6900 2950 7150 2950
Wire Wire Line
	6500 3100 7150 3100
Wire Wire Line
	7150 3200 6500 3200
Wire Wire Line
	6500 3400 6850 3400
Wire Wire Line
	6850 3400 6850 3300
Wire Wire Line
	6850 3300 7150 3300
Wire Wire Line
	7150 3400 6900 3400
Wire Wire Line
	6900 3400 6900 3500
Wire Wire Line
	6900 3500 6500 3500
Wire Wire Line
	6500 3600 7150 3600
Wire Wire Line
	7150 3700 6500 3700
Wire Wire Line
	6100 3700 5800 3700
Wire Wire Line
	5800 3600 6100 3600
Wire Wire Line
	5800 3400 6100 3400
Wire Wire Line
	5800 3500 6100 3500
Wire Wire Line
	5800 3100 6100 3100
Wire Wire Line
	6100 3200 5800 3200
Wire Wire Line
	5800 2900 6100 2900
Wire Wire Line
	6100 3000 5800 3000
Wire Wire Line
	5800 2450 6100 2450
Wire Wire Line
	6100 2550 5800 2550
Wire Wire Line
	5800 2650 6100 2650
Wire Wire Line
	6100 2750 5800 2750
$Comp
L tinkerforge:Rs R2
U 1 1 5F35F00F
P 6300 2150
F 0 "R2" V 6250 2150 31  0000 C CNN
F 1 "100" V 6300 2150 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 6300 2150 60  0001 C CNN
F 3 "" H 6300 2150 60  0000 C CNN
	1    6300 2150
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R3
U 1 1 5F3609FC
P 6300 2300
F 0 "R3" V 6250 2300 31  0000 C CNN
F 1 "100" V 6300 2300 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 6300 2300 60  0001 C CNN
F 3 "" H 6300 2300 60  0000 C CNN
	1    6300 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	5800 2150 6200 2150
Wire Wire Line
	6200 2300 5800 2300
Wire Wire Line
	6400 2150 7150 2150
Wire Wire Line
	7150 2300 6400 2300
$Comp
L tinkerforge:Rs R5
U 1 1 5EF67449
P 10400 2450
F 0 "R5" V 10350 2350 31  0000 C CNN
F 1 "40,2k" V 10350 2550 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 2450 60  0001 C CNN
F 3 "" H 10400 2450 60  0000 C CNN
	1    10400 2450
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R6
U 1 1 5EF67F8B
P 10400 3100
F 0 "R6" V 10350 3000 31  0000 C CNN
F 1 "40,2k" V 10350 3150 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 3100 60  0001 C CNN
F 3 "" H 10400 3100 60  0000 C CNN
	1    10400 3100
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R7
U 1 1 5EF6923A
P 10400 3200
F 0 "R7" V 10350 3100 31  0000 C CNN
F 1 "40,2k" V 10350 3250 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 3200 60  0001 C CNN
F 3 "" H 10400 3200 60  0000 C CNN
	1    10400 3200
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R8
U 1 1 5EFDF9B6
P 10400 3950
F 0 "R8" V 10350 3850 31  0000 C CNN
F 1 "31,6" V 10350 4000 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 3950 60  0001 C CNN
F 3 "" H 10400 3950 60  0000 C CNN
	1    10400 3950
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R9
U 1 1 5EF6A25F
P 10400 4050
F 0 "R9" V 10350 3950 31  0000 C CNN
F 1 "31,6" V 10350 4100 31  0000 C CNN
F 2 "kicad-libraries:R0805E" H 10400 4050 60  0001 C CNN
F 3 "" H 10400 4050 60  0000 C CNN
	1    10400 4050
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D2
U 1 1 5EF6D7F7
P 9250 4500
F 0 "D2" V 9100 4400 40  0000 L CNN
F 1 "SMBJ-6.5CA" H 9050 4600 40  0000 L CNN
F 2 "kicad-libraries:SMB" H 9250 4500 60  0001 C CNN
F 3 "" H 9250 4500 60  0000 C CNN
	1    9250 4500
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D5
U 1 1 5EF6DC78
P 9750 4500
F 0 "D5" V 9600 4400 40  0000 L CNN
F 1 "SMAJ-6.5CA" H 9550 4600 40  0000 L CNN
F 2 "kicad-libraries:DO-214AC" H 9750 4500 60  0001 C CNN
F 3 "" H 9750 4500 60  0000 C CNN
	1    9750 4500
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D6
U 1 1 5EF6E0A7
P 10000 4500
F 0 "D6" V 9850 4400 40  0000 L CNN
F 1 "SMAJ-6.5CA" H 9800 4600 40  0000 L CNN
F 2 "kicad-libraries:DO-214AC" H 10000 4500 60  0001 C CNN
F 3 "" H 10000 4500 60  0000 C CNN
	1    10000 4500
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D7
U 1 1 5EF6E495
P 10250 4500
F 0 "D7" V 10100 4400 40  0000 L CNN
F 1 "SMAJ-6.5CA" H 10050 4600 40  0000 L CNN
F 2 "kicad-libraries:DO-214AC" H 10250 4500 60  0001 C CNN
F 3 "" H 10250 4500 60  0000 C CNN
	1    10250 4500
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Cs C16
U 1 1 5EF68DE9
P 8650 1350
F 0 "C16" V 8457 1350 31  0000 C CNN
F 1 "470nF/16V" V 8519 1350 31  0000 C CNN
F 2 "kicad-libraries:0603F" H 8650 1350 60  0001 C CNN
F 3 "" H 8650 1350 60  0000 C CNN
	1    8650 1350
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Cs C14
U 1 1 5EF6AD25
P 8300 1350
F 0 "C14" V 8107 1350 31  0000 C CNN
F 1 "2,2uF/16V" V 8169 1350 31  0000 C CNN
F 2 "kicad-libraries:0805E" H 8300 1350 60  0001 C CNN
F 3 "" H 8300 1350 60  0000 C CNN
	1    8300 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	8250 1650 8250 1850
Wire Wire Line
	8400 1850 8400 1350
Wire Wire Line
	8700 1850 8700 1650
Wire Wire Line
	8700 1650 8750 1650
Wire Wire Line
	8550 1350 8550 1850
Wire Wire Line
	8750 1350 8750 1650
Wire Wire Line
	8200 1650 8200 1350
Wire Wire Line
	8200 1650 8250 1650
$Comp
L tinkerforge:CPs C10
U 1 1 5EF7BE71
P 6450 1350
F 0 "C10" H 6350 1400 31  0000 L CNN
F 1 "47uF" H 6500 1250 31  0000 L CNN
F 2 "kicad-libraries:3528-21" H 6450 1350 60  0001 C CNN
F 3 "" H 6450 1350 60  0000 C CNN
	1    6450 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1250 6450 1100
Connection ~ 6450 1100
Wire Wire Line
	6450 1100 6800 1100
Wire Wire Line
	6450 1450 6450 1600
Wire Wire Line
	6450 1600 6800 1600
Connection ~ 6800 1600
Wire Wire Line
	6800 1600 7150 1600
Text GLabel 3250 5900 0    50   Input ~ 0
RESET
Text GLabel 3450 2100 0    50   Output ~ 0
MB1-1
Text GLabel 3450 2200 0    50   Output ~ 0
MB1-2
Text GLabel 3450 2300 0    50   Output ~ 0
MB1-3
Text GLabel 3450 2400 0    50   Output ~ 0
MB2-1
Text GLabel 3450 2500 0    50   Output ~ 0
MB2-2
Text GLabel 3450 2600 0    50   Output ~ 0
MB2-3
Text GLabel 3450 2700 0    50   Output ~ 0
R2INT
Text GLabel 3450 2800 0    50   Output ~ 0
R2FLAG
Text GLabel 3450 2900 0    50   Output ~ 0
R1INT
Text GLabel 3450 3000 0    50   Output ~ 0
R1FLAG
Text GLabel 3450 3100 0    50   Output ~ 0
TEMPY
Text GLabel 3450 3200 0    50   Output ~ 0
TFULL
Text GLabel 3250 5600 0    50   Output ~ 0
M-CS
Text GLabel 3250 5200 0    50   Output ~ 0
M-MOSI
Text GLabel 3250 5500 0    50   Input ~ 0
M-CLK
Wire Wire Line
	3250 5200 3550 5200
Wire Wire Line
	3550 5300 3250 5300
Wire Wire Line
	3250 5500 3550 5500
Wire Wire Line
	3550 5600 3250 5600
Wire Wire Line
	3450 2100 3850 2100
Wire Wire Line
	3850 2200 3450 2200
Wire Wire Line
	3450 2300 3850 2300
Wire Wire Line
	3850 2400 3450 2400
Wire Wire Line
	3850 2500 3450 2500
Wire Wire Line
	3450 2600 3850 2600
Wire Wire Line
	3850 2700 3450 2700
Wire Wire Line
	3450 2800 3850 2800
Wire Wire Line
	3850 2900 3450 2900
Wire Wire Line
	3450 3000 3850 3000
Wire Wire Line
	3850 3100 3450 3100
Wire Wire Line
	3450 3200 3850 3200
Text GLabel 5850 4200 0    50   Output ~ 0
M-MISO
Text Notes 4500 5500 0    39   ~ 0
SPI Master\nP0.3 : USIC1_CH1-SELO0 : CS\nP0.0 : USIC1_CH1-DOUT0 : MOSI\nP0.2 : USIC1_CH1-SCLKOUT : CLK\nP0.1 : USIC1_CH1-DX0B : MISO
$Comp
L tinkerforge:LED D9
U 1 1 5F1AE48D
P 900 3850
F 0 "D9" H 900 3950 50  0000 C CNN
F 1 "blue" H 900 3750 50  0000 C CNN
F 2 "kicad-libraries:D0603E" H 900 3850 50  0001 C CNN
F 3 "" H 900 3850 50  0000 C CNN
	1    900  3850
	1    0    0    -1  
$EndComp
Text Notes 1050 3950 0    50   ~ 0
RX
$Comp
L tinkerforge:LED D8
U 1 1 5F1CED77
P 900 3550
F 0 "D8" H 900 3650 50  0000 C CNN
F 1 "blue" H 900 3450 50  0000 C CNN
F 2 "kicad-libraries:D0603E" H 900 3550 50  0001 C CNN
F 3 "" H 900 3550 50  0000 C CNN
	1    900  3550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR019
U 1 1 5F1CF321
P 700 3400
F 0 "#PWR019" H 700 3500 30  0001 C CNN
F 1 "VCC" H 700 3500 30  0000 C CNN
F 2 "" H 700 3400 60  0001 C CNN
F 3 "" H 700 3400 60  0001 C CNN
	1    700  3400
	1    0    0    -1  
$EndComp
Text Notes 1050 3650 0    50   ~ 0
TX\n
Text Notes 1050 4300 0    50   ~ 0
Status
Wire Wire Line
	700  3400 700  3550
Connection ~ 700  3550
Wire Wire Line
	700  3550 700  3850
Wire Wire Line
	700  3850 700  4150
$Comp
L tinkerforge:Rs R1
U 1 1 5F01172A
P 1600 3550
F 0 "R1" V 1550 3450 31  0000 C CNN
F 1 "1k" V 1550 3650 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 1600 3550 60  0001 C CNN
F 3 "" H 1600 3550 60  0000 C CNN
	1    1600 3550
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R10
U 1 1 5F015210
P 1600 3850
F 0 "R10" V 1550 3750 31  0000 C CNN
F 1 "1k" V 1550 3950 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 1600 3850 60  0001 C CNN
F 3 "" H 1600 3850 60  0000 C CNN
	1    1600 3850
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Rs R11
U 1 1 5F0159C2
P 1600 4150
F 0 "R11" V 1550 4050 31  0000 C CNN
F 1 "1k" V 1550 4250 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 1600 4150 60  0001 C CNN
F 3 "" H 1600 4150 60  0000 C CNN
	1    1600 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	1100 3550 1500 3550
Wire Wire Line
	1100 3850 1500 3850
Wire Wire Line
	3250 5900 3550 5900
Wire Wire Line
	1100 4150 1500 4150
Text GLabel 1700 3550 2    50   Input ~ 0
TX_LED
Text GLabel 1700 3850 2    50   Input ~ 0
RX_LED
Text GLabel 3250 5800 0    50   Input ~ 0
TX_LED
Text GLabel 3250 6000 0    50   Input ~ 0
RX_LED
Wire Wire Line
	3250 5700 3550 5700
Wire Wire Line
	3550 5800 3250 5800
Connection ~ 700  3850
Text GLabel 1700 4150 2    50   Input ~ 0
Status_LED
Text GLabel 3600 4650 0    50   Input ~ 0
Status_LED
Wire Wire Line
	2950 4550 3850 4550
Wire Wire Line
	2950 4350 3850 4350
Wire Wire Line
	3850 4650 3600 4650
Text GLabel 3250 5700 0    50   Input ~ 0
CLOCK
Wire Wire Line
	3250 6000 3550 6000
NoConn ~ 3850 4450
NoConn ~ 3800 1300
NoConn ~ 3800 1400
NoConn ~ 3550 6600
NoConn ~ 3550 6700
NoConn ~ 3550 5400
$EndSCHEMATC
