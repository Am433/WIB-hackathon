#include <WiFi.h>
#include <PubSubClient.h>

#define WIFISSID "Alexahome" // Put your WifiSSID here
#define PASSWORD "hngzhowxiantan" // Put your wifi password here
#define TOKEN "BBFF-RJ8ABBbh6G2ECGU0rkjRSOdXqhUnvj" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "alexnewton" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string;

#define VARIABLE_LABEL_EMG "EMG" // Variable label for EMG signal
#define VARIABLE_LABEL_GSR "GSR" // Variable label for GSR signal
#define VARIABLE_LABEL_ECG "ECG" // Variable label for ECG signal
#define VARIABLE_LABEL_EEG "EEG" // Variable label for EEG signal
#define DEVICE_LABEL "esp32"    // Device label

WiFiClient ubidots;
PubSubClient client(ubidots);

const int pinEMG = 34; // Pin for EMG sensor
const int pinGSR = A0; // Pin for GSR sensor
const int pinECG = A1; // Pin for ECG sensor
const int pinEEG = A2; // Pin for EEG sensor

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  pinMode(pinEMG, INPUT);
  pinMode(pinGSR, INPUT);
  pinMode(pinECG, INPUT);
  pinMode(pinEEG, INPUT);
  Serial.println();
  Serial.print("Waiting for WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer("things.ubidots.com", 1883); // Ubidots MQTT Broker
  client.setCallback(callback);
}

void loop() {
  int signalEMG = analogRead(pinEMG);
  int signalGSR = analogRead(pinGSR);
  int signalECG = analogRead(pinECG);
  int signalEEG = analogRead(pinEEG);

  Serial.print("EMG: ");
  Serial.println(signalEMG);
  Serial.print("GSR: ");
  Serial.println(signalGSR);
  Serial.print("ECG: ");
  Serial.println(signalECG);
  Serial.print("EEG: ");
  Serial.println(signalEEG);

  if (!client.connected()) {
    reconnect();
  }

  char topic[100];
  char payload[100];

  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);

  // Publish EMG data
  sprintf(payload, "{\"%s\": %d}", VARIABLE_LABEL_EMG, signalEMG);
  client.publish(topic, payload);
  delay(100);

  // Publish GSR data
  sprintf(payload, "{\"%s\": %d}", VARIABLE_LABEL_GSR, signalGSR);
  client.publish(topic, payload);
  delay(100);

  // Publish ECG data
  sprintf(payload, "{\"%s\": %d}", VARIABLE_LABEL_ECG, signalECG);
  client.publish(topic, payload);
  delay(100);

  // Publish EEG data
  sprintf(payload, "{\"%s\": %d}", VARIABLE_LABEL_EEG, signalEEG);
  client.publish(topic, payload);
  delay(100);

  client.loop();
  delay(500);
}
