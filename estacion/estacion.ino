#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // Include ThingSpeak Library

// Declaracion e inicializacion de variables utilizadas para
// realizar la conexion a la red Wifi local
const char WiFiSSID[] = "moto e5 plus 5902"; //SSID de la red Wifi
const char WiFiPSK[] = "roficom98";   //Contraseña WPA, WPA2 o WEP
unsigned long myChannelNumber = 922998; 
const char * myWriteAPIKey = "ACSLVHRPLC6XKOY2";

// Declaracion de la contraseña de nuestro Punto de Acceso (AP)
const char WiFiAPPSK[] = "12345678";
const int LED_PIN = 2;
float  tmp_dev2,hmd_dev2;
float uv1;
float mq73;
String value1;
String value2;
String value3;
String value4;
WiFiServer server(80);
WiFiClient client;
void setup() {
  ThingSpeak.begin(client); // Inicia ThingSpeak
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  setupWiFi();    //Configuramos y conectamos nuestra red Wifi
  server.begin(); //Se inicia el dispositivo como un servidor
}

void loop()
{

  String s;
  // Si el cliente esta conectado
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Se realiza la lectura de la solicitud
  String req = client.readStringUntil('\r');
  Serial.print(req);

  client.flush();
  if (req.indexOf("81b5") != -1){  //Dispositivo 2; escribe el ID del dispositivo 2, cliente ESP826

    // Se decodifican los valores recibidos por les clientes ESP8266
    // y se almacenan en variables internas
    decoder_values(req); //decodifica los valores de la trama: /macID_del_cliente&values1=##$values2=##
    tmp_dev2=value1.toFloat();
    hmd_dev2=value2.toFloat();
    Serial.println("");
    Serial.print("Temperatura_dev2: ");
    Serial.println(tmp_dev2);
    Serial.print("Humedad_dev2: ");
    Serial.println(hmd_dev2);
    ThingSpeak.setField(1, tmp_dev2);
    ThingSpeak.setField(2, hmd_dev2);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
  else if (req.indexOf("7a41") != -1){  //Dispositivo 1; escribe el ID del dispositivo 1, cliente ESP8266
    // Se decodifican los valores recibidos por les clientes ESP8266
    // y se almacenan en variables internas
    decoder_values_7a41(req); //decodifica los valores de la trama: /macID_del_cliente&values1=##$values2=##
    uv1=value1.toFloat();
    Serial.println("");
    Serial.print("UV1: ");
    Serial.println(uv1);
    ThingSpeak.setField(3, uv1);
  }
  
  // Se envia la respuesta a nuestro cliente
  client.print(s);
  delay(1000);
}

void decoder_values_7a41(String _req){

  //Se decodifican las dos variables "value" que los dispositivos ESP8266 nos envian
  int Start1 = _req.indexOf("=");
  int Finish1 = _req.indexOf("*", Start1 + 1);

  value1 = "";
 
  for (int i = Start1 + 1; i < Finish1; i++)
  {
    value1 = value1 + _req.charAt(i);
  }
}
void decoder_values(String _req){

  //Se decodifican las dos variables "value" que los dispositivos ESP8266 nos envian
  int Start1 = _req.indexOf("=");
  int Finish1 = _req.indexOf("°", Start1 + 1);
  int Start2 = _req.indexOf("=", Finish1 + 1);
  int Finish2 = _req.indexOf("%", Start2 + 1);

  value1 = "";
  value2 = "";
 
  for (int i = Start1 + 1; i < Finish1; i++)
  {
    value1 = value1 + _req.charAt(i);
  }
  for (int i = Start2 + 1; i < Finish2; i++)
  {
    value2 = value2 + _req.charAt(i);
  }
  
}   
void setupWiFi() {
  WiFi.mode(WIFI_AP_STA);
  
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, AP_NameString.length() + 1, 0);
  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  //WiFi.softAP(AP_NameChar, WiFiAPPSK);
  WiFi.softAP("RED_NODEMCU", "12345678");
  WiFi.begin(WiFiSSID, WiFiPSK);
  byte ledStatus = LOW;
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    delay(5000);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //192.168.43.220
}
