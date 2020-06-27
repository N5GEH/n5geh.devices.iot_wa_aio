#  <img src="fig/IoT-AIO.png" width=100 /> IoT-AIO <br/> Analog IO IoT Wireless Adapter

IoT-AIO is a wireless adapter for developing IoT applications for Industrial and Building Automation Systems based on the Espressif chipset ESP32. The adapter is compatible with:
- current loop signals (0/4-20 mA)
- voltage signals (0/2-10 V)
- passive sensors (RTD, Strain Gauge,..)

The adapter allows the implemention of IoT applications that rely on MQTT protocol via WiFi/NB-IoT/LTE Cat-M1 connection.

## Adapter versions
The Iot-AIO wireless adapter is available in two versions compatible with:
- [ESP32-PICO-KIT v4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-pico-kit.html) - is an ESP32-based mini development board produced by [Espressif](https://espressif.com/). The board features integrated Wifi and BLE capabilites.
- [FiPy](https://pycom.io/product/fipy/) - is an ESP32-based mini development board produced by [Pycom](https://pycom.io/). The board features integrated Wifi, BLE, LoRa, Sigfox, NB-IoT and LTE Cat-M1

## Development Framework
IoT-AIO relies on the Espressif IoT Development Framework [ESP-IDF](https://github.com/espressif/esp-idf), the official development framework for the [ESP32](https://espressif.com/en/products/hardware/esp32/overview) chip.

## Configuration
![IoT-AIO Mode](fig/IoT-AIO_mode.png)
### Pinout
IO pinout (top-down):
- 0: GND
- 1: I- amperometric negative pin
- 2: I+ amperometric positive pin
- 3: AO/V- analog output/voltmetric negative pin
- 4: AI/V+ analog output/voltmetric positive pin
- 5: 12 V power supply (use input resistor 120 Ω / 1.5 W for 24 V input)

### Mode
The working mode of the adapter is selected using SW1:
- UP: passive mode
    - pin 3 -> V-
    - pin 4 -> V+
- DOWN: analog IO mode
    - pin 3 -> AO
    - pin 4 -> AI

### Input mode
When SW1(DOWN) the input mode of the adapter is selected using SW2:
- UP: voltage input mode allows for signals in the range 0 to 10 V
- DOWN: current input mode  allows for signals in the range 0 to 20 mA

### Output mode
When SW1(DOWN) the output mode of the adapter is selected using SW3(1):
- UP: voltage output mode allows for signals in the range 0 to 10 V
- DOWN: current output mode  allows for signals in the range 0 to 20 mA

### Passive mode
When SW1(UP) the output mode of the adapter is selected using SW3(2,3):
- 2-wire: SW3(2)(UP), SW3(3)(UP)
- 3-wire: SW3(2)(UP), SW3(3)(DOWN)
- 4-wire: SW3(2)(DOWN), SW3(3)(DOWN)

## Publications

We kindly ask academic publications employing based or inspired to IoT-AIO to cite at least one of the following papers:

- M.H. Schraven, C. Guarnieri Calò Carducci, M.A. Baranski, D. Mueller, A. Monti, “Designing a Development Board for Research on IoT Appli-cations in Building Automation Systems,” 36th International Symposiumon  Automation  and  Robotics  in  Construction  (ISARC  2019),  Banff,Canada, 2019, pp. 82-90, [DOI: 10.22260/ISARC2019/0012](https://doi.org/10.22260/ISARC2019/0012)
- C. Guarnieri Calò Carducci, A. Monti, M. H. Schraven, M. Schumacher and D. Mueller, “Enabling ESP32-based IoT Applications in Building Automation Systems,” 2019 II Workshop on Metrology for Industry 4.0 and IoT (MetroInd4.0&IoT), Naples, Italy, 2019, pp. 306-311, [DOI: 10.1109/METROI4.2019.8792852](https://doi.org/10.1109/METROI4.2019.8792852)

## Copyright

2019-2020, Institute for Automation of Complex Power Systems, EONERC  

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

# Contact
[![EONERC ACS Logo](fig/eonerc_logo.png)](http://www.acs.eonerc.rwth-aachen.de)

- [Dr. Carlo Guarnieri Calò Carducci](mailto:cguarnieri@eonerc.rwth-aachen.de)

[Institute for Automation of Complex Power Systems (ACS)](http://www.acs.eonerc.rwth-aachen.de)  
[EON Energy Research Center (EONERC)](http://www.eonerc.rwth-aachen.de)  
[RWTH University Aachen, Germany](http://www.rwth-aachen.de)  
