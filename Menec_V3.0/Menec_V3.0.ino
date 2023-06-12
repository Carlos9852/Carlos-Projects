//===========================================================================
// --- mapeamento de hardware ---
int ena  = 5,                     //define ena  como digital  5(PD5)
    enb  = 6,                     //define enb  como digital  6(PD6)
    in1  = (1 << PORTB0),         //define in1  como digital  8(PB0)
    in2  = (1 << PORTB1),         //define in2  como digital  9(PB1)
    in3  = (1 << PORTB2),         //define in3  como digital 10(PB2)
    in4  = (1 << PORTB3),         //define in4  como digital 11(PB3)
    led  = (1 << PORTB5),         //define led  como digital 13(PB5)
    echo = (1 << PORTC0),         //define echo como digital 14(PC0)
    trig = (1 << PORTC1),         //define trig como digital 15(PC1)
    sens = (1 << PORTC2);         //define sens como digital 16(PC2)


//===========================================================================
// --- constantes globais ---
#define forward 1
#define backward 2
#define left 3
#define right 4


//===========================================================================
// --- variáveis globais ---
bool sensor, 
     estado, 
     flag = false;
int  pwma = 150,
     pwmb = 150;


//===========================================================================
// --- Protótipo das funções ---
int adConv(int input);
void motorConfig(int option);
float distanceMeasure();
void search();
void settings();


//===========================================================================
// --- Interrupção ---
ISR(TIMER1_OVF_vect) {
  TCNT1 = 0xc17f;

  static int cont = 0, cont2;
  cont++;

  if (cont == 50) {
    sensor = PINC & sens;
    if (sensor != estado) {
      Serial.print(sensor);
      Serial.println(" Achei a linha!");
      flag = true;
      pwma=pwmb=190;
      motorConfig(backward);
    }
    cont = 0;
  }

  if (flag) {
    cont2++;
    if (cont2 == 100){
      motorConfig(left);
      flag = false;
      cont2=0;
    }
  }
}//end ISR


//===========================================================================
// --- Função Principal ---
void setup() {
  settings();
  estado = PINC & sens;
  Serial.println(estado);
  delay(2000);
  motorConfig(forward);

}


//===========================================================================
// --- Loop infinito ---
void loop() {
  float  dist=distanceMeasure();

  if ((sensor == estado) && (flag==false)){
    Serial.println(dist);
    /*Serial.print(sensor);
    Serial.print(" ");
    Serial.println(estado);*/
    if (dist < 9) {
      Serial.println("Adversário Encontrado!");
      pwma=pwmb=255;
      motorConfig(forward);
    } else {
      Serial.println("Procurando adversário!");
      pwma=pwmb=140;
      search();
    }
  }
  delay(100);
}


//===========================================================================
// --- motorConfig ---
void motorConfig(int option) {
  analogWrite(ena, pwma);
  analogWrite(enb, pwmb);
  switch (option) {
    case forward:
      PORTB |=  in1;
      PORTB &= ~in2;
      PORTB |=  in3;
      PORTB &= ~in4;
      //Serial.println("frente");
      break;

    case backward:
      PORTB &= ~in1;
      PORTB |=  in2;
      PORTB &= ~in3;
      PORTB |=  in4;
      //Serial.println("Trás");
      break;

    case left:
      PORTB &= ~in1;
      PORTB |=  in2;
      PORTB |=  in3;
      PORTB &= ~in4;
      //Serial.println("Esquerda");
      break;

    case right:
      PORTB |=  in1;
      PORTB &= ~in2;
      PORTB &= ~in3;
      PORTB |=  in4;
      //Serial.println("Direita");
      break;

    default:
      Serial.println("Algo deu errado");
      break;
  }//end switch
}//end motorConfig


//===========================================================================
// --- distanceMeasure ---
float distanceMeasure() {
  float pulse, dist;
  PORTC |= trig;
  delayMicroseconds(10);
  PORTC &= ~trig;
  pulse = pulseIn(A0, HIGH);
  (pulse / 58.2) ? dist = pulse / 58.2 : dist = 1000;
  return dist;
}//end distanceMeasure


//===========================================================================
// --- search ---
void search() {
  motorConfig(forward);
  delay(400);
  motorConfig(right);
  delay(200);
}//end search


//===========================================================================
// --- settings ---
void settings() {

  cli();
  Serial.begin(9600);

  DDRB  |=  led;
  PORTB &= ~led;

  DDRD  |=  (1 << PORTD5);   //inicializa pwm1 como saída
  DDRD  |=  (1 << PORTD6);   //inicializa pwm2 como saída
  PORTD &= ~(1 << PORTD5);   //inicializa pwm1 com nível lógico baixo
  PORTD &= ~(1 << PORTD6);   //inicializa pwm2 com nível lógico baixo

  DDRB  |=  in1;             //inicializa in1 como saída      
  DDRB  |=  in2;             //inicializa in2 como saída 
  DDRB  |=  in3;             //inicializa in3 como saída 
  DDRB  |=  in4;             //inicializa in4 como saída 
  PORTB &= ~in1;             //inicializa in1 com nivel lógico baixo 
  PORTB &= ~in2;             //inicializa in2 com nivel lógico baixo 
  PORTB &= ~in3;             //inicializa in3 com nivel lógico baixo 
  PORTB &= ~in4;             //inicializa in4 com nivel lógico baixo 

  DDRC  |=  trig;             //inicializa trigger como saída   
  DDRC  &= ~echo;             //inicializa echo como entrada   
  DDRC  &= ~sens;             //inicializa sens como entrada   
  PORTC |=  trig;             //inicializa trigger com nivel lógico baixo 

  TCCR1A = 0x00;             //inicializa o timer1 no modo normal
  TCCR1B = 0x01;             //define prescaler como 1
  TCNT1  = 0xc17f;           //inicializa o timer1 com 49535
  TIMSK1 = 0x01;             //habilita interrupção por overflow

  ADCSRA = 0x93;             //configura o registrador
  ADMUX = 0x40;              //habilita Vref, deleciona canal A0
  sei();
}//end settings
