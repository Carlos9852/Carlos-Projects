// =========================================================
// --- Bibliotecas Auxiliares ---


// =========================================================
// --- Mapeamento de Hardware ---
#define IN1    10                //Driver L293D
#define IN2    11                //Driver L293D
#define ENA     5                //Enable driver
#define ENB     6                //Enable driver
#define IN3     9                //Driver L293D
#define IN4     8                //Driver L293D
#define trig    4                //Saída para o pino de trigger do sensor
#define echo    3                //Entrada para o pino de echo do sensor


// =========================================================
// --- Váriaveis Globais ---
float dist;
int   pwm,
       Y      = 0, 
       X      = 0;
bool flag = false;
int   pwm0a = 150,
      pwm0b = 150;


// =========================================================
// --- Constantes Globais ---
const int forward =  1,          //constante para motor ir pra frente
          backward = 2,          //constante para motor ir pra trás
          left     = 3,          //constante para motor ir pra esquerda
          right    = 4;          //constante para motor ir pra direita


// =========================================================
// --- Protótipo das Funções ---
float measureDistance();         //função para medir a distândia do ultrassônico
void motorConfig(int modo);      //função para o controle da ponte H


// =========================================================
// --- Interrupção ---
ISR(TIMER2_OVF_vect){
  static int baseT1 = 0,         //variável local estática para base de tempo 1
             baseT2 = 0;         //variável local estática para base de tempo 2
            

  TCNT2 = 0x06;                  //reinicializa Timer0

  baseT1++;                      //incremente baseT1 em um
  baseT2++;                      //incremente baseT2 em um

  if(baseT1 == 1000){            //se a baseT1 for igual a 1000
    baseT1 = 0;                  //zera a variável baseT1
    Y = PIND & (1<<PORTD3);      //armazena o valor do sensor1 em X
    X = PIND & (1<<PORTD2);      //armazena o valor do sensor2 em Y
  }//end if

  if(flag) motorConfig(right);   //se a flag igual verdadeiro, o robô fica girando
  
}//end ISR

// =========================================================
// --- Função Principal ---
void setup(){

  Serial.begin(9600);

  DDRD &= ~(1<<3);               //configura digital 4 (PD4) como entrada (sensor1)
  DDRD &= ~(1<<2);               //configura digital 2 (PD2) como entrada (sensor2)

  DDRC  |=  (1<<0);              //configura analogica 0 (PC0) como saída  (trig)
  PORTC &= ~(1<<0);              //inicializa analogica 0 (PC0) em LOW (trig)  
  DDRC  &= ~(1<<1);              //configura analogica 1 (PC1)) como entrada (echo)

  DDRD  |=  (1<<PORTD5);         //configura digital 5 (PD5) como saída (ENA)
  PORTD &= ~(1<<PORTD5);         //inicializa digital 5 (PD5) em LOW (ENA)
  DDRD  |=  (1<<PORTD6);         //configura digital 6 (PD6) como saída (ENB)
  PORTD &= ~(1<<PORTD6);         //inicializa digital 6 (PD6) em LOW (ENB)

  DDRD  |=  (1<<PORTD2);         //configura digital 2 (PD2) como saída (EN1)
  PORTD &= ~(1<<PORTD2);         //inicializa digital 2 (PD2) como saída (EN1)
  DDRD  |=  (1<<PORTD3);         //configura digital 3 (PD3) como saída (EN2)
  PORTD &= ~(1<<PORTD3);         //inicializa digital 3 (PD3) como saída (EN2)
  DDRD  |=  (1<<PORTD4);         //configura digital 4 (PD4) como saída (EN3)
  PORTD &= ~(1<<PORTD4);         //inicializa digital 4 (PD4) como saída (EN3)
  DDRD  |=  (1<<PORTD7);         //configura digital 7 (PD7) como saída (EN4)
  PORTD &= ~(1<<PORTD7);         //inicializa digital 7 (PD7) como saída (EN4)

  DDRB  |=  (1<<PORTB5);         //configura digital 13 (PB5) como saída
  PORTB &= ~(1<<PORTB5);         //inicializa digital 13 (PB5) em LOW

  cli();                         //desliga interrupções
  TCCR2A = 0x00;                 //define para operação normal
  TCCR2B = 0x04;                 //prescaler 1:64
  TCNT2  = 0x06;                 //inicia Timer0 para contar até 250
  TIMSK2 = 0x01;                 //habilita interrupção do Timer0
  sei();                         //liga interrupções

  analogWrite(5, pwm0b);
  analogWrite(6, pwm0a);
  
}//end main



void loop(){
 
  dist = measureDistance();      //chama a função para o cálculo da distância
  if(dist > 15.0){               //verifica se a distância é maior que 15cm
    pwm0b = 140;                 //define o duty clycle do pino ENB em 55%
    pwm0a = 140;                 //define o duty clycle do pino ENA em 55%
    flag = true;                 //seta flag auxiliar em verdadeiro
  }else{                         //se a distância é menor que 15cm
      flag=false;                //seta a flag auxiliar em falso
      pwm0b = 250;               //define o duty clycle do pino ENB em 98%
      pwm0a = 245;               //define o duty clycle do pino ENA em 96%
      motorConfig(forward);      //função para o mootor andar pra frente
    }//end else
    
   if(Y || X){
    if((X == true) && (Y == false)){
      motorConfig(backward);
      delay(600);
      motorConfig(left);
      delay(300);
      motorConfig(forward);
    }//end if
    else if((X == false) && (Y == true)){
      motorConfig(forward);
      delay(600);
      motorConfig(right);
      delay(300);
      motorConfig(backward);
    }//end else if
   
  }//end if
}//end loop



float measureDistance(){         //Função que retorna a distância em centímetros

  float pulse;                   //Armazena o valor de tempo em µs que o pino echo fica em nível alto

  PORTC |= (1<<0);               //Saída de trigger em nível alto
  delayMicroseconds(11);         //Por 10µs ...
  PORTC &= ~(1<<0);              //Saída de trigger volta a nível baixo

  pulse = pulseIn(A1, HIGH);      //Mede o tempo em que echo fica em nível alto e armazena na variável pulse
  Serial.print(pulse/58.2);
  Serial.println("cm");
  return (pulse/58.82);          //Calcula distância em centímetros e retorna o valor
}//end measureDistante


void motorConfig(int modo){
  analogWrite(6, pwm0a);
  analogWrite(5, pwm0b);
  switch(modo){
    case 1:

      PORTD |=  (1<<PORTD2);
      PORTD &= ~(1<<PORTD3);
      PORTD |=  (1<<PORTD4);
      PORTD &= ~(1<<PORTD7);
      break;
    case 2:
      PORTD &= ~(1<<PORTD2);
      PORTD |=  (1<<PORTD3);
      PORTD &= ~(1<<PORTD4);
      PORTD |=  (1<<PORTD7);
      break;
    case 3:
      PORTD &= ~(1<<PORTD2);
      PORTD |=  (1<<PORTD3);
      PORTD |=  (1<<PORTD4);
      PORTD &= ~(1<<PORTD7);
      break;
    case 4:
      PORTD |=  (1<<PORTD2);
      PORTD &= ~(1<<PORTD3);
      PORTD &= ~(1<<PORTD4);
      PORTD |=  (1<<PORTD7);
      break;
    default:
      Serial.println("Tudo errado");
      break;
  }
}//end motorConfig
