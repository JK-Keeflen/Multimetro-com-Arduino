

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// Pinos para o DFPlayer (RX, TX)
SoftwareSerial dfPlayerSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Pinos para os botões
int botao_Tensao = 3;          
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

// Constantes para os arquivos de áudio
const int AUDIO_VIRGULA = 1000;  // Arquivo "virgula.mp3" (1000.mp3)
const int AUDIO_VOLTS = 2000;    // Arquivo "volts.mp3" (2000.mp3)
const int AUDIO_AMPERES = 2001;  // Arquivo "amperes.mp3" (2001.mp3)
const int AUDIO_ZERO = 1001;     // Arquivo "zero.mp3" (1001.mp3)
const int AUDIO_MENOS = 1002;    // Arquivo "menos.mp3" (1002.mp3)

void setup() {
  Serial.begin(9600); // Inicializa o monitor serial
  dfPlayerSerial.begin(9600);
  
  pinMode(A0, INPUT);             // Configura o pino analógico como entrada
  pinMode(A2, INPUT);
  pinMode(botao_Tensao, INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+ com Audio");
  
  // Inicializa o DFPlayer
  Serial.println(F("Inicializando DFPlayer..."));
  
  if (!myDFPlayer.begin(dfPlayerSerial)) {
    Serial.println(F("Falha ao iniciar o DFPlayer:"));
    Serial.println(F("1. Verifique as conexões!"));
    Serial.println(F("2. Verifique se o cartão SD está inserido!"));
    while(true) {
      delay(0); // Compatibilidade com watchdog do ESP8266
    }
  }
  
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(25);  // Volume de 0 a 30
  
  // Toque um som de inicialização
  myDFPlayer.play(2002); // Supondo que 2002.mp3 seja um som de inicialização
  delay(2000);
}

// Função para medir a tensão no pino A0
float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);  // Leitura direta do pino A0
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0; // Cálculo da tensão real
  delay(450);  // Tempo de espera para evitar leituras rápidas
  return tensao_real;
}

float medidor_de_corrente(){
    int leituraAnalogica = 0;
    float tensao_sensor = 0.0;
    float corrente = 0;
    
    leituraAnalogica = analogRead(A2);  // Leitura do pino A2 
    tensao_sensor = (leituraAnalogica * 5 ) / 1024.0;  // Conversão para tensão     
    corrente = abs((tensao_sensor - 2.50) / 0.185);  // Cálculo da corrente | o "abs" é para o valor ser sempre positivo

    return corrente;
}

// Função para reproduzir um número em áudio
void reproduzirNumero(float valor) {
  // Verifica se o valor é negativo
  if (valor < 0) {
    myDFPlayer.play(AUDIO_MENOS);
    delay(800);
    valor = abs(valor);
  }
  
  // Separa a parte inteira e decimal
  int parteInteira = (int)valor;
  int parteDecimal = (int)((valor - parteInteira) * 100);
  
  // Reproduz a parte inteira
  if (parteInteira == 0) {
    myDFPlayer.play(AUDIO_ZERO);
    delay(800);
  } else {
    // Reproduz cada dígito da parte inteira
    int digitos[10]; // Máximo de 10 dígitos
    int numDigitos = 0;
    
    // Extrai os dígitos
    int temp = parteInteira;
    while (temp > 0) {
      digitos[numDigitos++] = temp % 10;
      temp /= 10;
    }
    
    // Reproduz os dígitos na ordem correta
    for (int i = numDigitos - 1; i >= 0; i--) {
      myDFPlayer.play(digitos[i] + 1); // +1 porque os arquivos começam em 0001.mp3
      delay(800);
    }
  }
  
  // Reproduz a vírgula
  myDFPlayer.play(AUDIO_VIRGULA);
  delay(800);
  
  // Reproduz a parte decimal
  if (parteDecimal < 10) {
    // Se for menor que 10, reproduz o zero primeiro
    myDFPlayer.play(AUDIO_ZERO);
    delay(800);
    
    if (parteDecimal > 0) {
      myDFPlayer.play(parteDecimal + 1); // +1 porque os arquivos começam em 0001.mp3
      delay(800);
    }
  } else {
    // Reproduz os dois dígitos da parte decimal
    int digito1 = parteDecimal / 10;
    int digito2 = parteDecimal % 10;
    
    myDFPlayer.play(digito1 + 1); // +1 porque os arquivos começam em 0001.mp3
    delay(800);
    myDFPlayer.play(digito2 + 1); // +1 porque os arquivos começam em 0001.mp3
    delay(800);
  }
}

// Exibe a corrente no monitor serial e reproduz em áudio
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente();
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);  // Exibe a corrente com 2 casas decimais
  Serial.println(" A");
  
  // Reproduz o valor em áudio
  reproduzirNumero(valor_corrente);
  delay(500);
  myDFPlayer.play(AUDIO_AMPERES);
  delay(1000);
}

// Exibe a tensão no monitor serial e reproduz em áudio
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);  // Exibe a tensão com 2 casas decimais
  Serial.println(" V");
  
  // Reproduz o valor em áudio
  reproduzirNumero(valor_tensao);
  delay(500);
  myDFPlayer.play(AUDIO_VOLTS);
  delay(1000);
}

void loop() {
  estadoBotaoTensao = digitalRead(botao_Tensao);      // Ler o pino digital do botão
  estadoBotaoCorrente = digitalRead(botao_Corrente);  // Ler o pino digital do botão

  if (estadoBotaoTensao == LOW) {
    continuarC = 0;  // Desativa a medição de corrente
    
    if (continuarT < 2){
      continuarT = continuarT + 1;
    } else {
      continuarT = 0;
    }
    
    delay(300); // Debounce do botão
  }

  if (estadoBotaoCorrente == LOW) {
    continuarT = 0;  // Desativa a medição de tensão

    if (continuarC < 2){
      continuarC = continuarC + 1;
    } else {
      continuarC = 0;
    }
    
    delay(300); // Debounce do botão
  }
  
  if (continuarT == 1) {
    exibir_Tensao_PTBR();      // Chama a função para exibir o valor da tensão
  }

  if (continuarC == 1) {
    exibir_Corrente_PTBR();  // Chama a função para exibir corrente
  }

  if ((continuarT == 0) && (continuarC == 0)) {
    delay(500);
  }
  
  // Verifica se há mensagens do DFPlayer
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    
    // Apenas para debug
    if (type == DFPlayerError) {
      Serial.print(F("Erro DFPlayer: "));
      switch (value) {
        case Busy:
          Serial.println(F("Cartão não encontrado"));
          break;
        case Sleeping:
          Serial.println(F("Em modo sleep"));
          break;
        case SerialWrongStack:
          Serial.println(F("Pilha errada"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Checksum não corresponde"));
          break;
        case FileIndexOut:
          Serial.println(F("Índice de arquivo fora dos limites"));
          break;
        case FileMismatch:
          Serial.println(F("Arquivo não encontrado"));
          break;
        case Advertise:
          Serial.println(F("Em anúncio"));
          break;
        default:
          break;
      }
    }
  }
}