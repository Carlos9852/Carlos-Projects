/*
 motor       | IN1  | IN2  |
 ------------|------|------|
 Horário     | HIGH | LOW  |
 Anti-Horário| LOW  | HIGH |
 Ponto Morto | LOW  | LOW  |
 Freio       | HIGH | HIGH |
 */

//===============================================================
// --- Bibliotecas ---


//===============================================================
// --- Mapeamento de Hardware ---
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
#define ENA 5
#define ENB 6
#define bot 13
#define infraEsquerda 3
#define infraDireita 2
#define sensor A5


//===============================================================
// --- Constantes Globais ---
#define direita 0
#define esquerda 1
#define frente 2
#define tras 3
#define freio 4
#define potMax 200
#define potMin 90
const int portas[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
int motor[2][2] = {{IN1, IN2},
                   {IN3, IN4}};


//===============================================================
// --- Váriaveis Globais ---
int distancia,
    pwm = 200;



//===============================================================
// --- Protótipo das Funções ---
void motorConfig(int direcao);
void sharp();


//===============================================================
// --- Void de Inicialização ---
void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 6; i++){
    pinMode(portas[i], OUTPUT);
    digitalWrite(portas[i], LOW);
  }
  pinMode(infraDireita, INPUT);
  pinMode(infraEsquerda, INPUT);
  pinMode(sensor, INPUT);
  pinMode(bot, INPUT_PULLUP);

  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);

  while( digitalRead(bot) == 1){
    motorConfig(freio);
    delay(1);
  }
}


//===============================================================
// --- Loop Infinito ---
void loop() {

  sharp();
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
      digitalWrite(motor[direita][0],   LOW);
      digitalWrite(motor[direita][1],  HIGH);
      digitalWrite(motor[esquerda][0], HIGH);
      digitalWrite(motor[esquerda][1],  LOW);
      break;
    
    case 1:
      Serial.println("Virando para Esquerda!");
      digitalWrite(motor[direita][0],  HIGH);
      digitalWrite(motor[direita][1],   LOW);
      digitalWrite(motor[esquerda][0],  LOW);
      digitalWrite(motor[esquerda][1], HIGH);
      break;
      
    case 2:
      Serial.println("Indo para frente!");
      digitalWrite(motor[direita][0],  HIGH);
      digitalWrite(motor[direita][1],   LOW);
      digitalWrite(motor[esquerda][0], HIGH);
      digitalWrite(motor[esquerda][1],  LOW);
      break;
      
    case 3:
      Serial.println("Voltando para Trás!");
      digitalWrite(motor[direita][0],   LOW);
      digitalWrite(motor[direita][1],  HIGH);
      digitalWrite(motor[esquerda][0],  LOW);
      digitalWrite(motor[esquerda][1], HIGH);
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

void sharp(){
  float volts = analogRead(sensor) * 0.0048828125;
  distancia = 26 * pow(volts, -1);
 }
