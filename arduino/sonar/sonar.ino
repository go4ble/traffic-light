#define NUM_READINGS  10
#define SONAR_PIN     A0

int readings[NUM_READINGS] = { 0 };
int readIndex = 0;
int total = 0;
int average = 0;

// int sum;
// int readingsAverage() {
//   sum = 0;
//   for (int i = 0; i < NUM_READINGS; i++) {
//     sum += readings[i];
//   }
//   return sum / NUM_READINGS;
// }

void setup() {
  Serial.begin(9600);
}

void loop() {
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(SONAR_PIN);
  total = total + readings[readIndex];
  readIndex += 1;
  if (readIndex >= NUM_READINGS) readIndex = 0;
  average = total / NUM_READINGS;
  Serial.println(average);
  delay(100);
}
