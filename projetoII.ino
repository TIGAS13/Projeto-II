/****************************************************
   Projeto: Servo motor controlado por sensor ultrassônico
   Descrição: Aciona um servo motor quando a distância
   detectada pelo sensor HC-SR04 for menor que 30 cm.
****************************************************/

// Bibliotecas necessárias
#include <ESP32Servo.h>

// Definição dos pinos
const int TRIGGER_PIN = 5;    // GPIO 5 para trigger do HC-SR04
const int ECHO_PIN = 18;      // GPIO 18 para echo do HC-SR04
const int SERVO_PIN = 4;      // GPIO 4 para o servo motor

// Constantes do projeto
const float DISTANCIA_LIMITE = 10.0;  // Distância limite em centímetros
const int ANGULO_FECHADO = 0;         // Ângulo quando não detecta objeto
const int ANGULO_ABERTO = 90;         // Ângulo quando detecta objeto

// Variáveis para cálculo da distância
long duracao;
float distancia;

// Objeto do servo motor
Servo servoMotor;

void setup() {
  // Inicializa comunicação serial para monitoramento
  Serial.begin(115200);
  Serial.println("Iniciando sistema...");

  // Configura pinos do sensor ultrassônico
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configura e posiciona o servo motor
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(ANGULO_FECHADO);
  
  Serial.println("Sistema iniciado!");
}

void loop() {
  // Mede a distância atual
  distancia = medirDistancia();

  // Mostra a distância no monitor serial
  Serial.print("Distância medida: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Verifica se há objeto próximo e aciona o servo
  if (distancia < DISTANCIA_LIMITE && distancia > 0) {
    servoMotor.write(ANGULO_ABERTO);
    Serial.println("Objeto detectado! Servo acionado.");
  } else {
    servoMotor.write(ANGULO_FECHADO);
  }

  // Pequeno delay para estabilização
  delay(2000);
}

// Função para medir a distância usando o HC-SR04
float medirDistancia() {
  // Limpa o trigger
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  
  // Envia pulso de trigger
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  // Mede a duração do pulso echo
  duracao = pulseIn(ECHO_PIN, HIGH);
  
  // Calcula e retorna a distância em centímetros
  // Fórmula: distância = (duração * velocidade do som) / 2
  // Velocidade do som = 340 m/s = 0.034 cm/µs
  return (duracao * 0.034) / 2;
}
