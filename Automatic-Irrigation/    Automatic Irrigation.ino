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
const int ralayPin =  2,        //pino definido para conexao com o rele
          Hour     = 10,        //variavel de hora que o rele ira acionar
          Minute   = 53,        //variavel de minuto que o rele ira acionar
          Second   =  0;        //variavel de segundo que o rele ira acionar


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

  pinMode(relayPin,   OUTPUT);
  digitalWrite(relayPin, LOW);
  
} /* end main */


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
