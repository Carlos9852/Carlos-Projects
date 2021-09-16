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
const int relayPin   =   2,        //pino definido para conexao com o rele
          Hour1      =  14,        //variavel de hora que o rele ira acionar
          Minute1    =  33,        //variavel de minuto que o rele ira acionar
          Second1    =   0,        //variavel de segundo que o rele ira acionar
          tempoRega  =  2 * 60000;

unsigned long delay1 = 0,
              delay2 = 0;

bool      logicRelay = false;


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
      
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"}; //Dias da semana


/* ========================================================================= */
/* --- Função Principal --- */
void setup(){
  Serial.begin(115200);                              //Inicializa a comunicacao serial
  if (!rtc.begin()) {                              //Se o RTC nao for inicializado, faz
    Serial.println("RTC NAO INICIALIZADO");        //Imprime o texto
    while (1);                                     //Trava o programa
  }
  //rtc.adjust(DateTime(2021, 9, 21, 13, 21, 00)); //Ajusta o tempo do RTC para a data e hora definida pelo usuario.
  delay(100);                                      //100 Milissegundos

  pinMode(relayPin,           OUTPUT);
  digitalWrite(relayPin, !logicRelay);
  
} /* end setup */


/* ========================================================================= */
/* --- Loop Infinito --- */
void loop ()
{
  DateTime agora = rtc.now();

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

  Serial.print("Hora: ");                                      //Imprime texto
  Serial.print(agora.hour(), DEC);                             //Imprime hora
  Serial.print(':');                                           //Imprime dois pontos
  Serial.print(agora.minute(), DEC);                           //Imprime os minutos
  Serial.print(':');                                           //Imprime dois pontos
  Serial.print(agora.second(), DEC);                         //Imprime os segundos
  
} /* end lcdPrint */

void solo(){
  int valor, 
      umid;
  
  valor = analogRead(sensorUmid);
  DateTime agora = rtc.now();
  umid = map(valor, 0, 1023, 100, 0);

  Serial.print("  ");
  Serial.print("Umidade: ");
  Serial.print(umid);
  Serial.println('%');
  
  if ((agora.hour() == Hour1) && (agora.minute() == Minute1) && (agora.second() == Second1)) {  //se no instante que hora atual for igual a hora da variavel
    delay2 = millis();
    if (umid <= 70){
      while((millis() - delay2) < tempoRega){
        digitalWrite(relayPin,  logicRelay);
      }
    }
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
