/* 
    Data: 13/03/2025 15h29
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

SoftwareSerial softSerial(4, 5); // RX, TX (ajuste conforme necessário)
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

  // Inicializando o DFPlayer Mini
  if (!myDFPlayer.begin(softSerial)) {
    Serial.println("Erro ao iniciar DFPlayer!");
    while (true); // Para o código caso o módulo não seja detectado
  }
  
  myDFPlayer.volume(20);  // Ajusta o volume (0 a 30)
}

// Função para medir a tensão no pino A0
float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0;
  delay(450);
  return tensao_real;
}

// Função para medir a corrente no pino A2
float medidor_de_corrente() {
  int leituraAnalogica = analogRead(A2);
  float tensao_sensor = (leituraAnalogica * 5) / 1024.0;
  float corrente = abs((tensao_sensor - 2.50) / 0.185);
  return corrente;
}

// Exibe e reproduz áudio da corrente
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente();
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);
  Serial.println(" A");

  myDFPlayer.play(2); // Reproduz "0002.mp3" (deve estar no cartão SD)
  delay(1000);
}

// Exibe e reproduz áudio da tensão
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);
  Serial.println(" V");

  myDFPlayer.play(1); // Reproduz "0001.mp3" (deve estar no cartão SD)
  delay(1000);
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
