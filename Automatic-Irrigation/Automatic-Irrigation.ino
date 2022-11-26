/* ============================================================================
    Smart Watering
    
    Autor: Carlos Daniel de Souza Nunes
    Data: 15/09/2021
============================================================================ */


/* ========================================================================= */
/* --- Bibliotecas --- */
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>


/* ========================================================================= */
/* --- Constantes e Variaveis Globais --- */
#define sensorUmid A0  
const int rs = 2, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8,      //variaveis dos pinos do lcd
          relayPin   =  9,                                //pino definido para conexao com o rele
          Hour1      =   2,                                //variavel de hora1 que o rele ira acionar
          Minute1    =  37,                                //variavel de minuto1 que o rele ira acionar
          Second1    =   0,                                //variavel de segundo1 que o rele ira acionar
          Hour2      =  2,                                //variavel de hora2 que o rele ira acionar
          Minute2    =  45,                                //variavel de minuto2 que o rele ira acionar
          Second2    =   0,                                //variavel de segundo2 que o rele ira acionar
          btn_blk    =  1,
          backlight  =   3;

int       tempoRega  =   2;                                //variavel para o tempo de que o rele vai ficar ligado
          
unsigned long delay1 = 0;                                  //variavel para o uso da millis();

bool      logicRelay = false;                               //variavel que controla a lógica do rele


/* ========================================================================= */
/* --- Interrupção --- */
ISR(TIMER2_OVF_vect){
  static int baseT1 = 0,         //variável local estática para base de tempo 1
             baseT2 = 0;         //variável local estática para base de tempo 2
             
  static bool       flag = false;
  TCNT2 = 0x06;                  //reinicializa Timer0

  if(digitalRead(btn_blk) == LOW) {
    flag = true;
    digitalWrite(backlight, HIGH);
  }

  if(flag = true) baseT1++;                      //incremente baseT2 em um

  if( (baseT1 == 10000) && (flag == true)){
    baseT1 = 0;
    flag = false;
    digitalWrite(backlight, LOW);
  }

}//end ISR


/* ========================================================================= */
/* --- Protótipos das Funções --- */
void lcdPrint();                                           //Protótipo da função lcdPrint.
void solo();                                               //Protótipo da função solo.


/* ========================================================================= */
/* --- Instâncias --- */
//As linhas de codigo a seguir devem ser comentadas, ou descomentadas, de acordo com o modelo de RTC utilizado (DS1307 ou DS3132)
//RTC_DS1307 rtc; //Objeto rtc da classe DS1307
RTC_DS3231 rtc;   //Objeto rtc da classe DS3132
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
      
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"}; //Dias da semana


/* ========================================================================= */
/* --- Função Principal --- */
void setup(){
  Serial.begin(115200);                                    //Inicializa a comunicacao serial
  digitalWrite(backlight, HIGH);
  lcd.begin(16, 2);                                        //Inicializa o LCD 16x2
  lcd.setCursor(1, 0);                                     //Define o cursor para a coluna 2 e linha 1
  lcd.print("Smart Watering");                             //Imprime a "Smart Watering" no display
  lcd.setCursor(4, 1);                                     //Define o cursor para a coluna 5 e linha 2
  lcd.print("By Carlos");                                  //Imprime "By Carlos" no LCD
  delay(5000);                                             //Espera 5 segundos
  if (!rtc.begin()) {                                      //RTC não foi inicializado? Sim.
    lcd.clear();                                           //Limpa o LCD
    lcd.print("RTC NAO INICIOU");                          //Imprime o texto no LCD
    while (1);                                             //Trava o programa
  }
  //rtc.adjust(DateTime(2021, 9, 21, 13, 21, 00));         //Ajusta o tempo do RTC para a data e hora definida pelo usuario.
  delay(100);                                              //Espera 100 Milissegundos
  
  tempoRega = tempoRega * 60;                              //Ajusta o tempo para rega de minutos para segundos

  pinMode(relayPin,           OUTPUT);                     //Define o pino do rele como saida
  pinMode(backlight, OUTPUT);
  pinMode(btn_blk, INPUT_PULLUP);
  digitalWrite(relayPin, !logicRelay);                     //Inicia o pino do rele como desligado
  digitalWrite(backlight, LOW);

  cli();                         //desliga interrupções
  TCCR2A = 0x00;                 //define para operação normal
  TCCR2B = 0x04;                 //prescaler 1:64
  TCNT2  = 0x06;                 //inicia Timer0 para contar até 250
  TIMSK2 = 0x01;                 //habilita interrupção do Timer0
  sei();                         //liga interrupções
  
} /* end setup */


/* ========================================================================= */
/* --- Loop Infinito --- */
void loop ()
{ 
  if((millis() - delay1) > 1000){
    delay1 = millis();
    lcdPrint();  
    solo();
  }
} /* end loop */


