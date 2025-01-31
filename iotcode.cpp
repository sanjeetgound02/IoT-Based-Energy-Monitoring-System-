#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Wi-Fi credentials
const char* ssid = "your_SSID";                // Replace with your Wi-Fi network name
const char* password = "your_PASSWORD";         // Replace with your Wi-Fi password

// ThingSpeak credentials
unsigned long myChannelNumber = YOUR_CHANNEL_ID;  // Replace with your ThingSpeak channel ID
const char* myWriteAPIKey = "YOUR_WRITE_API_KEY";  // Replace with your ThingSpeak API Key

// Wi-Fi client and ThingSpeak object
WiFiClient client;

// Pin for current sensor (ACS712)
const int currentPin = A0;  // Connect the current sensor to analog pin A0

void setup() {
  Serial.begin(115200); // Start serial communication for debugging
  
  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read the current sensor (ACS712) value
  int sensorValue = analogRead(currentPin);  // Read raw sensor value (0-1023)
  
  // Convert to voltage (Assuming a 5V system)
  float voltage = (sensorValue / 1024.0) * 5.0;

  // Calculate current in amperes (ACS712 with 185mV per Amp)
  // ACS712 outputs 2.5V at 0A, so we subtract 2.5V and scale by 0.185V per A
  float current = (voltage - 2.5) / 0.185;

  // Calculate power assuming the voltage is 230V (AC mains, adjust based on your local voltage)
  float power = current * 230.0;  // Power in Watts
  
  // Output values to the serial monitor (for debugging)
  Serial.print("Current (A): ");
  Serial.print(current);
  Serial.print("  Power (W): ");
  Serial.println(power);

  // Send data to ThingSpeak
  ThingSpeak.setField(1, current);  // Send current to Field 1
  ThingSpeak.setField(2, power);    // Send power to Field 2

  // Write data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  // Check for successful data transmission
  if (responseCode == 200) {
    Serial.println("Data sent successfully to ThingSpeak");
  } else {
    Serial.print("Failed to send data. Response code: ");
    Serial.println(responseCode);
  }

  // Wait for 10 seconds before sending the next data point
  delay(10000);
}
