#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//const char* ssid ="INFINITUM06B5";
//const char* password ="nvDtd7mfvd";

const char* ssid ="ALUMNOS TEC";
const char* password ="";

WebServer server(80);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
    <meta charset="UTF-8">
    <head>
        <title>Clima con ESP32</title>
        <style>

        </style>
    </head>
    <body>
        <h1>Temperatura Actual en Puebla</h1>
        <p id="tempValue">---</p>
        <script>
            function updateWeather(){
                fetch('/weather')
                .then(response => response.text())
                .then(data => 
                    document.getElementById('tempValue')
                    .innerText = data + "°C"
                );
            }
            setInterval(updateWeather, 5000);
        </script>
    </body>
</html>
)rawliteral";

void handleRoot(){
  server.send(200,"text/html",htmlPage);
}

float latitude = 19.04;
float longitude =-98.20;
bool current_weather = true ;

void handleWeather(){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    float longitud = 19.04;
    String url = "https://api.open-meteo.com/v1/forecast?latitude="+
                 String(latitude,2) +
                 "&longitude="+String(longitude,2)+
                 "&current_weather="+
                 String(current_weather ? "true":"false");
    http.begin(url);
    int httpCode = http.GET();

    if(httpCode > 0){
      String payload = http.getString();
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc,payload);
      
      if(!error){
        float temperatura = doc["current_weather"]["temperature"];
        server.send(200,"text/plain",String(temperatura));
        http.end();
        return;
      }
    }
    http.end();
  }
  server.send(200,"text/plain","ERROR");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi ...");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.println("IP del Servidor : ");
  Serial.println( WiFi.localIP() );

  server.on("/",handleRoot);
  server.on("/weather",handleWeather);
  server.begin();

}

void loop() {
  server.handleClient();
}
