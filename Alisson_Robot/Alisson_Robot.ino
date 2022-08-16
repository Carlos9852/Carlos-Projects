// =========================================================
// --- Váriaveis Globais ---
float dist;
int   pwm0a = 150,
      pwm0b = 150;
bool  flag;


// =========================================================
// --- Constantes Globais ---
const int backward = 0x03,
          forward  = 0x84,
          left     = 0x81,
          right    = 0x06;


// =========================================================
// --- Protótipo das Funções ---
float measureDistance();
void motorConfig(byte motor);


// =========================================================
// --- Interrupção ---
ISR(TIMER2_OVF_vect){
  static int baseT1 = 0,           //variável local estática para base de tempo 1
             baseT2 = 0,           //variável local estática para base de tempo 2
             Y      = 0, 
             X      = 0;

  TCNT2 = 0x06;                  //reinicializa Timer0

  baseT1++;                        //incremente baseT1 em um
  baseT2++;                        //incremente baseT2 em um

  if(baseT1 == 1000){
    baseT1 = 0;
    Y = PINC & (1<<PORTC0);
    X = PINC & (1<<PORTC1);
  }

  if(Y){
   baseT2 = 0;
   motorConfig(backward); 
  }

  if(X){
   baseT2 = 0;
   motorConfig(forward); 
  }

  if(baseT2 == 600) motorConfig(left);

  if(flag) motorConfig(right);
  
}//end ISR

// =========================================================
// --- Função Principal ---
void setup(){

  Serial.begin(9600);

  DDRB  |= (1<<PORTB0);                //configura digital 8 (PB0) como saída (data)
  DDRB  |= (1<<PORTB4);                //configura digital 12 (PB4) como saída (clk)
  DDRD  |= (1<<PORTD4);                //configura digital 4 (PD4) como saída (latch)

  DDRD  |= (1<<PORTD5);                //configura digital 5 (PD5) como entrada (pwm0b)
  PORTD &= ~(1<<PORTD5);               //inicializa digital 5 (PD5) em LOW (pwm0b)
  DDRD  |= (1<<PORTD6);                //configura digital 5 (PD5) como entrada (pwm0a)
  PORTD &= ~(1<<PORTD6);               //inicializa digital 6 (PD6) em LOW (pwm0a)

  DDRC  &= ~(1<<PORTC0);               //configura analogico 0 (PC0) como entrada (sensor1)
  PORTC |=  (1<<PORTC0);               //habilita o resistor de pull-up interno do A0
  DDRC  &= ~(1<<PORTC1);               //configura analogico 1 (PC1) como entrada (sensor2)
  PORTC |=  (1<<PORTC1);               //habilita o resistor de pull-up interno do A1

  DDRC  |=  (1<<PORTC2);              //configura analogico 2 (PC2) como saída  (trig)
  PORTC &= ~(1<<PORTC2);              //inicializa analogico 2 (PC2) em LOW (trig)  
  DDRC  &= ~(1<<PORTC3);              //configura analogico 3 (PC3) como entrada (echo)

  cli();                         //desliga interrupções
  TCCR2A = 0x00;                 //define para operação normal
  TCCR2B = 0x04;                 //prescaler 1:64
  TCNT2  = 0x06;                 //inicia Timer0 para contar até 250
  TIMSK2 = 0x01;                 //habilita interrupção do Timer0
  sei();                         //liga interrupções

  analogWrite(6, pwm0a);
  analogWrite(5, pwm0b);
}//end main



void loop(){

  dist = measureDistance();
  if(dist > 15.0){
    pwm0b = 110;
    pwm0a = 100;
    flag = true;
  }
    else{
      pwm0b = 250;
      pwm0a = 245;
      motorConfig(forward);
    }
}



float measureDistance(){         //Função que retorna a distância em centímetros

  float pulse;                   //Armazena o valor de tempo em µs que o pino echo fica em nível alto

  PORTC |= (1<<PORTC2);          //Saída de trigger em nível alto
  delayMicroseconds(10);         //Por 10µs ...
  PORTC &= ~(1<<PORTC2);         //Saída de trigger volta a nível baixo

  pulse = pulseIn(A3, HIGH);     //Mede o tempo em que echo fica em nível alto e armazena na variável pulse
  return (pulse/58.82);          //Calcula distância em centímetros e retorna o valor
}//end measureDistante


void motorConfig(byte motor){

  if((motor==left) || (motor==right)){
    pwm0a = 100;
    pwm0b = 110;
  }
  
  PORTB &= ~(1<<PORTB4);
  PORTD &= ~(1<<PORTD4);
  for(int i=0; i<8; i++){
    if((motor>>i) & 0x01) PORTB |= (1<<PORTB0);
      else PORTB &= ~(1<<PORTB0);
    PORTD &= ~(1<<PORTD4);
    delayMicroseconds(2);
    PORTD |= (1<<PORTD4);
    delayMicroseconds(2);
    PORTD &= ~(1<<PORTD4);
  }
  PORTB |= (1<<PORTB4);

  analogWrite(6, pwm0a);
  analogWrite(5, pwm0b);
}
