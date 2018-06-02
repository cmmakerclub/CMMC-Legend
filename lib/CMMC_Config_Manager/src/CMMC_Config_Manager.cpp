#include "CMMC_Config_Manager.h"
#include "FS.h"

CMMC_Config_Manager::CMMC_Config_Manager(const char* filename) {
  strcpy(this->filename_c, filename); 
  this->_user_debug_cb = [](const char* s) { };
}
CMMC_Config_Manager::~CMMC_Config_Manager() {
  configFile.close();
}

void CMMC_Config_Manager::_load_raw_content() {
  this->configFile = SPIFFS.open(this->filename_c, "r");
  // USER_DEBUG_PRINTF("> %s (%luB)\r\n", this->filename_c, configFile.size());
  size_t size = configFile.size() + 1;
  std::unique_ptr<char[]> buf(new char[size + 1]);
  bzero(buf.get(), size + 1);
  configFile.readBytes(buf.get(), size);
  if (this->fileContent != 0) {
    delete this->fileContent;
  }

  this->fileContent = new char[size + 1];
  strcpy(this->fileContent, buf.get());

  Serial.print(">");
  Serial.println(this->fileContent);
  this->configFile.close();
}

void CMMC_Config_Manager::init(const char* filename) {
  if (filename != NULL) {
    strcpy(this->filename_c, filename); 
  }
  Serial.printf("current file = %s\r\n", this->filename_c);
  if (SPIFFS.exists(this->filename_c)) {
    // _load_raw_content();
  }
  else {
    USER_DEBUG_PRINTF("creating.. %s \r\n", this->filename_c);
    _init_json_file();
    _load_raw_content();
  }
}

void CMMC_Config_Manager::commit() {
  static CMMC_Config_Manager *_this = this;
  USER_DEBUG_PRINTF("Commit FS..... from [%x]\r\n", _this);

  load_config([](JsonObject * root, const char* content) {
    Serial.println("------------");
    Serial.printf("before commit DO: print config... from [%x]\r\n", root);
    Serial.println(content);
    Serial.println("------------");
    if (root != NULL) {
      _this->configFile = SPIFFS.open(_this->filename_c, "w");
      for (Items::iterator it = _this->items.begin(); it != _this->items.end(); ++it) {
        String first  = it->first;
        String second = it->second;
        root->set(first, second);
        Serial.printf("[std::map]: %s->%s\r\n", first.c_str(), second.c_str());
      }
      size_t configSize = root->printTo(_this->configFile);
      root->printTo(Serial);
      Serial.printf(" has be written %d bytes to file.\r\n", _this->configFile.size());
      _this->configFile.close();
    }
    else {
      Serial.printf("loading config FAILED!!\r\n");
    }
  });
}

void CMMC_Config_Manager::add_field(const char* key, const char* value) {
  strcpy(this->_k, key);
  strcpy(this->_v, value);
  static CMMC_Config_Manager *that = this;
  USER_DEBUG_PRINTF("START [add_field] %s ----> %s (with addr: %x)\r\n", key, value, that);
  items[_k] = _v;
  // show content:
  USER_DEBUG_PRINTF("Iterate through items object...\r\n");
  for (Items::iterator it = items.begin(); it != items.end(); ++it) {
    USER_DEBUG_PRINTF("> %s->%s\r\n", it->first.c_str(), it->second.c_str());
  }
  // USER_DEBUG_PRINTF("millis() = %lu\r\n", millis());
  // USER_DEBUG_PRINTF("END add field\r\n");
}

void CMMC_Config_Manager::load_config(cmmc_json_loaded_cb_t cb) {
  _load_raw_content();
  jsonBuffer.clear(); 
  const char *b =  this->fileContent;
  JsonObject& json = jsonBuffer.parseObject(b);
  if (cb) {
    if (json.success()) {
      // USER_DEBUG_PRINTF("[load_config] Parsing config success.\r\n");
      cb(&json, this->fileContent);
    }
    else {
      // USER_DEBUG_PRINTF("[load_config] json parse failed.\r\n");
      cb(NULL, this->fileContent);
    }
  }
  else {
    Serial.println("CB = NULL");
  }
}

File CMMC_Config_Manager::_init_json_file() {
  USER_DEBUG_PRINTF("[_init_json_file]\r\n");
  this->configFile = SPIFFS.open(this->filename_c, "w");
  jsonBuffer.clear();
  JsonObject& json = this->jsonBuffer.createObject();
  json.printTo(configFile);
  this->configFile.close();
}

void CMMC_Config_Manager::add_debug_listener(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
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
