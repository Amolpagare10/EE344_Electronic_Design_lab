#include <WiFi.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // UART2: TX=16, RX=17
  
  WiFi.softAP(ssid, password);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    client.println("Connected! Type commands (RED/BLUE/GREEN):");
    
    while (client.connected()) {
      // PC → ESP32 → dsPIC
      if (client.available()) {
        char c = client.read();
        
        // Echo back to Putty
        client.print(c);
        Serial.println(c);
        // Forward to dsPIC
        Serial2.print(c);
        
        // Special handling for Enter key
        if (c == '\r' || c == '\n') {
          client.println(); // New line in Putty
          Serial.println();
          Serial2.println(); // Send line ending to dsPIC
        }
      }

      // dsPIC → ESP32 → PC
      if (Serial2.available()) {
        String uart_data = Serial2.readStringUntil('\n');
        client.println("dsPIC: " + uart_data);
      }
    }
    client.stop();
  }
}
