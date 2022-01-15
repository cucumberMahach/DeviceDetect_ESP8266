#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Pinger.h>

#define CHECK_INTERVAL 5000
#define PING_COUNT 5
#define PING_TIMEOUT 500

const char *ssid = "SSID"; //Your WiFi point name
const char *password = "PASS"; //Your WiFi point password
const char *hostName = "DD-ST-1";
IPAddress deviceAddress(192, 168, 0, 62); //Device ip address in WiFi network

Pinger pinger;
boolean deviceOnline = true;
unsigned long ping_previousTime = 0;

void deviceStateChanged()
{
  if (deviceOnline)
  {
    //Device toggle to online
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Device online");
  }
  else
  {
    //Device toggle to offline
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Device offline");
  }
}

void pingTick()
{
  if (millis() - ping_previousTime > CHECK_INTERVAL)
  {
    ping_previousTime = millis();
    pinger.Ping(deviceAddress, PING_COUNT, PING_TIMEOUT);
  }
}

bool pingerResponse(const PingerResponse &response)
{
  bool oldState = deviceOnline;

  deviceOnline = response.TotalReceivedResponses != 0;

  if (oldState != deviceOnline)
  {
    deviceStateChanged();
  }

  return false;
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostName);
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int ccount = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    ccount++;
    if (ccount >= 20)
    {
      Serial.print(WiFi.status());
      WiFi.begin(ssid, password);
      ccount = 0;
    }
  }
  Serial.println();
  Serial.print("WiFi connected. Local IP: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  connectToWiFi();

  //Setup builtin led to display device state
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //Setup pinger callback
  pinger.OnEnd(pingerResponse);
}

void loop()
{
  pingTick();
}
