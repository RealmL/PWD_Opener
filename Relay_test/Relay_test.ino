int SigPin=8;//信号引脚
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(SigPin,LOW);
  delay(1000);
  digitalWrite(SigPin,HIGH);
  delay(1000);
}
