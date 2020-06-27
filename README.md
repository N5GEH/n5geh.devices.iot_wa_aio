#  <img src="fig/IoT-AIO.png" width=100 /> IoT-AIO <br/> Analog IO IoT Wireless Adapter

IoT-AIO is a wireless adapter for developing IoT applications for Industrial and Building Automation Systems based on the Espressif chipset ESP32. The adapter is compatible with:
- current loop signals (0/4-20 mA)
- voltage signals (0/2-10 V)
- passive sensors (RTD, Strain Gauge,..)

The library allows the implemention of IoT applications that rely on MQTT protocol via WiFi/NB-IoT/LTE Cat-M1 interfaces.

## Adapter versions
The Iot-AIO wireless adapter is available in two versions developed using [KiCAD 5.1](https://kicad-pcb.org/) and compatible with:
- [ESP32-PICO-KIT v4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-pico-kit.html) - is an ESP32-based mini development board produced by [Espressif](https://espressif.com/). The board features integrated Wifi and BLE capabilites.
- [FiPy](https://pycom.io/product/fipy/) - is an ESP32-based mini development board produced by [Pycom](https://pycom.io/). The board features integrated Wifi, BLE, LoRa, Sigfox, NB-IoT and LTE Cat-M1

## Development Framework
IoT-AIO relies on the Espressif IoT Development Framework [ESP-IDF](https://github.com/espressif/esp-idf), the official development framework for the [ESP32](https://espressif.com/en/products/hardware/esp32/overview) chip.

<img src="fig/AIO_configurator.png" height=300 width=72 />

The adapter can be easily configured at run-time via a web interface based on HTML forms. The software configurator can be accessed by connecting to the adapter while in AP mode and typing in the browser the IP address `192.168.1.4`. After a succesful change in configuration the wireless adapter will reset. Source code and additional documentation can be found here:
- [AIO Configurator](Firmware/doc/aio_configurator.md)
- [HTML Request Page](Firmware/doc/aio_configurator_request.html)
- [HTML Response Page](Firmware/doc/aio_configurator_response.html)
- [Further Developments](Firmware/doc/developments.md)

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
The working mode of the adapter is selected using SW1
- UP: passive mode
    - pin 3 -> V-
    - pin 4 -> V+
- DOWN: analog IO mode
    - pin 3 -> AO
    - pin 4 -> AI

### Input mode
When SW1(DOWN) the input mode of the adapter is selected using SW2
- UP: voltage input mode allows for signals in the range 0 to 10 V
- DOWN: current input mode  allows for signals in the range 0 to 20 mA

### Output mode
When SW1(DOWN) the output mode of the adapter is selected using SW3(1):
- UP: voltage output mode allows for signals in the range 0 to 10 V
- DOWN: current output mode  allows for signals in the range 0 to 20 mA

### Passive mode
When SW1(UP) the output mode of the adapter is selected using SW3(2,3)

| Mode    | SW3(2) | SW3(3) |
|:-------:|:------:|:------:|
| 2-wires |   UP   |   UP   |
| 3-wires |   UP   |  DOWN  |
| 4-wires |  DOWN  |  DOWN  |

### Functions button
The push button BT performs multiple operations depending on the pressure time Tp.

| Pressure Time   |      Action                                                                      |
|:---------------:|:---------------------------------------------------------------------------------|
|     Tp < 1s     | Performs a soft-reset of the adapter                                             |
|1s < Tp < 5s     | Enables the AP mode. If not previously changed, the device will be visible by default with the SSID `ESP_<MAC>`, where <MAC> is the MAC address of the adapter network interface in hex format|
|5s < Tp          | Performs a hard-reset of the adapter deleting the previously stored configuration|

# Publications

We kindly ask academic publications employing based or inspired to IoT-AIO to cite at least one of the following papers:

- M.H. Schraven, C. Guarnieri Calò Carducci, M.A. Baranski, D. Mueller, A. Monti, “Designing a Development Board for Research on IoT Appli-cations in Building Automation Systems,” 36th International Symposiumon  Automation  and  Robotics  in  Construction  (ISARC  2019),  Banff,Canada, 2019, pp. 82-90, [DOI: 10.22260/ISARC2019/0012](https://doi.org/10.22260/ISARC2019/0012)
- C. Guarnieri Calò Carducci, A. Monti, M. H. Schraven, M. Schumacher and D. Mueller, “Enabling ESP32-based IoT Applications in Building Automation Systems,” 2019 II Workshop on Metrology for Industry 4.0 and IoT (MetroInd4.0&IoT), Naples, Italy, 2019, pp. 306-311, [DOI: 10.1109/METROI4.2019.8792852](https://doi.org/10.1109/METROI4.2019.8792852)

# Copyright

2019-2020, Institute for Automation of Complex Power Systems, EONERC  

# License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

# Contact
[![EONERC ACS Logo](fig/eonerc_logo.png)](http://www.acs.eonerc.rwth-aachen.de)

- [Carlo Guarnieri Calò Carducci (PhD)](mailto:cguarnieri@eonerc.rwth-aachen.de)

[Institute for Automation of Complex Power Systems (ACS)](http://www.acs.eonerc.rwth-aachen.de)  
[EON Energy Research Center (EONERC)](http://www.eonerc.rwth-aachen.de)  
[RWTH University Aachen, Germany](http://www.rwth-aachen.de)  
