/* ============================================================================

    Automatic Irrigation
    
    Autor: Carlos Daniel de Souza Nunes
    Data: 15/09/2021

============================================================================ */


/* ========================================================================= */
/* --- Bibliotecas --- */
#include <Wire.h>
#include "RTClib.h"


/* ========================================================================= */
/* --- Variaveis Globais --- */
const int ralayPin =   2,        //pino definido para conexao com o rele
          Hour1     =  7,        //variavel de hora que o rele ira acionar
          Minute1   = 30,        //variavel de minuto que o rele ira acionar
          Second1   =  0;        //variavel de segundo que o rele ira acionar

bool      logicRelay = false;


/* ========================================================================= */
/* --- Função Principal --- */
void setup(){
  Serial.begin(9600);                              //Inicializa a comunicacao serial
  if (!rtc.begin()) {                              //Se o RTC nao for inicializado, faz
    Serial.println("RTC NAO INICIALIZADO");        //Imprime o texto
    while (1);                                     //Trava o programa
  }
  //rtc.adjust(DateTime(2019, 3, 18, 10, 53, 00)); //Ajusta o tempo do RTC para a data e hora definida pelo usuario.
  delay(100);                                      //100 Milissegundos

  pinMode(relayPin,           OUTPUT);
  digitalWrite(relayPin, !logicRelay);
  
} /* end setup */


/* ========================================================================= */
/* --- Loop Infinito --- */
void loop ()
{
  DateTime agora = rtc.now();                                  //Faz a leitura de dados de data e hora
  Serial.print("Horas: ");                                     //Imprime texto
  Serial.print(agora.hour(), DEC);                             //Imprime hora
  Serial.print(':');                                           //Imprime dois pontos
  Serial.print(agora.minute(), DEC);                           //Imprime os minutos
  Serial.print(':');                                           //Imprime dois pontos
  Serial.print(agora.second(), DEC);                           //Imprime os segundos
  Serial.println();                                            //Quebra de linha
  delay(1000);                                                 //1 Segundo

  if ((agora.hour() == Hour1) && (agora.minute() == Minute1) && (agora.second() == Second1)) {  //se no instante que hora atual for igual a hora da variavel
    digitalWrite(relayPin,  logicRelay);                                                        //aciona rele
  } else {                                                                                      //senao
    digitalWrite(relayPin, !logicRelay);                                                        //desaciona o rele
  }
} /* end loop */


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
