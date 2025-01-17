/* 
    Data: 16/07/2025 23h10
    by JK
*/




/* #include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> */

// Adafruit_SSD1306 display = Adafruit_SSD1306();

int botao_Tensao = 3;          // Pino do botão conectado no pin digital
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;

// Debouncing dos botões
unsigned long ultimoTempoBotaoTensao = 0;
unsigned long ultimoTempoBotaoCorrente = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(9600); // Inicializa o monitor serial
  
  pinMode(A0, INPUT);             // Configura o pino analógico como entrada
  pinMode(botao_Tensao, INPUT_PULLUP);
  pinMode(botao_Corrente, INPUT_PULLUP);

  Serial.println("_______________");
  Serial.println("JoKenPRo+");
}

// Função para medir a tensão no pino A0
float medidor_de_tensao() {
  int leituraAnalogica = analogRead(A0);  // Leitura direta do pino A0
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0; // Cálculo da tensão real
  delay(450);  // Tempo de espera para evitar leituras rápidas
  return tensao_real;
}

// Função para medir a corrente com filtragem
float medidor_de_corrente_filtrado(int numAmostras = 50) {
  float soma = 0.0;

  for (int i = 0; i < numAmostras; i++) {
    int leituraAnalogica = analogRead(A0);  // Leitura do pino A0
    float tensao_sensor = (leituraAnalogica * 5.0) / 1024.0;  // Conversão para tensão
    float corrente = (tensao_sensor - 2.5) / 0.185;  // Cálculo da corrente
    soma += corrente;  // Soma os valores lidos
    delay(10);  // Pequeno atraso para evitar leituras rápidas demais
  }

  float correnteFiltrada = soma / numAmostras;  // Calcula a média das leituras
  return correnteFiltrada;
}

// Exibe a corrente no monitor serial
void exibir_Corrente_PTBR() {
  float valor_corrente = medidor_de_corrente_filtrado(50);  // 50 amostras para suavizar a leitura
  Serial.print("Corrente: ");
  Serial.print(valor_corrente, 2);  // Exibe a corrente com 2 casas decimais
  Serial.println(" A");
}

// Exibe a tensão no monitor serial
void exibir_Tensao_PTBR() {
  float valor_tensao = medidor_de_tensao(); // Corrigido: chamada sem argumentos
  Serial.print("Tensão: ");
  Serial.print(valor_tensao, 2);  // Exibe a tensão com 2 casas decimais
  Serial.println(" V");
}

void loop() {
  estadoBotaoTensao = digitalRead(botao_Tensao);      // Ler o pino digital do botão
  estadoBotaoCorrente = digitalRead(botao_Corrente);  // Ler o pino digital do botão

  if (estadoBotaoTensao == LOW && millis() - ultimoTempoBotaoTensao > debounceDelay) {
    ultimoTempoBotaoTensao = millis();
    continuarT = 1;  // Ativa a medição de tensão
    continuarC = 0;  // Desativa a medição de corrente
  }

  if (estadoBotaoCorrente == LOW && millis() - ultimoTempoBotaoCorrente > debounceDelay) {
    ultimoTempoBotaoCorrente = millis();
    continuarC = 1;  // Ativa a medição de corrente
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
