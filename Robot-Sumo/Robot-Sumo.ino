/*
 motor       | IN1  | IN2  |
 ------------|------|------|
 Horário     | HIGH | LOW  |
 Anti-Horário| LOW  | HIGH |
 Ponto Morto | LOW  | LOW  |
 Freio       | HIGH | HIGH |
 */

//===============================================================
// --- Mapeamento de Hardware ---
#include "Ultrasonic.h" //INCLUSÃO DA BIBLIOTECA NECESSÁRIA PARA FUNCIONAMENTO DO CÓDIGO


//===============================================================
// --- Mapeamento de Hardware ---
#define IN1 3
#define IN2 5
#define IN3 6
#define IN4 9
#define infraEsquerda 8
#define infraDireita 13
#define echoPin  11
#define trigPin  10


//===============================================================
// --- Constantes Globais ---
#define direita 0
#define esquerda 1
#define frente 2
#define tras 3
#define freio 4
#define potMax 200
#define potMin 90
const int portas[4] = {IN1, IN2, IN3, IN4};
int motor[2][2] = {{IN1, IN2},
                   {IN3, IN4}};


//===============================================================
// --- Váriaveis Globais ---
float cmMsec, inMsec;
long microsec;
int distancia;
String result;


//===============================================================
// --- Protótipo das Funções ---
void motorConfig(int direcao);
void hcsr04();


//===============================================================
// --- Protótipo das Funções ---
Ultrasonic ultrasonic(trigPin,echoPin);


//===============================================================
// --- Void de Inicialização ---
void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 4; i++){
    pinMode(portas[i], OUTPUT);
    digitalWrite(portas[i], LOW);
  }
  pinMode(infraDireita, INPUT);
  pinMode(infraEsquerda, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}


//===============================================================
// --- Loop Infinito ---
void loop() {
  if((digitalRead(infraDireita) == 1) || (digitalRead(infraEsquerda) == 1)){
    motorConfig(tras);
    delay(500);
    motorConfig(esquerda);
    delay(300);
    motorConfig(frente);
    delay(200);
  }

  motorConfig(frente);
  delay(1);
}


//===============================================================
// --- Outras Funções ---
void motorConfig(int direcao){
  switch(direcao){
    case 0:
      Serial.println("Virando para direita!");
      digitalWrite(motor[direita][0],  LOW);
      analogWrite(motor[direita][1],  potMin);
      analogWrite(motor[esquerda][0],   potMin);
      digitalWrite(motor[esquerda][1],  LOW);
      break;
    
    case 1:
      Serial.println("Virando para Esquerda!");
      analogWrite(motor[direita][0],   potMin);
      digitalWrite(motor[direita][1],     LOW);
      digitalWrite(motor[esquerda][0],    LOW);
      analogWrite(motor[esquerda][1], potMin);
      break;
      
    case 2:
      Serial.println("Indo para frente!");
      analogWrite(motor[direita][0], potMax);
      digitalWrite(motor[direita][1],    LOW);
      analogWrite(motor[esquerda][0], potMax);
      digitalWrite(motor[esquerda][1],   LOW);
      break;
      
    case 3:
      Serial.println("Voltando para Trás!");
      digitalWrite(motor[direita][0],    LOW);
      analogWrite(motor[direita][1],  potMax);
      digitalWrite(motor[esquerda][0],   LOW);
      analogWrite(motor[esquerda][1], potMax);
      break;
      
    case 4:
      Serial.println("Acionando Freio!");
      digitalWrite(motor[direita][0], HIGH);
      digitalWrite(motor[direita][1], HIGH);
      digitalWrite(motor[esquerda][0], HIGH);
      digitalWrite(motor[esquerda][1], HIGH);
      break;
  }
}

void hcsr04(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    distancia = (ultrasonic.Ranging(CM)); 
    result = String(distancia);
    delay(1);
 }
