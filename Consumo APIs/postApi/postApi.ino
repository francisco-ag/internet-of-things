#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Franc";
const char* password = "wifiPass";
const char* API_URL = "https://esp32-api-production-3779.up.railway.app/data";
const char* DEVICE_ID = "esp32-campo-10";

void conectarWifi() {
  Serial.print("Conectando a Wifi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWifi Conectado ");
  Serial.println("Ip :  " + WiFi.localIP().toString());
}

void enviarDatos(float temperatura, float humedad, int luz) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi Desconectado");
    Serial.println("reconectando ....");
    conectarWifi();
  }

  HTTPClient http;
  http.begin(API_URL);

  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<256> doc;

  doc["device_id"] = DEVICE_ID;
  JsonObject payload = doc.createNestedObject("payload");
  payload["temperatura"] = temperatura;
  payload["humedad"] = humedad;
  payload["luz"] = luz;

  String body;
  serializeJson(doc, body);

  Serial.println("Enviando : " + body);
  int httpCode = http.POST(body);

  if (httpCode == 201) {
    Serial.println(" Dato guardado correctamente (201)");
    String respuesta = http.getString();
    Serial.println("Respuesta " + respuesta);
  }else {
    Serial.println("Error HTTP \n "+httpCode);
    Serial.println(http.getString());
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  conectarWifi();
}

void loop() {
  // Simulacion de lectura de los sensores 

  float temperatura = 24.5 + random(-10,10) * 0.1;
  float humedad = 60.0 + random(-5,5);
  int luz = 1000 ;

  enviarDatos(temperatura, humedad, luz );

  delay(10000);

}