/* ========================================================================= */
/* --- Outras Funções --- */
void lcdPrint(){
  DateTime agora = rtc.now();                              //Faz a leitura de dados de data e hora

  lcd.clear();
  lcd.print("Hora: ");                                     //Imprime texto
  if(agora.hour() < 10){
    lcd.print('0');
  }
  lcd.print(agora.hour(), DEC);                            //Imprime hora
  lcd.print(':');                                          //Imprime dois pontos
  if(agora.minute() < 10){
    lcd.print('0');
  }
  lcd.print(agora.minute(), DEC);                          //Imprime os minutos
  lcd.print(':');                                          //Imprime dois pontos
  if(agora.second() < 10){
    lcd.print('0');
  }
  lcd.print(agora.second(), DEC);                          //Imprime os segundos
  
} /* end lcdPrint */

void solo(){

  static bool aux;                                         //Variavel auxiliar. OBS: é necessária?
  int       valor,                                         //Variavel para armazenar o valor lido pela porta analógica
             umid;                                         //Variavel que converte o valor SensorUmid para porcentagem
  
  valor = analogRead(sensorUmid);                          //Faz a leitura da porta analógica e salva o valor em "valor"
  DateTime agora = rtc.now();                              //Faz a leitura da horario
  umid = map(valor, 0, 1023, 100, 0);                      //Mapeia a variavel valor para umid
  
  lcd.setCursor(0,1);                                      //Define o cursor para coluna 1 e linha 2
  lcd.print("Umidade: ");                                  //Imprime o texto no LCD
  if(umid < 10){
    lcd.print('0');
  }
  lcd.print(umid);                                         //Imprime o valor de umid no LCD
  lcd.print('%');                                          //Imprime o caracter '%' no LCD

  if (((agora.hour() == Hour1) && (agora.minute() == Minute1) && (agora.second() == Second1)) || ((agora.hour() == Hour2) && (agora.minute() == Minute2) && (agora.second() == Second2))) {  //se no instante que hora atual for igual a hora da variavel
    lcd.clear();                                           //Limpa a tela do LCD
    digitalWrite(backlight, HIGH);
    lcd.setCursor(3, 0);                                   //Define o cursor para a linha 4 e coluna 1
    lcd.print("Analisando!");                              //Imprime o texto no LCD
    delay(5000);                                           //Espera por 5 segundos
    if(umid <= 70){                                        //A umidade é menor que 70%? sim?
      lcd.clear();                                         //Limpa a tela do LCD
      lcd.setCursor(3, 0);                                 //Define o cursor para a coluna 4 e linha 1
      lcd.print("Solo seco!");                             //Imprime o Texto no LCD
      delay(5000);                                         //Espera por 5 segundos
      for(int i = 0; i <= tempoRega; i++){                 //Para i igual a 0, menor que o tempoRega incrementa em um
        digitalWrite(relayPin, logicRelay);                //Liga o rele
        lcdPrint();                                        //Chama a função lcdPrint
        lcd.setCursor(0, 1);                               //Define o cursor para a coluna 1 e linha 2
        lcd.print("Regando");                              //Imprime o texto no LCD
        for(int i = 0; i < 4; i++){                        //Para i igual a 0, i menor que 3, i incrementa em um
          lcd.print('.');                                  //Imprime um caracter no LCD
          delay(250);                                      //Espera 333 milisegundos
        }
        aux = true;
      }
      lcd.clear();                                         //Limpa a tela do LCD
      lcd.print("Solo Regado");                            //Imprime o texto na tela do LCD
      delay(5000);                                         //Espera por 5 segundos
      digitalWrite(backlight, LOW);
    }
    else{                                                  //A umidade está menor que 70%? Não?
      lcd.clear();                                         //Limpa a tela do LCD
      lcd.setCursor(3, 0);                                 //Define o cursor para a coluna 4 e linha 1
      lcd.print("Solo umido!");                            //Imprime o texto na tela
      delay(5000);                                         //Espera por 5 segundos
      digitalWrite(backlight, LOW);
    }
  }

  if(aux == true){                                         //A variavel aux está como high? Sim?
    aux = false;                                           //Define aux com low
    digitalWrite(relayPin, !logicRelay);                   //Desliga o rele
  }
} /* end solo */

/* ============================================================================  
                                                              
                                       _                      
                                      / \                     
                                     |oo >                    
                                     _\=/_                    
                    ___         #   /  _  \   #               
                   /<> \         \\//|/.\|\\//                
                 _|_____|_        \/  \_/  \/                 
                | | === | |          |\ /|                    
                |_|  0  |_|          \_ _/                    
                 ||  0  ||           | | |                    
                 ||__*__||           | | |                    
                |* \___/ *|          []|[]                    
                /=\ /=\ /=\          | | |                    
________________[_]_[_]_[_]_________/_]_[_\_______________________________
                                                              
                                                              
============================================================================ */
/* --- Final do Programa --- */
