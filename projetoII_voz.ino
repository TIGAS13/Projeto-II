#include <WiFi.h>
#include <ESP32Servo.h>

// ---------------------------
// Configuração da Rede WiFi
// ---------------------------
const char* ssid = "Tigas";
const char* password = "10203040";
WiFiServer server(80);

// ---------------------------
// Definições de pinos
// ---------------------------
const int TRIGGER_PIN = 5;   // GPIO 5 - HC-SR04 Trigger
const int ECHO_PIN = 18;     // GPIO 18 - HC-SR04 Echo
const int SERVO_PIN = 4;     // GPIO 4 - Servo Motor

// ---------------------------
// Parâmetros do Servo
// ---------------------------
const float DISTANCIA_LIMITE = 10.0;  // cm
const int ANGULO_FECHADO = 90;         // Porta fechada
const int ANGULO_ABERTO = 0;         // Porta aberta

// ---------------------------
// Variáveis auxiliares
// ---------------------------
long duracao;
float distancia;
bool portaDestrancada = false;  // Estado da porta

Servo servoMotor;

// ---------------------------
// Setup
// ---------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema...");

  // Pinos do sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(ANGULO_FECHADO);

  // Conexão WiFi
  Serial.print("Conectando à rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //IPAddress ip(192, 168, 2, 124);
  //IPAddress gateway(192, 168, 2, 1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet);

  // WiFi.config(...) removido para usar DHCP


  Serial.println("\nWiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());


  server.begin();
}

// ---------------------------
// Loop
// ---------------------------
void loop() {
  // Checa comandos via web
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Novo cliente conectado.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Cabeçalho HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Página web
            client.println("<center>");
            client.println("<h2>Controle de Tranca</h2>");
            client.println("<a href=\"/destrancar\"><button>Destrancar Porta</button></a><br><br>");
            client.println("<a href=\"/trancar\"><button>Trancar Porta</button></a>");
            client.println("</center>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Comandos
        if (currentLine.endsWith("GET /destrancar")) {
          portaDestrancada = true;
          Serial.println("Porta DESTRANCADA.");
        }

        if (currentLine.endsWith("GET /trancar")) {
          portaDestrancada = false;
          servoMotor.write(ANGULO_FECHADO);
          Serial.println("Porta TRANCADA.");
        }
      }
    }

    client.stop();
    Serial.println("Cliente desconectado.");
  }

  // Se destrancada, executa lógica do sensor
  if (portaDestrancada) {
    distancia = medirDistancia();

    Serial.print("Distância medida: ");
    Serial.print(distancia);
    Serial.println(" cm");

    if (distancia < DISTANCIA_LIMITE && distancia > 0) {
      servoMotor.write(ANGULO_ABERTO);
      Serial.println("Objeto detectado! Porta aberta.");
    } else {
      servoMotor.write(ANGULO_FECHADO);
    }
  }

  delay(2000);  // intervalo entre medições
}

// ---------------------------
// Função para medir distância
// ---------------------------
float medirDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duracao = pulseIn(ECHO_PIN, HIGH);
  return (duracao * 0.034) / 2;
}
