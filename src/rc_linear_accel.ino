#include <Servo.h>

Servo esc;  // Create a Servo object for the ESC

const int receiverPin = 2;  // Pin where the RC receiver is connected
const int escPin = 9;       // Pin where the ESC signal wire is connected

// Setup and settings
const boolean calibration = true;    // Enables calibration mode
const boolean reverseSignal = false; // Enable if max throttle calibration is not working
boolean enableSerial = false;        // Enables serial output (forced during calibration)
int baselinePwm = 1500;              // Baseline PWM signal (neutral position)
int maxPwm = 2000;                   // Full throttle PWM signal (full throttle position)
int secondsToMax = 4;                // Duration in seconds from thresholdPwm to maxPwm
float thresholdPercent = 0.2;        // Threshold percent above which acceleration is linear

// Don't touch
int currentPwm;                   // Calculated PWM value
int increment;                    // PWM increment per iteration
int targetPwm = 0;                // Target PWM signal based on throttle position
int thresholdPwm;                 // Threshold above which acceleration is linear
const int interval = 40;          // Interval between increments (in milliseconds)
unsigned long previousMillis = 0; // Stores the last time the PWM was updated

void setup() {
  pinMode(receiverPin, INPUT);
  if (enableSerial || calibration) {
    Serial.begin(9600);  // Start serial communication at 9600 baud rate
  }

  if (calibration) {
    Serial.println();
    Serial.println("======== Running Calibration Process ========");
    Serial.println();
    Serial.print("Waiting for transmitter.");

    while (targetPwm <= 1000) {
      delay(1000);
      Serial.print(".");
      targetPwm = pulseIn(receiverPin, HIGH, 25000);
    }

    Serial.println();
    Serial.println();
    Serial.print("Connection detected, waiting for signal to stabilize.");

    for (int i = 0; i < 4; i++) {
      delay(1000);
      Serial.print(".");
    }

    Serial.println();
    Serial.println();
    Serial.println("Calculating neutral PWM signal.");

    int signalReading;  // Variable to store each reading
    long total = 0;     // Variable to store the sum of readings

    // Take neutral readings
    for (int i = 0; i < 500; i++) {
      signalReading = pulseIn(receiverPin, HIGH, 25000);
      total += signalReading;
    }

    int averageNeutral = total / 500;  // Calculate the average neutral

    Serial.println();
    Serial.print("Set the transmitter to maximum throttle and hold until calibration is over.");

    while (true) {
      if (reverseSignal) {
        if (pulseIn(receiverPin, HIGH, 25000) <= averageNeutral - 350) {
          break;
        }
      } else {
        if (pulseIn(receiverPin, HIGH, 25000) >= averageNeutral + 350) {
          break;
        }
      }

      delay(1000);
      Serial.print(".");
    }

    delay(1000);
    Serial.println();
    Serial.println();
    Serial.println("Calculating maximum throttle PWM signal.");

    int throttleMax = averageNeutral;

    for (int i = 0; i < 500; i++) {
      signalReading = pulseIn(receiverPin, HIGH, 25000);
      if (reverseSignal) {
        if (signalReading < throttleMax) {
          throttleMax = signalReading;  // Setting maximum throttle PWM signal
        }
      } else {
        if (signalReading > throttleMax) {
          throttleMax = signalReading;  // Setting maximum throttle PWM signal
        }
      }
    }

    // Print the average readings
    Serial.println();
    Serial.println("Calibration finished!");
    Serial.print("baselinePwm: ");
    Serial.println(averageNeutral);
    Serial.print("maxPwm: ");
    Serial.println(throttleMax);
    Serial.println("Set the values above, disable calibration and upload the sketch.");

    while (true) {
      // Infinite loop to stop further execution after calibration is over
    }
  } else {
    esc.attach(escPin);  // Attach the ESC to the appropriate pin
  }
  // Set calculation default value
  currentPwm = baselinePwm;

  // Calculate thresholdPwm based on thresholdPercent
  if (reverseSignal) {
    thresholdPwm = baselinePwm - (baselinePwm - maxPwm) * thresholdPercent;
  } else {
    thresholdPwm = (maxPwm - baselinePwm) * thresholdPercent + baselinePwm;
  }

  // Calculate increment based on secondsToMax and interval
  if (reverseSignal) {
    increment = (thresholdPwm - maxPwm) / (secondsToMax * 1000 / interval);
  } else {
    increment = (maxPwm - thresholdPwm) / (secondsToMax * 1000 / interval);
  }
  delay(1000);  // Prevents Signaljitter on startup
}

void loop() {
  // Read the PWM signal from the RC receiver
  targetPwm = pulseIn(receiverPin, HIGH, 25000);  // Reads in microseconds

  // Get the current time in milliseconds
  unsigned long currentMillis = millis();

  // Check if the interval in milliseconds has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Adjust the currentPwm
    if ((targetPwm > thresholdPwm && !reverseSignal) || (targetPwm < thresholdPwm && reverseSignal)) {
      // Ramp towards targetPwm if accelerating above thresholdPwm
      if (currentPwm < targetPwm && !reverseSignal) {
        currentPwm += increment;
      } else if (currentPwm > targetPwm && reverseSignal) {
        currentPwm -= increment;
      }
    } else {
      // Immediate response for throttle positions at or below thresholdPwm
      currentPwm = targetPwm;
    }

    // Send the adjusted PWM signal to the ESC in microseconds
    esc.writeMicroseconds(currentPwm);

    // Output the adjusted PWM signal for testing
    if (enableSerial) {
      Serial.print("Current PWM: ");
      Serial.println(currentPwm);
      Serial.print("Threshold PWM: ");
      Serial.println(thresholdPwm);
    }
  }
}