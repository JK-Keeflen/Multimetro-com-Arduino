/* 
    Data: 25/01/2025 20h05
    by JK
*/


int botao_Tensao = 3;          // Pino do botão conectado no pin digital
int botao_Corrente = 4;
int continuarT = 0;
int continuarC = 0;
int estadoBotaoTensao;
int estadoBotaoCorrente;


void setup() {
  Serial.begin(9600); // Inicializa o monitor serial
  
  pinMode(A0, INPUT);             // Configura o pino analógico como entrada
  pinMode(A2, INPUT);
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
  Serial.print(valor_corrente, 2);  // Exibe a corrente com 2 casas decimais
  Serial.println(" A");
  delay(950);
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

  if (estadoBotaoTensao == LOW) {
    
    continuarC = 0;  // Desativa a medição de corrente
    
    if (continuarT < 2){
      continuarT = continuarT + 1;
    }else {
      continuarT = 0;
    }
    
  }//FIM de um IF

  if (estadoBotaoCorrente == LOW) {
    
    continuarT = 0;  // Desativa a medição de tensão

    if (continuarC < 2){
      continuarC = continuarC + 1;
    }else {
      continuarC = 0;
    }
  }//FIM de um IF
  
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
