#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gpiod.h>
#include <linux/spi/spidev.h>
#include "gpiolib.h"


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

// Global variables
float x_a, y_a, z_a, x_g, y_g, z_g;
int f_spi;


// Function declerations
static struct spi_ioc_transfer xfer;



int set_gpio_pin(int gpio_num, int direction){
    // export the GPIO pin to gpiolib
    gpio_export(gpio_num);
    
    // Setting direction as output
    gpio_direction(gpio_num, 1); 
    
    return 0;    
}

int write_to_gpio(int gpio_num, int value){
    
    gpio_write(gpio_num, value);
    
    return 0;
}
    
int init_spi(){
    int val = 0;
    int iSPIMode = SPI_MODE_0;
    int spi_freq = 2000000;
    f_spi = open("/dev/spidev1.1", O_RDWR);

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

void spi_write(uint8_t *pstring, uint8_t length){
    int ret_val = 0;
    ret_val = write_to_gpio(CS1, 0);

    if (ret_val != 0){
        perror("Failed to write the GPIO pin\n");
        exit(EXIT_FAILURE);
    }
    xfer.rx_buf = 0;
    xfer.tx_buf = (unsigned long)pstring;
    xfer.len = length;

    ret_val = ioctl(f_spi, SPI_IOC_MESSAGE(1), &xfer);

    ret_val = write_to_gpio(CS1, 1);
    if (ret_val != 0){
        perror("Failed to write the GPIO pin\n");
        exit(EXIT_FAILURE);
    }

    return;
}


int main()
{
    int val = 0;
    uint8_t buf[16];


    val = init_spi();
    if (val == -1){
        perror("Failed to initialise SPI bus\n");
        return -1;
    }

    set_gpio_pin(CS0, GPIO_OUT); // CS to accelerometer set tp output
    write_to_gpio(CS1, 1); // To switch the accelerometer to SPI, it must detect a rising edge on the CS pin

    // Configuring the IMU

    /* Accelerometer*/
    buf[0] = (ACCEL_CONF << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x08; // ODR 100Hz
    spi_write(buf, 3);

    buf[0] = (ACCEL_INT_CONF << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x0A; // Initializing the INT pin 1
    spi_write(buf, 3);

    buf[0] = (ACCEL_IO_MAP << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x04; // IO map mapped to int pin 1
    spi_write(buf, 3);

    buf[0] = (ACCEL_RANGE << 1U)| 0U;
    buf[1] = 0x55;
    buf[2] = 0x03; // Accel range set to 16G
    spi_write(buf, 3);

    /* Gyroscope */
    buf[0] = (GYRO_RANGE << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x02; // Setting gyro range to +/- 500 dps
    spi_write(buf, 3);

    buf[0] = (GYRO_BW << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x07; // ODR Frequency to 100 Hz
    spi_write(buf, 3);

    /* Powering on the Accelerometer*/
    buf[0] = (REG_ACCEL_PWR_CTRL << 1U) | 0U;
    buf[1] = 0x55;
    buf[2] = 0x04; // This starts the accelerometer from sleep mode
    spi_write(buf, 3);
    sleep(50/1000000); // Sleep for 50 microseconds

    while(val != -1){
    /* Start the accelerometer readings */
    buf[0] = (0x12 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x12;
    spi_write(buf, 3);
    x_a = (int16_t)buf[2];
    printf("X_A:%c ", buf[1]);
    buf[0] = (0x14 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x14;
    spi_write(buf, 3);
    y_a = (int16_t)&buf[2];

    buf[0] = (0x16 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x16;
    spi_write(buf, 3);
    z_a = (int16_t)buf[2];

    printf("Accel X:%d, Accel Y:%f, Accel Z: %f\n", buf[2], y_a, z_a);

    /* Start the gyroscope readings*/
    buf[0] = (0x02 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x02;
    x_g = (int16_t)buf[2];

    buf[0] = (0x04 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x04;
    y_g = (int16_t)buf[2];

    buf[0] = (0x06 << 1U) | 1U;
    buf[1] = 0x55;
    buf[2] = 0x06;
    z_g = (int16_t)buf[2];

    printf("Gyro X:%f, Gyro Y:%f, Gyro Z:%f\n", x_g, y_g, z_g);
    }

    return 0;
}
