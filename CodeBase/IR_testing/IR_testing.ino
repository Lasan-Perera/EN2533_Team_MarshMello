#define S0 A0
#define S1 A2
#define S2 A4
#define S3 A6
#define S4 A8
#define S5 A10
#define S6 A12
#define S7 A14

void setup() {
  Serial.begin(9600);

  pinMode(S0, INPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(S6, INPUT);
  pinMode(S7, INPUT);
}

void loop() {
  int v0 = analogRead(S0);
  int v1 = analogRead(S1);
  int v2 = analogRead(S2);
  int v3 = analogRead(S3);
  int v4 = analogRead(S4);
  int v5 = analogRead(S5);
  int v6 = analogRead(S6);
  int v7 = analogRead(S7);

  Serial.print("S0: "); Serial.print(v0);
  Serial.print(" | S1: "); Serial.print(v1);
  Serial.print(" | S2: "); Serial.print(v2);
  Serial.print(" | S3: "); Serial.print(v3);
  Serial.print(" | S4: "); Serial.print(v4);
  Serial.print(" | S5: "); Serial.print(v5);
  Serial.print(" | S6: "); Serial.print(v6);
  Serial.print(" | S7: "); Serial.println(v7);

  delay(100);
}
