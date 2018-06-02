#ifndef CMMC_Config_Manager_H
#define CMMC_Config_Manager_H

#include <ArduinoJson.h>


#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#include "ESP8266WiFi.h"
#include <functional>
#include <map>
#include "FS.h"
#endif

#ifndef CMMC_NO_ALIAS
#define CMMC_Config_Manager ConfigManager
#endif


typedef void (*cmmc_err_status_t)(u8 status, const char* cause);
typedef void (*cmmc_succ_status_t)(u8 status);
typedef void (*cmmc_debug_cb_t)(const char* cause);
typedef void (*cmmc_dump_cb_t)(const char* msg, const char* k, const char* v);
typedef void (*cmmc_json_loaded_cb_t)(JsonObject* root, const char* content);

#define USER_DEBUG_PRINTF(fmt, args...) { \
    Serial.printf(fmt, ## args); \
  }

class CMMC_Config_Manager
{
  public:
    // constructor
    CMMC_Config_Manager(const char* filename = "/config.json");
    ~CMMC_Config_Manager();
    typedef std::map<String, String> Items; 
    void init(const char* filename = NULL);
    void commit();
    void load_config(cmmc_json_loaded_cb_t cb = NULL);
    void add_debug_listener(cmmc_debug_cb_t cb);
    void add_field(const char* key, const char* value);
    void dump_json_object(cmmc_dump_cb_t printer);
    char* fileContent = 0;
  private:
    bool _busy = false;
    File _init_json_file();
    Items items;
    DynamicJsonBuffer jsonBuffer;
    cmmc_debug_cb_t _user_debug_cb;
    File configFile;
    char filename_c[60];
    char _k[40] = {0};
    char _v[60] = {0};
    void _load_raw_content();
};

#endif //CMMC_Config_Manager_H
