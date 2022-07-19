#pragma once

#include "wled.h"

#define GOLF_PIN A0
#define GOLF_DEFAULT_COEFF 0.95

class UsermodGolf: public Usermod {

  private:

    bool enabled = true;
    bool triggered=false;
    float value=0;
    float avg=0;
    uint16_t threshold_high=20;
    uint16_t threshold_low=10;
    uint16_t interval=250;
    float coeff=GOLF_DEFAULT_COEFF;
    float coeff2=(1.0-GOLF_DEFAULT_COEFF);
    uint8_t preset_default = 1;
    uint8_t preset_score = 2;

    // strings to reduce flash memory usage
    static const char _name[];
    static const char _preset_default[];
    static const char _preset_score[];
    static const char _interval[];
    static const char _coeff[];
    static const char _enabled[];
    static const char _threshold_high[];
    static const char _threshold_low[];

  public:

    void setup() {
      pinMode(GOLF_PIN, INPUT);
      value=analogRead(GOLF_PIN);
      avg=value;
    }

    void loop() {
      static unsigned long t_lastReading=0;
      if ((!enabled) || strip.isUpdating())                                                                                                                                                                            
        return;

      unsigned long now = millis();
      if(!(now-t_lastReading > interval))
        return;
      t_lastReading=now;

      value=analogRead(GOLF_PIN);
      avg=(avg*coeff)+(value*coeff2);

      float diff=value-avg;

      if ((diff>threshold_high) && !triggered){
        triggered=true;
        applyPreset(preset_score,CALL_MODE_DIRECT_CHANGE);
      } else if ((diff<=threshold_low) && triggered) {
        triggered=false;
        applyPreset(preset_default,CALL_MODE_DIRECT_CHANGE);
      }

    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      top[FPSTR(_preset_default)] = preset_default;
      top[FPSTR(_preset_score)] = preset_score;
      top[FPSTR(_interval)] = interval;
      top[FPSTR(_coeff)] = coeff;
      top[FPSTR(_threshold_high)] = threshold_high;
      top[FPSTR(_threshold_low)] = threshold_low;
    }

    bool readFromConfig(JsonObject &root) {
      bool prev_enabled=enabled;
      JsonObject top = root[FPSTR(_name)];
      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
      configComplete &= getJsonValue(top[FPSTR(_preset_default)], preset_default);
      configComplete &= getJsonValue(top[FPSTR(_preset_score)], preset_score);
      configComplete &= getJsonValue(top[FPSTR(_interval)], interval);
      configComplete &= getJsonValue(top[FPSTR(_coeff)], coeff);
      coeff2=1.0-coeff;
      configComplete &= getJsonValue(top[FPSTR(_threshold_high)], threshold_high);
      configComplete &= getJsonValue(top[FPSTR(_threshold_low)], threshold_low);

      if (enabled!=prev_enabled){
        if(enabled){
          value=analogRead(GOLF_PIN);
          avg=value;
        }
      }

      return configComplete;
    }

  void addToJsonInfo(JsonObject &root) {
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");
      JsonObject golf = user.createNestedObject(FPSTR(_name));
      golf["value"]=value;
      golf["avg"]=avg;
      golf["diff"]=value-avg;
      golf["triggered"]=triggered;
  }
};

// strings to reduce flash memory usage (used more than twice)
const char UsermodGolf::_name[]            PROGMEM = "Golf";
const char UsermodGolf::_preset_default[]  PROGMEM = "preset default";
const char UsermodGolf::_preset_score[]    PROGMEM = "preset score";
const char UsermodGolf::_enabled[]         PROGMEM = "enabled";
const char UsermodGolf::_interval[]        PROGMEM = "interval";
const char UsermodGolf::_coeff[]           PROGMEM = "coefficient";
const char UsermodGolf::_threshold_high[]  PROGMEM = "threshold high";
const char UsermodGolf::_threshold_low[]   PROGMEM = "threshold low";
