void blinkLED(long blinkLen)
{
  digitalWrite(7, LOW);
  delay(blinkLen);
  digitalWrite(7, HIGH);
  delay(blinkLen);
}

void blinkError(int blinks)
{
  for (int i = 0; i<blinks; i++)
  {
    blinkLED(200);
  }
}

