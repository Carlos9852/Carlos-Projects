//==========================================================
// --- Libraries ---
#include <LCD.h>
#include <ResponsiveAnalogRead.h>



//==========================================================
// --- Libraries ---
const int rs = PORTD2,
          en = PORTD4,
          d4 = PORTB1,
          d5 = PORTB2,
          d6 = PORTB3,
          d7 = PORTB4;

#define readVolt  A0


//==========================================================
// --- Libraries ---
LCD lcd(rs, en, d4, d5, d6, d7);
ResponsiveAnalogRead analog0(readVolt, true);



//==========================================================
// --- Libraries ---
int pwm1  = 0x00,
    timer = 0x00;

bool btnUp_f   = false,
     btnDown_f = false,
     btn3_f    = false,
     btn3_func = false;


//==========================================================
// --- Libraries ---
void voltageAjust();
float map(float x, float in_min, float in_max, float out_min, float out_max);


//==========================================================
// --- Libraries ---
ISR(TIMER2_OVF_vect) {
  TCNT2 = 0x05;

  static int cont1 = 0;

  if (cont1 == 100) {
    if (btn3_f) timer++;

    if (timer == 20) {
      PORTB &= ~(1 << PORTB5);
      timer = 0x00;
      btn3_f = false;
      btn3_func = true;
    }

    cont1 = 0;
  }

  cont1++;

}



//==========================================================
// --- Libraries ---
void setup() {

  cli();

  DDRB |= (1 << PORTB5);

  DDRD  |=  (1 << PORTD5);
  DDRD  |=  (1 << PORTD6);
  PORTD &= ~(1 << PORTD5);
  PORTD &= ~(1 << PORTD6);

  DDRD  &= ~(1 << PORTD3);
  DDRD  &= ~(1 << PORTD7);
  DDRB  &= ~(1 << PORTB0);
  PORTD |=  (1 << PORTD3);
  PORTD |=  (1 << PORTD7);
  PORTB |=  (1 << PORTB0);

  lcd.begin();
  lcd.print("carlos");

  TCCR2A = 0x00;
  TCCR2B = 0x04;
  TCNT2  = 0x05;
  TIMSK2 = 0x01;

  TCCR0A = 0xA3;
  /*
     | option |   freq   |
     |   1    |  62,5kHz |
     |   2    |  7,81kHz |
     |   3    |  976,5Hz |
     |   4    |  244,1Hz |
     |   5    |  61,03Hz |
  */
  TCCR0B = 0x03;

  sei();

  OCR0A = 0x00;
  OCR0B = 0x7F;

  while (true) {
    voltageAjust();
    voltmeter();
    delay(20);
  }
}


//==========================================================
// --- Libraries ---
void voltageAjust() {

  if (!(PIND & (1 << PORTD3))) btnUp_f = true;
  if (!(PIND & (1 << PORTD7))) btnDown_f = true;
  if (!(PINB & (1 << PORTB0))) btn3_f = true;

  if ((PIND & (1 << PORTD3)) && btnUp_f) {
    /*Ação*/
    pwm1++;
    if (pwm1 > 253) pwm1 = 0xff;
    btnUp_f = false;
  }
  if ((PIND & (1 << PORTD7)) && btnDown_f) {
    /*Ação*/
    pwm1--;
    if (pwm1 < 1) pwm1 = 0x00;
    btnDown_f = false;
  }
  if ((PINB & (1 << PORTB0)) && btn3_f) {
    /*Ação*/
    btn3_f = false;
    timer = 0x00;
    if (!btn3_func) PORTB |= (1 << PORTB5);
    btn3_func = false;
  }
  OCR0A = pwm1;
}


void voltmeter() {
  static float volt;
  static int volt_i, mil_v, cen_v, dez_v, uni_v;
  
  analog0.update();

  volt  = ((analog0.getValue()*5.0)/1023.0) *400.0;
  volt_i = (int)(volt);

  mil_v = volt_i / 1000;
  cen_v = (volt_i%1000)/100;
  dez_v = (volt_i%100)/10;
  uni_v = volt_i % 10;

  lcd.setCursor(1,1);
  lcd.print("Voltage: ");
  lcd.sendChar(mil_v+0x30);
  lcd.sendChar(cen_v+0x30);
  lcd.sendChar('.');
  lcd.sendChar(dez_v+0x30);
  lcd.sendChar(uni_v+0x30);
  lcd.sendChar('V');

  

}


float map(float x, float in_min, float in_max, float out_min, float out_max) { //Range de valores mínimos e máximos
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;      //Retorna os valores como float
}
