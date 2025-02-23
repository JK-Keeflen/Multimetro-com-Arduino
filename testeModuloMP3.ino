#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define pinRx 10  //porta Rx
#define pinTx 11  //porta Tx
#define botao1 3
#define botao2 4

#define volumeMP3 30

#define DEBUG


// Instanciando Objetos
SoftwareSerial playerMP3Serial(pinRx, pinTx);
DFRobotDFPlayerMini playerMP3;


void setup() {
  // put your setup code here, to run once:
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);

  Serial.begin(9600);
  playerMP3Serial.begin(9600);

  Serial.println();
  Serial.println(F("Iniciando DFPlayer...(Espere 3~5 segundos)"));

  if (!playerMP3.begin(playerMP3Serial)){   //Comunicação
      Serial.println(F("Falha: "));
      Serial.println(F("1. Conexões!"));
      Serial.println(F("2. Cheque o cartão SD!"));
      while(true){
          delay(0);
      }
    
  }

  Serial.println(F("DFPlayer Iniciado!"));
  playerMP3.volume(volumeMP3);

  #ifdef DEBUG
    Serial.println("O Setup acabou");
  #endif

}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (digitalRead(botao1) == HIGH){
    playerMP3.playFolder(2, 1);
    Serial.println("Tocando pasta 02, musica 001");
    delay(1000);
  } else{
    
    if (digitalRead(botao2) == HIGH){
      playerMP3.playFolder(2, 2);
      Serial.println("Tocando pasta 02, musica 001");
      delay(1000);
    }  else{
      Serial.println("Não está tocando nada");
      delay(1000);
    }
  }

}
