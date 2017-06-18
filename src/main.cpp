#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <aREST.h>
#include <DHT.h>
#include <NTPClient.h>

#include "secrets.h"
#include "heartbeat.h"

#define DHTPIN 5      // GPIO5 == D1 on the ESP board.
#define DHTTYPE DHT11 // DHT11 is the model sensor I'm using

const char *ssid = SSID;
const char *password = PASSWORD;
float humidity, temp_f; // Values read from sensor

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);
String webString = ""; // String to display
aREST rest = aREST();

IPAddress ipBroadCast(255, 255, 255, 255);
unsigned int heartbeatPort = 3196;
Heartbeat _heartbeat(ipBroadCast, heartbeatPort);

// https://github.com/arduino-libraries/NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

byte mac[6];

long int startTime = 0;
long int currentTime = 0;

void setupWebServer()
{
  Serial.println("Entering: setupWebServer()");
  server.on("/", []() { // if you add this subdirectory to your webserver call, you get text below :)

    webString = "<!DOCTYPE html><html><h1>";
    webString += "Temperature: " + String((int)temp_f) + " *F"; // Arduino has a hard time with float to string
    webString += " Humidity: " + String((int)humidity) + "%";
    webString += "</h1></html>";

    server.send(200, "text/html", webString); // send to someones browser when asked
  });

  server.begin();
  Serial.println("HTTP server started");
}

void initWifi()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
}

const int ReadAmbianceSeconds = 2;
long int readAmbianceTime = 0;

void readAmbiance()
{
  if (readAmbianceTime > currentTime)
    return;

  // Set next read time.
  readAmbianceTime = currentTime + ReadAmbianceSeconds;

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  temp_f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_f))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(temp_f, humidity);

  //These are just debug printouts to console
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temp_f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hif);
  Serial.println(" *F");
}

const int BlinkHighSeconds = 1;
long int blinkTime = 0;
bool blinkIsHigh = false;

void blink()
{
  if (blinkTime > currentTime)
    return;

  // Set next blink time and current state.
  blinkTime = currentTime + BlinkHighSeconds;
  blinkIsHigh = !blinkIsHigh;

  int val = blinkIsHigh ? HIGH : LOW;
  digitalWrite(LED_BUILTIN, val);
}

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("DHTxx test!");
  initWifi();

  timeClient.begin();

  dht.begin();
  setupWebServer();
}

const int HeartbeatSendSeconds = 15;
long int heartbeatSendTime = 0;
void doHeartbeat()
{
  if (heartbeatSendTime > currentTime)
    return;

  // Set next heartbeat time.
  heartbeatSendTime = currentTime + HeartbeatSendSeconds;
  _heartbeat.Send(mac, currentTime, humidity, temp_f);
}

void loop()
{
  timeClient.update();

  if (startTime == 0)
  {
    startTime = timeClient.getEpochTime();
  }
  currentTime = timeClient.getEpochTime();

  blink();

  readAmbiance();

  server.handleClient();

  doHeartbeat();
}
