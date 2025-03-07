#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// Pinos para o DFPlayer (RX, TX)
SoftwareSerial dfPlayerSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;

// Pino para o botão
const int BOTAO_TENSAO = 3;

// Constantes para os arquivos de áudio
const int AUDIO_VIRGULA = 1000;  // Arquivo "virgula.mp3"
const int AUDIO_VOLTS = 2000;    // Arquivo "volts.mp3"
const int AUDIO_ZERO = 1001;     // Arquivo "zero.mp3"
const int AUDIO_MENOS = 1002;    // Arquivo "menos.mp3"

void setup() {
  Serial.begin(9600);
  dfPlayerSerial.begin(9600);
  
  pinMode(A0, INPUT);
  pinMode(BOTAO_TENSAO, INPUT_PULLUP);

  Serial.println("Teste de Tensão com Audio");
  
  if (!myDFPlayer.begin(dfPlayerSerial)) {
    Serial.println(F("Falha ao iniciar o DFPlayer!"));
    while(true) {
      delay(0);
    }
  }
  
  myDFPlayer.volume(25);  // Volume de 0 a 30
  myDFPlayer.play(2002);  // Som de inicialização
  delay(2000);
}

float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0;
  delay(450);
  return tensao_real;
}

void reproduzirNumero(float valor) {
  if (valor < 0) {
    myDFPlayer.play(AUDIO_MENOS);
    delay(800);
    valor = abs(valor);
  }
  
  int parteInteira = (int)valor;
  int parteDecimal = (int)((valor - parteInteira) * 100);
  
  if (parteInteira == 0) {
    myDFPlayer.play(AUDIO_ZERO);
    delay(800);
  } else {
    int digitos[10];
    int numDigitos = 0;
    
    int temp = parteInteira;
    while (temp > 0) {
      digitos[numDigitos++] = temp % 10;
      temp /= 10;
    }
    
    for (int i = numDigitos - 1; i >= 0; i--) {
      myDFPlayer.play(digitos[i] + 1);
      delay(800);
    }
  }
  
  myDFPlayer.play(AUDIO_VIRGULA);
  delay(800);
  
  if (parteDecimal < 10) {
    myDFPlayer.play(AUDIO_ZERO);
    delay(800);
    if (parteDecimal > 0) {
      myDFPlayer.play(parteDecimal + 1);
      delay(800);
    }
  } else {
    myDFPlayer.play(parteDecimal / 10 + 1);
    delay(800);
    myDFPlayer.play(parteDecimal % 10 + 1);
    delay(800);
  }
}

void exibir_Tensao() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);
  Serial.println(" V");
  
  reproduzirNumero(valor_tensao);
  delay(500);
  myDFPlayer.play(AUDIO_VOLTS);
  delay(1000);
}

void loop() {
  if (digitalRead(BOTAO_TENSAO) == LOW) {
    exibir_Tensao();
    delay(300);  // Debounce
  }
  
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    
    if (type == DFPlayerError) {
      Serial.print(F("Erro DFPlayer"));
    }
  }
} 