#ifndef BangController_H
#define BangController_H
#include <Arduino.h>
#include <NTPClient.h>

extern NTPClient timeClient ;

class BangController {
  public:
    BangController(uint8_t _coolPin, uint8_t _heatPin, String _configFile, String _profileFile)  ;
    ~BangController() ;
    void heat(bool _status) ;
    void cool(bool _status) ;
    void run(float _temp ) ;
    void loadConfig() ;
    void writeConfig() ;
    //Returns a json string with configuration
    String getConfig(float _temp) ;
    //set configuration received by web interface
    void setConfig(String _key, String _value) ;
    //start profile from web interface
    bool initProfile(float _array[14], uint32_t _timestamp) ;
    void runProfile(uint32_t _timestamp) ;
    float getTargetTemp() ;
    uint8_t getProfileRun() ;
    uint32_t getLastRamp() ;

  private:
    String configFile ;
    String profileFile;
    uint32_t lastRamp ;
    uint8_t controlStep ;
    bool profileRun ;
    uint8_t coolPin ;
    uint8_t heatPin ;
    uint8_t status ;
    uint8_t mode;
    uint32_t lastHeat;
    uint32_t lastCool;
    uint16_t heatTimer;
    uint16_t coolTimer;
    float coolDiff;
    float heatDiff;
    float targetTemp;
};

#endif
