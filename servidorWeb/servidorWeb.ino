#include <Wifi.h>
#include <WebServer.h>

const char* ssid = "ALUMNOS_TEC";
const char* password = "";

//inicializar server en puerto 80
WebServer server(80);
const int ledPin = 14;

void handleRoot(){
  String html = "<h1>Controlar LED</h1>"
  html += "<p><a href=\"/on\"><button>ON</button></a></p>";                       
  html += "<p><a href=\"/off\"><button>OFF</button></a></p>";                       
  server.send(200,"text/html",html);
}

void handleOn(){
  digitalWrite(ledPin, HIGH);
  server.send(200,"text/html",
  "LED ENCENDIDO <BR> <a href=\"/\">Volver</a>")
}

void handleOff(){
  digitalWrite(ledPin, LOW);
  server.send(200,"text/html",
  "LED APAGADO <BR> <a href=\"/\">Volver</a>")
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  delay(400);
  Wifi.begin(ssid, password); 
  
  Serial.print("Conectando a Wifi ...");

  while(Wifi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado");
  Serial.println("IP del Servidor :");
  Serial.println(Wifi.localIP());

  server.on("/",handleRoot);
  server.on("/on",handleOn);
  server.on("/off",handleOff);
  server.begin();

}

void loop() {
  server.handleClient();
}
