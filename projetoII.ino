/****************************************************
   Exemplo de uso de um ESP32 com sensor ultrassônico
   (HC-SR04) para acionar um servo motor caso a 
   distância seja menor que 30 cm.
****************************************************/

// Bibliotecas necessárias
#include <Arduino.h>
#include <ESP32Servo.h>

// Definição dos pinos do HC-SR04
const int triggerPin = 5;  // GPIO 5 (exemplo)
const int echoPin    = 18; // GPIO 18 (exemplo)

// Definição do pino do servo
const int servoPin   = 4;  // GPIO 4 (exemplo)

// Variáveis para o cálculo de distância
long duration;
float distanceCm;

// Cria um objeto Servo
Servo myServo;

void setup() {
  // Inicialização da serial para debug
  Serial.begin(115200);

  // Configuração dos pinos do sensor ultrassônico
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Anexa o servo ao pino especificado
  myServo.attach(servoPin);

  // Posiciona o servo inicialmente em 0 graus (fechado)
  myServo.write(0);
}

void loop() {
  // Limpa o pino trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Sinal de disparo do sensor ultrassônico
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Medição do tempo do eco
  duration = pulseIn(echoPin, HIGH);

  // Calcular a distância em centímetros
  // Velocidade do som ~ 340 m/s => 29.1 us/cm (aprox.)
  // distance = (duration * velocidadeSom) / 2
  // Para HC-SR04: distanceCm = duration * 0.034 / 2
  distanceCm = duration * 0.034 / 2;

  // Imprime distância para debug
  Serial.print("Distancia: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // Verifica se a distância é menor que 30 cm
  if (distanceCm < 30) {
    // Abre a “porta” (por exemplo, gira para 90°)
    myServo.write(90);
    Serial.println("Servo acionado! Distancia inferior a 30 cm.");
  } else {
    // Fecha a “porta” (retorna para 0°)
    myServo.write(0);
  }

  // Atraso para estabilizar leituras
  delay(200);
}
