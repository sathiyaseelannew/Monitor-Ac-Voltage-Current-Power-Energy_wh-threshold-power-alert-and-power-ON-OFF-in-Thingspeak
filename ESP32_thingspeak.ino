//Monitor Ac Voltage, Current, Power, Energy_wh, threshold power alert and power ON/OFF in Thingspeak website
#include <WiFi.h>
#include <ThingSpeak.h>

// Wi-Fi Credentials
const char* ssid = "Sathiyaseelan";
const char* password = "nosignal";

// ThingSpeak Configuration
WiFiClient client;
unsigned long channelID = 3134566;
const char* writeAPIKey = "O4RJA3XVRGHULY79";
const char* readAPIKey  = "YX5JHTK3V6MO73ZI";

// Pin Definitions
#define RELAY_PIN 2
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35
unsigned long lastTime;
// Variables
float voltage = 0, current = 0, power = 0,energy_wh=0,Vcc=3.3;
int samples=500;
void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Start with relay OFF

  Serial.println("\n Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}

void loop() {
  // --- measure Ac voltage and current ---
 voltage=voltage_measurement();
 current=Current_measurement();
  power = voltage * current;
  
    unsigned long now = millis();
  float elapsedHours = (now - lastTime) / 3600000.0;
  energy_wh += power * elapsedHours;
  lastTime = now;

  Serial.printf("\n V=%.2fV | I=%.2fA | P=%.2fW\n", voltage, current, power);

  // --- Upload sensor data to ThingSpeak ---
  ThingSpeak.setField(1, voltage);
  ThingSpeak.setField(2, current);
  ThingSpeak.setField(3, power);
  ThingSpeak.setField(4, energy_wh);
  ThingSpeak.setField(6, power);//threshold power alert indication

  int writeStatus = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (writeStatus == 200) Serial.println("Data uploaded to ThingSpeak");
  else Serial.println("Error uploading data");

  // --- Ensure Wi-Fi Connection Before Reading ---
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost, reconnecting...");
    WiFi.begin(ssid, password);
   
  }

  // --- Read relay control from ThingSpeak (Field 5) ---
  float controlValue = ThingSpeak.readFloatField(channelID, 5, readAPIKey);
  int readStatus = ThingSpeak.getLastReadStatus();

  Serial.print("ThingSpeak Read Status: ");
  Serial.println(readStatus);
  Serial.print("Control Value: ");
  Serial.println(controlValue);

  if (readStatus == 200) {
    if (controlValue == 1) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay ON");
    } 
    else if (controlValue == 0) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println(" Relay OFF");
    } 
    else {
      Serial.println(" Invalid control value");
    }
  } Serial.println(energy_wh/1000);//print how much energy consumed in units to predict bill
  delay(15000);
}
float voltage_measurement(){
   float sum = 0;
  float Vout, Vinst,Iinst;
  float sensitivity=0.007826;//0.007826v for 230v output
  for (int i = 0; i < samples; i++) {
    int adcValue = analogRead(VOLTAGE_PIN);
    Vout = (adcValue / 4095.0) * Vcc;   // Convert ADC to voltage
    Vinst =(Vout -(Vcc/2));       // Remove DC offset (center)offset voltage=3.3/2
   sum +=( Vinst * Vinst);              // Square and sum to remove negative sign
    delayMicroseconds(100);                   // Sampling delay for calibrations
  }

  float Vrms = sqrt(sum / samples);    // RMS voltage at sensor output
 float Vac =(Vrms / sensitivity); //output voltage the sensor is 20mV for 1v and transformer 
 return Vac;
}
float Current_measurement() {
  float sum = 0;
  float Vout, Iinst,sensitivity=0.066;

  for (int i = 0; i < samples; i++) {
    Vout = analogRead(CURRENT_PIN) * (Vcc / 4095.0);   // Convert ADC to voltage
    Iinst = (Vout -(Vcc/2)) / sensitivity;         // remove dc offset and Instant current 66mV for one ampere current
    sum +=(Iinst * Iinst);                             // Square of current to remove negative sign
    delayMicroseconds(100);                           // Small delay 
  }

  float Irms = sqrt(sum / samples);                   // RMS current
 return Irms;
}
