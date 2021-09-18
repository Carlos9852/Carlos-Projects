/* ============================================================================
    Automatic Irrigation
    
    Autor: Carlos Daniel de Souza Nunes
    Data: 15/09/2021
============================================================================ */


/* ========================================================================= */
/* --- Bibliotecas --- */
#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>


/* ========================================================================= */
/* --- Constantes e Variaveis Globais --- */
#define sensorUmid A0  
const int rs = D7, en = D6, d4 = D5, d5 = D4, d6 = D3, d7 = D0,
          relayPin   =  D8,        //pino definido para conexao com o rele
          Hour1      =   0,        //variavel de hora que o rele ira acionar
          Minute1    =  25,        //variavel de minuto que o rele ira acionar
          Second1    =   0,        //variavel de segundo que o rele ira acionar
          Hour2      =   0,        //variavel de hora que o rele ira acionar
          Minute2    =  31,        //variavel de minuto que o rele ira acionar
          Second2    =   0;        //variavel de segundo que o rele ira acionar

int       tempoRega  =   2,
          valor, 
          umid;

unsigned long delay1 = 0,
              delay2 = 0;

bool      logicRelay = true;


/* ========================================================================= */
/* --- Protótipos das Funções --- */
void lcdPrint();
void irrigar();
void solo();


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
  Serial.begin(115200);                              //Inicializa a comunicacao serial
  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Smart Watering");
  lcd.setCursor(4, 1);
  lcd.print("By Carlos");
  delay(5000);
  if (!rtc.begin()) {                              //Se o RTC nao for inicializado, faz
    lcd.clear();
    lcd.print("RTC NAO INICIOU");        //Imprime o texto
    while (1);                                     //Trava o programa
  }
  //rtc.adjust(DateTime(2021, 9, 21, 13, 21, 00)); //Ajusta o tempo do RTC para a data e hora definida pelo usuario.
  delay(100);                                      //100 Milissegundos
  
  tempoRega = tempoRega * 60;

  pinMode(relayPin,           OUTPUT);
  digitalWrite(relayPin, !logicRelay);
  
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
  DateTime agora = rtc.now();                                  //Faz a leitura de dados de data e hora

  valor = analogRead(sensorUmid);
  umid = map(valor, 0, 1023, 100, 0);
  lcd.clear();
  lcd.print("Hora: ");                                      //Imprime texto
  lcd.print(agora.hour(), DEC);                             //Imprime hora
  lcd.print(':');                                           //Imprime dois pontos
  lcd.print(agora.minute(), DEC);                           //Imprime os minutos
  lcd.print(':');                                           //Imprime dois pontos
  lcd.print(agora.second(), DEC);                         //Imprime os segundos
  
} /* end lcdPrint */

void solo(){

  static bool aux;
  
  valor = analogRead(sensorUmid);
  DateTime agora = rtc.now();
  umid = map(valor, 0, 1023, 100, 0);
  
  lcd.setCursor(0,1);
  lcd.print("Umidade: ");
  lcd.print(umid);
  lcd.print('%');

  if (((agora.hour() == Hour1) && (agora.minute() == Minute1) && (agora.second() == Second1)) || ((agora.hour() == Hour2) && (agora.minute() == Minute2) && (agora.second() == Second2))) {  //se no instante que hora atual for igual a hora da variavel
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Analisando!");
    delay(5000);
    if(umid <= 70){
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Solo seco!");
      delay(5000);
      for(int i = 0; i <= tempoRega; i++){
        digitalWrite(relayPin, logicRelay);
        lcdPrint();
        lcd.setCursor(0, 1);
        lcd.print("Regando");
        for(int i = 0; i < 3; i++){
          lcd.print('.');
          delay(333);
        }
        aux = true;
      }
      lcd.clear();
      lcd.print("Solo Regado");
      delay(5000);
    }
    else{
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Solo umido!");
      delay(5000);
    }
  }

  if(aux == true){
    aux = false;
    digitalWrite(relayPin, !logicRelay);
  }
}

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
