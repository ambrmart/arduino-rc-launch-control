#include <Servo.h>

Servo esc;  // Create a Servo object for the ESC

const int receiverPin = 2;  // Pin where the RC receiver is connected
const int escPin = 9;       // Pin where the ESC signal wire is connected

// Setup and settings
const boolean calibration = true;     // Enables calibration mode
const boolean reverse_Signal = false; // Enable if max throttle calibration is not working
boolean enable_serial = false;        // Enable or disable serial output
int baseline_pwm = 1500;              // Baseline PWM signal (neutral position)
int max_pwm = 2000;                   // Full throttle PWM signal (Full throttle position)
int seconds_to_max = 4;               // Duration in seconds from threshold_pwm to max_pwm
float threshold_percent = 0.2;        // Threshold percent above which acceleration is linear

// Don't touch
int current_pwm;                   // Calculated PWM value
int increment;                     // PWM increment per iteration
int target_pwm;                    // Target PWM signal based on throttle position
int threshold_pwm;                 // Threshold above which acceleration is linear
const int interval = 40;           // Interval between increments (in milliseconds)
unsigned long previousMillis = 0;  // Stores the last time the PWM was updated

void setup() {
  pinMode(receiverPin, INPUT);
  esc.attach(escPin);  // Attach the ESC to the appropriate pin
  if (enable_serial || calibration) {
    Serial.begin(9600);  // Start serial communication at 9600 baud rate
  }

  if (calibration) {

    Serial.println("======== Running Calibration Process ========");
    Serial.println();

    target_pwm = pulseIn(receiverPin, HIGH, 25000);  // Reads in microseconds

    Serial.print("Waiting for transmitter.");

    while (target_pwm <= 1000) {
      delay(1000);
      Serial.print(".");
      target_pwm = pulseIn(receiverPin, HIGH, 25000);
    }
    Serial.println(".");
    Serial.println("");
    Serial.print("Connection detected, waiting for signal to stabilize.");
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.println(".");
    delay(1000);
    Serial.println("");
    Serial.println("Calculating neutral PWM signal.");

    int signalReading;  // Variable to store each reading
    long total = 0;     // Variable to store the sum of readings

    // Take neutral readings
    for (int i = 0; i < 500; i++) {
      signalReading = pulseIn(receiverPin, HIGH, 25000);
      total += signalReading;
    }

    int averageNeutral = total / 500;  // Calculate the average neutral

    Serial.println("");
    Serial.print("Set the transmitter to maximum throttle and hold until calibration is over.");

    if (reverse_Signal) {
      while (pulseIn(receiverPin, HIGH, 25000) > averageNeutral - 350) {
        delay(1000);
        Serial.print(".");
      }
    } else {
      while (pulseIn(receiverPin, HIGH, 25000) < averageNeutral + 350) {
        delay(1000);
        Serial.print(".");
      }
    }

    Serial.println(".");
    delay(1000);
    Serial.println("");
    Serial.println("Calculating maximum throttle PWM signal.");

    int throttleMax = averageNeutral;

    if (reverse_Signal) {
      for (int i = 0; i < 500; i++) {
        signalReading = pulseIn(receiverPin, HIGH, 25000);
        if (signalReading < throttleMax) {
          throttleMax = signalReading;  // Setting maximum throttle PWM signal
        }
      }
    } else {
      for (int i = 0; i < 500; i++) {
        signalReading = pulseIn(receiverPin, HIGH, 25000);
        if (signalReading > throttleMax) {
          throttleMax = signalReading;  // Setting maximum throttle PWM signal
        }
      }
    }

    // Print the average readings
    Serial.println("");
    Serial.println("Calibration finished!");
    Serial.print("baseline_pwm: ");
    Serial.println(averageNeutral);
    Serial.print("max_pwm: ");
    Serial.println(throttleMax);
    Serial.println("Set the values above, disable calibration and upload the sketch.");

    while (true) {
      // Infinite loop to stop further execution
    }
  }
  // Set calculation default value
  current_pwm = baseline_pwm;

  // Calculate threshold_pwm based on threshold_percent
  if (reverse_Signal) {
    threshold_pwm = baseline_pwm - (baseline_pwm - max_pwm) * threshold_percent;
  } else {
    threshold_pwm = (max_pwm - baseline_pwm) * threshold_percent + baseline_pwm;
  }


  // Calculate increment based on seconds_to_max and interval
  if (reverse_Signal) {
    increment = (threshold_pwm - max_pwm) / (seconds_to_max * 1000 / interval);
  } else {
    increment = (max_pwm - threshold_pwm) / (seconds_to_max * 1000 / interval);
  }
  delay(1000);  // Prevents Signaljitter on startup
}

void loop() {
  // Read the PWM signal from the RC receiver
  target_pwm = pulseIn(receiverPin, HIGH, 25000);  // Reads in microseconds

  // Get the current time in milliseconds
  unsigned long currentMillis = millis();

  // Check if the interval in milliseconds has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Adjust the current_pwm
    if ((target_pwm > threshold_pwm && !reverse_Signal) || (target_pwm < threshold_pwm && reverse_Signal)) {
      // Ramp towards target_pwm if accelerating above threshold_pwm
      if (current_pwm < target_pwm && !reverse_Signal) {
        current_pwm += increment;
      } else if (current_pwm > target_pwm && reverse_Signal) {
        current_pwm -= increment;
      }
    } else {
      // Immediate response for throttle positions at or below threshold_pwm
      current_pwm = target_pwm;
    }

    // Send the adjusted PWM signal to the ESC in microseconds
    esc.writeMicroseconds(current_pwm);

    // Output the adjusted PWM signal for testing
    if (enable_serial) {
      Serial.print("Current PWM: ");
      Serial.println(current_pwm);
      Serial.print("Threshold PWM: ");
      Serial.println(threshold_pwm);
    }
  }
}
