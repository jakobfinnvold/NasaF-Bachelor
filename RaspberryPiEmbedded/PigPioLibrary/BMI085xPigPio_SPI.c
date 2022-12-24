#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pigpio.h>
#include <time.h>

// Headers
#include <bmi08x.h>

// MACROS
#define GRAVITY_EARTH (9.80665f)
#define PI (3.14159265f)
#define CS0 18
#define CS1 17
#define MISO 19
#define MOSI 20
#define SCLK 21
#define SPI_CHANNEL 1
#define BAUD 2000000
#define INTERRUPT_PIN 16
#define dt 0.01


// Accelerometer Addresses
const int REG_ACCEL_PWR_CTRL = 0x04;
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


// Function declerations

static float lsb_to_mps2(int16_t val, int8_t g_range, uint8_t bit_width);

static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

// FUNCTIONS


void ComplementaryFilter(float accData[3], float gyroData[3], float *pitch, float *roll);


// Variable declerations
    int setup, setupSPI;
    unsigned char buffer[100];
    volatile bool dataRdy;
    float x_a, y_a, z_a, x_g, y_g, z_g, readSPI; // Variables to hold sensordata and the readprocess
    float pitch = 0, roll = 0; // Variables for the complementary filter

int main()
{

    if (gpioInitialise() < 0){
        printf("Pigpio Init failed!");
    }
    else{
        printf("Pigpio OK");

    }

    gpioWrite(CS1, 0);
    sleep(1);
    int setup = bbSPIOpen(CS1, MISO, MOSI, SCLK, BAUD, PI_SPI_FLAGS_MODE(0));

    printf("%d\n", setup);
    char t1[] = {(BMI08X_REG_ACCEL_CHIP_ID << 1U) | 0, 0};
    unsigned char inBuf[8];
    int tx = bbSPIXfer(CS1, t1, (char*)inBuf, 2);

    printf("%d", tx);

    char t2[] = {(BMI08X_REG_ACCEL_PWR_CTRL << 1U | 0), REG_ACCEL_PWR_CTRL, 0};
    tx = bbSPIXfer(CS1, t2, (char*)inBuf, 2);

    char t3[] = {(BMI08X_REG_ACCEL_CONF << 1U | 0), ACCEL_CONF};
    tx = bbSPIXfer(CS1, t3, (char*)inBuf, 2);

    char t4[] = {(BMI08X_REG_ACCEL_INT1_IO_CONF << 1U | 0), ACCEL_INT_CONF};
    tx = bbSPIXfer(CS1, t4, (char*)inBuf, 2);

    char t5[] = {(BMI08X_REG_ACCEL_INT1_INT2_MAP_DATA << 1U | 0), ACCEL_IO_MAP};
    tx = bbSPIXfer(CS1, t5, (char*)inBuf, 2);

    char t6[] = {(BMI085_ACCEL_RANGE_16G << 1U | 0), ACCEL_RANGE};
    tx = bbSPIXfer(CS1, t6, (char*)inBuf, 2);

    char t7[] = {(BMI08X_GYRO_RANGE_500_DPS << 1U | 0), GYRO_RANGE};
    tx = bbSPIXfer(CS0, t7, (char*)inBuf, 2);

    char t8[] = {(BMI08X_GYRO_BW_32_ODR_100_HZ << 1U | 0), GYRO_BW};
    tx = bbSPIXfer(CS0, t8, (char*)inBuf, 2);

    while (gpioInitialise() > 0) {
        char t9[] = {(BMI08X_REG_GYRO_X_LSB << 1U | 1), BMI08X_REG_GYRO_Y_LSB, BMI08X_REG_GYRO_Z_LSB};
        unsigned char inBuf1[8];

        tx = bbSPIXfer(CS1, t9, (char*)inBuf1, 4);

        float x_val = (int16_t) ((inBuf1[1]&3)<<8);
        float y_val = (int16_t) ((inBuf1[2]&3)<<8);
        float z_val = (int16_t) ((inBuf1[3]&3)<<8);

        printf("X:%f, Y:%f, Z:%f\n", x_val, y_val, z_val);
    }
    //printf("Accel X: %f, Accel Y: %f, Accel Z: %f\n ", x_a, y_a, z_a);
    //printf("Gyro X: %f, Gyro Y: %f, Gyro Z: %f\n", x_g, y_g, z_g);

    // Store values in an array for the complementary filter
  //  float accArray[3] = {x_a, y_a, z_a};
   // float gyroArray[3] = {x_g, y_g, z_g};
   
   bbSPIClose(CS1);
   
return 0; 
}





static float lsb_to_mps2(int16_t val, int8_t g_range, uint8_t bit_width)
{
    float gravity;

    float half_scale = ((1 << bit_width) / 2.0f);

    gravity = (float)((GRAVITY_EARTH * val * g_range) / half_scale);

    return gravity;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (dps / ((half_scale) + 100)) * (val);
}

void ComplementaryFilter(float accData[3], float gyroData[3], float *pitch, float *roll){
    float pitchAcc, rollAcc;

    // Integrating gyro data
    *pitch += (gyroData[0] / 65.536) * dt; //Angel x-axis
    *roll -= (gyroData[1]/ 65.536) * dt; //Angel y-axis

    pitchAcc = (atan2f(accData[1], accData[2]) * 180)/PI;
    *pitch = *pitch * 0.98 + pitchAcc * 0.02;

    rollAcc = (atan2f(accData[0], accData[2]) * 180)/PI;
    *roll = *roll * 0.98 + rollAcc * 0.02;

    printf("Pitch%4.2f, Roll%4.2f\n", *pitch, *roll);

}
