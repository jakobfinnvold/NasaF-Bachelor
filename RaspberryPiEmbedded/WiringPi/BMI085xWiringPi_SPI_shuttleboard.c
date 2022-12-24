#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pigpio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#define GRAVITY_EARTH (9.80665f)
#define PI (3.14159265f)
#define CS0 8
#define CS1 7
#define MISO 19
#define MOSI 20
#define SCLK 21
#define SPI_CHANNEL 0
#define BAUD 2000000

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
const int GYRO_BW = 0x00;
const int GYRO_INT_CTRL = 0x15;
const int GYRO_IO_MAP = 0x18;

void dataRdy(void);
_Bool rdy;

unsigned char buf[100];
float x_a, y_a, z_a, x_g, y_g, z_g;
int main()
{
    if(wiringPiSetup() < 0){
        printf("WiringPi Setup failed!\n");
    }
    else {
        printf("Wiring Pi Setup OK!\n");
    }

    if (wiringPiSetupGpio() < 0){
        printf("Failed to open GPIO\n");
    }

    if (wiringPiSPISetupMode(SPI_CHANNEL, BAUD, 0) < 0){
        printf("Failed to set SPI-Mode\n");
    }

    digitalWrite(CS0, 1);
    delayMicroseconds(10);

    if(wiringPiISR(16, INT_EDGE_RISING, &dataRdy)){
        printf("Failed to set interrupt\n");
    }

    unsigned char buf[3] = {(ACCEL_CONF << 1U) | 0U, 0x08, 0x00};
     // ODR 100Hz
    wiringPiSPIDataRW(SPI_CHANNEL, buf, sizeof(buf));
    delayMicroseconds(2);

    unsigned char buf2[3] = {(ACCEL_INT_CONF << 1U) | 0U, 0x0A, 0x00};
     // Initializing the INT pin 1
    wiringPiSPIDataRW(SPI_CHANNEL, buf2, sizeof (buf2));
    delayMicroseconds(2);

    unsigned char buf3[3] = {(ACCEL_IO_MAP << 1U) | 0U, 0x04, 0x00};
     // IO map mapped to int pin 1
    wiringPiSPIDataRW(SPI_CHANNEL, buf3, sizeof(buf3));
    delayMicroseconds(2);

    unsigned char buf4[3] = {(ACCEL_RANGE << 1U)| 0U, 0x03, 0x00};
    // Accel range set to 16G
    wiringPiSPIDataRW(SPI_CHANNEL, buf4, sizeof (buf4));
    delayMicroseconds(2);

    /* Gyroscope */
    unsigned char buf5[3] = {(GYRO_RANGE << 1U) | 0U, 0x02, 0x00};
    // Setting gyro range to +/- 500 dps
    wiringPiSPIDataRW(SPI_CHANNEL, buf5, sizeof (buf5));
    delayMicroseconds(2);

    unsigned char buf6[3] = {(GYRO_BW << 1U) | 0U, 0x07, 0x00};
    // ODR Frequency to 100 Hz
    wiringPiSPIDataRW(SPI_CHANNEL, buf6, sizeof(buf6));
    delayMicroseconds(2);

    /* Powering on the Accelerometer*/

    unsigned char buf7[3] = {(REG_ACCEL_PWR_CTRL << 1U) | 0U, 0x04, 0x00};
    // This starts the accelerometer from sleep mode
    wiringPiSPIDataRW(SPI_CHANNEL, buf7, sizeof (buf7));
    delayMicroseconds(500);


    while (1) {
        unsigned char buffer[3] = {0x12 << 1U | 1U, 0x00, 0x00};

        wiringPiSPIDataRW(SPI_CHANNEL, buffer, sizeof (buffer));
        x_a = buffer[2];
        printf("Accel X:%f\n", x_a);
        delay(2);
        printf("Accel Xbuf: %s", &buffer[2]); 
    }
    return 0;
}

void dataRdy(void){
    rdy = 1;
}

