/***************************************************************************
* Example sketch for the INA219_WE library
*
* This sketch shows how to use the INA219 module in continuous mode. 
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
  if(!ina219.init()){
    Serial.println("INA219 not connected!");
    while(1);
  }

  /* Set ADC Mode for Bus and ShuntVoltage
    * Mode *                 * Res / Samples *       * Conversion Time *
    INA219_BIT_MODE_9        9 Bit Resolution             84 µs
    INA219_BIT_MODE_10       10 Bit Resolution            148 µs  
    INA219_BIT_MODE_11       11 Bit Resolution            276 µs
    INA219_BIT_MODE_12       12 Bit Resolution            532 µs  (DEFAULT)
    INA219_SAMPLE_MODE_2     Mean Value 2 samples         1.06 ms
    INA219_SAMPLE_MODE_4     Mean Value 4 samples         2.13 ms
    INA219_SAMPLE_MODE_8     Mean Value 8 samples         4.26 ms
    INA219_SAMPLE_MODE_16    Mean Value 16 samples        8.51 ms     
    INA219_SAMPLE_MODE_32    Mean Value 32 samples        17.02 ms
    INA219_SAMPLE_MODE_64    Mean Value 64 samples        34.05 ms
    INA219_SAMPLE_MODE_128   Mean Value 128 samples       68.10 ms
    
    If you measure both current and bus voltage (which is the default), the conversion time doubles.
  */
  //ina219.setADCMode(INA219_SAMPLE_MODE_128); // choose mode and uncomment for change of default
  
  /* Set measure mode
    INA219_POWER_DOWN              - INA219 switched off
    INA219_TRIGGERED               - measurement on demand, current and bus
    INA219_TRIGGERED_CURRENT_ONLY  - on demand, current only
    INA219_TRIGGERED_BUS_ONLY      - on demand, bus voltage only
    INA219_ADC_OFF                 - analog/digital converter switched off
    INA219_CONTINUOUS              - continuous measurements (DEFAULT)
    INA219_CONTINUOUS_CURRENT_ONLY - continuous, current only
    INA219_CONTINUOUS_BUS_ONLY     - continuous, bus voltage only
  */
  // ina219.setMeasureMode(INA219_CONTINUOUS); // choose mode and uncomment for change of default
  
  /* Set PGain
     * Gain *     * Shunt Voltage Range *   * Max Current (if shunt is 0.1 ohms) *
    INA219_PG_40         40 mV                    0.4 A
    INA219_PG_80         80 mV                    0.8 A
    INA219_PG_160        160 mV                   1.6 A
    INA219_PG_320        320 mV                   3.2 A (DEFAULT)
  */
  // ina219.setPGain(INA219_PG_320); // choose gain and uncomment for change of default
  
  /* Set Bus Voltage Range
    INA219_BRNG_16   -> 16 V
    INA219_BRNG_32   -> 32 V (DEFAULT)
  */
  // ina219.setBusRange(INA219_BRNG_32); // choose range and uncomment for change of default

  Serial.println("INA219 Current Sensor Example Sketch - Continuous");

  /* If the current values delivered by the INA219 differ by a constant factor
    from values obtained with calibrated equipment you can define a correction factor.
    Correction factor = current delivered from calibrated equipment / current delivered by INA219
  */
  // ina219.setCorrectionFactor(0.98); // insert your correction factor if necessary
  
  /* If you experience a shunt voltage offset, that means you detect a shunt voltage which is not 
    zero, although the current should be zero, you can apply a correction. For this, uncomment the 
    following function and apply the offset you have detected.   
  */
  // ina219.setShuntVoltOffset_mV(0.5); // insert the shunt voltage (millivolts) you detect at zero current

  /* If you use a shunt different from 0.1 ohms (R100), you can change the shunt size using the
    function below.
  */
  //ina219.setShuntSizeInOhms(0.01); 
}

void loop() {
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
  
  delay(3000);
}
