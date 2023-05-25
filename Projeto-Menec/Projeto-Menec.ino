//===========================================================================
// --- mapeamento de hardware ---
int ena  = 5,
    enb  = 6,
    in1  = (1 << PORTB0),
    in2  = (1 << PORTB1),
    in3  = (1 << PORTB2),
    in4  = (1 << PORTB3),
    led  = (1 << PORTB5),
    echo = (1 << PORTC0),
    trig = (1 << PORTC1),
    sens = (1 << PORTC2);

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
//===========================================================================
// --- Protótipo das funções ---
int ad_conv(int input);
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
      flag = true;
      motorConfig(backward);
    }
    cont = 0;
  }

  if (flag) {
    cont2++;
    if (cont2 == 200){
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
  motorConfig(forward);

}


//===========================================================================
// --- Loop infinito ---
void loop() {
  float  dist = distanceMeasure();

  if ((sensor == estado) && (flag==false)){
    //Serial.print(dist);
    if (dist <= 10) {
      motorConfig(forward);
    } else {
      search();
    }
  }
  //delay(10);
}


//===========================================================================
// --- Conversão AD ---
int ad_conv(int input) {
  static int analog;
  if (input <= 5) {
    ADMUX |= input;
  } else {
    return 0;
  }

  ADCSRA |= (1 << ADSC);
  while (! (ADCSRA &= ~(1 << ADIF)));
  ADCSRA |= (1 << ADIF);
  analog = (ADCH << 8) | ADCL; //une os dois bytes do conversor

  ADMUX = 0x40;   //seleciona o canal A0
  return analog;  //retorna o valor analogico
}//end ad_conv



//===========================================================================
// --- motorConfig ---
void motorConfig(int option) {
  analogWrite(ena, 128);
  analogWrite(enb, 128);
  switch (option) {
    case forward:
      PORTB |=  in1;
      PORTB &= ~in4;
      PORTB |=  in3;
      PORTB &= ~in4;
      Serial.println("frente");
      break;

    case backward:
      PORTB &= ~in1;
      PORTB |=  in2;
      PORTB &= ~in3;
      PORTB |=  in4;
      Serial.println("Trás");
      break;

    case left:
      PORTB &= ~in1;
      PORTB |=  in2;
      PORTB |=  in3;
      PORTB &= ~in4;
      Serial.println("Esquerda");
      break;

    case right:
      PORTB |=  in1;
      PORTB &= ~in4;
      PORTB &= ~in3;
      PORTB |=  in4;
      Serial.println("Direita");
      break;

    default:
      Serial.println("Algo deu errado");
      break;
  }
}


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
}


//===========================================================================
// --- search ---
void search() {
  motorConfig(forward);
  delay(400);
  motorConfig(right);
  delay(200);
}

//===========================================================================
// --- settings ---
void settings() {

  cli();
  Serial.begin(9600);

  DDRB  |=  led;
  PORTB &= ~led;

  DDRD  |=  (1 << PORTD5);
  DDRD  |=  (1 << PORTD6);
  PORTD &= ~(1 << PORTD5);
  PORTD &= ~(1 << PORTD6);

  DDRB  |=  in1;
  DDRB  |=  in2;
  DDRB  |=  in3;
  DDRB  |=  in4;
  PORTB &= ~in1;
  PORTB &= ~in2;
  PORTB &= ~in3;
  PORTB &= ~in4;

  DDRC  |=  trig;
  DDRC  &= ~echo;
  DDRC  &= ~sens;
  PORTC |=  trig;
  //PORTC |=  sens;

  TCCR1A = 0x00;             //inicializa o timer1 no modo normal
  TCCR1B = 0x01;             //define prescaler como 1
  TCNT1  = 0xc17f;           //inicializa o timer1 com 49535
  TIMSK1 = 0x01;             //habilita interrupção por overflow

  ADCSRA = 0x93;             //configura o registrador
  ADMUX = 0x40;              //habilita Vref, deleciona canal A0
  sei();
}
