/***************************************************************************
* Example sketch for the INA219_WE library
*
* This sketch shows how to use the INA219 Power Down mode. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ina219 (German)
* https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)
* 
***************************************************************************/
#include <Wire.h>
#include <INA219_WE.h>
#define I2C_ADDRESS 0x40

/* There are several ways to create your INA219 object:
 * INA219_WE ina219 = INA219_WE(); -> uses Wire / I2C Address = 0x40
 * INA219_WE ina219 = INA219_WE(I2C_ADDRESS); -> uses Wire / I2C_ADDRESS
 * INA219_WE ina219 = INA219_WE(&Wire); -> you can pass any TwoWire object
 * INA219_WE ina219 = INA219_WE(&Wire, I2C_ADDRESS); -> all together
 */
INA219_WE ina219 = INA219_WE(I2C_ADDRESS);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  // default parameters are set - for change check the other examples
  if(!ina219.init()){
    Serial.println("INA219 not connected!");
    while(1);
  }
  Serial.println("INA219 Current Sensor Example Sketch - PowerDown");
  Serial.println("Continuous Sampling starts");
  Serial.println();
}

void loop() {
  for(int i=0; i<5; i++){
    continuousSampling();
    delay(3000);
  }
  
  Serial.println("Power down for 10s");
  ina219.powerDown();
  for(int i=0; i<10; i++){
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("Power up!");
  Serial.println("");
  ina219.powerUp(); // requires 40 µs
}

void continuousSampling(){
  float shuntVoltage_mV = 0.0;
  float loadVoltage_V = 0.0;
  float busVoltage_V = 0.0;
  float current_mA = 0.0;
  float power_mW = 0.0; 
  bool ina219_overflow = false;
  
  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  busVoltage_V = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getBusPower();
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);
  ina219_overflow = ina219.getOverflow();
  
  Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV);
  Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V);
  Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V);
  Serial.print("Current[mA]: "); Serial.println(current_mA);
  Serial.print("Bus Power [mW]: "); Serial.println(power_mW);
  if(!ina219_overflow){
    Serial.println("Values OK - no overflow");
  }
  else{
    Serial.println("Overflow! Choose higher shunt voltage range or a smaller shunt.");
  }
  Serial.println();
}
