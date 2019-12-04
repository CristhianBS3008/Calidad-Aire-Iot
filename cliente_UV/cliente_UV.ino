#include <ESP8266WiFi.h>
int UVsensorIn = A0; //Output from the sensor
const char WiFiSSID[] = "moto e5 plus 5902"; //cambia macID por el ID del dispositivo servidor
const char WiFiPSK[] = "12345678";
//const char host[] = "192.168.4.1"; //Direccion IP de nuestro servidor
const char host[] = "192.168.43.29"; //Direccion IP de nuestro servidor
const int httpPort = 80; //Puerto de enlace
String macID; //String donde se almacenara nuestra macID
float value3;

void setup() {
  pinMode(UVsensorIn, INPUT);
  Serial.begin(115200); //Inicializacion y configuracion de puerto serial
  connectWiFi();  // Conexion nuestra red Wifi
}
void loop() {
  delay(2000);  
  int uvLevel = averageAnalogRead(UVsensorIn);
 
  float outputVoltage = 3.3 * uvLevel/1023;
  float uvIntensity = mapfloat(outputVoltage, 0.99, 3, 0.0, 15.0);
  
    Serial.println(); 
  delay(200);
  value3 = uvIntensity;

  Serial.print(value3); Serial.println("mW/cm^2"); 

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
  url += "IUV=";
  url += value3;
  url += "*mW/cm^2";
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
    delay(100);
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
 
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
 
  return(runningValue);  
 
}
 
 

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
