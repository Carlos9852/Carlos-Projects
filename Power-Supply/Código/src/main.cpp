//===================================================================
// --- Bibliotecas ---
#include <Arduino.h>


//===================================================================
// --- Constantes Globais ---
#define pwm1  3
#define pwm2  6
#define dutyCycle  128


//===================================================================
// --- Variáveis Globais ---
int duty = 2;


//===================================================================
// --- Protótipo das Funções ---


//===================================================================
// --- Void Setup ---
void setup() {
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
 digitalWrite(pwm2, HIGH);
  analogWrite(pwm1, dutyCycle);

}


//===================================================================
// --- Void Loop ---
void loop() {

  for(int i = 10; i < 240; i++){
    analogWrite(pwm2, duty);
    duty = i;
    delay(50);
  }
 
}


//===================================================================
// --- Outras Funções ---