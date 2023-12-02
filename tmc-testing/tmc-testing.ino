#include <AccelStepper.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
// #include <SoftwareSerial.h>


// HardwareSerial::mySerial(1);
// HardwareSerial SerialPort(1);
HardwareSerial SerialPort(1);

#define RX           4
#define TX           2

#define RXD2             16
#define TXD2             17

#define R_SENSE          0.10f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
TMC2209Stepper driver1(&SerialPort, R_SENSE , DRIVER_ADDRESS);

TMC2209Stepper driver2(&Serial2, R_SENSE , DRIVER_ADDRESS);

#define STEP_PIN1         15
#define DIR_PIN1          14
#define ENABLE_PIN1       27

#define STEP_PIN2         18
#define DIR_PIN2          19
#define ENABLE_PIN2       5

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN1, DIR_PIN1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN2, DIR_PIN2);

int  set_current = 800;

bool stalled_motor = false;
uint16_t motor_microsteps = 16;
long long current_position = 0;


void setup(){
	  pinMode(ENABLE_PIN1, OUTPUT);
    pinMode(ENABLE_PIN2, OUTPUT);

    digitalWrite(ENABLE_PIN1, HIGH);
    digitalWrite(ENABLE_PIN2, HIGH);

  	Serial.begin(115200);
    // Serial1.begin(115200, SERIAL_8N1,RX,TX);
  	SerialPort.begin(115200, SERIAL_8N1, RX, TX);
  	driver1.begin(); // Start all the UART communications functions behind the scenes
  	driver1.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  	driver1.blank_time(24); //Recommended blank time select value
  	driver1.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  	driver1.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  	driver1.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  	driver1.microsteps(motor_microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  	driver1.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  	driver1.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  	driver1.shaft(true); // Set the shaft direction.
  	driver1.en_spreadCycle(false); // disable SpreadCycle.We want stealthchop becuase it is quieter. 
  	driver1.pdn_disable(true); // Enable UART control
  	driver1.VACTUAL(0); // Enable UART control
  	driver1.rms_current(set_current);
    digitalWrite(ENABLE_PIN1, LOW);

    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // ESP32 can use any pins to Serial
    driver2.begin(); // Start all the UART communications functions behind the scenes
  	driver2.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  	driver2.blank_time(24); //Recommended blank time select value
  	driver2.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  	driver2.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  	driver2.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  	driver2.microsteps(motor_microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  	driver2.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  	driver2.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  	driver2.shaft(true); // Set the shaft direction.
  	driver2.en_spreadCycle(false); // disable SpreadCycle.We want stealthchop becuase it is quieter. 
  	driver2.pdn_disable(true); // Enable UART control
  	driver2.VACTUAL(0); // Enable UART control
  	driver2.rms_current(set_current);
    digitalWrite(ENABLE_PIN2, LOW);


    stepper1.setMaxSpeed(1500.0);
    stepper1.setAcceleration(1000.0);
    stepper1.moveTo(2000);

    stepper2.setMaxSpeed(1500.0);
    stepper2.setAcceleration(1000.0);
    stepper2.moveTo(2000);
  	

}

void loop(){
  if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
  stepper1.run();

  if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
  stepper2.run();
}