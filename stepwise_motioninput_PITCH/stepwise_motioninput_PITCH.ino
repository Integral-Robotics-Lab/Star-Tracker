#include <Stepper.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
int y;
const int t=240000;
const int vecy=26;
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the number of steps per revolution and the pins connected to the stepper motor
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);  // Assuming stepper motor is connected to pins 8, 9, 10, and 11

MPU6050 mpu;

int initialPitch = 0;
int initialYaw = 0;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  // Set the speed of the stepper motor
  myStepper.setSpeed(10);  // Set speed to 10 steps per second

  Wire.begin();

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Initialize MPU6050 sensor
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // Calibrate gyro
  mpu.calibrateGyro();

  // Read initial gyro values
  initialPitch = getPitch();
  initialYaw = getYaw();
}

void loop() {
  gyro_display();
  // Check if there's data available to read from the serial port
  if (Serial.available() > 0) {
    // Read the number of steps from the serial input
    int desiredSteps = Serial.parseInt();
    y=desiredSteps*vecy;

    // Move the stepper motor to the desired number of steps
    myStepper.step(y);

    // Optional: You might want to add a delay or perform other actions after the motor has moved
    delay(1000);  // Adjust delay as needed
    while(true){
      gyro_display();
      track();
    }
  }
}

void gyro_display(){
  // Read accelerometer and gyroscope data
  int pitch = getPitch() - initialPitch;
  int yaw = getYaw() - initialYaw;

  // Clear previous values on the display
  display.clearDisplay();

  // Print yaw and pitch values on the display
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Yaw: ");
  display.print(yaw);
  display.println(" degrees");

  display.setCursor(0, 10);
  display.print("Pitch: ");
  display.print(pitch);
  display.println(" degrees");

  // Display the updated information
  display.display();

  // Print yaw and pitch values on serial monitor
  Serial.print("Yaw: ");
  Serial.print(yaw);
  Serial.print(" degrees, Pitch: ");
  Serial.print(pitch);
  Serial.println(" degrees");

  delay(100); // Adjust delay as needed
}

int getPitch() {
  Vector rawAccel = mpu.readRawAccel();
  return atan2(-rawAccel.YAxis, sqrt(rawAccel.XAxis * rawAccel.XAxis + rawAccel.ZAxis * rawAccel.ZAxis)) * 180 / PI;
}

int getYaw() {
  Vector rawAccel = mpu.readRawAccel();
  return atan2(rawAccel.XAxis, sqrt(rawAccel.YAxis * rawAccel.YAxis + rawAccel.ZAxis * rawAccel.ZAxis)) * 180 / PI;
}
void track(){
  delay(t);
  myStepper.step(vecy);
}