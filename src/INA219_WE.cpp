/*****************************************
* This is a library for the INA219 Current Sensor Module
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)
* https://wolles-elektronikkiste.de/ina219 (German)
*
*******************************************/

#include "INA219_WE.h"

INA219_WE::INA219_WE(int addr){
    _wire = &Wire;
    i2cAddress = addr;   
}

INA219_WE::INA219_WE(){
    _wire = &Wire;
    i2cAddress = 0x40;   
}

INA219_WE::INA219_WE(TwoWire *w, int addr){
    _wire = w;
    i2cAddress = addr; 
}

INA219_WE::INA219_WE(TwoWire *w){
    _wire = w;
    i2cAddress = 0x40;
}
    
bool INA219_WE::init(){ 
    if( !reset_INA219() )
    {
        return false;
    }
    setADCMode(BIT_MODE_12);
    setMeasureMode(CONTINUOUS);
    setPGain(PG_320);
    setBusRange(BRNG_32);
    shuntFactor = 1.0;
    overflow = false;
    
    return true;
}

bool INA219_WE::reset_INA219(){
    byte ack = writeRegister(INA219_CONF_REG, INA219_RST); 
    return ack == 0;
}

void INA219_WE::setCorrectionFactor(float corr){
    calValCorrected = calVal * corr;
    writeRegister(INA219_CAL_REG, calValCorrected);
}

void INA219_WE::setADCMode(INA219_ADC_MODE mode){
    deviceADCMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0780);  
    currentConfReg &= ~(0x0078);
    uint16_t adcMask = mode<<3;
    currentConfReg |= adcMask;
    adcMask = mode<<7;
    currentConfReg |= adcMask;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void INA219_WE::setMeasureMode(INA219_MEASURE_MODE mode){
    deviceMeasureMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0007);
    currentConfReg |= deviceMeasureMode;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void INA219_WE::setPGain(INA219_PGAIN gain){
    devicePGain = gain;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x1800);
    currentConfReg |= devicePGain;
    writeRegister(INA219_CONF_REG, currentConfReg);
    
    switch(devicePGain){
        case PG_40:
            calVal = 20480;
            currentDivider_mA = 50.0;
            pwrMultiplier_mW = 0.4;
            shuntOverflowLimit = 4000;
            break;
        case PG_80:
            calVal = 10240;
            currentDivider_mA = 25.0;
            pwrMultiplier_mW = 0.8;
            shuntOverflowLimit = 8000;
            break;
        case PG_160:
            calVal = 8192;
            currentDivider_mA = 20.0;
            pwrMultiplier_mW = 1.0;
            shuntOverflowLimit = 16000;
            break;
        case PG_320:
            calVal = 4096;
            currentDivider_mA = 10.0;
            pwrMultiplier_mW = 2.0;
            shuntOverflowLimit = 32000;
            break;
    }
    
    writeRegister(INA219_CAL_REG, calVal);
            
}

void INA219_WE::setBusRange(INA219_BUS_RANGE range){
    deviceBusRange = range;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x2000);
    currentConfReg |= deviceBusRange;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void INA219_WE::setShuntSizeInOhms(float shuntSize){
    shuntFactor = shuntSize / 0.1;
}

float INA219_WE::getShuntVoltage_mV(){
    int16_t val;
    val = (int16_t) readRegister(INA219_SHUNT_REG);
    
    if((abs(val))== shuntOverflowLimit){
        overflow = true;
    }
    else{
        overflow = false;
    }
    return (val * 0.01);
}


float INA219_WE::getBusVoltage_V(){
    uint16_t val;
    val = readRegister(INA219_BUS_REG);
    val = ((val>>3) * 4);
    return (val * 0.001);
}


float INA219_WE::getCurrent_mA(){
    int16_t val;
    val = (int16_t)readRegister(INA219_CURRENT_REG);
    return (val / (currentDivider_mA * shuntFactor));
}

float INA219_WE::getBusPower(){
    uint16_t val;
    val = readRegister(INA219_PWR_REG);
    return (val * pwrMultiplier_mW / shuntFactor);
}

bool INA219_WE::getOverflow(){
    uint16_t val;
    val = readRegister(INA219_BUS_REG);
    if(val & 1){ 
        overflow = true;
    }
    return overflow;
}

void INA219_WE::startSingleMeasurement(){
    uint16_t val = readRegister(INA219_BUS_REG); // clears CNVR (Conversion Ready) Flag
    val = readRegister(INA219_CONF_REG);
    writeRegister(INA219_CONF_REG, val);
    uint16_t convReady = 0x0000;
    while(!convReady){
        convReady = ((readRegister(INA219_BUS_REG)) & 0x0002); // checks if sampling is completed
    }
}

void INA219_WE::powerDown(){
    confRegCopy = readRegister(INA219_CONF_REG);
    setMeasureMode(POWER_DOWN);
}

void INA219_WE::powerUp(){
    writeRegister(INA219_CONF_REG, confRegCopy);
    delayMicroseconds(40);  
}   

uint8_t INA219_WE::writeRegister(uint8_t reg, uint16_t val){
  _wire->beginTransmission(i2cAddress);
  uint8_t lVal = val & 255;
  uint8_t hVal = val >> 8;
  _wire->write(reg);
  _wire->write(hVal);
  _wire->write(lVal);
  return _wire->endTransmission();
}
  
uint16_t INA219_WE::readRegister(uint8_t reg){
  uint8_t MSByte = 0, LSByte = 0;
  uint16_t regValue = 0;
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(i2cAddress,2);
  if(_wire->available()){
    MSByte = _wire->read();
    LSByte = _wire->read();
  }
  regValue = (MSByte<<8) + LSByte;
  return regValue;
}
    


