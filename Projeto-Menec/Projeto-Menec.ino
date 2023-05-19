// =========================================================
// --- Bibliotecas Auxiliares ---
#include<Arduino.h>

// =========================================================
// --- Mapeamento de Hardware ---


// =========================================================
// --- Váriaveis Globais ---
float dist;
int   X     = 0; 
bool flag   = false,
     carlos = false;
int  pwm0a  = 0x82,
     pwm0b  = 0x82,
     estado = 0;


// =========================================================
// --- Constantes Globais ---
const int forward =  1,          //constante para motor ir pra frente
          backward = 2,          //constante para motor ir pra trás
          right     = 3,         //constante para motor ir pra esquerda
          left    = 4;           //constante para motor ir pra direita


// =========================================================
// --- Protótipo das Funções ---
float measureDistance();         //função para medir a distândia do ultrassônico
void motorConfig(int modo);      //função para o controle da ponte H
void search();


// =========================================================
// --- Interrupção ---
ISR(TIMER2_OVF_vect){
  static int baseT1 = 0,         //variável local estática para base de tempo 1
             baseT2 = 0;         //variável local estática para base de tempo 2
            

  TCNT2 = 0x06;                  //reinicializa Timer0

  baseT1++;                      //incremente baseT1 em um
  baseT2++;                      //incremente baseT2 em um

  if(baseT1 == 10){            //se a baseT1 for igual a 1000
    baseT1 = 0;                  //zera a variável baseT1
    flag = PINC & (1<<PORTC2);   //armazena o valor do sensor1 em X
    Serial.println(flag);
    if(flag == !estado) carlos = true;
  }//end if
  
  if(carlos){
    for(int i = 0; i < 0x96; i++){
      pwm0a = i;
      pwm0b = i;
      motorConfig(backward);
    }//end for
  }//end if

  if(baseT2 == 250){
    carlos = false;
    baseT2 = 0;
  }
}//end ISR

// =========================================================
// --- Função Principal ---
void setup(){

  Serial.begin(9600);

  DDRC &= ~(1<<PORTC2);          //configura analogica 2 (PC2) como entrada (sensor1)
  estado = PINC & (1<<PORTC2);

  DDRC  |=  (1<<PORTC1);         //configura analogica 1 (PC1) como saída  (trig)
  PORTC &= ~(1<<PORTC1);         //inicializa analogica 1 (PC1) em LOW (trig)  
  DDRC  &= ~(1<<PORTC0);         //configura analogica 0 (PC0)) como entrada (echo)

  DDRD  |=  (1<<PORTD5);         //configura digital 5 (PD5) como saída (ENA)
  PORTD &= ~(1<<PORTD5);         //inicializa digital 5 (PD5) em LOW (ENA)
  DDRD  |=  (1<<PORTD6);         //configura digital 6 (PD6) como saída (ENB)
  PORTD &= ~(1<<PORTD6);         //inicializa digital 6 (PD6) em LOW (ENB)

  DDRB  |=  (1<<PORTB0);         //configura digital 8 (PB0) como saída (EN1)
  PORTB &= ~(1<<PORTB0);         //inicializa digital 8 (PB0) como saída (EN1)
  DDRB  |=  (1<<PORTB1);         //configura digital 9 (PB1) como saída (EN2)
  PORTB &= ~(1<<PORTB1);         //inicializa digital 9 (PB1) como saída (EN2)
  DDRB  |=  (1<<PORTB2);         //configura digital 10 (PB2) como saída (EN3)
  PORTB &= ~(1<<PORTB2);         //inicializa digital 10 (PB2) como saída (EN3)
  DDRB  |=  (1<<PORTB3);         //configura digital 11 (PB3) como saída (EN4)
  PORTB &= ~(1<<PORTB3);         //inicializa digital 11 (PB3) como saída (EN4)

  DDRB  |=  (1<<PORTB5);         //configura digital 13 (PB5) como saída
  PORTB &= ~(1<<PORTB5);         //inicializa digital 13 (PB5) em LOW

  cli();                         //desliga interrupções
  TCCR2A = 0x00;                 //define para operação normal
  TCCR2B = 0x04;                 //prescaler 1:64
  TCNT2  = 0x06;                 //inicia Timer0 para contar até 250
  TIMSK2 = 0x01;                 //habilita interrupção do Timer0
  sei();                         //liga interrupções

  analogWrite(5, pwm0a);
  analogWrite(6, pwm0b);

  delay(5000);
  
}//end main



void loop(){
  dist = measureDistance();

   if(!carlos){
    if((dist <= 15.0)){
      for(int i = 0; i < 0xFF; i++){
        pwm0a = i;
        pwm0b = i;
        motorConfig(forward);
      }//end for
      PORTB |= (1<<PORTB5);
    }else if(dist > 15.0){
      motorConfig(left);
      PORTB &= ~(1<<PORTB5); 
    }//end else if
  }//end if
}//end loop



float measureDistance(){         //Função que retorna a distância em centímetros

  float pulse,                   //Armazena o valor de tempo em µs que o pino echo fica em nível alto
        newPulse;
        
  PORTC |= (1<<PORTC1);          //Saída de trigger em nível alto
  delayMicroseconds(10);         //Por 10µs ...
  PORTC &= ~(1<<PORTC1);         //Saída de trigger volta a nível baixo

  pulse = pulseIn(A0, HIGH);     //mede o tempo em que echo fica em nível alto e armazena na variável pulse
  (pulse/58.2) != 0 ? newPulse = (pulse/58.2) : newPulse = 500;
  Serial.print(newPulse);        //envia o valor em centimetros no monitor Serial
  Serial.println("cm");          //envia "cm" no monitor Serial
  return (newPulse);             //calcula distância em centímetros e retorna o valor
}//end measureDistante


void motorConfig(int modo){
  analogWrite(6, pwm0a);
  analogWrite(5, pwm0b);
  switch(modo){
     case 1:

      PORTB |=  (1<<PORTB0);
      PORTB &= ~(1<<PORTB1);
      PORTB |=  (1<<PORTB2);
      PORTB &= ~(1<<PORTB3);
      break;
    case 2:
      PORTB &= ~(1<<PORTB0);
      PORTB |=  (1<<PORTB1);
      PORTB &= ~(1<<PORTB2);
      PORTB |=  (1<<PORTB3);
      break;
    case 3:
      PORTB |=  (1<<PORTB0);
      PORTB &= ~(1<<PORTB1);
      PORTB &= ~(1<<PORTB2);
      PORTB |=  (1<<PORTB3);
      break;
    case 4:
      PORTB &= ~(1<<PORTB0);
      PORTB |=  (1<<PORTB1);
      PORTB |=  (1<<PORTB2);
      PORTB &= ~(1<<PORTB3);
      break;
    default:
      Serial.println("Tudo errado");
      break;
  }
}//end motorConfig

void search(){
  for(int i = 0; i < 0xB0; i++){
    pwm0a = i;
    pwm0b = i;
  motorConfig(forward);
  }
  delay(300);
  motorConfig(right);
  delay(300);
}
