# arduino-rc-lauch-control
Linear acceleration control for rc speedruns and drag using an Arduino Nano.

I randomly came across [this](https://www.youtube.com/watch?v=zmyIdEzfCtM) on youtube and expected it to be sub 5,00 €.
When I saw the actual price I wanted to see if I could replicate this using an Arduino Nano. This is only missing the 'instant full throttle protection' and instead of using a trimpot, the delay is set in the code. I will design a 3d printed case for this at some point, but there are already thousands on the internet for projects like this.

## Adjustable parmeters
|Variable|Type|Description|
| -------- | ------- | ------- |
|calibration|boolean|Enables calibration mode|
|reverse_Signal|boolean|Enable if max throttle calibration is not working|
|enable_serial|boolean|Enables serial output (forced during calibration)|
|baseline_pwm|integer|Set manually after calibration|
|max_pwm|integer|Set manually after calibration|
|seconds_to_max|integer|Duration in seconds from threshold_pwm to max_pwm|
|threshold_percent|float|Threshold percent above which acceleration is linear|

## Setup:
1. **Calibrate your ESC!!**
2. Upload the sketch to a Arduino Nano (Calibration mode is enabled by default) and unplug it.
3. Connect the receiver's CH2 to the Arduino like this:
   |Receiver| Arduino Nano|
   | -------- | ------- |
   |CH2 Negative|GND|
   |CH2 Positiv|5V|
   |CH2 Signal|D2|
   
   ![Calibration wiring](https://github.com/ambrmart/arduino-rc-lauch-control/blob/main/img/img001.jpg)
5. Connect the Arduino to the computer via USB and open the serial monitor. You should see the 'Waiting for transmitter'
6. Turn on the transmitter and leave the throttle at neutral. It will now calculate the average neutral PWM signal.
7. Next is the maximum forward calibration. Pull the trigger to the maximum throttle position and hold it until the calibration is done.
   - If nothing happens when holding full throttle go to the top of the sketch, set `reverse_Signal = true`, turn of the transmitter, upload the modified sketch unplug USB cable and go back to step 4.
8. After the calibration is done you have to go to the top of the sketch, set `baseline_pwm` and `max_pwm` to the calibration values and set `calibration = false`.
9. Upload the modified sketch to the Arduino and unplug the receiver and USB connection.
10. Wire the Arduino to the ESC and receiver like this:
    ![final wireing](https://github.com/ambrmart/arduino-rc-lauch-control/blob/main/img/img002.jpg)
11. Done, enjoy!

## Demonstration:
_coming soonish .._
