/******************************************************************************
 *
 * This is a library for the INA219 Current Sensor Module
 *
 * You'll find several example sketches which should enable you to use the library. 
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 * 
 * Written by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)
 * https://wolles-elektronikkiste.de/ina219 (German)
 *
 * 
 ******************************************************************************/

#ifndef INA219_WE_H_
#define INA219_WE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/* registers */
#define INA219_ADDRESS      0x40
#define INA219_CONF_REG     0x00 //Configuration Register
#define INA219_SHUNT_REG    0x01 //Shunt Voltage Register
#define INA219_BUS_REG      0x02 //Bus Voltage Register
#define INA219_PWR_REG      0x03 //Power Register 
#define INA219_CURRENT_REG  0x04 //Current flowing through Shunt
#define INA219_CAL_REG      0x05 //Calibration Register 

/* parameters */
#define INA219_RST             0x8000

typedef enum INA219_ADC_MODE{
    BIT_MODE_9      = 0b00000000,   
    BIT_MODE_10     = 0b00000001,
    BIT_MODE_11     = 0b00000010,
    BIT_MODE_12     = 0b00000011,
    SAMPLE_MODE_2   = 0b00001001,
    SAMPLE_MODE_4   = 0b00001010,
    SAMPLE_MODE_8   = 0b00001011,
    SAMPLE_MODE_16  = 0b00001100,
    SAMPLE_MODE_32  = 0b00001101,
    SAMPLE_MODE_64  = 0b00001110,
    SAMPLE_MODE_128 = 0b00001111
} adcMode;

typedef enum INA219_MEASURE_MODE{
    POWER_DOWN      = 0b00000000,
    TRIGGERED       = 0b00000011,
    ADC_OFF         = 0b00000100,
    CONTINUOUS      = 0b00000111,
    CONTINOUS       = 0b00000111  // keeping misspelled enum for backwards compatibility.
} measureMode;

typedef enum INA219_PGAIN{
    PG_40       = 0x0000,
    PG_80       = 0x0800,
    PG_160      = 0x1000,
    PG_320      = 0x1800
} pGain;

typedef enum INA219_BUS_RANGE{
    BRNG_16         = 0x0000,
    BRNG_32         = 0x2000,
} busRange;


class INA219_WE
{
public: 
    // Constructors: if not passed 0x40 / Wire will be set as address / wire object
    INA219_WE(int addr);
    INA219_WE();
    INA219_WE(TwoWire *w, int addr);
    INA219_WE(TwoWire *w);
  
    bool init();
    bool reset_INA219();
    void setCorrectionFactor(float corr);
    void setADCMode(INA219_ADC_MODE mode);
    void setMeasureMode(INA219_MEASURE_MODE mode);
    void setPGain(INA219_PGAIN gain);
    void setBusRange(INA219_BUS_RANGE range);
    float getShuntVoltage_mV();
    float getBusVoltage_V();
    float getCurrent_mA();
    float getBusPower();
    bool getOverflow();
    void startSingleMeasurement();
    void powerDown();
    void powerUp(); 
    
private:
    INA219_ADC_MODE deviceADCMode;
    INA219_MEASURE_MODE deviceMeasureMode;
    INA219_PGAIN devicePGain;
    INA219_BUS_RANGE deviceBusRange;
    TwoWire *_wire;
    int i2cAddress;
    uint16_t calVal;
    uint16_t calValCorrected;
    uint16_t confRegCopy;
    float currentDivider_mA;
    float pwrMultiplier_mW;
    bool calc_overflow;

public:
    byte writeRegister(uint8_t reg, uint16_t val);
    uint16_t readRegister(uint8_t reg);

};

#endif

