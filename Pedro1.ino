#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Configuração do DFPlayer Mini
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini mp3;

// Definição dos pinos
int botao_Tensao = 3;
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

// Definição das faixas de áudio
#define TRACK_ESTA_CORRENTE 50
#define TRACK_ESTA_TENSAO 51
#define TRACK_AMPERE 52
#define TRACK_VOLT 53
#define TRACK_PONTO 54
#define TRACK_MENOS 55
#define TRACK_E 98
#define TRACK_VIRGULA 99

void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  
  if (!mp3.begin(mySoftwareSerial)) {
    Serial.println(F("Não foi possível inicializar o DFPlayer!"));
    while(true);
  }
  
  mp3.volume(30); // Ajuste o volume (0-30)

  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(botao_Tensao, INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+");
}

float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0;
  delay(450);
  return tensao_real;
}

float medidor_de_corrente_filtrado(int numAmostras = 50) {
  float soma = 0.0;

  for (int i = 0; i < numAmostras; i++) {
    int leituraAnalogica = analogRead(A2);
    float tensao_sensor = (leituraAnalogica * 5.0) / 1024.0;
    float corrente = (tensao_sensor - 2.5) / 0.185;
    soma += corrente;
    delay(10);
  }

  return soma / numAmostras;
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
    // Tocar "e" antes do prefixo
    
    
    // Mapear nome do prefixo para número da faixa
    if(prefixo == "Mili") mp3.playMp3FolderTrack(40);
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


void exibir_Corrente_PTBR() {
  float valor = medidor_de_corrente_filtrado(50);
  Serial.print("Corrente: ");
  Serial.print(valor, 2);
  Serial.println(" A");
  anunciarMedicao(valor, false);
}

void exibir_Tensao_PTBR() {
  float valor = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor, 2);
  Serial.println(" V");
  anunciarMedicao(valor, true);
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
  
  if (continuarT == 1) exibir_Tensao_PTBR();
  if (continuarC == 1) exibir_Corrente_PTBR();
  if ((continuarT == 0) && (continuarC == 0)) delay(500);
}
