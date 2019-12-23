#include "userconfig.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TM1638.h>

const char* ssid = WiFi_SSID;
const char* password = WiFi_PASSWORD;
const char* mqtt_server = MQTTHost;
const char* mqtt_username = MQTTUsername;
const char * mqtt_password = MQTTPassword;
const int   mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
TM1638 module(TM1638_DIO, TM1638_CLK, TM1638_STB);

long lastMsg = 0;
long lastPnd = 0;
long lastSlp = 0;
byte lastkey = 0;

bool pndToggle = false;
bool slpToggle = true;

char msg[50];

void resetSleep()
{
  lastSlp = millis();
  slpToggle = true;
}

void setup_wifi() {
  delay(10);
  if (SERIAL_DEBUGGING == "ON"){
  Serial.println();
  Serial.print("Connecting to ");
  }
  module.clearDisplay();
  module.setDisplayToString("CONNECT ");
  if (SERIAL_DEBUGGING == "ON"){
  Serial.println(ssid);
  }

  WiFi.begin(ssid, password);

  byte dot = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    module.setDisplayToString("CONNECT  ",dot);
    dot++;
    if (dot > 256)
    {
      dot = 0;
    }
    if (SERIAL_DEBUGGING == "ON"){
    Serial.print(".");
    }
  }
  if (SERIAL_DEBUGGING == "ON"){
  Serial.println("");
  Serial.println("WiFi connected");
  }
  module.clearDisplay();
  //module.setDisplayToString("CONNECT");
  if (SERIAL_DEBUGGING == "ON"){
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (SERIAL_DEBUGGING == "ON"){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  }
  for (int i = 0; i < length; i++) {
    if (SERIAL_DEBUGGING == "ON"){
    Serial.print((char)payload[i]);
    }
  }
  if (SERIAL_DEBUGGING == "ON"){
  Serial.println();
  }

  char s[20];
  
  sprintf(s, "%s", payload);

  if ( (strcmp(topic,MQTT_Topic_Text)==0))
  {
    module.clearDisplay();
    module.setDisplayToString(s);
  }
  
  if (strcmp(topic,MQTT_Topic_Value)==0)
  { 
    module.clearDisplay();
    module.setDisplayToString(s,0x02);
  }

  if (strcmp(topic,MQTT_Topic_Led)==0)
  { 
    module.setLEDs(hex8(payload));
  }
  resetSleep();
}

/* interpret the ascii digits in[0] and in[1] as hex
* notation and convert to an integer 0..255.
*/
int hex8(byte *in)
{
   char c, h;

   c = (char)in[0];

   if (c <= '9' && c >= '0') {  c -= '0'; }
   else if (c <= 'f' && c >= 'a') { c -= ('a' - 0x0a); }
   else if (c <= 'F' && c >= 'A') { c -= ('A' - 0x0a); }
   else return(-1);

   h = c;

   c = (char)in[1];

   if (c <= '9' && c >= '0') {  c -= '0'; }
   else if (c <= 'f' && c >= 'a') { c -= ('a' - 0x0a); }
   else if (c <= 'F' && c >= 'A') { c -= ('A' - 0x0a); }
   else return(-1);

   return ( h<<4 | c);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (SERIAL_DEBUGGING == "ON"){
    Serial.print("Attempting MQTT connection...");
    }	 
    if (client.connect(DeviceName, mqtt_username, mqtt_password)) {
      if (SERIAL_DEBUGGING == "ON"){
      Serial.println(MSG_ONLINE);
      }									   
      client.publish(MQTT_Topic_ONLINE, MSG_ONLINE);
      client.subscribe(MQTT_Topic_ONLINE);
      client.subscribe(MQTT_Topic_Value);
      client.subscribe(MQTT_Topic_Set);
      client.subscribe(MQTT_Topic_Text);
      client.subscribe(MQTT_Topic_Led);
    } else {
      if (SERIAL_DEBUGGING == "ON"){
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      }
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  module.setLEDs(0);
  module.clearDisplay();
  if (SERIAL_DEBUGGING == "ON"){
  Serial.begin(115200);
  }
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  resetSleep();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  byte keys = module.getButtons();
  if (keys != lastkey)
  {
    lastkey = keys;
    int n = 0;
    int k = 0;
    while (n < TM1638_VERSION)

    {
      int i = (lastkey >> n) && 0x01;
      k = k + i;
      n++;
    }

    // send to command_topic
    switch (k) {
    case 1:
      client.publish(MQTT_Topic_Set, Button_01);
      module.setDisplayToString(Button_01);
      resetSleep();
      break;
    case 2:
      client.publish(MQTT_Topic_Set, Button_02);
      module.setDisplayToString(Button_02);
      resetSleep();
      break;
    case 3:
      client.publish(MQTT_Topic_Set, Button_03);
      module.setDisplayToString(Button_03);
      resetSleep();
      break;
    case 4:
      client.publish(MQTT_Topic_Set, Button_04);
      module.setDisplayToString(Button_04);
      resetSleep();
      break;    
    case 5:
      client.publish(MQTT_Topic_Set, Button_05);
      module.setDisplayToString(Button_05);
      resetSleep();
      break;    
    case 6:
      client.publish(MQTT_Topic_Set, Button_06);
      module.setDisplayToString(Button_06);
      resetSleep();
      break;    
    case 7:
      client.publish(MQTT_Topic_Set, Button_07);
      module.setDisplayToString(Button_07);
      resetSleep();
      break;    
    case 8:
      client.publish(MQTT_Topic_Set, Button_08);
      module.setDisplayToString(Button_08);
      resetSleep();
      break;    
    default:
      if (k != 0)
      {
        snprintf (msg, 75, "%ld", k);
        if (SERIAL_DEBUGGING == "ON"){
        Serial.println(msg);
        }
        client.publish(MQTT_Topic_Keys, msg);
      }
    break;
    }
  }

  // send an alive signal every minute
  long now = millis();
  
  if (now - lastMsg > (lastSlp_timer * 60000)) {
    lastMsg = now;
    if (SERIAL_DEBUGGING == "ON"){
    Serial.println(MSG_ONLINE);
    }
    client.publish(MQTT_Topic_ONLINE, "ALIVE");
  }

  // dim display to preserve energy
  if (
    (now - lastSlp > (lastSlp_timer * 1000)) && 
    slpToggle
    ) {
    slpToggle = false;
    lastSlp = now;
    module.clearDisplay();
    if (SERIAL_DEBUGGING == "ON"){
    Serial.println("Display off");
    }
  }
}
