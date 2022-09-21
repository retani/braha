#include "Arduino.h"
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "NurBaute";
const char *password = "5XWp7Q%G";

Servo throttleServo;
Servo turnServo;
ESP8266WebServer server(80);

const int led = 16;
const int throttleServoPin = 15;
const int turnServoPin = 13;

const int throttleNeutralDeg = 45;
const int throttleForwardDeg =  120;
const int throttleBackwardDeg = -10;
const int throttleDelay = 500;

const int turnNeutralDeg = 100;
const int turnLeftDeg = 145;
const int turnRightDeg = 15;
const int turnDelay = 500;

void handleRoot()
{
  digitalWrite(led, HIGH);
  server.send(200, "text/html",
              "<html>"
              "<head>"
              "<meta name='viewport' content = 'width=device-width, initial-scale=1' />"
              "</head>"
              "<body>"
              "<form onSubmit=\"return false\">"
              "<button onClick=\"fetch('/forward')\">Forward</button><br>"
              "<button onClick=\"fetch('/backward')\">Backward</button><br>"
              "<button onClick=\"fetch('/left')\">Left</button><br>"
              "<button onClick=\"fetch('/right')\">Right</button><br>"
              "</form>"
              "</body>"
              "</html>");
  digitalWrite(led, LOW);
}

void handleForward(void)
{
  throttleServo.write(throttleForwardDeg);
  delay(throttleDelay);
  throttleServo.write(throttleNeutralDeg);
  server.send(200, "text/plain", "went forward");
}

void handleBackward(void)
{
  throttleServo.write(throttleBackwardDeg);
  delay(throttleDelay);
  throttleServo.write(throttleNeutralDeg);
  server.send(200, "text/plain", "went backward");
}

void handleLeft(void)
{
  turnServo.write(turnLeftDeg);
  delay(turnDelay);
  turnServo.write(turnNeutralDeg);
  server.send(200, "text/plain", "rotated left");
}

void handleRight(void)
{
  turnServo.write(turnRightDeg);
  delay(turnDelay);
  turnServo.write(turnNeutralDeg);
  server.send(200, "text/plain", "rotated right");
}

void demoServos()
{
  handleForward();
  handleBackward();
  handleLeft();
  handleRight();
}

void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void)
{
  throttleServo.attach(throttleServoPin);
  throttleServo.write(throttleNeutralDeg);
  turnServo.attach(turnServoPin);
  turnServo.write(turnNeutralDeg);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  demoServos();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(led, LOW);
}

void loop(void)
{
  server.handleClient();
}