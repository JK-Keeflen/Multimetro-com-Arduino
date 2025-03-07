#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// Pinos para o DFPlayer (RX, TX)
SoftwareSerial dfPlayerSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;

// Constantes para os arquivos de áudio
const int AUDIO_VIRGULA = 1000;  // Arquivo "virgula.mp3"
const int AUDIO_AMPERES = 2001;  // Arquivo "amperes.mp3"
const int AUDIO_ZERO = 1001;     // Arquivo "zero.mp3"

void setup() {
  Serial.begin(9600);
  dfPlayerSerial.begin(9600);
  
  pinMode(A2, INPUT);  // Pino para sensor de corrente

  // Inicializa o DFPlayer
  if (!myDFPlayer.begin(dfPlayerSerial)) {
    Serial.println(F("Falha ao iniciar o DFPlayer!"));
    while(true) {
      delay(0);
    }
  }
  
  myDFPlayer.volume(25);  // Volume de 0 a 30
  Serial.println("Teste de Medição de Corrente Iniciado");
}

float medidor_de_corrente() {
    int leituraAnalogica = analogRead(A2);
    float tensao_sensor = (leituraAnalogica * 5.0) / 1024.0;
    float corrente = abs((tensao_sensor - 2.50) / 0.185);
    return corrente;
}

void reproduzirNumero(float valor) {
  // Separa a parte inteira e decimal
  int parteInteira = (int)valor;
  int parteDecimal = (int)((valor - parteInteira) * 100);
  
  // Reproduz a parte inteira
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
  
  // Reproduz a parte decimal
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

void loop() {
  float valor_corrente = medidor_de_corrente();
  
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);
  Serial.println(" A");
  
  reproduzirNumero(valor_corrente);
  delay(500);
  myDFPlayer.play(AUDIO_AMPERES);
  
  delay(2000);  // Espera 2 segundos antes da próxima leitura
}