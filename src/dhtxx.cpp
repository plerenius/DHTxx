#include "dhtxx.h"

#include "sensesp.h"
#include <RemoteDebug.h>


// DHTxx represents a DHT temperature & pressure sensor.
DHTxx::DHTxx(uint8_t pin, uint8_t type) {
    dht_ = new DHT(pin, type);
    dht_->begin();
}

// DHTvalue reads and outputs the specified type of value of a DHTxx sensor
DHTValue::DHTValue(DHTxx* dhtxx, DHTValType val_type,
                   uint read_delay, String config_path) :
                   NumericSensor(config_path), dhtxx_{dhtxx},
                   val_type_{val_type}, read_delay_{read_delay} {
      load_configuration();
}

// DHT outputs temp in Celsius. Need to convert to Kelvin
// before sending to Signal K.
void DHTValue::enable() {
  app.onRepeat(read_delay_, [this](){
      if (val_type_ == temperature) {
        // Kelvin is Celsius + 273.15
        output = dhtxx_->dht_->readTemperature() + 273.15;
      } else if (val_type_ == humidity) {
        // relative humidity is a percentage or ratio, so should be divided by 100
        output = dhtxx_->dht_->readHumidity() / 100;
      } else if (val_type_ == dewPointTemperature) {
          const float a = 17.27;
          const float b = 237.7; // in Celsius

          float h= dhtxx_->dht_->readHumidity()/100; //humidity in percent, so divide by 100
          float t= dhtxx_->dht_->readTemperature();

          float alfa=(a*t)/(b+t)+ log(h);

          float dpt = (b*alfa)/(a-alfa); 
        
          output = dpt + 273.15; // Kelvin is Celsius + 273.15
       } else {
        debugE("DHTValue:enable(): Didn't recognize the val_type.");
      }

      notify();
  });
}

_VOID DHTValue::get_configuration(JsonObject& root) {
  root["read_delay"] = read_delay_;
}

static const char SCHEMA[] PROGMEM = R"###({
  "type": "object",
  "properties": {
      "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" }
  }
})###";


String DHTValue::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool DHTValue::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay_ = config["read_delay"];
  return true;
}
