EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
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
L Regulator_Linear:LM341T-05_TO220 U8
U 1 1 5D2779E2
P 5950 3550
F 0 "U8" H 5950 3883 50  0000 C CNN
F 1 "LM340MP-5.0/NOPB" H 5950 3792 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 6000 3050 50  0000 C CIN
F 3 "http://www.ti.com/lit/ds/symlink/lm340.pdf" H 5950 3500 50  0001 C CNN
	1    5950 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR045
U 1 1 5D27A826
P 5950 4150
F 0 "#PWR045" H 5950 3900 50  0001 C CNN
F 1 "GND" H 5955 3977 50  0000 C CNN
F 2 "" H 5950 4150 50  0001 C CNN
F 3 "" H 5950 4150 50  0001 C CNN
	1    5950 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C12
U 1 1 5D27B48B
P 6550 3800
F 0 "C12" H 6665 3846 50  0000 L CNN
F 1 "100n" H 6665 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6588 3650 50  0001 C CNN
F 3 "~" H 6550 3800 50  0001 C CNN
	1    6550 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C11
U 1 1 5D27BB86
P 5400 3800
F 0 "C11" H 5515 3846 50  0000 L CNN
F 1 "220n" H 5515 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5438 3650 50  0001 C CNN
F 3 "~" H 5400 3800 50  0001 C CNN
	1    5400 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3550 5400 3550
Wire Wire Line
	5400 3550 5400 3500
Wire Wire Line
	5400 3650 5400 3550
Connection ~ 5400 3550
Wire Wire Line
	5950 4150 5950 3950
Wire Wire Line
	6550 3550 6250 3550
Wire Wire Line
	5400 3950 5950 3950
Connection ~ 5950 3950
Wire Wire Line
	5950 3950 5950 3850
Wire Wire Line
	5950 3950 6550 3950
Text HLabel 5400 3500 1    50   Input ~ 10
24V
Text HLabel 7100 3550 2    50   Output ~ 10
5V
$Comp
L Device:D_Schottky D1
U 1 1 5D39F0A5
P 6800 3550
F 0 "D1" H 6800 3244 50  0000 C CNN
F 1 "D_Schottky" H 6800 3335 50  0000 C CNN
F 2 "Footprint:BAT60A" H 6800 3426 50  0000 C CNN
F 3 "~" H 6800 3550 50  0001 C CNN
	1    6800 3550
	-1   0    0    1   
$EndComp
Wire Wire Line
	6550 3650 6550 3550
Wire Wire Line
	6550 3550 6650 3550
Connection ~ 6550 3550
Wire Wire Line
	6950 3550 7100 3550
$EndSCHEMATC
