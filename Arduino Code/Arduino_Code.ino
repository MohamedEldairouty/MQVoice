#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// === CONFIGURATION SECTION ===

// 1)  WiFi credentials:
const char* ssid     = "Dairo";
const char* password = "Dairo2005";

// 2) MQTT Broker details:
const char* mqtt_server = "je1b6b22.ala.eu-central-1.emqxsl.com";
const int   mqtt_port   = 8883;

// 3) MQTT Authentication:
const char* mqtt_user     = "dairo";
const char* mqtt_password = "Dairo2828";

// 4) MQTT Topics:
const char* mqtt_topic_sub = "led/control";

// 5) LED pin:
const int LED_PIN = 26;  


// ======================================================================
//  CA Certificate
static const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
"-----END CERTIFICATE-----\n";
// ======================================================================


WiFiClientSecure espClient;
PubSubClient client(espClient);

void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();


void setup() {
  // 1) Initialize Serial for debugging
  Serial.begin(115200);
  delay(100);

  // 2) Configure the LED pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(2, OUTPUT); // Built-in LED
  digitalWrite(LED_PIN, LOW);
  digitalWrite(2, LOW); // Turn OFF at start


  // 3) Connect to WiFi
  Serial.print("Connecting to WiFi SSID: ");
  Serial.print(ssid);
  Serial.print(" ... ");
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // 4) Configure the MQTT client to use TLS with CA certificate
  espClient.setCACert(ca_cert);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  // 5) Attempt to connect to MQTT
  reconnectMQTT();
}


void loop() {
  // Ensure MQTT stays connected
  if (!client.connected()) {
    reconnectMQTT();
  }
  // Process incoming messages and keep-alive pings
  client.loop();
}


//======================================================================
// MQTT callback: called whenever a subscribed message arrives
//======================================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim(); 

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // If the message is  "ON", turn LED on; if "OFF", turn LED off
  if (message.equalsIgnoreCase("ON")) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(2, HIGH);  // Built-in LED ON
    Serial.println("LEDs → ON");
  }
  else if (message.equalsIgnoreCase("OFF")) {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(2, LOW);   // Built-in LED OFF
    Serial.println("LEDs → OFF");
}
  else {
    Serial.println("Unrecognized message..");
  }
}


//======================================================================
// Attempt (and re-attempt) to connect to MQTT broker
//======================================================================
void reconnectMQTT() {
  // Loop until connected
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.print(mqtt_port);
    Serial.print(" ... ");

    // Attempt to connect with username/password
    if (client.connect("esp32", mqtt_user, mqtt_password)) {
      Serial.println("connected.");
      // Once connected, subscribe to the topic
      Serial.print("Subscribing to topic: ");
      Serial.println(mqtt_topic_sub);
      client.subscribe(mqtt_topic_sub);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" → retrying in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}