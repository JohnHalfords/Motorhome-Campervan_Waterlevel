
# Motorhome/Campervan Water Level #  
### by John Halfords, the Netherlands ###  

<p>Thanks to IOT Projects Ideas<br>
(https://iotprojectsideas.com/waterproof-ultrasonic-sensor-with-arduino-to-measure-water-level/)</p>  

---

### What's new:  
* 0.9 First released version  

---

## Hardware  

---

* CPU: ESP8266 Wemos D1 Mini Pro (V3.0.0)  
https://nl.aliexpress.com/item/1005006246625522.html  

* Ultrasonic Sensor: JSN-SR04T  
https://www.amazon.nl/dp/B0DDKXCCRH?ref=ppx_yo2ov_dt_b_fed_asin_title 

* Display: I2C OLED Display Module 0.91 Inch (SSD1306)  
https://nl.aliexpress.com/item/1005006365845676.html  

## Connections

---
![Schematic](/Schematic.png)  

*See Schematic.png*  

**Display:**  
- VCC = 3.3V !!  
- SDA = D2  
- SCK or SCL = D1  

**Ultrasonic Sensor:**
- VCC = 5V  
- RX Trigger pin = D5 = GPIO14  
- TX Echo pin = D6 = GPIO12  

**Sleep**
- When connecting a USB-C plug, switch off the 5v is not simple in some cases  
- and you don't want to pull out you USB-C cable every time, I've made a sleep function  
- Long press = Sleep  
- The unit starts again with a hard reset  
- SleepButton = D7 = GPIO13 (See Schematic)  

**ESP8266**
- Internal Led = D4  
- Reset switch = Pin RST to ground  

### If you have any ideas, build-up comment or questions, feel free to contact me: halfordsj@gmail.com  