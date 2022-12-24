#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "gpiolib.h"
#include <fcntl.h>
#include <gpiod.h>
#include <wiringPi.h>

#define CS0 8
#define CS1 7
#define MODE 0
#define SPEED 2000000

// ----- GLOBALS------
int f_spi, g_spi;
static struct spi_ioc_transfer xfer;
static struct spi_ioc_transfer gfer;

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

// -----Functions ------

int set_gpio(int pin_num){
    // export the GPIO pin to gpiolib
    wiringPiSetup();
    pinMode(pin_num, OUTPUT);

    // Setting direction as output/input
    

    return 0;
}

int write_gpio(int pin_num, char value){
    digitalWrite(pin_num, value);

}

int spi_accel_init(){
    int val = 0;
    int iSPIMode = SPI_MODE_0;
    int spi_freq = 2000000;
    f_spi = open("/dev/spidev0.0", O_RDWR);

    if(f_spi < 0){
        perror("Error initialising the SPI device\n");
        return -1;
    }

    val = ioctl(f_spi, SPI_IOC_WR_MODE, &iSPIMode);
    if (val < 0){
        perror("Error setting the SPI mode\n");
        return -1;
    }

    val = ioctl(f_spi, SPI_IOC_WR_MAX_SPEED_HZ, &spi_freq);
    if (val < 0){
        perror("Error setting the SPI bus speed\n");
        return -1;
    }

    memset(&xfer, 0, sizeof (xfer));
    xfer.speed_hz = spi_freq;
    xfer.cs_change = 0;
    xfer.delay_usecs = 2;
    xfer.bits_per_word = 8;

    return 0;
}

int spi_gyro_init(){
    int val = 0;
    int iSPIMode = SPI_MODE_0;
    int spi_freq = 2000000;
    g_spi = open("/dev/spidev0.1", O_RDWR);

    if(g_spi < 0){
        perror("Error initialising the SPI device\n");
        return -1;
    }

    val = ioctl(g_spi, SPI_IOC_WR_MODE, &iSPIMode);
    if (val < 0){
        perror("Error setting the SPI mode\n");
        return -1;
    }

    val = ioctl(g_spi, SPI_IOC_WR_MAX_SPEED_HZ, &spi_freq);
    if (val < 0){
        perror("Error setting the SPI bus speed\n");
        return -1;
    }

    memset(&gfer, 0, sizeof (gfer));
    gfer.speed_hz = spi_freq;
    gfer.cs_change = 0;
    gfer.delay_usecs = 2;
    gfer.bits_per_word = 8;

    return 0;
}

void spi_write_accel(uint8_t *pstring, uint8_t length){
    int ret_val = 0;
    digitalWrite(CS0, LOW);

    xfer.rx_buf = 0;
    xfer.tx_buf = (unsigned long)pstring;
    xfer.len = length;

    ret_val = ioctl(f_spi, SPI_IOC_MESSAGE(1), &xfer);

    digitalWrite(CS0, HIGH); 
    return;
}

void spi_write_gyro(uint8_t *pstring, uint8_t length){
    int ret_val = 0;
    digitalWrite(CS1, LOW);

    gfer.rx_buf = 0;
    gfer.tx_buf = (unsigned long)pstring;
    gfer.len = length;

    ret_val = ioctl(g_spi, SPI_IOC_MESSAGE(1), &gfer);

    digitalWrite(CS1, HIGH);
    return;
}


int main()
{
    int val = 0;
    uint8_t buf[3];

    set_gpio(CS0); // Accel CS as output
    digitalWrite(CS0, HIGH); // Accel CS-pin to HIGH to signal SPI


    val = spi_accel_init();
    if (val == -1){
        perror("Failed to initialise SPI bus\n");
        return -1;
    }

    val = spi_gyro_init();
    if (val == -1){
        perror("Failed to initialise SPI bus\n");
        return -1;
    }

    buf[0] = (ACCEL_CONF << 1U) | 0U;
    buf[1] = 0x08;
    buf[2] = 0x00; // ODR 100Hz
    spi_write_accel(buf, 3);
    sleep(2/1000000);

    buf[0] = (ACCEL_INT_CONF << 1U) | 0U;
    buf[1] = 0x0A;
    buf[2] = 0x00; // Initializing the INT pin 1
    spi_write_accel(buf, 3);
    sleep(2/1000000);

    buf[0] = (ACCEL_IO_MAP << 1U) | 0U;
    buf[1] = 0x04;
    buf[2] = 0x00; // IO map mapped to int pin 1
    spi_write_accel(buf, 3);
    sleep(2/1000000);

    buf[0] = (ACCEL_RANGE << 1U)| 0U;
    buf[1] = 0x03;
    buf[2] = 0x00; // Accel range set to 16G
    spi_write_accel(buf, 3);
    sleep(2/1000000);

    /* Gyroscope */
    buf[0] = (GYRO_RANGE << 1U) | 0U;
    buf[1] = 0x02;
    buf[2] = 0x00; // Setting gyro range to +/- 500 dps
    spi_write_gyro(buf, 3);
    sleep(2/1000000);

    buf[0] = (GYRO_BW << 1U) | 0U;
    buf[1] = 0x07;
    buf[2] = 0x00; // ODR Frequency to 100 Hz
    spi_write_gyro(buf, 3);
    sleep(2/1000000);

    /* Powering on the Accelerometer*/
    buf[0] = (REG_ACCEL_PWR_CTRL << 1U) | 0U;
    buf[1] = 0x04;
    buf[2] = 0x00; // This starts the accelerometer from sleep mode
    spi_write_accel(buf, 3);
    sleep(50/1000000); // Sleep for 50 microseconds

    while(val != -1){

        buf[0] = (0x12 << 1U) | 1U;
        buf[1] = 0x00;
        buf[2] = 0x00;
        spi_write_accel(buf, 3);
        printf("Accel X:%d\n", buf[2]);
        sleep(2/1000000);


        buf[0] = (0x14 << 1U) | 1U;
        buf[1] = 0x00;
        buf[2] = 0x00;
        spi_write_accel(buf, 3);
        printf("Accel Y:%d\n", buf[2]);
        sleep(2/1000000);


        buf[0] = (0x16 << 1U) | 1U;
        buf[1] = 0x00;
        buf[2] = 0x00;
        spi_write_accel(buf, 3);
        printf("Accel Z:%d\n", buf[2]);
        sleep(2/1000000);

    }



return 0;

}