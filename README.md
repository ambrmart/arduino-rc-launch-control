# arduino-rc-lauch-control
Linear acceleration control for rc speedruns and drag using an Arduino Nano.

I randomly came across [this](https://www.youtube.com/watch?v=zmyIdEzfCtM) on youtube and expected it to be sub 10,00 €.
When I saw the actual price I wanted to see if I could replicate this using an Arduino Nano.

### Setup:
1. **Calibrate your ESC!!**
2. Upload the sketch to a Arduino Nano (Calibration mode is enabled by default) and unplug it.
3. Connect the the receiver's CH2 to the Arduino like this:
   ![Calibration wiring](https://github.com/ambrmart/arduino-rc-lauch-control/blob/main/img/img001.jpg)
4. Connect the Arduino to the computer and open the serial monitor. You should see the 'Waiting for transmitter'
5. Turn on the transmitter. It will now calculate the average neutral PWM signal.
6. Next is the maximum forward calibration. Pull the trigger to the maximum throttle position and hold it until the calibration is done. (If nothing happens when holding full throttle go to the top of the sketch, set `reverse_Signal = true`, turn of the transmitter, upload the modified sketch unplug USB cable and start from step 4).
7. After the calibration is done you have to go to the top of the sketch, set `baseline_pwm` and `max_pwm` to the calibration values and set `calibration = false`. You can also modify `seconds_to_max` and `threshold_percent` to your preference.
8. Upload the final sketch to the Arduino and unplug the receiver and USB connection.
9. Now wire the Arduino and now wire the arduino to the ESC and receiver like this:
    ![final wireing]()
