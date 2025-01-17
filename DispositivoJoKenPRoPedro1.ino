
/* 
    Data: 16/07/2025 23h10
    by JK
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <cmath>

// =========== [ INÍCIO DAS ADIÇÕES PARA DFPLAYER ] ===========
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

// Definições do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Cria objeto display (I2C) - ajuste o reset para -1 se não usar
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos para conexão do DFPlayer
static const uint8_t DFPlayer_RX = 10; 
static const uint8_t DFPlayer_TX = 11; 

SoftwareSerial mySoftwareSerial(DFPlayer_RX, DFPlayer_TX);

class Mp3Notify {
  public:
    static void OnError(uint16_t errorCode) {
      Serial.print("DFPlayer Error: ");
      Serial.println(errorCode);
    }
};

DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mySoftwareSerial);
// [Removido o DFRobotDFPlayerMini myDFPlayer; pois não estava em uso]
// =========== [ FIM DAS ADIÇÕES PARA DFPLAYER ] ===========


// Pinos e variáveis originais
int botao_Tensao   = 3;
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

// Debouncing
unsigned long ultimoTempoBotaoTensao   = 0;
unsigned long ultimoTempoBotaoCorrente = 0;
const unsigned long debounceDelay = 200;

// -----------------------------------------------------
// setup()
// -----------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);
  pinMode(botao_Tensao,   INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+");

  // Inicializa o display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("JoKenPRo+ Iniciado");
  display.display();

  // Inicialização do DFPlayer
  mySoftwareSerial.begin(9600); 
  mp3.begin();
  delay(1000);
  mp3.setVolume(20); 
  Serial.println("DFPlayer Mini pronto para uso...");
}

// -----------------------------------------------------
// Função para medir a tensão no pino A0
// -----------------------------------------------------
float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);  
  // Ajuste a fórmula conforme seu divisor de tensão (se houver)
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0; 
  delay(450);  
  return tensao_real;
}

// -----------------------------------------------------
// Função para medir a corrente com filtragem
// -----------------------------------------------------
float medidor_de_corrente_filtrado(int numAmostras = 50) {
  float soma = 0.0;
  for (int i = 0; i < numAmostras; i++) {
    int leituraAnalogica = analogRead(A0);
    float tensao_sensor  = (leituraAnalogica * 5.0) / 1024.0;
    float corrente       = (tensao_sensor - 2.5) / 0.185;
    soma += corrente;
    delay(10);
  }
  float correnteFiltrada = soma / numAmostras;
  return correnteFiltrada;
}

// -----------------------------------------------------
// Falar número inteiro de 0..99 (com "menos" se negativo)
// Pré-requisitos de áudio no SD:
//   0..9, 10..19, 20..90 (passo de 10), 98="e", 9998="menos"
// -----------------------------------------------------
void falarNumero(int num) {
  // Se for negativo, fala "menos" e torna positivo
  if (num < 0) {
    mp3.playMp3FolderTrack(9998); // "menos"
    num = -num;
  }

  if (num < 10) {
    // 0..9 -> faixa = num
    mp3.playMp3FolderTrack(num);
  }
  else if (num < 20) {
    // 10..19 -> faixa = num
    mp3.playMp3FolderTrack(num);
  }
  else {
    // 20..99
    int dezena = (num / 10) * 10; // ex.: 26 -> 20
    int unidade = num % 10;      // ex.: 26 -> 6

    // Toca a dezena (20,30,40..)
    mp3.playMp3FolderTrack(dezena);

    // Se a unidade não for zero, toca "e" + unidade
    if (unidade != 0) {
      mp3.playMp3FolderTrack(98);    // "e"
      mp3.playMp3FolderTrack(unidade);
    }
  }
}

// -----------------------------------------------------
// Falar valor float (ex.: 9.84 => "nove ponto oitenta e quatro")
// Pré-requisitos de áudio no SD:
//   99 = "ponto"
// -----------------------------------------------------
void falarValorFloat(float valor) {
  // Separa parte inteira e fracionária
  int parteInteira;
  double parteDecimalF = modf(valor, &parteInteira); 
  int parteDecimal = abs(static_cast<int>(parteDecimalF * 100));
  
  // Fala a parte inteira
  falarNumero(parteInteira);

  // Se decimal != 0, fala "ponto" e depois a parte decimal
  if (parteDecimal != 0) {
    mp3.playMp3FolderTrack(99);    // "ponto"
    falarNumero(parteDecimal);
  }
}

// -----------------------------------------------------
// Função tocarPorValor() - se quiser somente falar
// o float de forma 9.84 => "nove ponto oitenta e quatro"
// -----------------------------------------------------
void tocarPorValor(float valor) {
  // Se você não precisar de lógicas adicionais, pode chamar direto:
  falarValorFloat(valor);
}

// -----------------------------------------------------
// Exibir Corrente no Serial e também falar o valor
// -----------------------------------------------------
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente_filtrado(50);
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);
  Serial.println(" A");

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Corrente (A):");
  display.println(valor_corrente, 2);
  display.display();

  // Fala o valor (ex.: "cinco ponto trinta e dois")
  tocarPorValor(valor_corrente);
}

// -----------------------------------------------------
// Exibir Tensão no Serial e também falar o valor
// -----------------------------------------------------
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);
  Serial.println(" V");

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Tensao (V):");
  display.println(valor_tensao, 2);
  display.display();

  // Fala o valor (ex.: "nove ponto oitenta e quatro")
  tocarPorValor(valor_tensao);
}

// -----------------------------------------------------
// loop()
// -----------------------------------------------------
void loop() {
  estadoBotaoTensao   = digitalRead(botao_Tensao);
  estadoBotaoCorrente = digitalRead(botao_Corrente);

  // Botão Tensao
  if (estadoBotaoTensao == LOW && millis() - ultimoTempoBotaoTensao > debounceDelay) {
    ultimoTempoBotaoTensao = millis();
    continuarT = 1;
    continuarC = 0;
  }

  // Botão Corrente
  if (estadoBotaoCorrente == LOW && millis() - ultimoTempoBotaoCorrente > debounceDelay) {
    ultimoTempoBotaoCorrente = millis();
    continuarC = 1;
    continuarT = 0;
  }
  
  if (continuarT == 1) {
    exibir_Tensao_PTBR();
    continuarT = 0; // opcional se quiser exibir apenas uma vez
  }

  if (continuarC == 1) {
    exibir_Corrente_PTBR();
    continuarC = 0; // opcional se quiser exibir apenas uma vez
  }

  // Se nenhum dos dois está ativo, só aguarda
  if ((continuarT == 0) && (continuarC == 0)) {
    delay(500);
  }

  // Se quiser gerenciar eventos do DFPlayer (não é obrigatório):
  // mp3.loop();
}

