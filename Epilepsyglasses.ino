#define LCD_PIN 3
#define LED_PIN 4

#define BUFSIZE 128
#define JITTER_THRESHOLD 10
#define ABS_RISING_THRESHOLD 102
#define STATE_DURATION 100
#define BLACKOUT_MS 500
#define HZ_MIN 5
#define DELAY 10

int sensorValue;
uint16_t prevValue = 0;
size_t lastRead = 0;
size_t lastRisingTime = 0;
size_t blockingTime = 0;
bool isRising = false;
uint16_t absRising = 0;
uint16_t risingTimeoutMin = 1000 / HZ_MIN;

void setBlackout(size_t* timer)
{
  blockingTime = *timer + BLACKOUT_MS;
}

void testBlackout(size_t* timer)
{
  if (blockingTime > *timer) {
    //Serial.println("BLACKOUT");
    digitalWrite(LCD_PIN, HIGH);
  } else {
    digitalWrite(LCD_PIN, LOW);
  }
}

void setState(int value, size_t* timer)
{
  if (value > 0) {
    absRising += abs(value);
    //Serial.println(absRising);
    if (absRising > ABS_RISING_THRESHOLD && !isRising) {
      isRising = true;
      //Serial.println("RISE");
      digitalWrite(LED_PIN, HIGH);
      if (*timer - lastRisingTime < risingTimeoutMin) {
        setBlackout(timer);
      }
      lastRisingTime = *timer;
    }
  } else {
    absRising = 0;
    isRising = false;
    digitalWrite(LED_PIN, LOW);
  }
}

void bootCheck()
{
  digitalWrite(LED_PIN, HIGH);
  delay(333);
  digitalWrite(LED_PIN, LOW);
  delay(333);
  digitalWrite(LED_PIN, HIGH);
  delay(333);
  digitalWrite(LED_PIN, LOW);
  delay(333);
  digitalWrite(LED_PIN, HIGH);
  delay(333);
  digitalWrite(LED_PIN, LOW);
  delay(333);
  digitalWrite(LCD_PIN, HIGH);
  delay(333);
  digitalWrite(LCD_PIN, LOW);
  delay(333);
  digitalWrite(LCD_PIN, HIGH);
  delay(333);
  digitalWrite(LCD_PIN, LOW);
  delay(333);
  digitalWrite(LCD_PIN, HIGH);
  delay(333);
  digitalWrite(LCD_PIN, LOW);
  delay(333);
}

void setup() {
  //Serial.begin(115200);
  pinMode(2, INPUT);
  pinMode(LCD_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  bootCheck();
  //Serial.println("Initialized!");
}

void loop() {
  size_t timer = millis();
  if (lastRead + DELAY <= timer) {
    sensorValue = analogRead(1);
    //Serial.println(sensorValue);
    int curDiff = prevValue - sensorValue;
    if (abs(curDiff) > JITTER_THRESHOLD) {
      setState(curDiff, &timer);
    }
    testBlackout(&timer);
    prevValue = sensorValue;
    lastRead = timer;
    delay(max(0, millis() - timer));
  }
    
}

