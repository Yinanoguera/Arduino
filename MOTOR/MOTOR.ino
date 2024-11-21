#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <Stepper.h>

int stepperRevolution = 1024;
int motSpeed = 12;
//stepper myStepper (stepperRevolution,8,10,9,11)
Stepper myStepper(stepperRevolution, 2, 12, 4, 14);
int dt = 2000;

#define WIFI_SSID "Aula1"
#define WIFI_PASSWORD "clashroyale"
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6









WebServer server(80);

const int LED1 = 26;
const int LED2 = 27;

bool PuertaAbierta = false;

void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
          h1 { margin-bottom: 1.2em; } 
          h2 { margin: 0; }
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em;
                 font-size: 2em; text-decoration: none }
          .btn.OFF { background-color: #333; }
        </style>
      </head>
            
      <body>
        <h1>ESP32 Web Server</h1>

        <div>
          <h2>PUERTA ABIERTA </h2>
          <a href="/toggle/1" class="btn LED1_TEXT">LED1_TEXT</a>
        </div>
      </body>
    </html>
  )";
  response.replace("LED1_TEXT", PuertaAbierta ? "SI" : "NO");
  server.send(200, "text/html", response);
}

void setup(void) {

  // Configurar motor paso:
  myStepper.setSpeed(motSpeed);


  Serial.begin(115200);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);

  server.on(UriBraces("/toggle/{}"), []() {
    String led = server.pathArg(0);
    Serial.print("Toggle LED #");
    Serial.println(led);

    if (PuertaAbierta) {
      myStepper.step(-stepperRevolution);
    } else {
      myStepper.step(stepperRevolution);
    }
    delay(dt);
    PuertaAbierta = !PuertaAbierta;
    
    sendHtml();
});

server.begin();
Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);
}
