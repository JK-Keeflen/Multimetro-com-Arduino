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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BLUE 0x001F //color

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
DFRobotDFPlayerMini myDFPlayer;          // Create a DFPlayerMini object

// =========== [ FIM DAS ADIÇÕES PARA DFPLAYER ] ===========


// Seus pinos e variáveis originais:
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


void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);
  pinMode(botao_Tensao,   INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+");

  // Inicialização do DFPlayer
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

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
  // Ajuste aqui a fórmula de acordo com seu circuito (divisor de tensão, etc.)
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


void separarPartes(float valor, int& parteInteira, int& parteDecimal) {
  double parteDecimalF;
  // modf separa o valor em parte inteira e parte decimal
  parteDecimalF = modf(valor, &parteInteira);

  // Multiplica a parte decimal por 100 para obter dois dígitos (ex.: 0.84 -> 84)
  parteDecimal = abs(static_cast<int>(parteDecimalF * 100)); 
}

// Função para tocar arquivos MP3 com base nos valores separados
void tocarPorValor(float valor) {
  int parteInteira, parteDecimal;

  // Separa as partes
  separarPartes(valor, parteInteira, parteDecimal);

  // Converte as partes em números de faixa para o DFPlayer
  int faixaInteira = parteInteira;         // ex.: 9 -> 9
  int faixaDecimal = parteDecimal + 100;  // ex.: 84 -> 184 (supondo 0084.mp3 é faixa 184)

  // Toca a parte inteira
  if (faixaInteira > 0) {
    mp3.playMp3FolderTrack(faixaInteira);
  }

  mp3.playMp3FolderTrack(9999);

  // Aguarda um pouco antes de tocar a parte decimal
  delay(1000);

  // Toca a parte decimal
  if (faixaDecimal > 100) {
    mp3.playMp3FolderTrack(faixaDecimal);
  }
}
// -----------------------------------------------------
// Exibir Corrente no Serial e TOCAR MP3 DIFERENTE
// conforme o valor da corrente
// -----------------------------------------------------
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente_filtrado(50);
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);
  Serial.println(" A");
  tocarPorValor(valor_corrente);

  // Exemplo de lógica:
  //   Se corrente < 1 A  -> toca 0003.mp3
  //   Se corrente < 3 A  -> toca 0004.mp3
  //   Caso contrário     -> toca 0005.mp3
  //
  // Ajuste as faixas conforme a sua necessidade


// -----------------------------------------------------
// Exibir Tensão no Serial e TOCAR MP3 DIFERENTE
// conforme o valor da tensão
// -----------------------------------------------------
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao();
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);
  Serial.println(" V");
  tocarPorValor(valor_tensao);

  // Exemplo de lógica:
  //   Se tensao < 3 V  -> toca 0001.mp3
  //   Se tensao < 6 V  -> toca 0002.mp3
  //   Caso contrário   -> toca 0006.mp3
  //
  // Ajuste as faixas conforme a sua necessidade




void loop() {
  estadoBotaoTensao   = digitalRead(botao_Tensao);
  estadoBotaoCorrente = digitalRead(botao_Corrente);

  if (estadoBotaoTensao == LOW && millis() - ultimoTempoBotaoTensao > debounceDelay) {
    ultimoTempoBotaoTensao = millis();
    continuarT = 1;
    continuarC = 0;
  }

  if (estadoBotaoCorrente == LOW && millis() - ultimoTempoBotaoCorrente > debounceDelay) {
    ultimoTempoBotaoCorrente = millis();
    continuarC = 1;
    continuarT = 0;
  }
  
  if (continuarT == 1) {
    exibir_Tensao_PTBR();
  }

  if (continuarC == 1) {
    exibir_Corrente_PTBR();
  }

  if ((continuarT == 0) && (continuarC == 0)) {
    delay(500);
  }

  // Se quiser gerenciar eventos do DFPlayer:
  // mp3.loop();
}

