// *********************
// [Motorhome/Campervan Water Level], by John Halfords, the Netherlands
// This is Version 0.92 (22-09-2026)
// *********************
// What's new:
// -----------
// 0.9 First released version, not tested in Camper
// 0.91 Also suitable for the RCWL-1670 and small changes
// 0.92 Piezo speaker and alarm functions (set the amount of water you want to fill with the button, and the piezo speaker lets you know when to stop)
//      Button Short press = alarm on (for example) 10 Ltr, press again 20 Ltr, etc
//      Button Long press = sleep
// *********************
// -- Hardware --
// Display = OLED 0.91
// ESP8266 Wemos D1 Mini Pro (clone)
// JSN-SR04T or RCWL-1670 Ultrasonic Sensor
// *********************
// -- Connections --
// See Schematic.png
// :Display:
// VCC = 3.3V !!
// SDA = D2
// SCK or SCL = D1
// :Ultrasonic Sensor:
// VCC = 5V
// RX Trigger pin = D5 = GPIO14
// TX Echo pin = D6 = GPIO12
// :ESP:
// Button = D7 = GPIO13 (Long press = Sleep, short press is set alarm to fill tank)
// Internal Led = D4 = Power led
// Reset switch = Pin RST to ground
// Piezo speaker = D8 (via 1k resistor to ground, parallel to the piezo a zener diode of max. 3.3 Volts - anode to ground)
// *********************
// If you have any ideas, build-up comment or questions, feel free to contact me: halfordsj@gmail.com
// *********************

// Librarys to include
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Things to define
#define intLed D4 // Internal led
#define trigPin D5 // Triggerpin used for Ultrasonic Sensor
#define echoPin D6 // Echopin used for Ultrasonic Sensor
#define displayWidth 128 // Width of the Oled display
#define displayHeight 32 // Height of the Oled display
#define Button D7 // Button (Long press = Sleep, short press is set alarm to fill tank)
#define piezoPin D8 // Pin used for piezo speaker (beep)

// Set initial values for button press
volatile bool buttonPressed = false;
volatile unsigned long buttonPressStart = 0;
unsigned long lastShortPress = 0;
unsigned long alarmDisplayUntil = 0;
int Alarm = 0; // Startvalue for the waterlevel alarm, 0 means No alarm

// Constants to declare 
const int Press2Sleep = 2000; // Time in milliseconds to press the button (1000 = 1 second) to go to sleep
const int MaxMeasureVolume = 120; // Everything higher than this value will be displayed as [DisplayHighestValue] (next declare line), this way you can work around the eventual deadzone of the Ultrasonic Sensor
                                  // NOTE: This will also be the highest Alarm step !
const String DisplayHighestValue = ">120 Ltr"; // What will be displayed when the waterlevel becomes higher than the [MaxMeasureVolume]
const int MinMeasureVolume = 2; // Everything lower (including negative values) than this value will be displayed as [DisplayLowestValue] (next declare line)
const String DisplayLowestValue = "-Empty-"; // What will be displayed when the waterlevel becomes lower than the [DisplayLowestValue]
                                             // Advice: set the value of MaxMeasureVolume to a high value when calibrating
                                             // and set the value of MinMeasureVolume to a low negative value when calibrating
                                             // Then you see all the possible values and it's easier to calculate the calibration value
const int TankHeight = 43; // Height of the inside of the watertank in cm (from the bottom to the Ultrasonic Sensor)
const float CalibrationValue = 3.25581; // My tank is 43 cm high and can consist 140 Ltr, so 0 cm = 140 Ltr and 43 cm = 140 Ltr, 140/73 = 3.25581 (In other words: every cm = 3.25581 Ltr)
const int AlarmSteps = 10; // The amount of Liters you want each Alarm step to be, for example 10: then the steps are 10, 20, 30, etc until MaxMeasureVolume, press again and the alarm = 0 = disabled

// Variables to declare 
long duration; // The measured duration from trigger to echo
float volume; // The calculated corresponding watervolume of the camper watertank
float measurements[5]; // I put the measurements in an array to filter out wrong values en get the volume more stable and accurate
String volumeText; // The text that will be displayed

// Instances
Adafruit_SSD1306 display = Adafruit_SSD1306(displayWidth, displayHeight, &Wire); // Display

// Sub for handling interupt for button (needs to be mentioned before Setup)
void IRAM_ATTR ButtonISR() {
  if (!buttonPressed) {
    buttonPressed = true;
    buttonPressStart = millis();
    }
  }

// Setup
void setup() {
  pinMode(trigPin, OUTPUT); // Triggerpin used for Ultrasonic Sensor
  pinMode(intLed, OUTPUT); // Triggerpin used for Ultrasonic Sensor
  pinMode(echoPin, INPUT); // Echopin used for Ultrasonic Sensor
  pinMode(Button, INPUT_PULLUP); // Button
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Start display instance
  delay(2000);
  display.clearDisplay(); // Clear display buffer
  display.setTextSize(3); // Set display text size to 3
  display.setTextColor(WHITE); // Set display text color to white
  display.setRotation(0); // Set display rotation to 0
  display.display(); // Show cleared display buffer
  
  // Set (interrupt) action on button release
  attachInterrupt(digitalPinToInterrupt(Button), ButtonISR, FALLING);
}
 
