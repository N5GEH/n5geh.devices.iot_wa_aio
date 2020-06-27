EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 6
Title "IoT - Wireless Adapter - Analog IO"
Date "2020-06-27"
Rev ""
Comp "RWTH - ACS"
Comment1 "Author: Carlo Guarnieri Calò Carducci"
Comment2 "email: cguarnieri@eonerc.rwth-aachen.de"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R_US R11
U 1 1 5CD12962
P 3150 3500
F 0 "R11" V 2945 3500 50  0000 C CNN
F 1 "10k" V 3036 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3190 3490 50  0001 C CNN
F 3 "~" H 3150 3500 50  0001 C CNN
	1    3150 3500
	0    1    1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 5CD14505
P 2500 4150
F 0 "C3" H 2615 4196 50  0000 L CNN
F 1 "1u" H 2615 4105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2538 4000 50  0001 C CNN
F 3 "~" H 2500 4150 50  0001 C CNN
	1    2500 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR027
U 1 1 5CD16BEC
P 3850 3700
F 0 "#PWR027" H 3850 3450 50  0001 C CNN
F 1 "GND" H 3855 3527 50  0000 C CNN
F 2 "" H 3850 3700 50  0001 C CNN
F 3 "" H 3850 3700 50  0001 C CNN
	1    3850 3700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR025
U 1 1 5CD17059
P 2500 4300
F 0 "#PWR025" H 2500 4050 50  0001 C CNN
F 1 "GND" H 2505 4127 50  0000 C CNN
F 2 "" H 2500 4300 50  0001 C CNN
F 3 "" H 2500 4300 50  0001 C CNN
	1    2500 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 3500 3550 3500
Wire Wire Line
	3000 3500 2500 3500
Wire Wire Line
	2500 3500 2500 4000
Wire Wire Line
	2500 3500 1900 3500
Connection ~ 2500 3500
Wire Wire Line
	3550 4150 4150 4150
$Comp
L Device:C C4
U 1 1 5CD2517D
P 4100 2850
F 0 "C4" H 4215 2896 50  0000 L CNN
F 1 "100n" H 4215 2805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4138 2700 50  0001 C CNN
F 3 "~" H 4100 2850 50  0001 C CNN
	1    4100 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5CD257B8
P 4100 2700
F 0 "#PWR028" H 4100 2450 50  0001 C CNN
F 1 "GND" H 4105 2527 50  0000 C CNN
F 2 "" H 4100 2700 50  0001 C CNN
F 3 "" H 4100 2700 50  0001 C CNN
	1    4100 2700
	-1   0    0    1   
$EndComp
$Comp
L Switch:SW_DPDT_x2 SW4
U 1 1 5CDA438D
P 6700 3350
F 0 "SW4" H 6700 3635 50  0000 C CNN
F 1 "SW_DPDT_x2" H 6700 3544 50  0000 C CNN
F 2 "Footprint:AYZ0202AGRL" H 7900 3150 50  0000 C CNN
F 3 "~" H 6700 3350 50  0001 C CNN
	1    6700 3350
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_DPDT_x2 SW4
U 2 1 5CDAB9F8
P 9100 3350
F 0 "SW4" H 9100 3635 50  0000 C CNN
F 1 "SW_DPDT_x2" H 9000 3550 50  0000 C CNN
F 2 "Footprint:AYZ0202AGRL" H 9100 3350 50  0001 C CNN
F 3 "~" H 9100 3350 50  0001 C CNN
	2    9100 3350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6900 3250 8050 3250
Wire Wire Line
	8350 3250 8900 3250
Wire Wire Line
	6900 3450 7150 3450
Wire Wire Line
	7150 3700 7150 3450
Connection ~ 7150 3450
Wire Wire Line
	7150 3450 8900 3450
Wire Wire Line
	6500 3700 6500 3350
$Comp
L Device:R R19
U 1 1 5CDB10D8
P 6500 3850
F 0 "R19" H 6570 3896 50  0000 L CNN
F 1 "3.3k 0.1%" H 6570 3805 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6430 3850 50  0001 C CNN
F 3 "~" H 6500 3850 50  0001 C CNN
	1    6500 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R20
U 1 1 5CDB181D
P 7150 3850
F 0 "R20" H 7220 3896 50  0000 L CNN
F 1 "45.3 0.1%" H 7220 3805 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7080 3850 50  0001 C CNN
F 3 "~" H 7150 3850 50  0001 C CNN
	1    7150 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R21
U 1 1 5CDB1E54
P 8200 3250
F 0 "R21" V 7993 3250 50  0000 C CNN
F 1 "33k 0.1%" V 8084 3250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8130 3250 50  0001 C CNN
F 3 "~" H 8200 3250 50  0001 C CNN
	1    8200 3250
	0    1    1    0   
$EndComp
Wire Wire Line
	7150 4600 7150 4000
Wire Wire Line
	6500 4000 6500 4600
