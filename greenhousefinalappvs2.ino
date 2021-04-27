/* Jake Hamilton, Mobile apps Assesment 2 
Date: 25/5/21 */
#define APP_NAME "GrowBox Monitoring System"
String VERSION = "Version 2.0";
/***************************************************************
 * changes in version 2.0:
    *First version
***************************************************************/
// This #include statement was automatically added by the Particle IDE.
#include <HC_SR04.h>
// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_LiquidCrystal.h>
// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>
// This #include statement was automatically added by the Particle IDE.
#include <blynk.h>
/*****************************************************************
 * BLYNK setup
******************************************************************/
#define USE_BLYNK 
#ifdef USE_BLYNK 
//#include  - Add in the blynk UI 
char auth[] = "BLKTSI5RSupBmw1up3dFRd5E2iyUPi4h"; //The blynk token 
#define BLYNK_PRINT Serial // set serial outputs for debug prints
#define BLYNK_DEBUG //this shows the detailed prints.
#endif
/*****************************************************************
 * DHT22 Sensor Setup
*****************************************************************/
#define DHTPIN 2            // what pin we're connected to
#define DHTTYPE DHT22       // DHT 22 (AM2302)
#define INTERVAL 10000       // number of milliseconds between checks
                            // Reading temperature or humidity takes about 250 milliseconds!
                            // Sensor readings may also be up to 2 seconds 'old' (its a 
                            // very slow sensor)
/******************************************************************
 * RGB Fan setup 
******************************************************************/
#define BLUE 2 //RGB colour blue
#define GREEN 5//RGB colour Green
#define RED 6//RGB colour Red
#define delaytime 10//1o seconds delay time that the colour will change
/*******************************************************************
 * LCD Screen Setup
*******************************************************************/
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Initialise an object to represent the DHT22 sensor
DHT dht(DHTPIN, DHTTYPE);

// Declare global variables for the readings and the time last checked
double temp;
double hum;
unsigned long lastCheck = 0;

//Declaring global variables for the self automation water system
int Moist = 4;
int sense_Pin = 0;//THis is for the analog pin AO is connected to sensor input.
float value = 0;// This is for the value of the data and IF statement 

//Declaring global variables for the RGB Fan.
int redValue;
int greenValue;
int blueValue;

// Declaring global variables for the photoresistor
int RGBlight = A5;
int photoresistor = A0;
int analogValue; 
// Function that can be called from the Particle console
void DHTreadSensor() {
    hum = dht.getHumidity();
    temp = dht.getTempCelcius();
}

// Callback function to allow the Particle console access to readSensor()
int checkHandler(String command) {
DHTreadSensor();
return 1;
}

void PhotoResistor()
{
    analogValue = analogRead(photoresistor);
    
    if (analogValue > 15) 
    {
        digitalWrite(RGBlight, LOW);
    }
    else
    {
        digitalWrite(RGBlight, HIGH);
    }
}

void RGBfan()
{
    redValue = 255;
    greenValue = 0;
    blueValue = 0;
    
    for(int i = 0; i < 255; i += 1)
    {
        redValue -= 1;
        greenValue += 1;

        analogWrite(RED, redValue);
        analogWrite(GREEN, greenValue);
        delay(delaytime);
    }
    
    redValue = 0;
    greenValue = 255;
    blueValue = 0;

    for(int i = 0; i < 255; i += 1)
    {
        greenValue -= 1;
        blueValue += 1;

        analogWrite(GREEN,greenValue);
        analogWrite(BLUE, blueValue);
        delay(delaytime);
    }
    
    redValue = 0;
    greenValue = 0;
    blueValue = 255;


    for(int i = 0; i < 255; i += 1)
    {
        blueValue -= 1;
        redValue += 1;

        analogWrite(BLUE,greenValue);
        analogWrite(RED, blueValue);
        delay(delaytime);
    }
    
}

void LCDScreen()
{
    lcd.setCursor(0,1);//used for the LCD screen MUST TEST
    lcd.print(millis() / 1000);
}

void WaterSystem()
{
    Serial.print("MOISTURE LEVEL"); value = analogRead(sense_Pin);
    Serial.println(value);
    //IF statement checks to see if the moisture level is lower that 750 
    if(value>750)
    {
        digitalWrite(Moist, LOW);//will say that the moisture is low 
        
    }
    else 
    {
        digitalWrite(Moist, HIGH);//will say that the moisture is high.
        
        delay(1000);//Delays the check for 1000ms which is 1 min. 
    }
    
}

void DHT22Sensor()
{
     if (lastCheck + INTERVAL < millis()) {
        lastCheck = millis();
        DHTreadSensor();

        // Check if any reads failed and exit early (to try again).
        if (isnan(hum) || isnan(temp)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }

        Particle.publish("temperature", String(temp), PRIVATE);
        Particle.publish("humidity", String(hum), PRIVATE);
    }
}

void setup() {
     dht.begin();
    
    lcd.begin(16,2);//The aspect ration of the Screen
    lcd.print("Welcome, this is the current status of the Grow Box...");//A message that will show on the LCD Screen.

    // Declare variables for the DHT22 temp and hum sensor.
    Particle.variable("temperature", temp);
    Particle.variable("humidity", hum);
    Particle.function("readSensor", checkHandler);
    
    Serial.begin(9600);
    
    //Declearing variables that are pinModes and digital for Automation water system.
    pinMode(Moist, OUTPUT);
    pinMode(sense_Pin, INPUT);
    digitalWrite(Moist, HIGH);
    delay(500);
    //Declearing variables that are Pinmodes and digitalwrite for RGB fan.
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    //Making sure that the colour goes in high and low so it can chnange colour. 
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);

}

void loop() {
    
    Blynk.run();
    DHT22Sensor();
    PhotoResistor();
    RGBfan();
    WaterSystem();
    LCDScreen();
    
}