#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>

// Accelerometer Addresses
const int REG_ACCEL_PWR_CTRL = 0x7D;
const int REG_ACCEL_FIFO_CONFIG_1 = 0x49;
const int ACCEL_CONF = 0x40;
const int ACCEL_RANGE = 0x41;
const int ACCEL_INT_CONF = 0x53;
const int ACCEL_IO_MAP = 0x58;

// Gyrometer Addresses
const int GYRO_PWR_CTRL = 0x11;
const int GYRO_FIFO_CONF = 0x3E;
const int GYRO_RANGE = 0x0F;
const int GYRO_BW = 0x10;
const int GYRO_INT_CTRL = 0x15;
const int GYRO_IO_MAP = 0x18;

void configReg(const int fd, const int regAddr, const int regVal, const uint16_t ms);
void msleep(uint16_t ms);
float x, y, z, xg, yg, zg;


int main()
{
    if (wiringPiSetup() < 0){
        printf("Failed to set up Wiring Pi\n");
        return -1;
    }

    int fd_acc = wiringPiI2CSetup(0x19);
    if (fd_acc == -1){
        printf("Accelerometer not found!\n");
        return -1;
    }

    int fd_gyr = wiringPiI2CSetup(0x68);
    if (fd_gyr == -1) {
        printf("Gyroscope not found!\n");
        return -1;
    }
    configReg(fd_acc, REG_ACCEL_PWR_CTRL, 0x00, 2); // Power of the accelerometer to ensure it is woken with the correct configurations each time
    
    // ----Basic configurations----
    configReg(fd_acc, ACCEL_CONF, 0xA8, 2);
    configReg(fd_acc, ACCEL_INT_CONF, 0x0A, 2);
    configReg(fd_acc, ACCEL_IO_MAP, 0x04, 2);
    configReg(fd_acc, ACCEL_RANGE, 0x01, 2);

    configReg(fd_gyr, GYRO_RANGE, 0x02, 2);
    configReg(fd_gyr, GYRO_BW, 0x87, 2);

    configReg(fd_acc, REG_ACCEL_PWR_CTRL, 0x04, 50);

    while(1){
      x = ((int16_t)wiringPiI2CReadReg16(fd_acc, 0x12)) / (5460/9.81);
      y = ((int16_t)wiringPiI2CReadReg16(fd_acc, 0x14))/ (5460/9.81);
      z = ((int16_t)wiringPiI2CReadReg16(fd_acc, 0x16))/ (5460/9.81);
      //printf("X.%f, Y:%f, Z:%f\n", x, y, z);
      float accPitch = (180/3.141592) * atan2f(y, z);
      float accRoll = (180/3.141592) * atan2f(x,z); 
      printf("Pitch:%f, Roll:%f\n", accPitch, accRoll);
      delay(50);
      }
}

void configReg(const int fd, const int regAddr, const int regVal, const uint16_t ms){
    wiringPiI2CWriteReg8(fd, regAddr, regVal);
    msleep(ms);
}

void msleep(uint16_t ms){
    useconds_t uSec = 1000*ms;
    usleep(uSec);
}

