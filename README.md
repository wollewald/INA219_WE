# INA219_WE
An Arduino library for the INA219 current and power sensor module.

It allows to set various ADC modes, gains and bus voltage ranges. Continuous mode, triggered mode, power down and ADC off mode are supported. Default shunt size is 0.1 ohms, but you can change it. 

You can run the library on an ATtiny, I tested it on the ATtiny85. To do so, you need to also install the TinyWireM library from Adafruit. An example sketch is included. 

I attached a list of public functions and a fritzing scheme for the example sketches.

<h2>Important changes with release 1.4.0</h2>
The enumeration names I had chosen could lead to collisions with other libraries. I have therefore added an preceeding "INA219_" to them. Unfortunately, users will have to change existing programs accordingly. The example sketches of the library are updated. 
Furthermore, I have added current-only and bus-voltage-only conversion modes. 
Finally, I have added a non-blocking version of the triggered mode including an example sketch. 

You find further details of the INA219 and my library here: 

https://wolles-elektronikkiste.de/ina219  (German)

https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)

