/* 
    Data: 25/01/2025 20h05
    by JK
*/

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

int botao_Tensao = 3;          
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

SoftwareSerial softSerial(4, 5); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600); 
  softSerial.begin(9600);

  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(botao_Tensao, INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+");

  if (!myDFPlayer.begin(softSerial)) {
    Serial.println("Erro ao iniciar DFPlayer!");
    while (true);
  }
  
  myDFPlayer.volume(20);
}

// Converte número e toca os áudios
void falarNumero(float valor, bool isTensao) {
  int parteInteira = (int)valor;
  int parteDecimal = (int)((valor - parteInteira) * 10);

  myDFPlayer.play(parteInteira); // Exemplo: 3 → "0003.mp3"
  delay(1000);

  if (parteDecimal > 0) { 
    myDFPlayer.play(99); // "vírgula.mp3" (99 é um exemplo, ajuste conforme necessário)
    delay(500);
    myDFPlayer.play(parteDecimal);
    delay(1000);
  }

  if (isTensao) {
    myDFPlayer.play(98); // "volts.mp3" (98 é um exemplo)
  } else {
    myDFPlayer.play(97); // "amperes.mp3" (97 é um exemplo)
  }

  delay(1000);
}

// Medir e exibir tensão
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 1);
  Serial.println(" V");

  falarNumero(valor_tensao, true);
}

// Medir e exibir corrente
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente();
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 1);
  Serial.println(" A");

  falarNumero(valor_corrente, false);
}

// Medir tensão
float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);
  return (leituraAnalogica * 5.0 * 5.0) / 1024.0;
}

// Medir corrente
float medidor_de_corrente() {
  int leituraAnalogica = analogRead(A2);
  float tensao_sensor = (leituraAnalogica * 5) / 1024.0;
  return abs((tensao_sensor - 2.50) / 0.185);
}

void loop() {
  estadoBotaoTensao = digitalRead(botao_Tensao);
  estadoBotaoCorrente = digitalRead(botao_Corrente);

  if (estadoBotaoTensao == LOW) {
    continuarC = 0;
    continuarT = (continuarT < 2) ? continuarT + 1 : 0;
  }

  if (estadoBotaoCorrente == LOW) {
    continuarT = 0;
    continuarC = (continuarC < 2) ? continuarC + 1 : 0;
  }

  if (continuarT == 1) {
    exibir_Tensao_PTBR();
  }

  if (continuarC == 1) {
    exibir_Corrente_PTBR();
  }

  if (continuarT == 0 && continuarC == 0) {
    delay(500);
  }
}
