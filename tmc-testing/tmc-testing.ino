#include <AccelStepper.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>


HardwareSerial MySerial(1);

#define RX           3
#define TX           1

#define RXD2             16
#define TXD2             17

#define R_SENSE          0.10f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
TMC2209Stepper driver(&Serial2, R_SENSE , DRIVER_ADDRESS);

#define STEP_PIN         15
#define DIR_PIN          14
#define ENABLE_PIN       27

AccelStepper stepper1(AccelStepper::DRIVER, 15, 14);

int  set_stall = 80;      //Do not set the value too high or the TMC will not detect it. Start low and work your way up.
long  set_tcools = 200;   // Set slightly higher than the max TSTEP value you see
int  set_current = 1000;

bool stalled_motor = false;
uint16_t motor_microsteps = 128;
long long current_position = 0;


void setup(){
	  pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, HIGH);
  	Serial.begin(115200);
    MySerial.begin(115200, SERIAL_8N1,RX, TX );
  	// Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // ESP32 can use any pins to Serial
  	driver.begin(); // Start all the UART communications functions behind the scenes
  	driver.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  	driver.blank_time(24); //Recommended blank time select value
  	driver.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  	driver.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  	driver.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  	driver.microsteps(motor_microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  	driver.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  	driver.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  	driver.shaft(true); // Set the shaft direction.
  	driver.en_spreadCycle(true); // enable SpreadCycle. We want spreadcycle becuase it is faster.
  	driver.pdn_disable(true); // Enable UART control
  	driver.VACTUAL(0); // Enable UART control
  	driver.rms_current(set_current);
  	// driver.SGTHRS(set_stall);
  	// driver.TCOOLTHRS(set_tcools);
    digitalWrite(ENABLE_PIN, LOW);
    stepper1.setMaxSpeed(3000.0);
    stepper1.setAcceleration(1000.0);
    stepper1.moveTo(10000);
  	

}

void loop(){
  

  if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
  stepper1.run();
}