#define BLYNK_TEMPLATE_ID "TMPL6Wgb60DP3"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "KNXVCEfcQXwUXzgq1s6ZVECpU63spCec"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>

#define BLYNK_PRINT Serial

// WiFi Credentials
char auth[] = "KNXVCEfcQXwUXzgq1s6ZVECpU63spCec";
char ssid[] = "Sylgraph's Guest";  
char pass[] = "onlyforguest";  

// Relay & Sensor Pins
#define RELAY_1 D0
#define RELAY_2 D1
#define RELAY_3 D2  // Motor relay
#define RELAY_4 D3
#define BUZZER_PIN D5
#define WATER_SENSOR D7
#define GAS_SENSOR_PIN A0

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  
  // Set relay pins as output
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WATER_SENSOR, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);

  // Default all relays OFF
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Timer functions
  timer.setInterval(5000L, sendGasData);      // Check gas sensor every 1 sec
  timer.setInterval(5000L, waterLabelSensor); // Check water sensor every 1 sec
}

// 🔹 Blynk Button Control for Relays
BLYNK_WRITE(V0) {
  digitalWrite(RELAY_1, param.asInt() ? LOW : HIGH);
}
BLYNK_WRITE(V1) {
  digitalWrite(RELAY_2, param.asInt() ? LOW : HIGH);
}
BLYNK_WRITE(V2) {
  digitalWrite(RELAY_3, param.asInt() ? LOW : HIGH);
}
BLYNK_WRITE(V3) {
  digitalWrite(RELAY_4, param.asInt() ? LOW : HIGH);
}

// 🔹 Gas Sensor Function
void sendGasData() {
    int gasValue = analogRead(GAS_SENSOR_PIN);    
    int gasPercentage = map(gasValue, 0, 1023, 0, 100);
    
    Blynk.virtualWrite(V5, gasPercentage);
    Serial.print("Gas Level: ");
    Serial.print(gasPercentage);
    Serial.println("%");

    if (gasPercentage >= 50) {
        digitalWrite(BUZZER_PIN, HIGH);  // Turn ON buzzer if gas > 50%
        Blynk.logEvent("gas_alert", "Gas level exceeded 50%! Take action!");
    } else {
        digitalWrite(BUZZER_PIN, LOW);   // Turn OFF buzzer
    }
}

// 🔹 Water Level Sensor Function
void waterLabelSensor() {
  int waterValue = digitalRead(WATER_SENSOR);
  Serial.print("Water Level: ");
  Serial.println(waterValue);
  
  if (waterValue == HIGH) {
    digitalWrite(RELAY_3, HIGH);  // Turn OFF motor relay
    Serial.println("🚨 Water detected! Motor OFF!");
  } else {
    digitalWrite(RELAY_3, LOW);   // Motor can run
  }
  
  Blynk.virtualWrite(V6, waterValue);
}

void loop() {
  Blynk.run();
  timer.run();
}
