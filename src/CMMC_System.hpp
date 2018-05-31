#ifndef CMMC_SYSTEM_H
#define CMMC_SYSTEM_H

class CMMC_System {
  public:
    virtual void init_gpio() = 0;
    virtual void init_fs() = 0;
    virtual void init_userconfig() = 0;
    virtual void init_network() = 0;
    virtual void run() = 0;
    virtual void setup() = 0;
};
#endif