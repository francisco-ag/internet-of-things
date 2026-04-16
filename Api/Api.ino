#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Tokio, Japón 
// lat: 35.68
// lon: 139.76


//const char* ssid ="INFINITUM06B5";
//const char* password ="nvDtd7mfvd";

// IMPORTANTE :
// DESCARGAR LIBERIA
// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

const char* ssid ="ALUMNOS TEC";
const char* password ="";

WebServer server(80);

float latitude = 18.37;
float longitude =-7.37;
bool current_weather = true ;

String myJson = "";
const int ledPin = 14;
int ledState = LOW;  

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
        <h1>Consulta de Temperatura</h1>
       
        <form action="/setcoords" method="GET">
            Latitud : <input type="text" name="lat" value="18.37">
            <br>
            <br>
            Longitud : <input type="text" name="lon" value="-97.37">
            <br>
            <br>

            <input type="submit" value="Consultar">

        </form>
        <p>Temperatura Actual : </p>
        <p id="tempValue">---</p>

        <p>Estado del LED (noche = encendido) : </p>
        <p id="ledStatus">---</p>

        <h2>Respuesta JSON :</h2>
        <pre id="jsonData">---</pre>

        <script>
            function updateWeather(){
                fetch('/weather')
                .then(response => response.json())
                .then(data => { 
                    document.getElementById('tempValue')
                    .innerText = data.temperature + "°C";

                    document.getElementById('ledStatus')
                    .innerText = data.led == 1 ? "Encendido" : "Apagado";

                    document.getElementById('jsonData')
                    .innerText = JSON.stringify(data.json,null,2) ;

                });
            }
            updateWeather();
            setInterval(updateWeather, 5000);            
        </script>
    </body>
</html>
)rawliteral";

void handleRoot(){
  server.send(200,"text/html",htmlPage);
}

void handleSetCoords(){
  if(server.hasArg("lat")) latitude = server.arg("lat").toFloat();
  if(server.hasArg("lon")) longitude = server.arg("lon").toFloat();

  server.sendHeader("Location","/");
  server.send(303);

}

void handleWeather(){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = "https://api.open-meteo.com/v1/forecast?latitude="+
                 String(latitude,2) +
                 "&longitude="+String(longitude,2)+
                 "&current_weather="+
                 String(current_weather ? "true":"false");
    http.begin(url);
    int httpCode = http.GET();

    if(httpCode > 0){
      
      myJson = http.getString();
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc,myJson);
      
      if(!error){
        float temperatura = doc["current_weather"]["temperature"];
        int isDay = doc["current_weather"]["is_day"];

        if(isDay == 0){
          //Es de noche
          ledState = HIGH;
          digitalWrite(ledPin,ledState)
        }else{
          ledState = LOW;
          digitalWrite(ledPin,ledState)
        }

        String response; 

        StaticJsonDocument<2048> out;

        out["temperature"] = temperatura;
        out["led"] = ledState;
        out["json"] = doc.as<JsonObject>();

        serializeJson(out, response);
        server.send(200,"application/json",response);
        http.end();
        return;
      }
    }
    http.end();
  }
  server.send(200,"application/json",
  "{\"error\":\"No se pudieron obtener los datos\"}"
  );
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin,OUTPUT);
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
  server.on("/setcoords",handleSetCoords);
  server.on("/weather",handleWeather);
  server.begin();

}

void loop() {
  server.handleClient();
}
