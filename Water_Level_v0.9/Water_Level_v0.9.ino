
// *********************
// [Motorhome/Campervan Water Level], by John Halfords, the Netherlands
// This is Version 0.9 (27-08-2026)
// *********************
// What's new:
// -----------
// 0.9 First released version, not tested in Camper
// *********************
// -- Hardware --
// Display = OLED 0.91
// ESP8266 Wemos D1 Mini Pro (clone)
// JSN-SR04T Ultrasonic Sensor
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
// SleepButton = D7 = GPIO13 (Long press = Sleep)
// Internal Led = D4 = Power led
// Reset switch = Pin RST to ground
// *********************
// If you have any ideas, build-up comment or questions, feel free to contact me: halfordsj@gmail.com
// *********************

// Librarys to include
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Things to define
#define intLed D4 // Internal led
#define trigPin D5 // Triggerpin used for JSN-SR04T Ultrasonic Sensor
#define echoPin D6 // Echopin used for JSN-SR04T Ultrasonic Sensor
#define displayWidth 128 // Width of the Oled display
#define displayHeight 32 // Height of the Oled display
#define SleepButton D7 // SleepButton

// Constants to declare 
const int Press2Sleep = 2000; // Time in milliseconds to press the RestartSleep button (1000 = 1 second)
const int MaxMeasureVolume = 99; // Everything higher than this value will be displayed as [DisplayHighestValue] (next define line), this has to do with the approx. 21 cm deadzone of the JSN-SR04T
const String DisplayHighestValue = ">99 Ltr"; // What will be displayed when the waterlevel comes within 21 cm (deadzone) of the JSN-SR04T
const int MinMeasureVolume = 3; // Everything lower (including negative values) than this value will be displayed as [DisplayLowestValue] (next define line)
const String DisplayLowestValue = "-Empty-"; // What will be displayed when the waterlevel comes lower than the [DisplayLowestValue]
// Advice: set the value of MaxMeasureVolume to a high value when calibrating
// and set the value of MinMeasureVolume to a low negative value when calibrating
// The you see all the possible values and it's easier to calculate the calibration value
const int TankHeight = 73; // Height of the inside of the watertank in cm
const float CalibrationValue = 1.91781; // My tank is 73 cm high and can consist 140 Ltr, so 0 cm = 140 Ltr and 73 cm = 140 Ltr, 140/73 = 1.91781 (In other words: every cm = 1.91781 Ltr)

// Variables to declare 
long duration; // The measured duration from trigger to echo
float volume; // The calculated corresponding watervolume of the camper watertank
float measurements[5]; // I put the measurements in an array to filter out wrong values en get the volume more stable and accurate
String volumeText; // The text that will be displayed

// Instances
Adafruit_SSD1306 display = Adafruit_SSD1306(displayWidth, displayHeight, &Wire); // Display
 
void setup() {
  pinMode(trigPin, OUTPUT); // Triggerpin used for JSN-SR04T Ultrasonic Sensor
  pinMode(intLed, OUTPUT); // Triggerpin used for JSN-SR04T Ultrasonic Sensor
  pinMode(echoPin, INPUT); // Echopin used for JSN-SR04T Ultrasonic Sensor
  pinMode(SleepButton, INPUT_PULLUP); // SleepButton
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Start display instance
  delay(2000);
  display.clearDisplay(); // Clear display buffer
  display.setTextSize(3); // Set display text size to 3
  display.setTextColor(WHITE); // Set display text color to white
  display.setRotation(0); // Set display rotation to 0
  display.display(); // Show cleared display buffer

  // Serial.begin(9600);// ---------------------------- temp for debugging ;-)
  }
 
void loop() {

  checkSleepButton(); // Check is SleepButton is pressed

  digitalWrite(intLed, LOW); // Power Led ON

for (int i = 0; i < 5; i++) {
  // Send triggersignal via ultrasoon
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); // Measure time from send to receive

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
  display.clearDisplay();
  display.setCursor(0, 6);
  display.print(volumeText);
  display.display();
}

// Sub for checking if the sleepbutton is pressed
void checkSleepButton() {
  if (digitalRead(SleepButton) == LOW)
  {
    display.clearDisplay();
    display.setCursor(0, 6);
    display.print("......."); // Show something so you know the button is pressed
    display.display();

    unsigned long start = millis();

    // Loop while pressed
    while (digitalRead(SleepButton) == LOW)
    {
      delay(10);
    }

    // After loop calculate how long it is pressed
    unsigned long pressduration = millis() - start;

    if (pressduration >= Press2Sleep) // If pressed long enough, Say "..Bye.." and goto Deep Sleep
    {
      display.clearDisplay();
      display.setCursor(0, 6);
      display.print("..Bye..");
      display.display();
      for (int count = 0; count <= 12; count++)
        {
          digitalWrite(intLed, HIGH); // Led OFF
          delay(150);
          digitalWrite(intLed, LOW); // Led ON
          delay(150);
        }
      display.clearDisplay();
      display.display();
      delay(150);
      ESP.deepSleep(0);
    }
    else
    {
      display.clearDisplay();
      display.setCursor(0, 6);
      display.print("noSleep"); // If not pressed long enough, comeback after saying "noSleep"
      display.display();
      delay(1000);
    }
  }
}
