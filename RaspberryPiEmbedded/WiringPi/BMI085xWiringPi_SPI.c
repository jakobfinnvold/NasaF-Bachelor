#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>

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
// GLOBALS


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


// FUNCTIONS from Bosch
static float lsb_to_mps2(int16_t val, int8_t g_range, uint8_t bit_width);
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

// Function defenitions
void dataRdyInt(void);
void regConfig (const int setup, uint8_t *regAddr, uint8_t regVal, const uint16_t ms);
void ComplementaryFilter(float accData[3], float gyroData[3], float *pitch, float *roll);


// Variable declerations
    int setup, setupSPI;
    unsigned char buffer[100];
    volatile bool dataRdy;
    float x_a, y_a, z_a, x_g, y_g, z_g, readSPI; // Variables to hold sensordata and the readprocess
    float pitch = 0, roll = 0; // Variables for the complementary filter

int main()
{
    setup = wiringPiSetup();
    if(setup < 0){
        printf("WiringPi Setup failed!");
        return -1;
    }

    setupSPI = wiringPiSPISetup(SPI_CHANNEL, BAUD);

    if (setupSPI == -1){
        printf("Spi Setup failed!");
        return -1;
    }

    if (wiringPiISR(INTERRUPT_PIN, INT_EDGE_RISING, &dataRdyInt) < 0){
        printf("Unable to set up interrupt");
        return -1;
    }

    // Accelerometer Setup
    buffer[0] = (ACCEL_CONF << 1U) | 0U;
    regConfig(1, buffer, 0x08, 2); // ODR 100 HZ

    buffer[0] = (ACCEL_INT_CONF << 1U) | 0U;
    regConfig(1, buffer, 0x80, 2); // Accel data rdy int initilazer

    buffer[0] = (ACCEL_IO_MAP << 1U) | 0U;
    regConfig(1, buffer, 0x04, 2); // Data Ready Interrupt to INT pin 1

    buffer[0] = (ACCEL_RANGE << 1U) | 0U;
    regConfig(1, buffer, 0x03, 2); // Setting accel range to 16 G

    buffer[0] = (REG_ACCEL_PWR_CTRL << 1U) | 0U; // Send the Register Address in the first charcter of the buffer
    regConfig(1, buffer, 0x04, 50); // Waking the Accelerometer

    // Gyrometer Setup
    buffer[0] = (GYRO_RANGE << 1U) | 0U;
    regConfig(1, buffer, 0x02, 2); // Setting gyrometer range to +/- 500 dps

    buffer[0] = (GYRO_BW << 1U) | 0U;
    regConfig(1, buffer, 0x07, 2); // Gyro ODR to 100Hz

    while(!dataRdy){
        delay(10000);
    }

    buffer[0] = (0x12 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    x_a = lsb_to_mps2(readSPI, 16, 16);

    buffer[0] = (0x14 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    y_a = lsb_to_mps2(readSPI, 16, 16);

    buffer[0] = (0x16 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;    
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    z_a = lsb_to_mps2(readSPI, 16, 16);

    buffer[0] = (0x02 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;    
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    x_g = lsb_to_dps(readSPI, 16, 16);

    buffer[0] = (0x04 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;    
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    y_g = lsb_to_dps(readSPI, 16, 16);

    buffer[0] = (0x06 << 1U) | 1U;
    buffer[1] = 0x55;
    buffer[2] = 0x00;    
    readSPI = ((int16_t)wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2));
    z_g = lsb_to_dps(readSPI, 16, 16);

    printf("Accel X: %f, Accel Y: %f, Accel Z: %f\n ", x_a, y_a, z_a);
    printf("Gyro X: %f, Gyro Y: %f, Gyro Z: %f\n", x_g, y_g, z_g);

    // Store values in an array for the complementary filter
   //  float accArray[3] = {x_a, y_a, z_a};
   // float gyroArray[3] = {x_g, y_g, z_g};
}

void dataRdyInt (void){
    dataRdy = 1;
}

void regConfig (const int channel, uint8_t *regAddr, uint8_t regVal, const uint16_t ms){

    uint8_t *ADDRESS = regAddr;
    printf("Data from Register %p\n", ADDRESS);

    wiringPiSPIDataRW(channel, &regVal, 1);
    sleep(ms);
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

    return (dps / ((half_scale) + BMI08X_GYRO_RANGE_2000_DPS)) * (val);
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
