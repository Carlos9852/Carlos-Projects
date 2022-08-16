// =========================================================
// --- Bibliotecas Auxiliares ---


// =========================================================
// --- Mapeamento de Hardware ---
#define IN1    10                             //Driver L293D
#define IN2    11                             //Driver L293D
#define ENA     5                             //Enable driver
#define ENB     6                             //Enable driver
#define IN3     9                             //Driver L293D
#define IN4     8                             //Driver L293D
#define trig    4                             //Saída para o pino de trigger do sensor
#define echo    3                             //Entrada para o pino de echo do sensor


// =========================================================
// --- Váriaveis Globais ---
float dist;
int   pwm;

// =========================================================
// --- Protótipo das Funções ---
float measureDistance();


// =========================================================
// --- Interrupção ---
ISR(TIMER2_OVF_vect){
  static int baseT1 = 0,           //variável local estática para base de tempo 1
             baseT2 = 0;           //variável local estática para base de tempo 2

  TCNT2 = 0x06;                  //reinicializa Timer0

  baseT1++;                        //incremente baseT1 em um
  baseT2++;                        //incremente baseT2 em um

  if(baseT1 == 10){
    baseT1 = 0;
    PORTB ^= (1<<PORTB5);          //inverte o estado de digital 13 (PB5)
  }
}//end ISR


// =========================================================
// --- Função Principal ---
void setup(){

  Serial.begin(9600);

  DDRD &= ~(1<<4);               //configura digital 4 (PD4) como entrada (sensor1)
  DDRD &= ~(1<<7);               //configura digital 7 (PD7) como entrada (sensor2)

  DDRC  |=  (1<<0);              //configura digital 3 (PD3) como saída  (trig)
  PORTC &= ~(1<<0);              //inicializa digital 3 (PD3) em LOW (trig)  
  DDRC  &= ~(1<<1);              //configura digital 2 (PD2) como entrada (echo)

  DDRD  |=  (1<<5);              //configura digital 5 (PD5) como saída (ENA)
  PORTD &= ~(1<<5);              //inicializa digital 5 (PD5) em LOW (ENA)
  DDRD  |=  (1<<6);              //configura digital 6 (PD6) como saída (ENB)
  PORTD &= ~(1<<6);              //inicializa digital 6 (PD6) em LOW (ENB)
  for(int i = 2; i <= 5; i++){
    DDRC  |=  (1<<i);            //configura A2-A5(PORTC) como saída (IN1--IN4)
    PORTC &= ~(1<<i);            //inicializa A2-A5(PORTC) em LOW (IN1--IN4)
  }


  DDRB  |=  (1<<PORTB5);         //configura digital 13 (PB5) como saída
  PORTB &= ~(1<<PORTB5);         //inicializa digital 13 (PB5) em LOW

  cli();                         //desliga interrupções
  TCCR2A = 0x00;                 //define para operação normal
  TCCR2B = 0x04;                 //prescaler 1:64
  TCNT2  = 0x06;                 //inicia Timer0 para contar até 250
  TIMSK2 = 0x01;                 //habilita interrupção do Timer0
  sei();                         //liga interrupções

  analogWrite(5, 250);
  analogWrite(6, 250);
  
}//end main



void loop(){
  PORTC |= (1<<2);
  PORTC &= ~(1<<3);
  PORTC |= (1<<4);
  PORTC &= ~(1<<5);

}



float measureDistance(){         //Função que retorna a distância em centímetros

  float pulse;                   //Armazena o valor de tempo em µs que o pino echo fica em nível alto

  PORTC |= (1<<0);               //Saída de trigger em nível alto
  delayMicroseconds(11);         //Por 10µs ...
  PORTC &= ~(1<<0);              //Saída de trigger volta a nível baixo

  pulse = pulseIn(A1, HIGH);      //Mede o tempo em que echo fica em nível alto e armazena na variável pulse
  return (pulse/58.82);          //Calcula distância em centímetros e retorna o valor
}//end measureDistante
