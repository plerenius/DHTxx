#ifndef _DHTXX_H_
#define _DHTXX_H_

#include <DHT.h>

#include "sensors/sensor.h"

/**
 * @brief Represents an ADAfruit (or compatible) DHT temperature/humidity sensor.
 * 
 * The constructor creates a pointer to the sensor and
 * starts up the sensor. The pointer is passed to DHTValue, which retrieves
 * the specified value.
 * 
 * @param pin specifies which pin that is being used for the sensor.
 * 
 * @param type specifies which version of the sensor that is being used.
 **/
class DHTxx {
 public:
  DHTxx(uint8_t pin, uint8_t type);
  DHT* dht_;
};


/** 
 * @brief DHTValue reads and outputs the specified value of a DHT sensor
 * 
 * @param dhtxx A pointer to an instance of a DHTxx.
 * 
 * @param val_type The type of value you're reading: temperature or humidity.
 * 
 * @param read_delay How often to read the sensor - in ms.
 * 
 * @param config_path Path in the Config UI to configure read_delay
 **/
class DHTValue : public NumericSensor {
 public:
  // Pass one of these in the constructor to DHTvalue() to tell
  // which type of value you want to output
  enum DHTValType { temperature, humidity };
  DHTValue(DHTxx* dhtxx, DHTValType val_type, uint read_delay = 500,
           String config_path = "");
  void enable() final;
  DHTxx* dhtxx_;

 private:
    DHTValType val_type_;
    uint read_delay_;
    void get_configuration(JsonObject& doc) override;
    bool set_configuration(const JsonObject& config) override;
    String get_config_schema() override;
};

#endif  // _DHTXX_H_
