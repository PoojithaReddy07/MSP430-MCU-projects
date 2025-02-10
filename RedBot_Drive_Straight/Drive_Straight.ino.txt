/***********************************************************************
 * Drive straight forward up to 48 inches
 ***********************************************************************/
#include <RedBot.h>

// Initialize motor and encoder objects
RedBotMotors motors;
RedBotEncoder encoder = RedBotEncoder(A2, 10);

// Constants and variables
int buttonPin = 12;                                         // Pin for the start button
int countsPerRev = 192;                                     // Encoder counts per wheel revolution (4 pairs of N-S x 48:1 gearbox)
float wheelDiam = 2.56;                                     // Wheel diameter in inches (65mm / 25.4 mm/in)
float wheelCirc = PI * wheelDiam;                           // Wheel circumference in inches

void setup() 
{
  // Initialize button pin and serial communication
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() 
{
  // Start driving straight when the button is pressed
  if (digitalRead(buttonPin) == LOW) 
  {
    delay(2000);                                            // Wait 2 seconds after button press
    driveStraight(48, 150);                                 // Drive 48 inches at motor power 150
  }
}

/***********************************************************************
 * Function to drive the RedBot straight for a specified distance
 * Parameters:
 *   - distance: Distance to drive in inches
 *   - motorPower: Power level for the motors (0-255)
 ***********************************************************************/

void driveStraight(float distance, int motorPower) 
{
  float numRev;                                             // Number of wheel revolutions required
  long lCount = 0, rCount = 0;                              // Left and right encoder counts
  long targetCount;                                         // Target encoder count for the specified distance
  long prevlCount = 0, prevrCount = 0;                      // Previous encoder counts
  long lDiff, rDiff;                                        // Difference in encoder counts between cycles
  int leftPower = motorPower, rightPower = motorPower;      // Motor power levels
  int offset = 5;                                           // Adjustment value for motor power correction

  // Calculate the target number of encoder counts
  numRev = distance / wheelCirc;                            // Number of wheel revolutions needed
  targetCount = numRev * countsPerRev;                      // Target encoder count

  // Clear encoder counts and start motors
  encoder.clearEnc(BOTH);
  motors.drive(motorPower);

  // Loop until the target encoder count is reached
  while (rCount < targetCount) 
  {
    // Get current encoder counts
    lCount = encoder.getTicks(LEFT);
    rCount = encoder.getTicks(RIGHT);

    // Drive motors with adjusted power levels
    motors.leftDrive(leftPower);
    motors.rightDrive(rightPower);

    // Calculate the difference in encoder counts since the last cycle
    lDiff = (lCount - prevlCount);
    rDiff = (rCount - prevrCount);

    // Store current counts for the next cycle
    prevlCount = lCount;
    prevrCount = rCount;

    // Adjust motor power to maintain straight-line motion
    if (lDiff > rDiff) 
    {
      // Left wheel is faster, slow it down and speed up the right wheel
      leftPower -= offset;
      rightPower += offset;
    } 
    else if (lDiff < rDiff) 
    {
      // Right wheel is faster, slow it down and speed up the left wheel
      leftPower += offset;
      rightPower -= offset;
    }

    delay(35);                                              // Short delay to allow motors to respond
  }

  // Stop the motors after reaching the target distance
  motors.brake();

  // Infinite loop to stop further execution
  while (true);
}