#include "CMMC_Config_Manager.h"
#include "FS.h"

CMMC_Config_Manager::CMMC_Config_Manager() {
      this->_user_debug_cb = [](const char* s) { };
}
CMMC_Config_Manager::~CMMC_Config_Manager() {
      configFile.close();
}

void CMMC_Config_Manager::init(const char* filename) {   
  strcpy(this->filename_c, filename); 
  if (SPIFFS.exists(this->filename_c)) {
    USER_DEBUG_PRINTF("file exists donot create......"); 
  }
  else {
    _init_json_file(); 
  }
}

void CMMC_Config_Manager::commit() {
  static CMMC_Config_Manager *_this = this;
  USER_DEBUG_PRINTF("Commit FS..... from [%x]", _this);

  load_config([](JsonObject * root, const char* content) {
    Serial.printf("loading config... from [%x]", root);
    if (root != NULL) {
      _this->configFile.close();
      _this->configFile = SPIFFS.open(_this->filename_c, "w");
      Serial.println("loading config OK"); 
      Serial.print("FS PTR: ");
      Serial.println(_this->configFile);
      for (Items::iterator it = _this->items.begin(); it != _this->items.end(); ++it) {
        String first  = it->first;
        String second = it->second;
        root->set(first, second);
        Serial.printf("W: %s->%s\r\n", first.c_str(), second.c_str());
      }
      size_t configSize = root->printTo(_this->configFile);
      root->printTo(Serial);
      Serial.print("wrote ");
      Serial.print(configSize);
      Serial.print(" bytes to file.");
      Serial.println();
      size_t size = _this->configFile.size() + 1;
      Serial.printf("config file size =%d \r\n", _this->configFile.size());
      _this->configFile.close();
    }
    else {
      Serial.println("loading config FAILED!!"); 
    }
  });
  
}

void CMMC_Config_Manager::add_field(const char* key, const char* value) {
  strcpy(this->_k, key);
  strcpy(this->_v, value);
  static CMMC_Config_Manager *_this = this;
  USER_DEBUG_PRINTF("___ START [add_field] with %s:%s (%x)", key, value, this);
  items[_k] = _v;
  // show content:
  USER_DEBUG_PRINTF("Iterate through items...");
  for (Items::iterator it = items.begin(); it != items.end(); ++it) {
    USER_DEBUG_PRINTF("> %s->%s", it->first.c_str(), it->second.c_str());
  } 
  USER_DEBUG_PRINTF("millis() = %lu\r\n", millis());
  USER_DEBUG_PRINTF("___ END add field");
}

void CMMC_Config_Manager::load_config(cmmc_json_loaded_cb_t cb) {
  USER_DEBUG_PRINTF("[load_config] Loading Config..");
  _open_file();
  size_t size = configFile.size() + 1;
  std::unique_ptr<char[]> buf(new char[size + 1]);
  bzero(buf.get(), size + 1);
  configFile.readBytes(buf.get(), size);
  configFile.close();
  const char* b = buf.get();
  USER_DEBUG_PRINTF("[load_config] size = %d\r\n", size); 
  USER_DEBUG_PRINTF("[load_config] config content ->%s<-", b);
  Serial.printf("[0] heap: %lu\r\n", ESP.getFreeHeap());
  this->jsonBuffer.clear();
  JsonObject& json = this->jsonBuffer.parseObject(b); 
  Serial.printf("[1] heap: %lu\r\n", ESP.getFreeHeap());
  if (json.success()) {
    USER_DEBUG_PRINTF("[load_config] Parsing config success.");
    if (cb != NULL) {
      USER_DEBUG_PRINTF("[load_config] calling callback fn");
      cb(&json, b);
    }
  }
  else {
    USER_DEBUG_PRINTF("[load_config] Failed to parse config file.");
    _init_json_file(cb); 
    cb(NULL, NULL);
  }
}

void CMMC_Config_Manager::_init_json_file(cmmc_json_loaded_cb_t cb) {
  USER_DEBUG_PRINTF("[_init_json_file]");
  configFile = SPIFFS.open(this->filename_c, "w");
  JsonObject& json = this->jsonBuffer.createObject();
  json.printTo(configFile);
  USER_DEBUG_PRINTF("[_init_json_file] closing file..");
  configFile.close();
  load_config(cb);
}

void CMMC_Config_Manager::dump_json_object(cmmc_dump_cb_t printer) {
  // this->load_config();
  // if (this->currentJsonObject == NULL) {
  //   return;
  // }
  // else {
  //   this->currentJsonObject->printTo(Serial);
  //   char str_buffer[30];
  //   JsonObject* obj = this->currentJsonObject;
  //   for (JsonObject::iterator it = obj->begin(); it != obj->end(); ++it) {
  //     const char* key = it->key;
  //     const char* value = it->value;
  //     sprintf(str_buffer, "[key] %s -> %s\r\n", key, value);
  //     printer(str_buffer, key, value);
  //   }
  // }
}

void CMMC_Config_Manager::add_debug_listener(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
}

void CMMC_Config_Manager::_open_file()  {
  USER_DEBUG_PRINTF("[open_file] open filename: %s", this->filename_c);
  // USER_DEBUG_PRINTF("DEBUGGIN SPIFFS ...");
  // Dir dir = SPIFFS.openDir("/");
  // while (dir.next()) {
  //   String fileName = dir.fileName();
  //   size_t fileSize = dir.fileSize();
  //   USER_DEBUG_PRINTF("FS File: %s, size: %s", fileName.c_str(), String(fileSize).c_str());
  // }
  if (SPIFFS.exists(this->filename_c)) {
    configFile = SPIFFS.open(this->filename_c, "r");
    USER_DEBUG_PRINTF("[open_file] config size = %lu bytes", configFile.size());
    if (configFile.size() > 512) {
      USER_DEBUG_PRINTF("[open_file] Config file size is too large");
    } else {
      USER_DEBUG_PRINTF("[open_file] check file size ok.");
    }
  } else { // file not exists
    USER_DEBUG_PRINTF("[open_file] file not existsing so create a new file");
    _init_json_file();
  }
}
