int VQ;
int ACSPin = A2;

void setup() {
analogReference(DEFAULT);
Serial.begin(9600);
VQ = determineVQ(ACSPin); 
delay(1000);
}

void loop() {
Serial.print("Leitura: ");
Serial.print(readCurrent(ACSPin),2);
Serial.println(" mA");
delay(150);
}

int determineVQ(int PIN) {
Serial.print("Estimando a Media de coeficiente de tensao:");
long VQ = 0;
//le 1000 amostra para estabilizar o valor
for (int i=0; i<100; i++) {
VQ += abs(analogRead(PIN));
delay(1);
}
VQ /= 100;
Serial.print(map(VQ, 0, 1023, 0, 5000));
Serial.println(" mV");
return int(VQ);
}

float readCurrent(int PIN) {
int current = 0;
int sensitivity = 185;  //muda para 185 sefor ACS712-20A ou para 66 for ACS712-5A
//le 200 vezes
for (int i=0; i<200; i++) {
current += abs(analogRead(PIN) - VQ);
delay(1);
}
current = map(current/200, 0, 1023, 0, 5000);
return float(current)/sensitivity;
}
