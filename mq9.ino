int analogsensor = A0;

void setup() {
  // put your setup code here, to run once:

pinMode(analogsensor, OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
int inputanalog = analogRead(analogsensor);

Serial.println(inputanalog + " ppm");

delay(100);
}