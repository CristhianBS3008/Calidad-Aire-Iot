#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "HUAWEI CRISTHIAN";
const char* wifi_password = "qweasdzxc";

const char* mqtt_server = "192.168.43.40";
const char* mqtt_topic = "test";            //canal MQTT
const char* mqtt_username = "username";
const char* mqtt_password = "username";
const char* clientID = "Client ID";         //clientID es el identificar del esp8266 en el canal MQTT

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); //el puerto 1883 es del protocolo MQTT

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);
  
  while (WiFi.status() != WL_CONNECTED) {                       //esperamos que nos podamos conectar a la red del broker
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());                               //ip asignada al esp8266

  if (client.connect(clientID, mqtt_username, mqtt_password)) {   //nos conectamos al broker
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void loop() {
  if (client.publish(mqtt_topic, "")) {
      Serial.println("Mensaje enviado");
    }
  else {
      Serial.println("Mensaje fallido. Reconectando a MQTT");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // el delay asegura que client.publish no entre en conflicto con client.connect
      client.publish(mqtt_topic, "1024");
    }
    delay(30000); //Envía datos cada 30 seg
}
