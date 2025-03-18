/*  
    Data: 13/03/2025 10h22  
    by JK  
*/  

#include <SoftwareSerial.h>  

SoftwareSerial bluetooth(2, 3);  // RX, TX (ajuste conforme necessário)  

int botao_Tensao = 6;  
int botao_Corrente = 7;  
int continuarT = 0;  
int continuarC = 0;  
int estadoBotaoTensao;  
int estadoBotaoCorrente;  

void setup() {  
  Serial.begin(9600);  
  bluetooth.begin(9600); // Inicializa Bluetooth  

  pinMode(A0, INPUT);  
  pinMode(A2, INPUT);  
  pinMode(botao_Tensao, INPUT_PULLUP);  
  pinMode(botao_Corrente, INPUT_PULLUP);  

  //Serial.println("_______________");  
  //Serial.println("JoKenPRo+");  
  //bluetooth.println("Bluetooth pronto!");  
}  

float medidor_de_tensao() {  
  int leituraAnalogica = analogRead(A0);  
  float tensao_real = (leituraAnalogica * 5.0 * 5.0) / 1024.0;  
  delay(450);  
  return tensao_real;  
}  

float medidor_de_corrente() {  
  int leituraAnalogica = analogRead(A2);  
  float tensao_sensor = (leituraAnalogica * 5 ) / 1024.0;  
  float corrente = abs((tensao_sensor - 2.50) / 0.185);  
  return corrente;  
}  

void exibir_Corrente_PTBR() {  
  float valor_corrente = medidor_de_corrente();  
  /*Serial.print("Corrente: ");  
  Serial.print(valor_corrente, 2);  
  Serial.println(" A");  */

  // Enviar para o Bluetooth em formato TTS-friendly  
  bluetooth.print(valor_corrente);  
  //delay(500);
  bluetooth.println(" amperes");  

  delay(1000);  
}  

void exibir_Tensao_PTBR() {  
  float valor_tensao = medidor_de_tensao();
  
  /*Serial.print("Tensão: ");  
  Serial.print(valor_tensao, 2);  
  Serial.println(" V"); */

  // Enviar para o Bluetooth em formato TTS-friendly  
  bluetooth.print(valor_tensao);  
  bluetooth.println(" volts");  
  delay(3500);

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
