
# Motorhome/Campervan Water Level #  
### by John Halfords, the Netherlands ###  

<p>Thanks to IOT Projects Ideas<br>
(https://iotprojectsideas.com/waterproof-ultrasonic-sensor-with-arduino-to-measure-water-level/)</p>  

---

### What's new:  
* 0.9 First released version  
* 0.91 Also suitable for the RCWL-1670 and small changes
* 0.92 Piezo speaker and alarm functions (set the amount of water you want to fill with the button, and the piezo speaker lets you know when to stop)
*      Button Short press = alarm on (for example) 10 Ltr, press again 20 Ltr, etc
*      Button Long press = sleep
---

## Hardware  

* CPU: ESP8266 Wemos D1 Mini Pro (V3.0.0)  
https://nl.aliexpress.com/item/1005006246625522.html  

* Ultrasonic Sensor: JSN-SR04T  
https://www.amazon.nl/dp/B0DDKXCCRH?ref=ppx_yo2ov_dt_b_fed_asin_title 

* Display: I2C OLED Display Module 0.91 Inch (SSD1306)  
https://nl.aliexpress.com/item/1005006365845676.html  

---

## The code is highly commented!

---

![Schematic](/schematic_v0.92.png)  

## Connections

*See Schematic_v0.92.png*  

**Display:**  
- VCC = 3.3V !!  
- SDA = D2  
- SCK or SCL = D1  

**Ultrasonic Sensor:**
- VCC = 5V  
- RX Trigger pin = D5 = GPIO14  
- TX Echo pin = D6 = GPIO12  

**ESP8266**
- Internal Led = D4  
- Reset switch = Pin RST to ground  

## Functions

**Read waterlevel:**  
The display will show you the currect waterlevel.  
If your sensor has a dead zone you can set a MaxMeasureVolume constant. Every level within the deadzone will be displayed as what you set in the constant DisplayHighestValue.  
The lowest waterlevels can be inacurate depending on your water tank and sensor. Therefore you set the constant MinMeasureVolume. Every level lower than this value will be displayed as what you set in the constant DisplayLowestValue.  

**Sleep**  
When connecting a USB-C plug, switch off the 5v is not simple in some cases.  
And you don't want to pull out you USB-C cable every time, So i've made a sleep function.  
Button Long press = Sleep  
The unit starts again with a hard reset. (Sometimes you need to press this twice)  
SleepButton = D7 = GPIO13 (See Schematic)  

**Set alarm**  
Short press of the button gives you the possibility to set an acoustic alarm via a Piezo speaker.  
1st short press sets the alarm to the next ten above the current waterlevel.  
Every following press the alarm will be set a step higher.  
In the code you can set the size of the steps as well as the maximum level and the minimum level.  

### If you have any ideas, build-up comment or questions, feel free to contact me: halfordsj@gmail.com  

![Complete](/complete.jpg)
![Waterlevel reading](/read_waterlevel.jpg)
![Setting the Alarm](/set_alarm.jpg)
![Ultrasoon Sensor](/ultrasoon_sensor.jpg)