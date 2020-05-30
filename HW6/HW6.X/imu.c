#include "imu.h"

void imu_setup(){
    unsigned char who = 0;
    
    // read from IMU_WHOAMI
    who = readPin(IMU_ADDR, IMU_WHOAMI);
    
    if (who != 0b01101001){
        while(1){
        _CP0_SET_COUNT(0);
        LATAbits.LATA4 = !LATAbits.LATA4;
        
        while (_CP0_GET_COUNT() < 1000000){}
        }
    }

    // init IMU_CTRL1_XL
    setPin(IMU_ADDR, 0x10, 0b10000010);
    
    // init IMU_CTRL2_G
    setPin(IMU_ADDR, 0x11, 0b10001000);

    // init IMU_CTRL3_C
    setPin(IMU_ADDR, 0x12, 0b00000100);
}

void imu_read(unsigned char reg, signed short * data, int len){
    
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    unsigned char charData[len*2];
    int ii;
    
    i2c_master_start();
    i2c_master_send(IMU_ADDR);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(IMU_ADDR+0b1);
    for (ii=0; ii<(len*2); ii++){
        charData[ii] = i2c_master_recv();
        if (ii == (len*2)-1){ //only ack(1) if it's the last read
            i2c_master_ack(1);
        }
        else {
            i2c_master_ack(0); //keep reading
        }
    }
    i2c_master_stop();
    
    // turn the chars into the shorts
    for (ii=0; ii<(len*2); ii = ii+2){
        data[ii/2] = (charData[ii+1] << 8) | charData[ii]; //2nd char is MSB, 1st one is LSB
    }
}