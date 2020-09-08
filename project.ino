#include <SoftwareSerial.h>    // Allow serial communication on other digital pins of the Arduino
#define DEBUG true        // For debugging of code
#define SoilSensorPin A0    // Soil sensor pin
#include "DHT.h"                // DHT Library
#define DHTPIN 14       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Type of DHT sensor i.e. DHT 11
SoftwareSerial wifi_module(2,3); // Connect TX pin of esp to the pin 2 of Arduino and RX pin of esp to the pin 3 of Arduino

DHT dht(DHTPIN, DHTTYPE);   

int led = 7;      //LED for LDR
int red_led_pin = 9;  //LED for Ultrsonic sensor
int buzzer_pin = 10;  // Buzzer pin
int smoke_sensor_pin = 4;  // Smoke sensor pin
int sensorValue = analogRead(A1); // LDR Sensor 
int sensor = 13;  // Digital pin D7 sensor input for PIR
int trig = 5;    // Ultrasonc sensor
int echo = 6;    // Ultrasonic sensor
float SensorValue = 0;  // For soil sensor
int WATERPUMP = 8;  //motor pump connected to pin 8

long lecture_echo;    //Variable
long cm;           //Variable



void setup()
{
  Serial.begin(9600);     // begin the serial communication
  wifi_module.begin(9600); // Set the baudrate according to your esp8266
  pinMode(red_led_pin, OUTPUT);    // declaring LED pin as input for Ultrasonic sensor
  pinMode(led, OUTPUT);       // declaring LED pin as input for LDR
  pinMode(buzzer_pin, OUTPUT);      // declaring buzzer pin as input
  pinMode(smoke_sensor_pin, INPUT);    // declaring smoke sensor pin as input
  pinMode(sensor, INPUT);   // declaring sensor as input of PIR
  pinMode(trig, OUTPUT);   // declaring trig as output of Ultrasonc sensor
  digitalWrite(trig, LOW);   // declaring trig as low in starting of Ultrasonc sensor
  pinMode(echo, INPUT);     // declaring echo as input of Ultrasonc sensor
  pinMode(WATERPUMP, OUTPUT); //declaring waterpump as OUTPUT 
  
  dht.begin();      // Begins the DHT Sensor

    //   *******   The below lines will call the function and will set up a server at an IP address provided by the ESP. The ESP will then send the data at this IP address  ********
  esp8266_command("AT+RST\r\n",2000,DEBUG); // reset module
  esp8266_command("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  esp8266_command("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  esp8266_command("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  esp8266_command("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  
}

void loop()
{
  //  ******************* Temperature and Hunidity SENSOR *******************  //
  delay(1000);         // Wait a few seconds between measurements
  float h = dht.readHumidity();             // Reading Humidity value
  float t = dht.readTemperature();         // Read temperature as Celsius (the default)
  float f = dht.readTemperature(true);     // Read temperature as Fahrenheit (isFahrenheit = true)

  float hif = dht.computeHeatIndex(f, h);           // Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(t, h, false);     // Compute heat index in Celsius (isFahreheit = false)

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  if(hif > 110)            // Checking if heat index value is greater than 110F or not
  {
    tone(buzzer_pin, 1000, 200);        // If yes Buzzer will ON
    Serial.println("DANGER! Heat Index Value is Increasing Move Somewhere Else");
    digitalWrite(WATERPUMP, HIGH);      // And pump will also ON
    delay(1000);
  }
  else
  {
    noTone(buzzer_pin);           // If not nothing will happen
    Serial.println("Heat Index Value is Normal");
    digitalWrite(WATERPUMP, LOW);
    delay(1000);
  }
  
  
  
  //  ******************* SMOKE SENSOR *******************  //
  int analogSensor = analogRead(smoke_sensor_pin);               // reading smoke value from smoke sensor
  Serial.println("SMOKE SENSOR:");
  Serial.print("\t");
  Serial.println(analogSensor);
  if (analogSensor > 400)                      // Checking if smoke sensor value is greater than 400
  {
    tone(buzzer_pin, 1000, 200);            // then  buzzer will be ON
    Serial.println("DANGER! SMOKE Value is Increasing");
    delay(1000);
  }
  else
  {
    noTone(buzzer_pin);                  // Otherwise buzzer will be OFF
    Serial.println("Smoke Value is Normal");
    delay(1000);
  }
  


  //  ******************* LDR SENSOR *******************  //                    
  int sensorValue = analogRead(A1);             // reading LDR sensor values
  float voltage = sensorValue * (5.0 / 1023.0);    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  Serial.print("LDR SENSOR");
  Serial.print("\t");
  Serial.println(voltage);  
  
  if (voltage < 1)              // Checking if voltage is less than one 
  {
    digitalWrite(led, HIGH);          // then turn on LED
    delay(1000);
  }
  else
  {
    digitalWrite(led, LOW);         // Otherwise don't turn it on
    delay(1000);
  }


  //  ******************* PIR SENSOR *******************  //
  long state = digitalRead(sensor);      // Reading PIR sensor values
  
    if(state == HIGH)                  // If it is high
    {
    tone(buzzer_pin, 1000, 200);      // Then Buzzer will be ON
    Serial.println("Motion detected!");
    delay(1000);
  }
  else
  {
    noTone(buzzer_pin);             // Otherwise It will be OFF
    Serial.println("Motion absent!");
    delay(1000);
  }
  


   //  ******************* Ultrasonic SENSOR *******************  //
   digitalWrite(trig, HIGH);         // Sets the trigPin on HIGH state for 10 micro seconds
   delayMicroseconds(10); 
   digitalWrite(trig, LOW);          // Clears the trigPin
   lecture_echo = pulseIn(echo, HIGH);       // Reads the echoPin, returns the sound wave travel time in microseconds
   cm = lecture_echo / 58;                    // Calculating the distance
   Serial.print("Distance in cm : "); 
   Serial.println(cm); 
   if(cm < 5)                            // Checking is there any object near 5 cm
   {
      digitalWrite (red_led_pin, HIGH);      // If yes LED ON
      Serial.println("Animal detected!");
      delay(1000);
    }
    
    else {
      digitalWrite (red_led_pin, LOW);      // Otherwise it will be OFF
      Serial.println("Animal absent!");
      delay(1000);
      }
   

  //  ******************* SOIL SENSOR *******************  //
  for (int i = 0; i <= 100; i++) 
  { 
    SensorValue = SensorValue + analogRead(SoilSensorPin);    // Reading 100 values from soil sensor 
    delay(1); 
  } 
  SensorValue = SensorValue/100.0;           // Taking average for better Exact value
  Serial.println(SensorValue); 
  delay(30); 
   if(SensorValue < 20)                      // Checking If value is less than 20
   {
    tone(buzzer_pin, 1000, 200);           // If yes then buzzer will be ON
    Serial.println("Need Water!");
    digitalWrite(WATERPUMP, HIGH);         // and Pump will switch ON
    delay(1000);
  }
  else
  {
    noTone(buzzer_pin);              // Otherwise nothing will happen
    Serial.println("Normal!");
    digitalWrite(WATERPUMP, LOW);
    delay(1000);
  }
  


  
  //  ******************* WIFI MODULE *******************  //  
  if(wifi_module.available()) 
  {
    if(wifi_module.find("+IPD,"))
    {
     delay(1000);
     int connectionId = wifi_module.read()-48;   

 // ********************  BELOW CODE WILL PRINT ALL THE RESULT ON WEBPAGE  ********************
       
     String webpage = "<h1>FOREST FIRE ALERT AND SMART VILLAGE</h1>";

     // **********  TEMPERATURE AND HUMIDITY SENSOR  **********
     
      webpage +="<p>Temperature Value is ";        
      webpage += t;
      webpage +="</p>";
      webpage +="<p>Humidity Value is ";
      webpage += h;
      webpage +="</p>";
      webpage +="<p>Heat Index Value is ";
      webpage += hif;
      webpage +="</p>";

       if (hif > 110)                   
  {
    webpage +="<h9>DANGER! Heat Index Value is Increasing Move Somewhere Else</h9>";
  }
  else
  {
    webpage +="<h8>Heat Index Value is Normal</h8>";
  }


    // **********  SMOKE SENSOR  **********
  
      webpage +="<p>Smoke Value is ";       
      webpage += analogSensor;
      webpage +="</p>";

       if (analogSensor > 400)         
  {
    webpage +="<h13>DANGER! Smoke Value is Increasing Move Somewhere Else</h13>";
  }
  else
  {
    webpage +="<h12>Smoke Value is Normal</h12>";    
  }


    // **********  LDR SENSOR  **********
    
      webpage +="<p>Amount of Light is ";            
      webpage += voltage;
      webpage +="</p>";

      if (voltage < 1)                
  {
    webpage +="<h17>Lights will be ON</h17>";
  }
  else
  {
    webpage +="<h16>Lights will be OFF</h16>";
  }


    // **********  PIR sensor  **********
    
      webpage +="<p>PIR Sensor </p>";              
     
      if (state == HIGH)                
  {
    webpage +="<h17>DANGER! Birds are attacking the crops</h17>";
  }
  else
  {
    webpage +="<h16>Everything is normal</h16>";
  }
  

   // **********  ULTRASONIC SENSOR  **********
  
      webpage +="<p>Animal Distance is ";         
      webpage += cm;
      webpage +="</p>";

      
    if (cm < 5)                    
  {
    webpage +="<h21>DANGER! Animal is Attacking the Village Move Somewhere Else</h21>";
  }
  else
  {
    webpage +="<h20>No Animal Attacking</h20>";
  }


   // **********  SOIL SENSOR  **********
   
      webpage +="<p>Soil Moisture is ";     
      webpage += SensorValue;
      webpage +="</p>";

  if (SensorValue < 20)           
  {
    webpage +="<h25>DANGER! Soil Needs Water</h25>";
  }
  else
  {
    webpage +="<h24>Soil Moisture is Good</h24>";
  }

  
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     esp8266_command(cipSend,1000,DEBUG);
     esp8266_command(webpage,1000,DEBUG);
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId;            // append connection id
     closeCommand+="\r\n";
     esp8266_command(closeCommand,3000,DEBUG);
    }
  }
}

  //    ********  The code below will send the commands to the ESP and will print the output of the ESP on the serial monitor  ********
  
String esp8266_command(String command, const int timeout, boolean debug)
{
    String response = "";
    wifi_module.print(command); 
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(wifi_module.available())
      {
        char c = wifi_module.read(); 
        response+=c;
      }  
    }
  
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
