#include "i2c_master_noint.h"

void setPin(unsigned char addr, unsigned char reg, unsigned char val){
    i2c_master_start();
    i2c_master_send(addr);
    i2c_master_send(reg);
    i2c_master_send(val);
    i2c_master_stop();
}

unsigned char readPin(unsigned char addr, unsigned char reg){
    i2c_master_start();
    i2c_master_send(0b01000000);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(addr);
    unsigned char data = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return data;
}