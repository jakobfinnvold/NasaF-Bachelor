#include <PID_v1.h>

#include <Wire.h>
#include <MPU9250_WE.h>
#include <Servo.h>


#define MPU6500_ADDR 0x68
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;

// ------ PID ------
double pitchSetpoint, pitchInput, pitchOutput;
double rollSetpoint, rollInput, rollOutput;
double Kp = 1, Ki = 0.01, Kd = 0;

PID myPID(&pitchInput, &pitchOutput, &pitchSetpoint, Kp, Ki, Kd, REVERSE); // Motor 1+2 is CCW
PID myPID2(&rollInput, &rollOutput, &rollSetpoint, Kp, Ki, Kd, REVERSE); // Motor 3+4 is CW

// Globals
int speed;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  delay(200);

  Motor1.attach(3, 1000, 2000);
  Motor2.attach(9, 1000, 2000);
  Motor3.attach(10, 1000, 2000);
  Motor4.attach(11, 1000, 2000);

  delay(50);
  myMPU6500.autoOffsets();
  myMPU6500.enableGyrDLPF();
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6); // Low pass filter setting for lowest noise
  myMPU6500.setSampleRateDivider(5);
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  myMPU6500.enableAccDLPF(true);
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);

  delay(200);

  pitchSetpoint = 0.0;
  rollSetpoint = 0.0;
  myPID.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  
}

void loop() {
  xyzFloat gValue = myMPU6500.getGValues();
  xyzFloat gyr = myMPU6500.getGyrValues();
  xyzFloat angels = myMPU6500.getAngles();
  
  float pitch = -1 * myMPU6500.getPitch();
  float roll = -1 * myMPU6500.getRoll();
  

  Serial.print("Pitch :"); Serial.println(pitch);
  Serial.print("Roll:"); Serial.println(roll); 

  speed = analogRead(A0);
  speed = map(speed, 0, 1023, 0, 180); // PWM converted to servo signal for the motors
  pitchInput = pitch;
  rollInput = roll; 
  int pulse, pulse1, pulse2, pulse3;
  myPID.Compute();
  myPID2.Compute();
  pulse = speed + pitchOutput - rollOutput;
  pulse1 = speed - pitchOutput + rollOutput;
  pulse2 = speed + pitchOutput + rollOutput;
  pulse3 = speed - pitchOutput - rollOutput;
  
  Motor1.write(pulse);
  Motor2.write(pulse1);
  Motor3.write(pulse2);
  Motor4.write(pulse3);  

}
