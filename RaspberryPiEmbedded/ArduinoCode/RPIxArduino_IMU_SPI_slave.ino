#include <SPI.h>
volatile byte count = 0; // In Arduino, byte and uint8_t are reffered to as the same
volatile bool init; // Variable for initializing and stopping a read operation
float inVal; // Variable to store the incoming byte as a float
uint8_t *ptr = (uint8_t *)&inVal; // Making a pointer to the float variable to avoid data type mixing 

void setup() {
  Serial.begin(9600); 
  pinMode(MISO, OUTPUT); // Setting the MISO as output

  SPCR |= _BV(SPE); // This command enables slave mode on the Arduino
  SPCR |= _BV(SPIE);
  init = false; 
  SPI.attachInterrupt(); // Enabeling interrupts

}

ISR (SPI_STC_vect) // Setting up interrupt routine
{
  ptr[count++]= SPDR; // Storing the recieved data from the SPI Data Register to the pointer which point to the float variable

  if (count == 2){
    init = true; // When the bytes are recieved, we set the print initialization to true 
  }
}
void loop() {
  if (init = true) {
    
    Serial.print("Acceleration X:"); Serial.println(value); // Printing the value variable, which contains the bytes sent by the RPI
    count = 0; // Resetting the byte counter
    init = false; // Putting init to false to indicate we want to read the next transfer
    }
}
