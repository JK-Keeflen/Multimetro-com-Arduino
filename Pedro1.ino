/* 
    Data: 21/02/2025 10h00
    by Pedro
*/

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// Definição dos pinos para o DFPlayer
#define PIN_RX 10
#define PIN_TX 11

// Definição das tracks de áudio
#define TRACK_MENOS 130      // Movido para 130 - símbolos especiais
#define TRACK_E 131         // Movido para 131 - palavras de conexão
#define TRACK_VIRGULA 132   // Movido para 132 - símbolos especiais
#define TRACK_ESTA_TENSAO 140    // Movido para 140 - frases de medição
#define TRACK_ESTA_CORRENTE 141  // Movido para 141 - frases de medição
#define TRACK_VOLT 150      // Movido para 150 - unidades
#define TRACK_AMPERE 151    // Movido para 151 - unidades
#define TRACK_OHMS 152      // Movido para 152 - unidades
#define TRACK_MILI 160      // Movido para 160 - prefixos
#define TRACK_KILO 161      // Movido para 161 - prefixos


// Criação dos objetos para o DFPlayer
SoftwareSerial mySoftwareSerial(PIN_RX, PIN_TX);
DFRobotDFPlayerMini mp3;

int botao_Tensao = 3;          // Pino do botão conectado no pin digital
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

void setup() {
  Serial.begin(9600);
  
  mySoftwareSerial.begin(9600);
  if (!mp3.begin(mySoftwareSerial)) {
    Serial.println(F("Falha ao iniciar DFPlayer"));
    while(true);
  }
  
  // Adicionar verificação do cartão SD
  if (!mp3.available()) {
    Serial.println(F("Cartão SD não detectado!"));
    while(true);
  }
  
  mp3.setTimeOut(500);
  mp3.volume(25);
  
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(botao_Tensao, INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("=================");
  Serial.println("Multimetro Sonoro");
  Serial.println("=================");
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
    //Serial.println(tensao_sensor);
    corrente = abs((tensao_sensor - 2.50) / 0.185);  // Cálculo da corrente | o "abs" é para o valor ser sempre positivo

    return corrente;
}

// Exibe a corrente no monitor serial
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente();
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);
  Serial.println(" A");
  anunciarMedicao(valor_corrente, false);
}

// Exibe a tensão no monitor serial
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);
  Serial.println(" V");
  anunciarMedicao(valor_tensao, true);
}

void loop() {
  estadoBotaoTensao = digitalRead(botao_Tensao);
  estadoBotaoCorrente = digitalRead(botao_Corrente);

  if (estadoBotaoTensao == LOW) {
    delay(50); // Debounce
    continuarT = !continuarT; // Toggle
    continuarC = 0;  // Desativa a medição de corrente
  }

  if (estadoBotaoCorrente == LOW) {
    delay(50); // Debounce
    continuarC = !continuarC; // Toggle
    continuarT = 0;  // Desativa a medição de tensão
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
}

String obterPrefixo(float &valor, bool isTensao) {
  const float absValor = abs(valor);
  const char* prefixos[] = {"Tera", "Giga", "Mega", "Quilo", "", "Mili", "Micro", "Nano", "Pico"};
  const float fatores[] = {1e12, 1e9, 1e6, 1e3, 1, 1e-3, 1e-6, 1e-9, 1e-12};
  
  for(int i = 0; i < 9; i++) {
    if(absValor >= fatores[i]) {
      valor /= fatores[i];
      return prefixos[i];
    }
  }
  return "";
}

void falarNumero(int num) {
  if (num < 0) {
    mp3.playMp3FolderTrack(TRACK_MENOS);
    num = -num;
  }

  if (num < 20) {
    mp3.playMp3FolderTrack(num);
  } else {
    int dezena = (num / 10) * 10;
    int unidade = num % 10;

    if(dezena > 0) mp3.playMp3FolderTrack(dezena);
    if(unidade > 0) {
      mp3.playMp3FolderTrack(TRACK_E);
      mp3.playMp3FolderTrack(unidade);
    }
  }
  delay(500);
}

void falarValorFloat(float valor) {
  int parteInteira;
  double parteDecimalF = modf(valor, &parteInteira); 
  int parteDecimal = abs(static_cast<int>(parteDecimalF * 100));

  falarNumero(parteInteira);

  if (parteDecimal != 0) {
    mp3.playMp3FolderTrack(TRACK_VIRGULA);
    falarNumero(parteDecimal);
  }
}

void anunciarMedicao(float valor, bool isTensao) {
  String prefixo = obterPrefixo(valor, isTensao);
  
  // Tocar frase inicial
  if(isTensao) {
    mp3.playMp3FolderTrack(TRACK_ESTA_TENSAO);
  } else {
    mp3.playMp3FolderTrack(TRACK_ESTA_CORRENTE);
  }
  delay(1000);

  // Tocar valor
  falarValorFloat(valor);
  
  // Tocar prefixo se existir
  if(prefixo.length() > 0) {
    mp3.playMp3FolderTrack(TRACK_E);
    
    // Mapear nome do prefixo para número da faixa
    if(prefixo == "Mili") mp3.playMp3FolderTrack(TRACK_MILI);
    else if(prefixo == "Micro") mp3.playMp3FolderTrack(41);
    // Adicionar outros prefixos conforme necessário
  }
  
  // Tocar unidade
  if(isTensao) {
    mp3.playMp3FolderTrack(TRACK_VOLT);
  } else {
    mp3.playMp3FolderTrack(TRACK_AMPERE);
  }
  delay(1000);
}