void loop() {

  checkButton(); // Check if Button is pressed

  digitalWrite(intLed, LOW); // Power Led ON

  for (int i = 0; i < 5; i++) {
    // Send triggersignal via ultrasoon
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5); // was 5
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // was 10
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 100000); // Measure time from send to receive

    measurements[i] = (duration * 0.034 / 2) + 1; // Calculate from time to centimeters
    delay(50);
    }

  // Filter/Sort measurements for stability
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 5; j++) {
      if (measurements[j] < measurements[i]) {
        float temp = measurements[i];
        measurements[i] = measurements[j];
        measurements[j] = temp;
      }
    }
  }

  // Median value is nr. 2 in the array, because of the sort
  volume = measurements[2];

  // Convert distance (still in cm) to liters
  volume = (TankHeight - volume) * CalibrationValue;

  // Value to "xx Lt", DisplayLowestValue or DisplayHighestValue
  if (volume <= MinMeasureVolume) {
    volumeText = DisplayLowestValue;
  }
  else if (volume >= MaxMeasureVolume) {
    volumeText = DisplayHighestValue;
  }
  else {
    volumeText = String(int(round(volume))) + " Ltr";
  }
  // Display the volume in Ltr
  if (millis() >= alarmDisplayUntil) { // Do this only if set value is not still displayed (1,5 sec)
    display.clearDisplay();
    display.setCursor(0, 6);
    display.print(volumeText);
    if (buttonPressed) display.fillCircle(126, 30, 1, WHITE); // Small white dot in display to indicate button is pressed
    display.display();
  }

  // Beep when tank level is at value of Alarm
  if(Alarm != 0 && volume >= Alarm) {
    piezoBeep(150, 3800);
    piezoBeep(150, 3700);
  }
  // Beep when tank level is at value - AlarmSteps of Alarm
  if(Alarm != 0 && volume <= Alarm && volume >= (Alarm - AlarmSteps)) {
    piezoBeep(50, 3600);
    piezoBeep(50, 3500);
  }
  // Beep when tank level is at value - 2xAlarmSteps of Alarm
  if(Alarm != 0 && volume <= Alarm && volume <= (Alarm - AlarmSteps) && volume >= (Alarm - (2 * AlarmSteps))) {
    piezoBeep(30, 3500);
  }
}

///////////////////////////////////// Subs ////////////////////////////////

// Sub for checking if the Button is pressed
void checkButton() {

  // Long press = Sleep, immediately after Press2Sleep time
  if (buttonPressed && millis() - buttonPressStart >= Press2Sleep) {

    noInterrupts(); // Switch off Interrups
    buttonPressed = false; // Reset
    interrupts(); // Switch on Interrups

    // Long press = Say 'Bye', blink and goto Sleep
    display.clearDisplay();
    display.setCursor(0, 6);
    display.print("..Bye..");
    display.display();

    for (int count = 0; count <= 10; count++) {
      digitalWrite(intLed, HIGH);
      delay(100);
      digitalWrite(intLed, LOW);
      delay(100);
    }

    display.clearDisplay();
    display.display();
    delay(200);

    ESP.deepSleep(0); // Goto sleep
  }

  // Button released, so it's a short press
  if (buttonPressed && digitalRead(Button) == HIGH) { // If the button is pressed and released...

    noInterrupts(); // Switch off Interrups
    buttonPressed = false; // Reset
    unsigned long pressduration = millis() - buttonPressStart; // Calculate time of press
    interrupts(); // Switch on Interrups

    // Short press = Set the alarm
    if (pressduration < Press2Sleep) {

      // First new press only displaying the last set alarm value
      if (millis() - lastShortPress <= 1500) { // If press is released within 1,5 sec it's a short press
        if (volume >= Alarm + AlarmSteps) { // If there is already water in the tank it's no use of setting al lower value than then the present volume...
          Alarm = ((int)(volume / 10) + 1) * 10; // ...so start with the next step of 10
        }
        else Alarm += AlarmSteps; // Add 1 AlarmStep to Alarm

        if (Alarm > MaxMeasureVolume) { // If the next step is higher than the MaxMeasureVolume...
          Alarm = 0; // ...start from 0
        }
      }

      lastShortPress = millis(); // save time of lastShortPress
      alarmDisplayUntil = millis() + 1500; // calculate time untill when the alarm setting had to be displayed

      // Display the Set value for the alarm
      display.clearDisplay();
      display.setCursor(0, 6);
      display.print("Set " + String(Alarm));
      display.display();
      piezoBeep(10, 3000);
    }
  }
}

// Sub for the piezo beep
void piezoBeep(int howLong, int howHigh) {
tone(piezoPin, howHigh); // Start the beep with frequentie howHigh
delay(howLong); // keep beeping for howLong milliseconds
noTone(piezoPin);     // Switch off beep
}