Connection ~ 6500 4600
Text Notes 2150 3500 2    50   ~ 10
Vin\n
$Comp
L power:+3V3 #PWR026
U 1 1 5CDCD34F
P 3850 3000
F 0 "#PWR026" H 3850 2850 50  0001 C CNN
F 1 "+3V3" H 3865 3173 50  0000 C CNN
F 2 "" H 3850 3000 50  0001 C CNN
F 3 "" H 3850 3000 50  0001 C CNN
	1    3850 3000
	1    0    0    -1  
$EndComp
Text HLabel 1900 3500 0    50   Output ~ 10
ADC
Text HLabel 5850 3350 2    50   Input ~ 10
Vcal
Wire Wire Line
	6500 4600 7150 4600
$Comp
L power:GNDA #PWR042
U 1 1 5CE1846D
P 6500 4600
F 0 "#PWR042" H 6500 4350 50  0001 C CNN
F 1 "GNDA" H 6505 4427 50  0000 C CNN
F 2 "" H 6500 4600 50  0001 C CNN
F 3 "" H 6500 4600 50  0001 C CNN
	1    6500 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 3350 9800 3350
Text HLabel 9800 3350 2    50   Input ~ 10
AI
Wire Wire Line
	6200 4600 6500 4600
Text HLabel 6050 3900 3    50   Input ~ 10
AOCAL
Wire Wire Line
	6200 4600 6200 3500
Wire Wire Line
	6050 3900 6050 3650
Text HLabel 4950 4300 3    50   Input ~ 10
A1
Text HLabel 5050 4300 3    50   Input ~ 10
A0
Wire Wire Line
	5150 4300 5150 4150
Wire Wire Line
	5050 4300 5050 4150
Wire Wire Line
	4950 4300 4950 4150
Wire Wire Line
	5650 3500 6200 3500
Wire Wire Line
	5650 3650 6050 3650
Wire Wire Line
	3850 3000 3850 3100
Wire Wire Line
	3850 3100 4100 3100
Wire Wire Line
	4100 3100 4100 3000
$Comp
L power:GND #PWR029
U 1 1 5D2DC7D9
P 4600 4150
F 0 "#PWR029" H 4600 3900 50  0001 C CNN
F 1 "GND" H 4605 3977 50  0000 C CNN
F 2 "" H 4600 4150 50  0001 C CNN
F 3 "" H 4600 4150 50  0001 C CNN
	1    4600 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C10
U 1 1 5D2DDA15
P 4850 2700
F 0 "C10" H 4965 2746 50  0000 L CNN
F 1 "100n" H 4965 2655 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4888 2550 50  0001 C CNN
F 3 "~" H 4850 2700 50  0001 C CNN
	1    4850 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 2900 4850 2900
Wire Wire Line
	4850 2900 4850 2850
Wire Wire Line
	4650 2650 4650 2900
$Comp
L power:GND #PWR041
U 1 1 5D2E1194
P 4850 2550
F 0 "#PWR041" H 4850 2300 50  0001 C CNN
F 1 "GND" H 4855 2377 50  0000 C CNN
F 2 "" H 4850 2550 50  0001 C CNN
F 3 "" H 4850 2550 50  0001 C CNN
	1    4850 2550
	-1   0    0    1   
$EndComp
$Comp
L power:+3V3 #PWR040
U 1 1 5D2E3161
P 4650 2650
F 0 "#PWR040" H 4650 2500 50  0001 C CNN
F 1 "+3V3" H 4665 2823 50  0000 C CNN
F 2 "" H 4650 2650 50  0001 C CNN
F 3 "" H 4650 2650 50  0001 C CNN
	1    4650 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 3400 4350 3400
Wire Wire Line
	3550 3500 3550 4150
Wire Wire Line
	4150 4150 4150 3600
$Comp
L IoT_AIO_libraries:TLV333IDCKR U6
U 1 1 5D38D3D0
P 3850 3500
F 0 "U6" H 3850 3971 50  0000 C CNN
F 1 "TLV333IDCKR" H 3850 3880 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-353_SC-70-5" H 3850 3789 50  0000 C CNN
F 3 "" H 3900 3650 50  0001 C CNN
	1    3850 3500
	-1   0    0    -1  
$EndComp
Connection ~ 3550 3500
Wire Wire Line
	3850 3300 3850 3100
Connection ~ 3850 3100
Wire Wire Line
	6500 3350 6500 3200
Connection ~ 6500 3350
Connection ~ 4650 2900
$Comp
L IoT_AIO_libraries:TMUX1204 U9
U 1 1 5D2D8F06
P 5000 3550
F 0 "U9" H 4783 4381 50  0000 C CNN
F 1 "TMUX1204" H 4783 4290 50  0000 C CNN
F 2 "Package_SO:VSSOP-10_3x3mm_P0.5mm" H 4950 4050 50  0000 C CNN
F 3 "" H 4950 4050 50  0001 C CNN
	1    5000 3550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5650 3350 5850 3350
Wire Wire Line
	5650 3200 6500 3200
Text HLabel 5150 4300 3    50   Input ~ 10
EN
$EndSCHEMATC
