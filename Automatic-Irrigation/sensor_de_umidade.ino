//#define pinSolenoide 7
#define pinSensorA A0
#define pinSensorD 8


void setup() {
  pinMode(pinSensorD, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print("Digital:");

  if (digitalRead(pinSensorD)) {
    Serial.print("UMIDADE BAIXA");

  } else {
    Serial.print("UMIDADE ALTA");
  }

  Serial.print(" Analogico:");
  Serial.print(analogRead(pinSensorA));
  Serial.print(" ");

  Serial.print(" Atuador:");
  if (analogRead(pinSensorA) > 700) {
    Serial.println("SOLENOIDE LIGADO");
    //digitalWrite(pinSolenoide, HIGH);
  } else {
    Serial.println("SOLENOIDE DESLIGADO");
    //digitalWrite(pinSoneloide< LOW);
  }
}
