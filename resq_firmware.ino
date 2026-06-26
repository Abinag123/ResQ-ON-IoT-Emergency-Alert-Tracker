/*
  ResQ-ON IoT Collision Detection & Emergency Telemetry Firmware
  Microcontroller Target: ESP32 DevKit V1
  Peripherals: ADXL345 Accelerometer, NEO-6M GPS Receiver, SIM800L GSM Module
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Simulated Pins & Constants
const int ACCEL_THRESHOLD = 3.0; // Force threshold in Gs indicating collision
const char* FIREBASE_URL = "https://resq-on-iot-default-rtdb.firebaseio.com/collision_events.json";

// Mock hardware parameters
float mock_lat = 8.5241;  // Simulated Latitude (Trivandrum)
float mock_lng = 76.9366; // Simulated Longitude (Trivandrum)

void setup() {
  Serial.begin(115200);
  Serial.println("[SYSTEM INIT] Initializing ResQ-ON IoT modules...");
  
  // Setup Accelerometer & GPS
  Serial.println("[HARDWARE] ADXL345 Accelerometer connected and calibrated.");
  Serial.println("[HARDWARE] NEO-6M GPS receiver waiting for satellite lock...");
  
  // Initialize Mock GSM
  Serial.println("[GSM] SIM800L module connected. Signal level: 85%");
}

void trigger_gsm_alert(float lat, float lng) {
  /*
     Sends hardware command strings to the SIM800L GSM transceiver 
     to send a distress SMS with coordinates.
  */
  Serial.println("\n[GSM ALERT] Triggering emergency GSM SMS dispatch...");
  Serial.println("AT+CMGF=1"); // Set SMS format to text mode
  delay(100);
  Serial.println("AT+CMGS=\"+918943940539\""); // Dest phone number (user's input)
  delay(100);
  Serial.print("CRITICAL ALERT: Collision event detected! Live Location: https://maps.google.com/?q=");
  Serial.print(lat, 5);
  Serial.print(",");
  Serial.println(lng, 5);
  delay(100);
  Serial.write(26); // ASCII character 26 (Ctrl+Z) to send message
  Serial.println("\n[GSM ALERT] Distress SMS successfully sent via GSM tower.");
}

void upload_collision_telemetry(float g_force, float lat, float lng) {
  /*
     Uploads geolocation coordinates and collision telemetry 
     to the Firebase database over HTTP POST.
  */
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(FIREBASE_URL);
    http.addHeader("Content-Type", "application/json");
    
    // Construct JSON String
    String jsonPayload = "{\"timestamp\": \"2026-06-25T11:00:00Z\", \"g_force\": " + String(g_force, 2) + 
                          ", \"latitude\": " + String(lat, 5) + 
                          ", \"longitude\": " + String(lng, 5) + 
                          ", \"status\": \"CRITICAL_COLLISION\"}";
                          
    Serial.println("[CLOUD] Uploading JSON telemetry to Firebase database...");
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("[CLOUD] HTTP Success Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("[CLOUD] Error sending HTTP POST request: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("[CLOUD] WiFi offline. Telemetry upload skipped. Relying on backup GSM SMS.");
  }
}

void loop() {
  // Simulate polling accelerometer values
  delay(3000);
  
  // Create simulated deceleration event (simulate crash on iteration)
  static int cycles = 0;
  cycles++;
  
  float simulated_g_force = 1.0; // Normal gravity force
  if (cycles >= 5) {
    simulated_g_force = 4.2; // Exceeds deceleration threshold (Crash simulated!)
    cycles = 0; // reset
  }
  
  Serial.print("[SENSOR] Read current Deceleration: ");
  Serial.print(simulated_g_force, 2);
  Serial.println(" Gs");
  
  if (simulated_g_force >= ACCEL_THRESHOLD) {
    Serial.println("\n[CRITICAL] COLLISION EVENT IDENTIFIED!");
    
    // Read coordinates
    Serial.print("[GPS] Fetching absolute coordinates: ");
    Serial.print(mock_lat, 5);
    Serial.print(", ");
    Serial.println(mock_lng, 5);
    
    // Trigger alarm outputs
    trigger_gsm_alert(mock_lat, mock_lng);
    upload_collision_telemetry(simulated_g_force, mock_lat, mock_lng);
  }
}
