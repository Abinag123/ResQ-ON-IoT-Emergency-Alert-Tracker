# ResQ-ON: IoT Collision Detection & Emergency Tracker

ResQ-ON is an integrated IoT-based vehicle collision detection system designed to track impact accidents and dispatch instant distress telemetry to family members and cloud databases.

---

## 🔌 Hardware Components

*   **ESP32 DevKit V1:** Core micro-controller processing sensor signals and coordinating communications.
*   **ADXL345 Accelerometer:** Monitos multi-axis deceleration metrics to identify crash impacts (threshold set to >3 Gs).
*   **NEO-6M GPS Receiver:** Receives real-time geolocation telemetry coordinates.
*   **SIM800L GSM Module:** Backup cellular transceiver to send text alerts.

---

## ⚙️ Architecture & Core Logic

1.  **Continuous Polling:** The ESP32 queries deceleration inputs from the ADXL345 accelerometer over I2C communication.
2.  **Threshold Detection:** If deceleration forces cross 3.0 Gs, a collision interrupt event is triggered.
3.  **Coordinate Acquisition:** The micro-controller queries NMEA sentences from the GPS receiver, extracting latitude and longitude.
4.  **Telemetry Uplink:** Coordinates and impact analytics are transmitted via WiFi HTTP POST requests to a Firebase Realtime Database.
5.  **Backup SMS:** In areas without internet connectivity, the SIM800L GSM transceiver sends a high-priority distress SMS with Google Maps coordinates directly to emergency contacts.
