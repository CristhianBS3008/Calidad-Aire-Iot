
#include <ESP8266WiFi.h>
const char WiFiSSID[] = "moto e5 plus 5902"; //cambia macID por el ID del dispositivo servidor
const char WiFiPSK[] = "12345678";
//const char host[] = "192.168.4.1"; //Direccion IP de nuestro servidor
const char host[] = "192.168.43.29"; //Direccion IP de nuestro servidor
const int httpPort = 80; //Puerto de enlace
String macID; //String donde se almacenara nuestra macID
float value1;
float value2;
const int LED_PIN = 2;

void setup() {
  Serial.begin(115200); //Inicializacion y configuracion de puerto serial
  pinMode(LED_PIN, OUTPUT); // Configuracion de nuestro LED
  digitalWrite(LED_PIN, HIGH);
  connectWiFi();  // Conexion nuestra red Wifi
}
void loop() {
  float sensorValue=0;
  float RS_air;
  float sensor_volt;
  float R0;
  delay(2000);  
  int sensor= analogRead(A0);
  
  for(int i = 0; i <= 100; i++){
    sensorValue = sensorValue + sensor;

   }  
 sensorValue = sensorValue/100.0; //get the avarage value
 sensor_volt = sensorValue/1024*5.0;
 RS_air = (5.0-sensor_volt)/sensor_volt;
 R0 = RS_air/(26+(1/3)); // Not sure how they came up with this ?


 float RS_gas = 0;
 float ratio = 0;

 sensor_volt = 0;
 sensorValue = 0;
 sensor_volt = 0;

 sensorValue = analogRead(A0);
 sensor_volt = sensorValue/1024*5.0;
 RS_gas = (5.0-sensor_volt)/sensor_volt;
 ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
 //Serial.print(ratio);
  //Serial.println("PPM:"); 

  value1 =ratio;
  Serial.println("MQ7");
  Serial.print(value1); 
  // Inicia la conexion con el servidor
  Serial.print("connecting to ");
  Serial.println(host);
  // Se intenta la conexion con el servidor
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Si ha existido conexion se conntinua, y se concatena la macID de nuestros ESP8266
  // y los valores de temperatura y humedad leidos, para convertirlo en una URL
  String url = macID;
  url += "MQ7: ";
  url += value1;
  url+="PPM";
  delay(500);

  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // Se envia la solicitud al servidor
  client.print(url);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Se lee todo lo recibido desde el servidor y se imprime en el puerto serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}
//**************
void connectWiFi()
{
  // Obtenemos la macID de nuestro ESP8266
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  Serial.print(macID);
  Serial.print("Connecting to ");
  Serial.println(WiFiSSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin("RED_NODEMCU","12345678");
  byte ledStatus = LOW;
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    Serial.print(".");
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    delay(100);
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
